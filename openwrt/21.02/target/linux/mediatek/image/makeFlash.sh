#!/bin/bash

echo $1
echo $2
echo $3

num=$(sed -n 1p $1 | awk -F= '{print $2}')
for i in $(seq $num);
	do
		line=`expr $i + 1`
		baseaddr=$(sed -n ${line}p $1 | awk '{print $3}' | awk -F0x '{print $2}')
		max_size=$(sed -n ${line}p $1 | awk '{print $4}' | awk -F0x '{print $2}')
		file=$(sed -n ${line}p $1 | awk '{print $5}')
		file_path=$2/$file
		file_size=`ls -l $file_path | awk '{ print $5 }'`
		partition_name=$(sed -n ${line}p $1 | awk '{print $2}')
		[ X$partition_name = X"Factory" ] && ( tr '\000' '\377' < /dev/zero | dd of=$3 bs=1 count=$((16#$max_size)) seek=$((16#$baseaddr)) ) && continue
		[ X$file = X"" ] && continue
		echo "$file"
		[ $file_size -gt $((16#$max_size)) ] && echo "$file is too large" && exit 1
		dd if=$file_path of=$3 bs=1 count=$((16#$max_size)) seek=$((16#$baseaddr)) conv=sync
	done
