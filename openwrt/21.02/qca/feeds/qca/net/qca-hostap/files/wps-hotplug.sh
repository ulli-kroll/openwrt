#
# Copyright (c) 2017,2023 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.
#

#
# Copyright (c) 2014, The Linux Foundation. All rights reserved.
#

. /lib/wifi/wifi-utils.sh
. /lib/functions.sh

WPAD_VARRUN="$(get_wpad_var_run)"

dpp_over_wps=0
config_load repacd
config_get dpp_over_wps MAPConfig 'enableDPPOverWPS' '0'

send_to_son()
{
    local staname1
    local staname2
    local ret
    local pattern
    local count
    [ -r /var/run/son.conf ] || return 0
    staname1=$(grep sta /var/run/son.conf | head -n 1 | cut -f3 -d" " )
    staname2=$(grep sta /var/run/son.conf | tail -n 1 | cut -f3 -d" " )
    echo "$staname1 $staname2"
    count=0
    pattern="Not-Associated"
    for sta in $staname1 $staname2;do
    result=$(iwconfig $sta | head -2 | tail -1 | grep -o "Not-Associated")
    if [ $result == $pattern ];then
       count=$((count+1))
    fi
    done
    if [ $count -lt 2 ];then
    echo "Sending to son"
    echo "wps_pbc $SEEN" > /var/run/sonwps.pipe &
    touch /var/run/son_active
    ret=1
    else
    ret=0
    echo "Son not processing push - sta not connected"
    fi
    return $ret
}

run_hostapd_cli_daemon() {
	local ifname=$1
	local ctrl_path=$2
	local arguments=$3
	/etc/init.d/hostapd_cli start "$ifname" "$ctrl_path" "$arguments"
}

wps_pbc_hostapd_config() {
	local iface=$1
	local disabled ifname pid device phy
	config_get_bool disabled "$iface" disabled 0
	[ $disabled = 0 ] || return

	config_get_bool wps_pbc "$iface" wps_pbc 0
	[ "$wps_pbc" -gt 0 ] || return

	config_get ifname "$iface" ifname
	pid=/var/run/hostapd_cli-$ifname.pid

	config_get device "$iface" device
	config_get phy "$device" phy

	run_hostapd_cli_daemon $ifname "/var/run/hostapd-$phy" "-P $pid -a /lib/wifi/wps-hostapd-update-uci -B"
	hostapd_cli -i "$ifname" -p "/var/run/hostapd-$phy" "wps_pbc"
}

if [ "$ACTION" = "pressed" -a "$BUTTON" = "wps" ]; then
	echo "" > /dev/console
	echo "WPS PUSH BUTTON EVENT DETECTED" > /dev/console
	env -i ACTION="wps-start" /sbin/hotplug-call iface
    # cleanup old file
    rm -f /var/run/son_active
    #check for son.conf - if not there exit
    # ret is 1 if push is consumed by son , if ret == 0 continue to next scripts
    send_to_son
    ret=$?
    [ $ret -eq 1 ] && exit 0
    # Son did not process the push  call wps-extender if confgured
    # If multi node wps is enabled then wps-extender will not process the push.
	if [ -r /var/run/wifi-wps-enhc-extn.conf ] &&
		[ ! -r /var/run/son.conf ]; then
		exit 0
    fi

    ifstatus wan | grep up | grep -q true
    if [ "$?" -eq 0 -o x"0" = x"$(uci -q get luci.main.wizard)" ]; then
		killall hostapd_cli 2>/dev/null
		config_load wireless
		config_foreach wps_pbc_hostapd_config wifi-iface
    fi

    # son and wps_extender are not processing the push
	#for dir in $WPAD_VARRUN/hostapd-*; do
    #            [ -d "$dir" ] || continue
    #            for vap_dir in $dir/ath* $dir/wlan*; do
    #                    [ -r "$vap_dir" ] || continue
    #                    nopbn=`cfg80211tool "${vap_dir#"$dir/"}"   get_nopbn  |   cut -d':' -f2`
    #                    if [ $nopbn != 1 ]; then
    #                            if [ $dpp_over_wps -eq 1 ]; then
    #                                    hostapd_cli -i "${vap_dir#"$dir/"}" -p "$dir" SET dpp_wps 1
    #                            fi
    #                            hostapd_cli -i "${vap_dir#"$dir/"}" -p "$dir" wps_pbc
    #                    fi
    #            done
    #    done
fi
