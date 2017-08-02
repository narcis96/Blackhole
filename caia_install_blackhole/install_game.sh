#!/bin/sh

## Caia installer
## Wilmer van der Gaast <wilmer@gaast.net>

## When not calling from the right directory, move!
myhome=`dirname $0`
[ -n "$myhome" ] && cd $myhome

if [ -z "$1" ]; then
	count=`ls games | wc --lines`
	if [ "$count" -eq 1 ]; then
		game=`basename games/*`
	elif [ "$count" -gt 0 ]; then
		echo 'You have to specify the game to install. Possible games are:'
		echo
		ls games
		exit 1
	else
		echo 'No installable games present'
		exit 1
	fi
else
	game=$1
fi

if [ ! -d "games/$game" ]; then
	echo 'That game does not exist.' > /dev/fd/2
	exit 1
fi

make -C caiaio
make -C jarwrapper
make -C javawrapper

## Do this before the make process so we'll only copy source files.
mkdir -p ~/caia/$game/src/
## Copy all source files
cp -r games/$game/src/* ~/caia/$game/src

## If there's any source, compile it.
for i in games/$game/src/*/; do
	make -C $i
done

mkdir -p ~/caia/$game/bin
mkdir -p ~/caia/$game/refereelogs
mkdir -p ~/caia/$game/playerlogs
mkdir -p ~/caia/$game/competitionlogs

cp caiaio/caiaio javawrapper/javawrapper jarwrapper/jarwrapper ~/caia/$game/bin/
chmod +x games/$game/bin/referee* games/$game/bin/player*
cp -R games/$game/bin/* ~/caia/$game/bin/

echo
echo
echo 'Installation finished. The game was installed in ~/caia/'$game'/.'
echo
echo 'Have fun!'

