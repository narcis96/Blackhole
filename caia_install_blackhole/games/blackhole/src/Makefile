.PHONY: all clean

all clean:
	for dir in `ls -d */`; do if [ -e $${dir}Makefile ]; then make -C $$dir $@; fi done
