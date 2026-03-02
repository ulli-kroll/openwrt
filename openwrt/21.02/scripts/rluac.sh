#!/bin/sh

SPATH="$1"
LUAC_PATH="$2"

filelist=`find ${SPATH} -type f -name "*.lua"`

for filename in $filelist ; do
	case "$filename" in
	*/debug.lua)
		;;
	*.lua)
		$LUAC_PATH -s -o $filename $filename
		;;
	esac
done

