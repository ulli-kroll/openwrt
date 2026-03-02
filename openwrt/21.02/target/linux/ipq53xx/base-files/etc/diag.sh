#!/bin/sh
# Copyright (C) 2010-2013 OpenWrt.org

. /lib/functions/leds.sh
. /lib/ipq53xx.sh

get_status_led() {
	status_led="oem:green:status"
}

set_state() {
	get_status_led $1

	case "$1" in
	preinit)
		status_led_blink_preinit
		;;
	done)
		status_led_on
		;;
	upgrade)
		led_blink_upgrade
		;;
	esac
}
