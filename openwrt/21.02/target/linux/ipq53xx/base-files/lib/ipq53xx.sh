#!/bin/sh
#
# Copyright (c) 2014 The Linux Foundation. All rights reserved.
# Copyright (C) 2011 OpenWrt.org
#

IPQ53xx_BOARD_NAME=
IPQ53xx_MODEL=

oem_boot_version_dump() {
	if [ image_is_nand ];then
		oem_boot_version_nand
	else
		echo "TO do"
	fi
}

oem_boot_version_nand() {
	local mtdname="$(grep -w "0:APPSBL" /proc/mtd | awk -F: '{print $1}')"

	dd if=/dev/$mtdname ibs=1k skip=600 2>/dev/null | dd of=/tmp/sysinfo/oem_boot_version bs=18 skip=0 count=1 2>/dev/null
	cat /tmp/sysinfo/oem_boot_version | grep -qs "OEM_BOOT"
	if [ $? -ne 0 ]; then
		echo "OEM_BOOT 00000000" >/tmp/sysinfo/oem_boot_version
	fi
}

get_oem_boot_version() {
	if [ image_is_nand ];then
		[ -e /tmp/u-boot.bin ] && (dd if=/tmp/u-boot.bin ibs=1k skip=600 2>/dev/null | dd bs=17 skip=0 count=1 2>/dev/null | awk '{print $2}') || echo "00000000"
	else
		echo "To do"
	fi
}

oem_boot_version() {
	cat /tmp/sysinfo/oem_boot_version | awk '{print $2}'
}

ipq53xx_board_detect() {
	[ -z "$IPQ53xx_BOARD_NAME" ] && IPQ53xx_BOARD_NAME="$(strings /proc/device-tree/compatible | head -1)"
	[ -z "$IPQ53xx_MODEL" ] && IPQ53xx_MODEL="$(cat /proc/device-tree/model | awk -F/ 'BEGIN{OFS="/"}{$3=""}1' | sed 's/\/$//')"
	
	local oem_board_name="$(cat /proc/device-tree/model | awk -F/ '{print $3}')"

	[ -e "/tmp/sysinfo/" ] || mkdir -p "/tmp/sysinfo/"

	oem_boot_version_dump
	
	echo "$IPQ53xx_BOARD_NAME" > /tmp/sysinfo/sdk_board_name
	echo "$IPQ53xx_MODEL" > /tmp/sysinfo/model
	echo "$oem_board_name" > /tmp/sysinfo/board_name
}

ipq53xx_board_name() {
	local name

	[ -f /tmp/sysinfo/sdk_board_name ] && name=$(cat /tmp/sysinfo/sdk_board_name)
	[ -z "$name" ] && name="unknown"

	echo "$name"
}

ipq53xx_oem_board_name() {
	local name

	[ -f /tmp/sysinfo/board_name ] && name=$(cat /tmp/sysinfo/board_name)
	[ -z "$name" ] && name="unknown"

	echo "$name"
}
