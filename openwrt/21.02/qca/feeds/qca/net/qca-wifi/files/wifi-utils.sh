#
# Copyright (c) 2023 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.
#

. /lib/functions.sh

get_board_name()
{
	local board_name

	[ -f /tmp/sysinfo/sdk_board_name  ] && {
		board_name=ap$(cat /tmp/sysinfo/sdk_board_name | awk -F 'ap' '{print$2}')
	}
	echo "$board_name"
}

is_cert_enabled()
{
        local enable
        config_load sigma-dut
        config_get_bool enable global enable 0
        echo "$enable"
}

get_wpad_var_run()
{
	local board_name="$(get_board_name)"

	case "$board_name" in
	ap-sdxpinn*)
		# If certification is enabled, default to standard path;
		# default to ujail path, otherwise
		local cert_enabled="$(is_cert_enabled)"
		if [ "${cert_enabled}" -eq 0 ]; then
		        WPAD_VARRUN=/var/run/wpad
		else
			WPAD_VARRUN=/var/run
		fi
	;;
	*)
	        WPAD_VARRUN=/var/run
	;;
	esac

	echo "$WPAD_VARRUN"
}

get_ap_sdxpinn_ko_path()
{
	release=$(uname -r | awk -F '.' '{print $1,$2}' | tr ' ' '.')
	if [ -d /lib/modules/$release-debug ]; then
		echo "/lib/modules/$release-debug/extra"
	elif [ -d /lib/modules/$release-perf ]; then
		echo "/lib/modules/$release-perf/extra"
	else
		echo ""
	fi
}

add_ap_up_boot_kpi_marker()
{
	local device="$1"
	local BOOT_KPI_NODE=/sys/kernel/boot_kpi/kpi_values
	local board_name="$(get_board_name)"

	local kpi_marker

	case "$board_name" in
	ap-sdxpinn*)
	;;
	*)
		return
	;;
	esac

	kpi_marker="M - $device - enable beaconing"

	# Add marker only for the first AP beaconing for a $device
	cat "$BOOT_KPI_NODE" | grep "$kpi_marker" 2>&1 > /dev/null
	if [ $? -eq 0 ]; then
		return
	fi

	echo -n "$kpi_marker" >> $BOOT_KPI_NODE
}

get_ap_sdxpinn_bridge_interface()
{
	local net_cfg="$1"
	local board_name=$(get_board_name)
	local bridge_tmp bridge

	case "$board_name" in
	ap-sdxpinn*)
	;;
	*)
		return
	;;
	esac

	bridge_tmp="$(bridge_interface "$net_cfg")"
	bridge_tmp=$(echo $bridge_tmp | sed 's/\n/ /g')
	bridge=$(echo "$bridge_tmp" | cut -d '{' -f1 | tr -d ' ')
	echo "$bridge"
}

get_radio_hwmode(){
	local hwmodes=$1
	local hwmode=$2

	case "$hwmodes" in
		11bg*)
			case "$hwmode" in
				11bgnaxbe) hwmode="11beg";;
				11bgnax) hwmode="11axg";;
				11bgn) hwmode="11ng";;
			esac
		;;
		11an*)
			case "$hwmode" in
				11anacaxbe) hwmode="11bea";;
				11anacax) hwmode="11axa";;
				11anac) hwmode="11ac";;
				11an) hwmode="11na";;
			esac
		;;
	esac

	echo "$hwmode"
}

get_radio_htmode(){
	local hwmodes=$1
	local htbw=$2
	local channel=$3
	local htmode 
	local device=$4

	[ "auto" = "$channel" ] && channel=0
	[ "AUTO" = "$channel" ] && channel=0

	case "$hwmodes" in
		11bg*)
			case "$htbw" in
				ht20) htmode="HT20";;
				ht40)
					if [ "$channel" = 0 ]; then
						htmode="HT40"
					elif [ "$channel" -lt 7 ]; then
						htmode="HT40+"
					else
						htmode="HT40-"
					fi
				;;
				*)
					if [ "$channel" = 0 ]; then
						htmode="HT40"
					elif [ "$channel" -lt 7 ]; then
						htmode="HT40+"
					else
						htmode="HT40-"
					fi
				;;
			esac
		;;
		11an*)
			case "$htbw" in
				ht20) htmode="HT20";;
				ht40)
					if [ "radio1" = "$device" ]; then	
						case "$channel" in
							36|44|52|60|100|108|116|124|132|140|149|157) htmode="HT40+";;
							40|48|56|64|104|112|120|128|136|144|153|161) htmode="HT40-";;
							*) htmode="HT40";;
						esac
					else
						# to-do
						case "$channel" in
							1|9|17|25|33|41|49|57|65|73|81|89) htmode="HT40+";;
							5|13|21|29|37|45|53|61|69|77|85|93) htmode="HT40-";;
							*) htmode="HT40";;
						esac
					fi
				;;
				ht80) htmode="HT80";;
				ht160) htmode="HT160";;
				ht320) htmode="HT320";;
				*)
					if [ "radio2" = "$device" ]; then
						htmode="HT320"
					elif [ "radio1" = "$device" -a "$channel" -gt 128 ]; then
						htmode="HT80"
					else
						htmode="HT160"
					fi
				;;
			esac

			if [ "radio1" = "$device" -a "$channel" = 165 ]; then
				htmode="HT20"
			elif [ "radio2" = "$device" -a "$channel" = 233 ]; then
				htmode="HT20"
			fi
		;;
	esac

	echo "$htmode"
}
