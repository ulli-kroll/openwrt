#!/bin/sh
#
# Copyright (c) 2023 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.
#

# This script will be used in lowmem profiles for FW_UMOUNT feature.
# This helps mounting back the FW Partition to DDR

. /etc/init.d/wifi_fw_mount
. /lib/functions.sh

is_fw_umount_supported()
{
        local platform=$(grep -o "IPQ.*" /proc/device-tree/model | awk -F/ '{print $1}')

        if [ "$platform" == "IPQ5332" ]; then
                echo "1"
        else
                echo"0"
        fi
}

ini_path="ini/internal/global_i.ini"
fw_umount_enable=`grep "cnss_fw_umount" $ini_path | grep -m1 -v "^[#]" | awk -F'=' '{print $2}'`
supported_arch=$( is_fw_umount_supported )

if [ "$supported_arch" == "1" ] && [ "$fw_umount_enable" == "1" ] && [ -e /sys/firmware/devicetree/base/MP_256 ] ; then
        echo " WIFI FW mount started " > /dev/console 2>&1
        boot
fi
