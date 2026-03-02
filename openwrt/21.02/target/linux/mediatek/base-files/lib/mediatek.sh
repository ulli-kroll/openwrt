#!/bin/sh
#
# Copyright (C) 2010-2013 OpenWrt.org
#

MEDIATEK_BOARD_NAME=
MEDIATEK_MODEL=

oem_boot_version_spi() {
	local mtdname="$(grep -w "FIP" /proc/mtd | awk -F: '{print $1}')"

	dd if=/dev/$mtdname ibs=1k skip=511 2>/dev/null | dd of=/tmp/sysinfo/oem_boot_version bs=18 skip=0 count=1 2>/dev/null
	cat /tmp/sysinfo/oem_boot_version | grep -qs "OEM_BOOT"
	if [ $? -ne 0 ]; then
		echo "OEM_BOOT 00000000" >/tmp/sysinfo/oem_boot_version
	fi
}

oem_boot_version_nand() {
	local mtdname="$(grep -w "FIP" /proc/mtd | awk -F: '{print $1}')"

	dd if=/dev/$mtdname ibs=1k skip=2047 2>/dev/null | dd of=/tmp/sysinfo/oem_boot_version bs=18 skip=0 count=1 2>/dev/null
	cat /tmp/sysinfo/oem_boot_version | grep -qs "OEM_BOOT"
	if [ $? -ne 0 ]; then
		echo "OEM_BOOT 00000000" >/tmp/sysinfo/oem_boot_version
	fi
}

oem_boot_version_dump() {
	local board_flash
	board_flash=$(oem_board_flash)

	case "$board_flash" in
	"snand")
		oem_boot_version_nand
		;;
	*)
		oem_boot_version_spi
		;;
	esac
}

get_oem_boot_version() {
	local board_flash
	board_flash=$(oem_board_flash)
	case "$board_flash" in
	"snand")
		dd if="$@" ibs=1k skip=2047 2>/dev/null | dd bs=17 skip=0 count=1 2>/dev/null | awk '{print $2}' || echo "00000000"
		;;
	*)
		dd if="$@" ibs=1k skip=511 2>/dev/null | dd bs=17 skip=0 count=1 2>/dev/null | awk '{print $2}' || echo "00000000"
		;;
	esac	
}

oem_boot_version() {
	cat /tmp/sysinfo/oem_boot_version | awk '{print $2}'
}

oem_board_flash() {
	local name="snor"

	[ -e /dev/ubi0 ] && name="snand"

	echo "$name"
}

mediatek_board_detect() {
	[ -z "$MEDIATEK_BOARD_NAME" ] && MEDIATEK_BOARD_NAME="$(cat /proc/device-tree/model | awk -F/ '{print $1}')"
	[ -z "$MEDIATEK_MODEL" ] && MEDIATEK_MODEL="$(cat /proc/device-tree/model)"
	
	local oem_board_name="$(cat /proc/device-tree/model | awk -F/ '{print $2}')"

	[ -e "/tmp/sysinfo/" ] || mkdir -p "/tmp/sysinfo/"

	oem_boot_version_dump
	
	echo "$MEDIATEK_BOARD_NAME" > /tmp/sysinfo/sdk_board_name
	echo "$MEDIATEK_MODEL" > /tmp/sysinfo/model
	echo "$oem_board_name" > /tmp/sysinfo/board_name
}

mediatek_board_name() {
	local name

	[ -f /tmp/sysinfo/board_name ] && name=$(cat /tmp/sysinfo/board_name)
	[ -z "$name" ] && name="unknown"

	echo "$name"
}
