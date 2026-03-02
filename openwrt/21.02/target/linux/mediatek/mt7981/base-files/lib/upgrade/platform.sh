RAMFS_COPY_BIN='mkfs.f2fs blkid blockdev fw_printenv fw_setenv dmsetup /usr/bin/dumpimage'
RAMFS_COPY_DATA="/etc/fw_env.config /var/lock/fw_printenv.lock"

PART_NAME=firmware
BOOT_NAME=FIP

nor_do_upgrade() {
	sync
	echo 3 > /proc/sys/vm/drop_caches

	local dd_count=256
	local dd_skip=257

	#sysupagrade.bin: [bootloader(512KB) + sig(256B) + (2KB-256B) + ( firmware(kernel_fit_image + rootfs)+ metadata )]
    if [ ! -f "/tmp/OEM_NOT_UPGRADE_BOOT" ]; then
		local img_boot_ver="$(get_oem_boot_version "$1")"
		local local_boot_ver="$(oem_boot_version)"
		if [[ "$img_boot_ver" -gt "$local_boot_ver" ]]; then
			echo "upgrade u-boot" > /dev/console
			get_image "$1" | dd bs=2k count="$dd_count" conv=sync 2>/dev/null | mtd write - "${BOOT_NAME:-u-boot}"
		fi
	fi

	if [ -n "$UPGRADE_BACKUP" ]; then
		echo "upgrade firmware with UPGRADE_BACKUP" > /dev/console
		if [ ! -f "/tmp/OEM_NOT_UPGRADE_BOOT" ]; then
			get_image "$1" "$2" | dd bs=2k skip="$dd_skip" conv=sync 2>/dev/null | mtd $MTD_ARGS $MTD_CONFIG_ARGS -j "$UPGRADE_BACKUP" write - "${PART_NAME:-image}"
		else
			get_image "$1" "$2" | mtd $MTD_ARGS $MTD_CONFIG_ARGS -j "$UPGRADE_BACKUP" write - "${PART_NAME:-image}"
		fi
	else
		echo "upgrade firmware without UPGRADE_BACKUP" > /dev/console
		if [ ! -f "/tmp/OEM_NOT_UPGRADE_BOOT" ]; then
			get_image "$1" "$2" | dd bs=2k skip="$dd_skip" conv=sync 2>/dev/null | mtd $MTD_ARGS write - "${PART_NAME:-image}"
		else
			get_image "$1" "$2" | mtd $MTD_ARGS write - "${PART_NAME:-image}"
		fi
	fi

	[ $? -ne 0 ] && exit 1
}

image_demux()
{
	sync
	echo 3 > /proc/sys/vm/drop_caches
	
	local img=$1
	local dd_skip=1025
	local dd_count=1024
	
	#sysupagrade.bin: [bootloader(2MB) + sig(256 byte) + (2KB-256B) + ( FIT{ubi(kernel_fit_image+rootfs)}+ metadata )]
	dd bs=2k if="$img" count="$dd_count" of=/tmp/uboot-sysupgrade.bin conv=sync 2>/dev/null
	dd bs=2k if="$img" skip="$dd_skip" of=/tmp/fit-ubi-sysupgrade.bin conv=sync 2>/dev/null
	rm -rf "$img"
	sync
	echo 3 > /proc/sys/vm/drop_caches

	dumpimage -T flat_dt -p 0 -o /tmp/ubi-sysupgrade.bin /tmp/fit-ubi-sysupgrade.bin > /dev/null || {
		echo "Error while extracting ubi from ${img}"
		rm /tmp/fit-ubi-sysupgrade.bin
		return 1
	}

	ln -s /tmp/ubi-sysupgrade.bin "$img" 
	rm /tmp/fit-ubi-sysupgrade.bin
	sync
	echo 3 > /proc/sys/vm/drop_caches
	return 0
}

nand_do_upgrade_uboot() {
	local img_boot_ver="$(get_oem_boot_version "$1")"
	local local_boot_ver="$(oem_boot_version)"
	local dd_count=1024

	#sysupagrade.bin: [bootloader(2MB) + sig(256 byte) + (2KB-256B) + ( FIT{ubi(kernel_fit_image+rootfs)}+ metadata )]
	if [[ "$img_boot_ver" -gt "$local_boot_ver" ]]; then
		echo "upgrade u-boot" > /dev/console
		get_image "$1" | dd bs=2k count="$dd_count" conv=sync 2>/dev/null | mtd write - "${BOOT_NAME:-u-boot}"
	fi
}

platform_do_upgrade() {
	local board=$(mediatek_board_name)
	local board_flash=$(oem_board_flash)

	case "$board_flash" in
	*snand*)
		if [ ! -f "/tmp/OEM_NOT_UPGRADE_BOOT" ]; then
			nand_do_upgrade_uboot "/tmp/uboot-sysupgrade.bin"
			ubi_do_upgrade "/tmp/ubi-sysupgrade.bin"
		else
			mv "$1" "/tmp/ubi-sysupgrade.bin"
			ubi_do_upgrade "/tmp/ubi-sysupgrade.bin"
		fi
		;;
	*emmc*)
		mtk_mmc_do_upgrade "$1"
		;;
	*snor*)
		nor_do_upgrade "$1"
		;;
	*)
		default_do_upgrade "$1"
		;;
	esac
}

platform_check_image() {
	local board=$(mediatek_board_name)
	local board_flash=$(oem_board_flash)
	local magic="$(get_magic_long "$1")"

	[ "$#" -gt 1 ] && return 1

	case "$board_flash" in
	*snand*)
		[ -L $1 ] || {
		#FIP size is 0x200000
		oem-check -b "$board" -o 0x200000 -f "$1"
		[ "$?" -eq 0 ] || return 1
		
		image_demux $1 || {\
			echo "Error: \"$1\" couldn't be extracted. Abort..."
			return 1
		}
		}

		return 0
		;;
	*emmc*)
		# tar magic `ustar`
		magic="$(dd if="$1" bs=1 skip=257 count=5 2>/dev/null)"

		[ "$magic" != "ustar" ] && {
			echo "Invalid image type."
			return 1
		}

		return 0
		;;
	*snor*)
		#FIP size is 0x80000
		oem-check -b "$board" -o 0x80000 -f "$1"

		[ "$?" -eq 0 ] && return 0

		return 1
		;;
	*)
		[ "$magic" != "d00dfeed" ] && {
			echo "Invalid image type."
			return 1
		}

		return 0
		;;
	esac

	return 0
}

blink_led() {
	. /etc/diag.sh; set_state upgrade
}

append sysupgrade_pre_upgrade blink_led
