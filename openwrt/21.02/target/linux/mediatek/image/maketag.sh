#!/bin/bash

### FIXME: Shoule be more smart!!!
NAND_PAGE_SIZE=2048
#NAND_PAGE_SIZE="512"
NAND_SPARE_SIZE=64
NAND_BCH_PROCESS_NUM=16

[ -z "$1" ] && echo "need input file" && return
inputfile=$1
outputfile=$2
echo "$inputfile need do ecc tag"
[ -n "$3" ] && NAND_PAGE_SIZE=$3
[ -n "$4" ] && NAND_SPARE_SIZE=$4
NAND_PAGE_SIZE_ALL=$(($NAND_PAGE_SIZE+$NAND_SPARE_SIZE))

###############################################################
#               Local functions
###############################################################
### brief   generating BCH file.
### @param  input file path
bch()
{

    local input_file=$1
    
    echo "$input_file.bch begin"
    
	local bch_counter=$(($(stat -c %s "$input_file")/$NAND_PAGE_SIZE))
	local step=0
	
	while [ "$step" -lt "$bch_counter" ];do
		if [ -f "$input_file.bch" ]; then
			dd if="$input_file" of="$input_file.bch" bs=1 count=$NAND_PAGE_SIZE skip=$(($NAND_PAGE_SIZE*$step)) seek=$(stat -c %s "$input_file.bch")
		else 
			dd if="$input_file" of="$input_file.bch" bs=1 count=$NAND_PAGE_SIZE skip=$(($NAND_PAGE_SIZE*$step))
		fi 
		
		tr '\000' '\377' < /dev/zero | dd of="$input_file.bch" bs=1 count=$NAND_SPARE_SIZE seek=$(stat -c %s "$input_file.bch")
		step=$(($step+1))
	done
    
    rm -rf "$input_file"
    echo "$input_file.bch done"
}

### brief   generating BCH file.
### Use 16 processes generating it. But still takes much time.
### @param  input original flash file path.
generate_bch()
{
    local input=$1
    local output=$2
    local tmpdir=$(dirname $input)/tmp
	local nand_bch_part_len=$(($(stat -c %s "$input")/$NAND_BCH_PROCESS_NUM))
	#echo ${nand_bch_part_len}
	#echo "tmpdir=${tmpdir}"
    ### make sure the original file can be split 
	[ "$(($nand_bch_part_len%$NAND_PAGE_SIZE))" = "0" ] || \
	{
        echo "bch process num should be aligned to page size!"
        exit 1
    }
    
	local counter=0
	
	rm -rf $tmpdir
	mkdir -p $tmpdir

    ### split the original file into 16 parts and process each part
	while [ "$counter" -lt "$NAND_BCH_PROCESS_NUM" ];do
        dd bs=$NAND_PAGE_SIZE if="$input" of="${tmpdir}/${counter}.part" \
        count=$(($nand_bch_part_len/$NAND_PAGE_SIZE)) skip=$(($nand_bch_part_len*$counter/$NAND_PAGE_SIZE))
		
		bch "${tmpdir}/${counter}.part" &
		
        counter=$(($counter+1))
    done
    
    counter=0
    
    ### wait for all processes OK
    while true; do
        counter=$(find "$tmpdir" -name "*.part" | wc -l)
        if [ "$counter" -le "0" ];then
            break
        fi
        sleep 3
    done
    
    counter=0
    echo -n >"$output"

    ### put all parts together
    while [ "$counter" -lt "$NAND_BCH_PROCESS_NUM" ];do
        dd if="${tmpdir}/${counter}.part.bch" of="$output" \
        bs=$NAND_PAGE_SIZE_ALL count=$(($(stat -c %s "${tmpdir}/${counter}.part.bch")/$NAND_PAGE_SIZE_ALL)) \
        seek=$(($(stat -c %s "$output")/$NAND_PAGE_SIZE_ALL))   
       counter=$(($counter+1))
    done

	rm -rf $tmpdir    
}

generate_bch "$inputfile" "${inputfile}.out"

inlen=$(stat -c %s "$inputfile")
outlen=$(stat -c %s "${inputfile}.out")
if [ $inlen -ge $outlen ]; then
    rm -f "${inputfile}.out"
	echo "ecc out file error"
    return
fi

#mv -f "$inputfile" "${inputfile}.noecc"
mv -f "${inputfile}.out" "$outputfile"
