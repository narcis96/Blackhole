# Copyright (C) by
# Jaap Taal
# The Hague, 29 August 2004

# This file autocompiles and creates dependency files for all .cc files inside the directory.
# - It gets all .cc files using the function wildcard
# - gets the basename from these .cc files
# - creates a list of object files that need to be created
# - 

.PHONY: clean
# yank all .cc files
ccsourcefiles = $(wildcard *.cc)
csourcefiles = $(wildcard *.c)
ifneq ($(ccsourcefiles),)
CC = g++
else
CC = gcc
endif
sourcefiles = $(ccsourcefiles) $(csourcefiles)
# derive basenames from sourcesfiles
basenames = $(basename $(sourcefiles))
# append .o to each basename to obtain objectfiles
objectfiles = $(addsuffix .o,$(basenames))
# append .d to each basename dmakefiles
dmakefiles = $(addsuffix .d,$(basenames))

# program name is the same as the directory name
targetfile = ./$(shell pwd | sed 's,.*[~/]\(.*\),\1,')
# targetfile = ./customname

# set cxx compile flags (this variable is used with Make's rules to compile a .cc to a .o)
CXXFLAGS = -Wall -O2
CFLAGS = -std=c99 -Wall -O2

# set link flags to include ncurses (this variable is used with Make's rules to link .o files)
# LDFLAGS= -lncurses

# create a formatted timestamp for backups etc (not used here)
timestamp=$(shell date +%Y%m%d_%H%M)

all: $(targetfile)
	if [ -e "./install.sh" ]; then sh install.sh; fi

# rule to create $(targetfile) from all $(objectfiles)
$(targetfile): $(objectfiles)
	$(CC) $(CXXFLAGS) $(LDFLAGS) -o $(targetfile) $(objectfiles)

$(objectfiles): Makefile

# clean all unnecessary files which can be rebuilt (usefull when backuping)
clean:
	rm -f $(objectfiles) $(targetfile) $(dmakefiles) $(targetfile).exe

# Auto prerequisite makefile rule:
# tell make how to create a .d file from a .cc file
%.d: %.cc
	@$(CC) -MM $(CXXFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

# include will trigger .d files to be updated by the prerequisite rule if necessary
-include $(dmakefiles)

# because of the former include make automagically makes .d files with the %.d:%.cc rule
# the alert(s) that .d files don't exist can be ignored


