#!/bin/sh
targetfile=`pwd | sed 's,.*[~/]\(.*\),\1,'`
destpath=../../bin
cp "$targetfile" "$destpath"
