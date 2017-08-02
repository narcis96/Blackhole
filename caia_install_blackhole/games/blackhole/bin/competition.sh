#!/bin/sh -e

# Run a CAIA competition, but many matches in parallel using xargs.
# Keep the competition-stubs and -results directories empty for best results.

# Author: Wilmer van der Gaast <wilmer@gaast.net>

BASE=$HOME/caia/more
STUBS=$BASE/competition-stubs
RES=$BASE/competition-results

# Number of matches to play per pair of players.
NUM_PER_PAIR=10
# Number of milliseconds to allow per player per game.
PLAYER_TIME_LIMIT=30000

DATE_FMT="%Y-%m-%d %H:%M:%S"

if [ -z "$*" ]; then
	echo Must pass participants as arguments.
	echo Example: $0 mysubmission player2 player3
	exit 1
fi

players="$*"

for player in $players; do
	if [ ! -x $player ]; then
		echo "$player doesn't appear to be a valid executable"
		exit 1
	fi
	if [ -n "$(echo $player | tr -d _A-Za-z0-9)" ]; then
		echo "Executable name $player contains invalid characters."
		echo "It should be in current working directory."
		exit 1
	fi
done

# Don't count the number of logical cores/threads. Although a competition
# finishes slightly faster when using all (hyper)threads, it's only a 10-20%
# improvement instead of the ~100% you'd expect. You just get threads tied
# up waiting for execution units very often. Nice when testing, but it ruins
# the time limits/etc.
if [ -z "$num_cores" ] && [ -e /proc/cpuinfo ]; then
	num_cores=$(grep ^'core id\b' /proc/cpuinfo | sort | uniq | wc -l)
fi

if [ -z "$num_cores" ] || [ "$num_cores" -lt "1" ]; then
	# OS X, src:
	# http://stackoverflow.com/questions/1715580/how-to-discover-number-of-cores-on-mac-os-x
	# http://www.opensource.apple.com/source/xnu/xnu-792.13.8/libkern/libkern/sysctl.h
	num_cores=$(sysctl -n hw.physicalcpu || echo 0)
fi

if [ -z "$num_cores" ] || [ "$num_cores" -lt "1" ]; then
	num_cores=2
	echo "Couldn't figure out number of cores, will guess $num_cores."
else
	echo "Number of cores (w/o Hyper-Threading): $num_cores."
fi

# For good hygiene, you should rm -rf these two dirs first, but I'm always
# very very wary of shellscripts using that command.
mkdir -p $STUBS $RES
rm -rf $STUBS/*.sh

OUT=/proc/$$/fd/1
if [ ! -e "$OUT" ]; then
	# On OS X, we'll have to use a different trick.
	OUT=$BASE/competition-logs.fifo
	[ -e "$OUT" ] || mkfifo "$OUT"
	tail -F "$OUT" &
	
	# I hope this cleanup will be solid..
	tailpid=$!
	trap "kill $tailpid" EXIT
fi

scorefiles=""
# Write a bunch of shellscripts that will work as caiaio clients, instructing
# it to do one match and write the results to the right logs.
for p1 in $players; do
	for p2 in $players; do
		if [ "$p1" = "$p2" ]; then
			continue
		fi
		case "$p1" in
			zetter*)
				case "$p2" in
					zetter*)
						continue;;
				esac;;
		esac
					
		scores=$RES/$p1-$p2.scores
		scorefiles="$scorefiles $scores"
		if [ -e "$scores" ] && [ "$scores" -nt "$p1" -a "$scores" -nt "$p2" ]; then
			echo "Skipping $p1 - $p2 since binaries didn't change."
			continue
		fi
		rm -f $scores
		for i in $(seq 1 $NUM_PER_PAIR); do
			f=$STUBS/$p1-$p2.$(printf %03d $i).sh
			cat << EOF2 > $f
#!/bin/sh

cat<<EOF
I number_players 2
I player 1 $p1 $PLAYER_TIME_LIMIT $BASE/playerlogs/$p1.white.$p2.$i.log
I start 1
I player 2 $p2 $PLAYER_TIME_LIMIT $BASE/playerlogs/$p2.black.$p1.$i.log
I start 2
I referee $BASE/bin/referee $BASE/refereelogs/$p1-$p2.$i.log reflog.log
EOF
read discard p1s p2s text
if [ \$p1s -gt \$p2s ]; then
	win=$p1
elif [ \$p2s -gt \$p1s ]; then
	win=$p2
else
	win=nobody
fi
echo \$(date +"$DATE_FMT") \$win \$p1s \$p2s \$text >> $RES/$p1-$p2.res
echo \$(date +"$DATE_FMT") \$win wins match $i!   $p1 - $p2: \$p1s - \$p2s >> $OUT
echo $p1 \$p1s >> $scores
echo $p2 \$p2s >> $scores
cat <<EOF
I kill_referee
I kill 1
I kill 2
I stop_caiaio
EOF
sleep 1
EOF2
			chmod 755 $f
		done
	done
done

if [ -n "$(ls $STUBS/*.sh 2> /dev/null)" ]; then # OSX-- xargs doesn't have -r
	# Have xargs run caiaio with all the stub clients created above. The
	# little- known flag -P tells it to run stuff in parallel.  :-D
	ls $STUBS/*.sh | xargs -P$num_cores -n1 -- $BASE/bin/caiaio -m
else
	echo "No new matches to play."
fi

# Horrible shell hack to sum up all the scores and sort them.
for p in $players; do
	printf "%6d  %-32s\n" $(($(grep -w ^$p $scorefiles | cut -d\  -f2 | xargs | tr \  +))) $p
done | sort -rn
