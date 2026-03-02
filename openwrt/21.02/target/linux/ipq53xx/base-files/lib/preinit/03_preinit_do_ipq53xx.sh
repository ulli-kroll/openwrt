#!/bin/sh
#
# Copyright (c) 2014 The Linux Foundation. All rights reserved.
#
. /lib/ipq53xx.sh

do_ipq53xx() {

	ipq53xx_board_detect
}

boot_hook_add preinit_main do_ipq53xx
