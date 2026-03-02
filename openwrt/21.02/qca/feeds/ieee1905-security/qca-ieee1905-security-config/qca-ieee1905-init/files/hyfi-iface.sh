WLAN_DEVICES=
WLAN_INCLUDED_DEVICES=
WLAN_MLD_LINK_INTERFACE_DEVICES=
WLAN_VLAN_DEVICES=
WLAN_VLAN_STA_DEVICES=
WLAN_DEVICE_NUM=
ETHER_DEVICES=
PLC_DEVICE=
ALL_DEVICES=
IFACE_VLANIDS=

config_load 'repacd'
config_get dumpInitLog MAPConfig MapLogLevel ''

if [ "$dumpInitLog" = "DUMP" ]; then
	HYFI_DEBUG_OUTPUT=1
fi

. /lib/functions/hyfi-debug.sh

ieee1905managed_found=0
ieee1905managed_bridge=""
ieee1905managed_bridge2=""
hyfi_ieee1905managed_bridges=""

bound_bridge=""
device_wlan=1
backhaul_network="backhaul"
traffic_separation_enabled=0
traffic_separation_active=0
map_traffic_separation_active=0
map_nw_BH="backhaul"
# Network names parsed in hyfi_get_ieee1905_brguest_map()
map_nw_One=""
map_nw_Two=""
map_nw_Three=""
map_version=0

# Fast Onboarding
map_fast_onboarding=0
MLD_VLAN_DEVICES=""
WSPLCD_GLOBAL_FLAG=0
EZMESH_GLOBAL_FLAG=0

# Device List for secondary nws
WLAN_DEVICES_NW_One=""
WLAN_INCLUDED_DEVICES_NW_One=""
WLAN_MLD_LINK_INTERFACE_DEVICES_NW_One=""
WLAN_VLAN_DEVICES_NW_One=""
ETHER_DEVICES_NW_One=""
MLD_VLAN_DEVICES_NW_One=""

WLAN_DEVICES_NW_Two=""
WLAN_INCLUDED_DEVICES_NW_Two=""
WLAN_MLD_LINK_INTERFACE_DEVICES_NW_Two=""
WLAN_VLAN_DEVICES_NW_Two=""
ETHER_DEVICES_NW_Two=""
MLD_VLAN_DEVICES_NW_Two=""

WLAN_DEVICES_NW_Three=""
WLAN_INCLUDED_DEVICES_NW_Three=""
WLAN_MLD_LINK_INTERFACE_DEVICES_NW_Three=""
WLAN_VLAN_DEVICES_NW_Three=""
ETHER_DEVICES_NW_Three=""
MLD_VLAN_DEVICES_NW_Three=""

WLAN_DEVICES_NW_BH=""
WLAN_INCLUDED_DEVICES_NW_BH=""
WLAN_MLD_LINK_INTERFACE_DEVICES_NW_BH=""
WLAN_VLAN_DEVICES_NW_BH=""
ETHER_DEVICES_NW_BH=""
MLD_VLAN_DEVICES_NW_BH=""

iface_config=
mobile_config_xml="/etc/data/mobileap_cfg.xml"
wlan_config_xml="/etc/data/wlan_cfg.xml"
board_name=""
[ -f /tmp/sysinfo/sdk_board_name  ] && {
    board_name=ap$(cat /tmp/sysinfo/sdk_board_name | awk -F 'ap' '{print$2}')
}

build_name=""
[ -f /version.txt ] && {
    build_name=$(cat /version.txt | grep imagename | cut -d "-" -f4)
    board_name=ap$(cat /tmp/sysinfo/sdk_board_name | awk -F 'ap' '{print$2}')
}

#From openwrt v23.05 onwards, needs different handling for network config
openwrt_version=''
openwrt_version_check=0
openwrt_version=`cat /etc/openwrt_release | grep 'RELEASE' | grep -Eo '[+-]?[0-9]+([.][0-9]+)?'`
openwrt_version_check=$(awk -v a="$openwrt_version" -v b="23.05" 'BEGIN{print(a>=b)}')

config_load 'repacd'
config_get_bool ezmesh repacd 'Ezmesh' '0'
    if [ "$ezmesh" -eq 1 ]; then
        MAP='ezmesh'
    else
        MAP='hyd'
    fi

config_get_bool enable_yocto_build MAPConfig 'EnableYoctoBuild' '0'
    if [ "$enable_yocto_build" -eq 1 ]; then
        brprefix=''
        primarybridgename='bridge0'
    elif [ "$build_name" = "image_rdkb" ]; then
        brprefix=''
        primarybridgename='brlan0'
    else
        brprefix='br-'
        primarybridgename='lan'
    fi

config_get_bool multi_inst_enable repacd 'multi_inst_enable' '0'
config_get_bool auto_config_enable repacd 'auto_config_enable' '0'
brcount=0
__hyfi_get_wlan_vifnum() {
	local config="$1"
	local iface network disabled
	local phydev phydisabled

	config_get network "$config" network
	config_get disabled "$config" disabled '0'
	config_get phydev "$config" device ""

	if [ -z "$phydev" ]; then
		return
	fi

	config_get phydisabled ${phydev} disabled 0
	if [ $phydisabled -eq 0 -a "$2" = "$network" -a "$disabled" -eq 0 ]; then
		WLAN_DEVICE_NUM=$((WLAN_DEVICE_NUM + 1))
	fi
}

# hyfi_get_wlan_vifnum()
# input: $1 IEEE1905.1 managed bridge interface
# output: $2 number of WLAN interfaces bound to the bridge
hyfi_get_wlan_vifnum() {
	local ieee1905managed="$1"

	WLAN_DEVICE_NUM=0
	config_load wireless
	config_foreach __hyfi_get_wlan_vifnum wifi-iface $ieee1905managed

	eval "$2='${WLAN_DEVICE_NUM}'"
}

__hyfi_get_iface_vlanids() {
	local config="$1"
	local loc_iface loc_vid

	config_get loc_iface "$config" ifname
	config_get loc_vid "$config" vid

	if [ "$loc_iface" -a "$loc_vid"  ]; then
		IFACE_VLANIDS="${IFACE_VLANIDS}${IFACE_VLANIDS:+","}${loc_iface}.${loc_vid}"
	fi
}

hyfi_get_iface_vlanids() {
	IFACE_VLANIDS=""

	config_load $MAP
        config_foreach __hyfi_get_iface_vlanids Vlanid

	eval "$1='${IFACE_VLANIDS}'"
}

__hyfi_is_iface_up() {
	local iface="$1"
	bitRate=$(cfg80211tool $iface get_maxrate | awk -F ":" '{print $2}' | cut -c1-1)
	accessPoint=$(iw dev $iface info | grep ssid | awk -F " " '{print $2}')

	hyfi_debug hyiface "iface=$iface, bitRate=$bitRate, accessPoint=$accessPoint"
	if [ "$bitRate" -eq 0 ] || [ -z "$accessPoint" ]; then
		return 0
	fi

	return 1
}

__hyfi_get_wlan_ifaces() {
	local config="$1"
	local iface network disabled device
	local ignore_bstavap isMldExist

	config_load 'wireless'
	if [ "$build_name" = "image_rdkb" ]; then
		if [ "$2" = "$ieee1905managed" ]; then
			alias_rdkb=$primarybridgename
		else
			alias_rdkb=`syscfg get network.$2.alias`
		fi
	fi

	config_get network "$config" network
	if [ "$build_name" = "image_rdkb" ]; then
		if [ "$network" != "$alias_rdkb" ] && [ "$network" != "backhaul" ]; then
			return
		fi
	elif [ "$network" != "$2" ] && [ "$network" != "backhaul" ]; then
		hyfi_debug hyiface "debug: returning network=$network, arg=$2"
		return
	fi

	config_get iface "$config" ifname
	config_get disabled "$config" disabled '0'
	config_get device "$config" device
	config_get mapEnable "$config" map
	config_get mode "$config" mode
	config_get mld "$config" mld
	#Will be 1 if MBsta_Onboarding=1 & in EthernetBH
	config_get ignore_bstavap "$config" ignore_bstavap '0'

	local band=$(cfg80211tool $device g_oper_reg_inf | awk -F "," '{print $2}' | awk -F "=" '{print $2}' | cut -c1-4)
	hyfi_debug hyiface "iface=$iface, band=$band, network=$network, arg=$2, disabled=$disabled, mapEnable=$mapEnable, mode=$mode, mld=$mld, ignore_bstavap=$ignore_bstavap"

	if [ "$mode" = "sta" ] && [ "$ignore_bstavap" -eq 1 ]; then
		hyfi_debug hyiface "SKIPPING IFACE $iface from MANAGEDINTF"
		return
	fi
	config_load 'repacd'
	config_get_bool ezmesh repacd 'Ezmesh' '0'
	config_get_bool map_6g_enable repacd 'Ezmesh6GEnable' '0'

	if [ -n "$mapEnable" ]; then
		config_load $MAP
		config_get sigmaEnabled MultiAP 'EnableSigmaDUT' '0'

		# Ignore if sigma flag is enabled
		if [ -n "$iface" -a "$mapEnable" -gt 0 -a "$disabled" -eq 0 \
				-a "$mode" = "sta" -a "$sigmaEnabled" -eq 0 ]; then
			__hyfi_is_iface_up $iface
			is_iface_up=$?
			if [ "$is_iface_up" -eq 0 ]; then
				hyfi_debug hyiface "debug: returning is_iface_up=$is_iface_up"
				return
			fi
		fi
	fi

	if [ -n "$iface" -a "$disabled" -eq 0 ]; then
		if [ "$network" = "$2" ] || [ "$build_name" = "image_rdkb" -a "$network" = "$alias_rdkb" ]; then
			if [ "$ezmesh" -eq 1 ] ; then 
				if [ "$band" -lt 5950 ] || [ "$map_6g_enable" -eq 1 ]; then
					if [ -z "$mld" ]; then
						WLAN_DEVICES="${WLAN_DEVICES}${WLAN_DEVICES:+","}${iface}@${device}&${network}:WLAN"
						hyfi_debug hyiface "wlan_device1=${WLAN_DEVICES}"
					else
						hyfi_debug hyiface "WSPLCD_GLOBAL_FLAG=$WSPLCD_GLOBAL_FLAG, EZMESH_GLOBAL_FLAG=$EZMESH_GLOBAL_FLAG"
						if [ "$WSPLCD_GLOBAL_FLAG" -eq 0 ] && [ "$EZMESH_GLOBAL_FLAG" -eq 0 ]; then
							isMldExist=$(echo $WLAN_DEVICES | grep -w $mld)
							hyfi_debug hyiface "isMldExist=$isMldExist"
							if [ -z "$isMldExist" ]; then
								WLAN_DEVICES="${WLAN_DEVICES}${WLAN_DEVICES:+","}${mld}&${network}:MLD"
								hyfi_debug hyiface "wlan_device2=${WLAN_DEVICES}"
							fi
						else
							WLAN_DEVICES="${WLAN_DEVICES}${WLAN_DEVICES:+","}${iface}@${device}&${network}:WLAN"
							hyfi_debug hyiface "wlan_device3=${WLAN_DEVICES}"
						fi
					fi
				fi
			else
			    WLAN_DEVICES="${WLAN_DEVICES}${WLAN_DEVICES:+","}${iface}@${device}&${network}:WLAN"
			    hyfi_debug hyiface "wlan_device4=${WLAN_DEVICES}"
			fi
		fi

		if [ "$build_name" = "image_rdkb" ]; then
			if [ "$network" = "$alias_rdkb" -o "$network" = "backhaul" ]; then
				WLAN_INCLUDED_DEVICES="${WLAN_INCLUDED_DEVICES}${WLAN_INCLUDED_DEVICES:+","}${device}:${iface}"
			fi
		# include network=backhaul for guest network support
		elif [ "$network" = "$2" -o "$network" = "backhaul" ]; then
			WLAN_INCLUDED_DEVICES="${WLAN_INCLUDED_DEVICES}${WLAN_INCLUDED_DEVICES:+","}${device}:${iface}"
		fi
	fi
	hyfi_debug hyiface "wlan_device=${WLAN_DEVICES}"
}

__hyfi_get_mld_link_ifaces() {
	local config="$1"
	local iface network disabled
	local interface
	local device ignore_bstavap

	config_get iface "$config" ifname
	config_get network "$config" network
	config_get disabled "$config" disabled '0'
	config_get device "$config" device
	config_get mld "$config" mld
	#Will be 1 if MBsta_Onboarding=1 & in EthernetBH
	config_get ignore_bstavap "$config" ignore_bstavap '0'

	hyfi_debug hyiface "__hyfi_get_mld_link_ifaces, iface=$iface, disabled=$disabled, ignore_bstavap=$ignore_bstavap, mld=$mld"
	if [ -n "$iface" -a "$disabled" -eq 0 ] && [ "$ignore_bstavap" -eq 0 ]; then
		if [ -n "$mld" ]; then
			WLAN_MLD_LINK_INTERFACE_DEVICES="${WLAN_MLD_LINK_INTERFACE_DEVICES}${WLAN_MLD_LINK_INTERFACE_DEVICES:+","}${iface}#${device}@${mld}&${network}:MLD"
			hyfi_debug hyiface "$WLAN_MLD_LINK_INTERFACE_DEVICES"
		fi
	fi
}

# hyfi_get_wlan_ifaces()
# input: $1 IEEE1905.1 managed bridge interface
# output: $2 List of all WLAN interfaces bound to the bridge
# output: $3 List of all WLAN devices/radios bound to the bridge
hyfi_get_wlan_ifaces() {
	local ieee1905managed="$1"

	WLAN_DEVICES=""
	WLAN_INCLUDED_DEVICES=""
	WLAN_MLD_LINK_INTERFACE_DEVICES=""

	hyfi_network_sync
	config_load wireless
	config_foreach __hyfi_get_wlan_ifaces wifi-iface $ieee1905managed
	if [ "$WSPLCD_GLOBAL_FLAG" -eq 0 ]; then
		config_load wireless
		config_foreach __hyfi_get_mld_link_ifaces wifi-iface $ieee1905managed
	fi

	eval "$2='${WLAN_DEVICES}'"
	eval "$3='${WLAN_INCLUDED_DEVICES}'"
	eval "$4='${WLAN_MLD_LINK_INTERFACE_DEVICES}'"

	if [ "$map_fast_onboarding" -eq 1 ]; then
		for i in One Two Three BH; do
			local nw=map_nw_$i
			local map_nw=$(eval echo \$$nw)
			WLAN_DEVICES=''
			WLAN_INCLUDED_DEVICES=''
			WLAN_MLD_LINK_INTERFACE_DEVICES=''
			eval WLAN_DEVICES_NW_$i=''
			eval WLAN_INCLUDED_DEVICES_NW_$i=''
			eval WLAN_MLD_LINK_INTERFACE_DEVICES_NW_$i=''

			if [ -n "$map_nw" ]; then
				config_load wireless
				config_foreach __hyfi_get_wlan_ifaces wifi-iface $map_nw
				if [ "$WSPLCD_GLOBAL_FLAG" -eq 0 ]; then
					config_load wireless
					config_foreach __hyfi_get_mld_link_ifaces wifi-iface $map_nw
				fi

				if [ "$i" = "One" ]; then
					WLAN_DEVICES_NW_One=$WLAN_DEVICES
					WLAN_INCLUDED_DEVICES_NW_One=$WLAN_INCLUDED_DEVICES
					WLAN_MLD_LINK_INTERFACE_DEVICES_NW_One=$WLAN_MLD_LINK_INTERFACE_DEVICES
				fi
				if [ "$i" = "Two" ]; then
					WLAN_DEVICES_NW_Two=$WLAN_DEVICES
					WLAN_INCLUDED_DEVICES_NW_Two=$WLAN_INCLUDED_DEVICES
					WLAN_MLD_LINK_INTERFACE_DEVICES_NW_Two=$WLAN_MLD_LINK_INTERFACE_DEVICES
				fi
				if [ "$i" = "Three" ]; then
					WLAN_DEVICES_NW_Three=$WLAN_DEVICES
					WLAN_INCLUDED_DEVICES_NW_Three=$WLAN_INCLUDED_DEVICES
					WLAN_MLD_LINK_INTERFACE_DEVICES_NW_Three=$WLAN_MLD_LINK_INTERFACE_DEVICES
				fi
				if [ "$i" = "BH" ]; then
					WLAN_DEVICES_NW_BH=$WLAN_DEVICES
					WLAN_INCLUDED_DEVICES_NW_BH=$WLAN_INCLUDED_DEVICES
					WLAN_MLD_LINK_INTERFACE_DEVICES_NW_BH=$WLAN_MLD_LINK_INTERFACE_DEVICES
				fi
			fi
		done
	fi
}

# hyfi_get_ft_ifaces(()
# will check if FT is enabled & get
# the list of FT interfaces.
# Add 256 hex key as well.
__hyfi_get_ft_ifaces() {
    local config="$1"
    local iface network disabled device

    config_get iface "$config" ifname
    config_get network "$config" network
    config_get nasid "$config" nasid
    config_get ft_auth "$config" ft_auth
    config_get r1_key_holder "$config" r1_key_holder
    config_get kh_key_hex "$config" kh_key_hex
    config_load 'repacd'
    config_get ftauthbridge repacd 'FTAuthBridge' "$primarybridgename"
    config_get_bool ftauth repacd 'FTAuth' '0'
    config_get_bool map_ftauth MAPConfig 'EnableFTAuth' '0'
        if [ "$ftauthbridge" = "$network" ] && [ "$ft_auth" = 1 ] && [ -n "$kh_key_hex" ] ; then
            FT_INCLUDED_DEVICES="${FT_INCLUDED_DEVICES}${FT_INCLUDED_DEVICES:+","}${iface}"
            R1_KEY="$r1_key_holder"
            R1_256_HEX="$kh_key_hex"
        fi

        if [ "$map_ftauth" = 1 ] && [ -n "$kh_key_hex" ] ; then
            FT_INCLUDED_DEVICES="${FT_INCLUDED_DEVICES}${FT_INCLUDED_DEVICES:+","}${iface}"
            R1_KEY="$r1_key_holder"
            R1_256_HEX="$kh_key_hex"
        fi

        if [ "$ftauth" = 1 ] && [ "$ft_auth" = 0 ] && [ "$ftauthbridge" = "$network" ] ; then
            uci delete "wireless.${config}.r0kh"
            uci delete "wireless.${config}.r1kh"
            uci_set wireless "$config" 'ieee80211r' '0'
            uci commit
            rm /tmp/FT*
        fi
}

hyfi_get_ft_ifaces() {
    local ieee1905managed="$1"

    FT_INCLUDED_DEVICES=""
    R1_KEY=""
    R1_256_HEX=""
    hyfi_network_sync
    config_load wireless
    config_foreach __hyfi_get_ft_ifaces wifi-iface $ieee1905managed

    eval "$2='${FT_INCLUDED_DEVICES}'"
    eval "$3='${R1_KEY}'"
    eval "$4='${R1_256_HEX}'"
}

__hyfi_get_switch_iface() {
	local loc_switch_iface loc_eswitch_support="0"
	local ref_design
	local loc_switch_num="0"
	local loc_switch_present="1"

	config_load hyd
	config_get loc_switch_iface config SwitchInterface ""

	if [ "$loc_switch_iface" = "auto" ]; then
		ref_design=ap$(cat /tmp/sysinfo/sdk_board_name | awk -F 'ap' '{print$2}')

		# List of supported reference designs. For other designs
		# either add to cases, or setup SwitchInterface.
		case "$ref_design" in
		ap145|ap147|ap148*|db149|ap151)
		# S17c switch
		# ap148_1xx is also a supported board type
			loc_switch_iface="eth1"
			loc_eswitch_support="1"
			;;
		ap160|ap161)
		# S17c switch support is disabled even though interface is identified
			loc_switch_iface="eth1"
			loc_eswitch_support="0"
			;;
		ap152|apjet01)
			loc_switch_iface="eth0"
			loc_eswitch_support="0"
			;;
		ap-dk*)
		# Malibu switch with a single host port connected in a VLAN
		# tagged manner. Since the switch config indicates the host
		# port is tagged (as it is from the switch's perspective) but
		# the host interfaces are not VLAN tagged, a special option
		# is needed to force the interface names to be untagged.
			loc_switch_iface="eth1"
			force_untagged_iface=1
			loc_eswitch_support="0"
			;;
		ap-mp03.1*)
			loc_switch_iface="eth1"
			force_untagged_iface=1
			loc_eswitch_support="1"
			loc_switch_num="1"
			echo 1 > /sys/ssdk/dev_id
			;;
		ap-mp02.1*|ap-mp03.3*|ap-mp03.4*|ap-mp03.5*|ap-mp03.6*)
			loc_switch_iface=""
			loc_switch_present="0"
			;;
		ap135)
			loc_switch_iface="eth1"
			loc_eswitch_support="0"
			;;
		*)
			loc_switch_iface=""
			;;
		esac
	fi

	if [ -z "$loc_switch_iface" ]; then
		eval "$1=''"
		eval "$2='$loc_eswitch_support'"
		eval "$3='$loc_switch_num'"
		eval "$4='$loc_switch_present'"
		return
	fi

	local loc_switch_cpu_port
	__hyfi_get_switch_cpu_port loc_switch_cpu_port

	local lan_vid
	__hyfi_get_switch_lan_vid lan_vid

	if [ -z "$switch_cpu_port_tagged" -o -n "$force_untagged_iface" ]; then
		eval "$1='$loc_switch_iface'"
	else
		eval "$1='${loc_switch_iface}.${lan_vid}'"
	fi

	eval "$2='$loc_eswitch_support'"
	eval "$3='$loc_switch_num'"
	eval "$4='$loc_switch_present'"
}

__hyfi_get_switch_lan_vid() {
	local loc_lan_vid

	config_load hyd
	config_get loc_lan_vid config SwitchLanVid ""

	eval "$1='$loc_lan_vid'"
}

__hyfi_get_switch_cpu_port_iterate() {
	config_get vlan "$1" "vlan"
	config_get ports "$1" "ports"

	if [ "${vlan}" = "$2" ]; then
		switch_cpu_port=`echo ${ports} |sed 's/t//g' |cut -f 1 -d " "`
		switch_cpu_port_tagged=`echo ${ports} |grep t`
	fi
}

__hyfi_get_switch_cpu_port() {
	local lan_vid
	__hyfi_get_switch_lan_vid lan_vid

	config_load network
	config_foreach __hyfi_get_switch_cpu_port_iterate switch_vlan $lan_vid

	eval "$1='$switch_cpu_port'"
}

# hyfi_network_add_device_port()
# input: $1 Device section name
# input: $2 Network name
# input: $3 Interface name
hyfi_network_add_device_port() {
    local config="$1"
    local nw_interface="$2"
    local nw_interface_device=''
    local device_name=''
    local iface="$3"
    local list_ports=''

    config_get interface_name $config name
    device_name=$(uci get "network.$config.name" 2&> /dev/null)
    config_get device_name "$config" name
    nw_interface_device=$(uci get "network.$nw_interface.device" 2&> /dev/null)

    if [ "$device_name" == "$nw_interface_device" ]; then
        config_get list_ports $config ports
        search_iface=$(echo $iface | sed 's/\./\\\./g')
        echo $list_ports | grep -E "\b$search_iface\b" -c
        if [ $? -ne 0 ]; then
            uci add_list network.$config.ports="$iface"
        fi
    fi
}

# hyfi_network_delete_device_port()
# input: $1 Device section name
# input: $2 Network name
# input: $3 Interface name
hyfi_network_delete_device_port() {
    local config="$1"
    local nw_interface="$2"
    local device_name=''
    local iface="$3"
    local nw_interface_device=''

    config_get device_name "$config" name
    nw_interface_device=$(uci get "network.$nw_interface.device" 2&> /dev/null)
    if [ "$device_name" == "$nw_interface_device" ]; then
        uci del_list network.$config.ports="$iface"
    fi

    uci commit network
}

# __hyfi_network_get_device_config()
# input: $1 IEEE1905.1 managed bridge interface
# input: $2 Device of managed bridge interface
# output: $3 Device section name
__hyfi_network_get_device_config() {
    local config="$1"
    local interface_device="$2"
    local device_name=''
    config_get device_name "$config" name

    if [ "$device_name" == "$interface_device" ]; then
        eval "$3=$config"
        return
    fi
}

# hyfi_network_get_device_config()
# input: $1 IEEE1905.1 managed bridge interface
# output: $2 Device section name
hyfi_network_get_device_config() {
    local config="$1"
    local interface_device
    local ifname_config

    config_get interface_device "$config" device

    config_load network
    config_foreach __hyfi_network_get_device_config device $interface_device ifname_config
    eval "$2=$ifname_config"
}

# hyfi_network_get_ifnames()
# input: $1 IEEE1905.1 managed bridge interface
# output: Print the list of all interfaces bound to the bridge
hyfi_network_get_ifnames() {
    local config="$1"
    local ifnames=''

    if [ "$openwrt_version_check" -eq 1 ]; then
        local ifnames_config
        hyfi_network_get_device_config "$config" ifnames_config
        ifnames=`uci get network.$ifnames_config.ports 2&> /dev/null`
    else
        ifnames=`uci get network.$config.ifname 2&> /dev/null`
    fi
    echo "$ifnames"
    return
}

__hyfi_get_all_device_ifaces() {
    local config="$1"
    local ifnames
    local ifaceType
    local i ignore_bstavap

    config_get mode "$config" mode
    config_get iface "$config" ifname
	#Will be 1 if MBsta_Onboarding=1 & in EthernetBH
    config_get ignore_bstavap "$config" ignore_bstavap '0'


    local switch_iface eswitch_support switch_num switch_present
    __hyfi_get_switch_iface switch_iface eswitch_support switch_num switch_present

    if [ "$map_fast_onboarding" -eq 1 ]; then
        if [ "$config" = "loopback" ] || [ "$config" = "wan" ] || [ "$config" = "wan6" ]; then
            return
        fi

        for i in $primarybridgename One Two Three; do
            local nw=map_nw_$i
            local map_nw=$(eval echo \$$nw)
	    ### Temp WAR as primarybridgename is hardcoded as brlan0 in ezmesh.init
            if [ "$i" = "$primarybridgename" ] && [ "$build_name" == "image_rdkb" ]; then
	         map_nw="lan"
	    elif  [ "$i" = "$primarybridgename" ]; then
                 map_nw=$ieee1905managed_bridge
            fi

            ifnames=''
            if [ -n "$map_nw" ] && [ "$map_nw" = "$config" ]; then
                eval ETHER_DEVICES_NW_$i=''
                eval WLAN_VLAN_DEVICES_NW_$i=''
                eval MLD_VLAN_DEVICES_NW_$i=''
		if [ "$build_name" == "image_rdkb" ]; then
			ifnames="`syscfg get network.$map_nw.ifname`"
		else
			ifnames="$(hyfi_network_get_ifnames $map_nw)"
		fi
                for iface in $ifnames; do
                    ### Map lan to brlan in syscfg for rdkb
                    if [ "$build_name" = "image_rdkb" ]; then
                        config=`syscfg get network.$map_nw.alias`
                    fi

                    if [ "$map_version" -gt 0 ]; then
                        ifaceType=$(echo $iface | cut -c 1-3)
                        # Dont add ath interface as ETHER device
                        if [ "$ifaceType" = "ath" ]; then
                            if [ "$traffic_separation_enabled" -gt 0 ] && \
                               [ "$traffic_separation_active" -gt 0 ] || \
                               [ "$map_traffic_separation_active" -gt 0 ] && [ "$ignore_bstavap" -eq 0 ]; then
                                if __hyfi_is_vlan_iface $iface; then
                                    local varName=WLAN_VLAN_DEVICES_NW_$i
                                    local varVal=$(eval echo \$$varName)
                                    local radioIdx=$(echo $iface | cut -c 4)
                                    if [ "$build_name" = "image_rdkb" ]; then
					config=`syscfg get network.$map_nw.alias`
					config_load wireless
					config_get dev "device" 0
					radioIdx=`echo $dev | grep -o '[0-9]'`
                                    fi

                                    if [ "$i" = "$primarybridgename" ]; then
                                        varName=WLAN_VLAN_DEVICES
                                        varVal=$(eval echo \$$varName)
                                        WLAN_VLAN_DEVICES="${varVal}${varVal:+","}${iface}@wifi$radioIdx&${config}:WLAN_VLAN"
                                    fi
                                    if [ "$i" = "One" ]; then
                                        WLAN_VLAN_DEVICES_NW_One="${varVal}${varVal:+","}${iface}@wifi$radioIdx&${config}:WLAN_VLAN"
                                    fi
                                    if [ "$i" = "Two" ]; then
                                        WLAN_VLAN_DEVICES_NW_Two="${varVal}${varVal:+","}${iface}@wifi$radioIdx&${config}:WLAN_VLAN"
                                    fi
                                    if [ "$i" = "Three" ]; then
                                        WLAN_VLAN_DEVICES_NW_Three="${varVal}${varVal:+","}${iface}@wifi$radioIdx&${config}:WLAN_VLAN"
                                    fi
                                fi
                            fi
                            continue
                        elif [ "$ifaceType" = "mld" ]; then
                            if [ "$EZMESH_GLOBAL_FLAG" -eq 0 ]; then
                                if [ "$traffic_separation_enabled" -gt 0 ] && \
                                   [ "$traffic_separation_active" -gt 0 ] || \
                                   [ "$map_traffic_separation_active" -gt 0 ] && [ "$ignore_bstavap" -eq 0 ]; then
                                    if __hyfi_is_vlan_iface $iface; then
                                        local varName=MLD_VLAN_DEVICES_NW_$i
                                        local varVal=$(eval echo \$$varName)
                                        if [ "$i" = "$primarybridgename" ]; then
                                            varName=MLD_VLAN_DEVICES
                                            varVal=$(eval echo \$$varName)
                                            MLD_VLAN_DEVICES="${varVal}${varVal:+","}${iface}&${config}:MLD_VLAN"
                                        fi
                                        if [ "$i" = "One" ]; then
                                            MLD_VLAN_DEVICES_NW_One="${varVal}${varVal:+","}${iface}&${config}:MLD_VLAN"
                                        fi
                                        if [ "$i" = "Two" ]; then
                                            MLD_VLAN_DEVICES_NW_Two="${varVal}${varVal:+","}${iface}&${config}:MLD_VLAN"
                                        fi
                                        if [ "$i" = "Three" ]; then
                                            MLD_VLAN_DEVICES_NW_Three="${varVal}${varVal:+","}${iface}&${config}:MLD_VLAN"
                                        fi
                                    fi
                                fi
                            fi
                            continue
                        fi
                    fi

                    local varName=ETHER_DEVICES_NW_$i
                    local varVal=$(eval echo \$$varName)
                    if [ "$iface" = "$switch_iface" -a "$eswitch_support" = "1" ]; then
                        if [ "$i" = "$primarybridgename" ]; then
                            varName=ETHER_DEVICES
                            varVal=$(eval echo \$$varName)
                            ETHER_DEVICES="${varVal}${varVal:+","}${iface}&${config}:ESWITCH"
                        fi
                        if [ "$i" = "One" ]; then
                            ETHER_DEVICES_NW_One="${varVal}${varVal:+","}${iface}&${config}:ESWITCH"
                        fi
                        if [ "$i" = "Two" ]; then
                            ETHER_DEVICES_NW_Two="${varVal}${varVal:+","}${iface}&${config}:ESWITCH"
                        fi
                        if [ "$i" = "Three" ]; then
                            ETHER_DEVICES_NW_Three="${varVal}${varVal:+","}${iface}&${config}:ESWITCH"
                        fi
                    else
                        if [ "$i" = "$primarybridgename" ]; then
                            varName=ETHER_DEVICES
                            varVal=$(eval echo \$$varName)
                            ETHER_DEVICES="${varVal}${varVal:+","}${iface}&${config}:ETHER"
                        fi
                        if [ "$i" = "One" ]; then
                            ETHER_DEVICES_NW_One="${varVal}${varVal:+","}${iface}&${config}:ETHER"
                        fi
                        if [ "$i" = "Two" ]; then
                            ETHER_DEVICES_NW_Two="${varVal}${varVal:+","}${iface}&${config}:ETHER"
                        fi
                        if [ "$i" = "Three" ]; then
                            ETHER_DEVICES_NW_Three="${varVal}${varVal:+","}${iface}&${config}:ETHER"
                        fi
                    fi
                done
            fi
        done
    fi
}

__hyfi_get_ether_ifaces_rdkb() {
        local config="$2"
        local ethiface bridgeifacelist bridgeethiface
        bridgeifacelist="$(brctl show | grep -i eth | awk '{print $1}' | awk '{printf "%s ",$0}')"

        for bridgeethiface in $bridgeifacelist; do
            ETHER_DEVICES="${ETHER_DEVICES}${ETHER_DEVICES:+","}${bridgeethiface}&${config}:ETHER"
        done
}

__hyfi_get_ether_ifaces_sdx() {
	local config="$2"
	local ifacelist lanif ethiface bridgeifacelist bridgeethiface
	ifacelist="$(grep -w 'EthIfaceType\|EthIfaceName' "$mobile_config_xml" | awk '{print $1}' | cut -d ">" -f 2 | awk -F"<" '{print $1}' | awk '{printf "%s ",$0}')"
	bridgeifacelist="$(brctl show | grep -i eth | awk '{print $1}' | awk '{printf "%s ",$0}')"

	if [ -z "$ifacelist" ]; then
		hyfi_debug hyiface "Interface List not populated correctly from config xml"
	else
		for ethiface in $ifacelist; do
			if [ "$ethiface" = "0" ]; then
				if [ -z "$bridgeifacelist" ]; then
					hyfi_debug hyiface "No eth iface attached to bridge"
				else
					for bridgeethiface in $bridgeifacelist; do
						if [ "$lanif" = "$bridgeethiface" ]; then
							ETHER_DEVICES="${ETHER_DEVICES}${ETHER_DEVICES:+","}${lanif}&${config}:ETHER"
						fi
					done
				fi
			else
				lanif=$ethiface
			fi
		done
	fi
}

__hyfi_get_ether_ifaces() {
	local config="$1"
	local ifnames network plciface bridge_name dev
	local ifaceType

	#config_get ifnames "$config" device
	ifnames="$(hyfi_network_get_ifnames $config)"

	config_get bridge_name "$config" ifname
	config_load plc
	config_get plciface config PlcIfname


	local switch_iface eswitch_support switch_num switch_present
	__hyfi_get_switch_iface switch_iface eswitch_support switch_num switch_present

	if [ "$2" = "$config" ]; then
		# Check the ifnames parameter is populated correctly
		if [ -z "$ifnames" ]; then
			# If the ifnames parameter doesn't contain the device list, it should
			# be present in the bridge_name.  However, it's possible a race
			# condition occured, and neither parameter contains the device list.
			# Check the device list again if the bridge_name actually contains the
			# name of the bridge.
			if echo $bridge_name | grep -q $brprefix; then
				hyfi_debug hyiface "Missing device names, and ifname matches bridge, fetching device names again"
				config_get ifnames "$config" device
				if [ -z "$ifnames" ]; then
					ifnames=`uci get "network.$1.ifname" 2&> /dev/null`
				    if [ "$auto_config_enable" -eq 1 -a "$multi_inst_enable" -eq 1 ]; then
					hyfi_debug hyiface "Device name: not yet configured for $1"
				    else
					hyfi_error hyiface "Device names missing, defaulting to $ifnames"
				    fi
				else
					hyfi_debug hyiface "Device names now populated correctly"
				fi
			else
				ifnames=$bridge_name
				hyfi_debug hyiface "Missing device names, so using ifname parameter instead"
			fi
		fi

		for iface in $ifnames; do
			if [ "$map_version" -gt 0 ]; then
				ifaceType=$(echo $iface | cut -c 1-3)
				# Dont add ath interface as ETHER device
				if [ "$ifaceType" = "ath" ]; then
					continue
				fi
				if [ "$ifaceType" = "mld" ]; then
					continue
				fi
			fi

			# Not required for MAP
			if [ "$map_version" -eq 0 ]; then
				[ "$iface" = "$plciface" ] && continue
				if [ "$traffic_separation_enabled" -gt 0 ] && \
					[ "$traffic_separation_active" -gt 0 ]; then
						if __hyfi_is_vlan_iface $iface; then
							if __hyfi_is_device_wlan $iface dev; then
								continue
							fi
						fi
				fi
			fi

			if [ "$iface" = "$switch_iface" -a "$eswitch_support" = "1" ]; then
				ETHER_DEVICES="${ETHER_DEVICES}${ETHER_DEVICES:+","}${iface}&${config}:ESWITCH"
			else
				ETHER_DEVICES="${ETHER_DEVICES}${ETHER_DEVICES:+","}${iface}&${config}:ETHER"
			fi
		done
	fi
}

# hyfi_get_ether_ifaces()
# input: $1 IEEE1905.1 managed bridge interface
# output: $2 List of all Ethernet interfaces bound to the bridge
hyfi_get_ether_ifaces() {
	local ieee1905managed="$1"

	ETHER_DEVICES=""
	hyfi_network_sync

	config_load repacd
	config_get traffic_separation_enabled repacd TrafficSeparationEnabled '0'
	config_get traffic_separation_active repacd TrafficSeparationActive '0'
	config_get backhaul_network repacd NetworkBackhaul 'backhaul'
	config_get map_nw_BH MAPConfig VlanNetworkBackHaul 'backhaul'
	config_get map_traffic_separation_active MAPConfig 'MapTrafficSeparationActive' '0'
	config_get map_version MAPConfig 'MapVersionEnabled' '0'

	config_load network
	case "$board_name" in
		ap-sdx*)
			__hyfi_get_ether_ifaces_sdx interface $ieee1905managed
			;;
		*)
			if [ "$build_name" = "image_rdkb" ]; then
				__hyfi_get_ether_ifaces_rdkb interface $ieee1905managed
			else
				config_foreach __hyfi_get_ether_ifaces interface $ieee1905managed
			fi
			;;
	esac

	eval "$2='${ETHER_DEVICES}'"
}

__hyfi_is_vlan_iface() {
	local iface="$1"

	echo "$iface" | grep '\.' >/dev/null 2>&1
	if [ "$?" -eq "0" ]; then
		return 0
	fi

	return 1
}

__hyfi_iterate_wlan_ifaces() {
	local config="$1"
	local iface network disabled
	local interface
	local device

	config_get iface "$config" ifname
	config_get network "$config" network
	config_get disabled "$config" disabled '0'
	config_get device "$config" device
	config_get mld "$config" mld

	if [ -n "$iface" -a "$backhaul_network" = "$network" -a "$disabled" -eq 0 ]; then
		interface=`echo "$2" | cut -d '.' -f1`
		if [ "$interface" = "$iface" ] || [ "$interface" = "$mld"  ]; then
			eval "$3='0'"
			eval "$4=$config"
			eval "$5=$device"
		fi
	fi

	if [ "$map_traffic_separation_active" -gt 0 ]; then
		if [ -n "$iface" -a "$disabled" -eq 0 ]; then
			if [ "$network" = "$map_nw_BH" ] || \
				[ "$network" = "$map_nw_One" ] || \
					[ "$network" = "$map_nw_Two" ] || \
						[ "$network" = "$map_nw_Three" ] || \
							[ "$network" = "$primarybridgename" ]; then
							interface=`echo "$2" | cut -d '.' -f1`
							if [ "$interface" = "$iface" ] || [ "$interface" = "$mld"  ]; then
								eval "$3='0'"
								eval "$4=$config"
								eval "$5=$device"
							fi
			fi
		fi
	fi
}

__hyfi_is_device_wlan() {
	local iface="$1"
	device_wlan=1
	local wdev

	if [ -n "$backhaul_network" ] || [ -n "$map_nw_BH" ]; then
		config_load wireless
		config_foreach __hyfi_iterate_wlan_ifaces wifi-iface $iface device_wlan iface_config wdev
	fi
	eval "$2=$wdev"
	return $device_wlan
}

__hyfi_get_wlan_vlan_ifaces() {
	local config="$1"
	local ifnames bridge_name
	local wdevice ignore_bstavap

	config_get ifnames "$config" device
	config_get bridge_name "$config" ifname
	config_get mode "$config" mode
	config_get iface "$config" ifname
	#Will be 1 if MBsta_Onboarding=1 & in EthernetBH
	config_get ignore_bstavap "$config" ignore_bstavap '0'

	if [ "$mode" = "sta" ] && [ "$ignore_bstavap" -eq 1 ]; then
	    return
	fi
	if [ "$2" = "$config" ]; then
                #initially VLAN interfaces are added using vconfig and brctl tool
                #to avoid multiple restarts.VLAN interfaces added this way are not
                #detected by config_get, so using direct command here.
		ifnames="$(hyfi_network_get_ifnames $config)"

		for iface in $ifnames; do
			if __hyfi_is_vlan_iface $iface; then
				if __hyfi_is_device_wlan $iface wdevice; then
					ifacetype=$(echo $iface | cut -c 1-3)
					if [ "$ifacetype" = "mld" ]; then
						WLAN_VLAN_DEVICES="${WLAN_VLAN_DEVICES}${WLAN_VLAN_DEVICES:+","}${iface}&${config}:MLD_VLAN"
					else
						WLAN_VLAN_DEVICES="${WLAN_VLAN_DEVICES}${WLAN_VLAN_DEVICES:+","}${iface}@${wdevice}&${config}:WLAN_VLAN"
					fi
				fi
			fi
		done
	fi
}

__hyfi_get_wlan_vlan_ifaces_sdx() {
	local config="$1"
	local iface network disabled device mode MapBSSType vlanidlist bridgeinfo valnidval ifacename iterval iterval2 bridgelist ethifacelist ethiface updated

	config_load wireless
	config_get iface "$config" ifname
	config_get device "$config" device
	config_get disabled "$config" disabled '0'
	config_get mode "$config" mode
	config_get MapBSSType "$config" MapBSSType '0'
	config_get network "$config" network

	vlanidlist="$(grep -w 'VlanID' "$wlan_config_xml" | awk '{print $1}' | cut -d ">" -f 2 | awk -F"<" '{print $1}' | awk '{printf "%s ",$0}')"
	if [ -z iface ] && [ $((MapBSSType & 0x40)) -eq 64 ] || [ $((MapBSSType & 0x48)) -eq 72 ] || [ $((MapBSSType & 0x60)) -eq 96 ]; then
		config_load repacd
		for iterval in Primary One Two Three; do
			config_get bridgeinfo MAPConfig "VlanNetwork"$iterval '0'
			if [ "$bridgeinfo" = "$2"  ];then
				config_get vlanidval MAPConfig "VlanIDNw"$iterval '0'
					for iterval2 in $vlanidlist; do
						if [ "$vlanidval" = "$iterval2" ]; then
							ifacename=""$iface"."$vlanidval""
							if __hyfi_is_vlan_iface $ifacename; then
								if __hyfi_is_device_wlan $ifacename wdevice; then
									WLAN_VLAN_DEVICES="${WLAN_VLAN_DEVICES}${WLAN_VLAN_DEVICES:+","}${ifacename}@${wdevice}&${2}:WLAN_VLAN"
								fi
							fi
						fi
					done
			fi
		done
	fi
}

__hyfi_get_wlan_vlan_sta_ifaces() {
	local config="$1"
	local ifnames bridge_name dev
	local mode

	config_get ifnames "$config" device
	config_get bridge_name "$config" ifname

	if [ "$2" = "$config" ]; then
		ifnames="$(hyfi_network_get_ifnames $config)"

		for iface in $ifnames; do
			if __hyfi_is_vlan_iface $iface; then
				if __hyfi_is_device_wlan $iface dev; then
					config_load wireless
					config_get mode "$iface_config" mode
					if [ "$mode" = "sta" ]; then
						WLAN_VLAN_STA_DEVICES="${WLAN_VLAN_STA_DEVICES}${WLAN_VLAN_STA_DEVICES:+","}${iface}"
					fi
				fi
			fi
		done
	fi
}

__hyfi_get_eth_vlan_ifaces_sdx() {
	local config="$1"
	vlanidlist="$(grep -w 'VlanID' "$wlan_config_xml" | awk '{print $1}' | cut -d ">" -f 2 | awk -F"<" '{print $1}' | awk '{printf "%s ",$0}')"
	ifacelist="$(grep -w 'EthIfaceType\|EthIfaceName' "$mobile_config_xml" | awk '{print $1}' | cut -d ">" -f 2 | awk -F"<" '{print $1}' | awk '{printf "%s ",$0}')"

	if [ -z "$ifacelist" ] || [ "$1" = "backhaul" ]; then
		return 0
	else
		bridgeifacelist="$(brctl show $1 | grep -i eth | awk '{print $1}' | awk '{printf "%s ",$0}')"
		for ethiface in $ifacelist; do
			if [ "$ethiface" = "0" ]; then
				if [ -z "$bridgeifacelist" ]; then
					return 0
				else
					for bridgeethiface in $bridgeifacelist; do
						if [ "$lanif" = "$bridgeethiface" ]; then
							ethifacelist="${ethifacelist} $lanif"
						else
							for vlanval in $vlanidlist; do
								if [ "$lanif.$vlanval" = "$bridgeethiface" ]; then
									ethifacelist="${ethifacelist} $lanif.$vlanval"
								fi
							done
						fi
					done
				fi
			else
				lanif=$ethiface
			fi
		done
	fi

	for iterval in Primary One Two Three; do
		config_get bridgeinfo MAPConfig "VlanNetwork"$iterval '0'
		if [ "$bridgeinfo" = "$1"  ];then
			for addiface in $ethifacelist; do
				WLAN_VLAN_DEVICES="${WLAN_VLAN_DEVICES}${WLAN_VLAN_DEVICES:+","}${addiface}&${1}:ETHER"
			done
		fi
	done
}

hyfi_get_wlan_vlan_ifaces() {
	local ieee1905managed="$1"

	WLAN_VLAN_DEVICES=""
	hyfi_network_sync

	config_load repacd
	config_get traffic_separation_enabled repacd TrafficSeparationEnabled '0'
	config_get traffic_separation_active repacd TrafficSeparationActive '0'
	config_get backhaul_network repacd NetworkBackhaul 'backhaul'
	config_get map_nw_BH MAPConfig VlanNetworkBackHaul 'backhaul'
	config_get map_traffic_separation_active MAPConfig 'MapTrafficSeparationActive' '0'
	if [ "$traffic_separation_enabled" -gt 0 ] && \
		[ "$traffic_separation_active" -gt 0 ] || \
			[ "$map_traffic_separation_active" -gt 0 ]; then
		config_load network
		config_foreach __hyfi_get_wlan_vlan_ifaces interface $ieee1905managed
	fi

	eval "$2='${WLAN_VLAN_DEVICES}'"

}

hyfi_get_wlan_vlan_sta_ifaces() {
	local ieee1905managed="$1"

	WLAN_VLAN_STA_DEVICES=""
	hyfi_network_sync

	config_load repacd
	config_get traffic_separation_enabled repacd TrafficSeparationEnabled '0'
	config_get traffic_separation_active repacd TrafficSeparationActive '0'
	config_get backhaul_network repacd NetworkBackhaul 'backhaul'
	config_get map_nw_BH MAPConfig VlanNetworkBackHaul 'backhaul'
	config_get map_traffic_separation_active MAPConfig 'MapTrafficSeparationActive' '0'
	if [ "$traffic_separation_enabled" -gt 0 ] && \
		[ "$traffic_separation_active" -gt 0 ] || \
			[ "$map_traffic_separation_active" -gt 0 ]; then
		config_load network
		config_foreach __hyfi_get_wlan_vlan_sta_ifaces interface $ieee1905managed
	fi

	eval "$2='${WLAN_VLAN_STA_DEVICES}'"

}

__hyfi_get_plc_iface() {
	if [ "$map_version" -ge 1 ]; then
		return
	fi

	local plciface iface bridge_name
	local ieee1905managed="$1"

	config_load plc
	config_get plciface config PlcIfname

	[ -z "$plciface" ] && return

    config_get bridge_name "$ieee1905managed" ifname
	config_load network
	config_get ifnames $ieee1905managed device

    # Check the ifnames parameter is populated correctly
    if [ -z "$ifnames" ]; then
        # If the ifnames parameter doesn't contain the device list, it should
        # be present in the bridge_name.  However, it's possible a race
        # condition occured, and neither parameter contains the device list.
        # Check the device list again if the bridge_name actually contains the
        # name of the bridge.
        if echo $bridge_name | grep -q $brprefix; then
            hyfi_debug hyiface "Missing device names, and ifname matches bridge, fetching device names again"
            config_get ifnames "$ieee1905managed" device
            if [ -z "$ifnames" ]; then
                ifnames=`uci get "network.$1.ifname" 2&> /dev/null`
                hyfi_error hyiface "Device names missing, defaulting to $ifnames"
            else
                hyfi_debug hyiface "Device names now populated correctly"
            fi
        else
            ifnames=$bridge_name
            hyfi_debug hyiface "Missing device names, so using ifname parameter instead"
        fi
    fi

	for iface in $ifnames; do
		if [ "$iface" = "$plciface" ]; then
			PLC_DEVICE="${plciface}&${ieee1905managed}:PLC"
			return
		fi
	done
}

# hyfi_get_plc_iface()
# input: $1 IEEE1905.1 managed bridge interface
# output: $2 PLC interface bound to the bridge
hyfi_get_plc_iface() {
	local ieee1905managed="$1"

	PLC_DEVICE=""
	hyfi_network_sync

	__hyfi_get_plc_iface $ieee1905managed
	eval "$2='${PLC_DEVICE}'"
}

hyfi_get_ifaces_fast_onboarding() {
	local nw_name="$1"
	ALL_DEVICES=""

	if [ "$map_fast_onboarding" -eq 1 ]; then
		if [ -n "$map_nw_One" ] && [ "$nw_name" = "$map_nw_One" ]; then
			ALL_DEVICES=$WLAN_DEVICES_NW_One
			if [ -n "$ETHER_DEVICES_NW_One" ]; then
				[ -z "$ALL_DEVICES" ] || ALL_DEVICES="${ALL_DEVICES},"
				ALL_DEVICES="${ALL_DEVICES}${ETHER_DEVICES_NW_One}"
			fi
			if [ -n "$WLAN_VLAN_DEVICES_NW_One" ]; then
				[ -z "$ALL_DEVICES" ] || ALL_DEVICES="${ALL_DEVICES},"
				ALL_DEVICES="${ALL_DEVICES}${WLAN_VLAN_DEVICES_NW_One}"
			fi
			if [ -n "$MLD_VLAN_DEVICES_NW_One" ]; then
				[ -z "$ALL_DEVICES" ] || ALL_DEVICES="${ALL_DEVICES},"
				ALL_DEVICES="${ALL_DEVICES}${MLD_VLAN_DEVICES_NW_One}"
			fi
			eval "$2='${ALL_DEVICES}'"
			eval "$3='${WLAN_INCLUDED_DEVICES_NW_One}'"
			eval "$4='${WLAN_MLD_LINK_INTERFACE_DEVICES_NW_One}'"
		fi

		if [ -n "$map_nw_Two" ] && [ "$nw_name" = "$map_nw_Two" ]; then
			ALL_DEVICES=$WLAN_DEVICES_NW_Two
			if [ -n "$ETHER_DEVICES_NW_Two" ]; then
				[ -z "$ALL_DEVICES" ] || ALL_DEVICES="${ALL_DEVICES},"
				ALL_DEVICES="${ALL_DEVICES}${ETHER_DEVICES_NW_Two}"
			fi
			if [ -n "$WLAN_VLAN_DEVICES_NW_Two" ]; then
				[ -z "$ALL_DEVICES" ] || ALL_DEVICES="${ALL_DEVICES},"
				ALL_DEVICES="${ALL_DEVICES}${WLAN_VLAN_DEVICES_NW_Two}"
			fi
			if [ -n "$MLD_VLAN_DEVICES_NW_Two" ]; then
				[ -z "$ALL_DEVICES" ] || ALL_DEVICES="${ALL_DEVICES},"
				ALL_DEVICES="${ALL_DEVICES}${MLD_VLAN_DEVICES_NW_Two}"
			fi
			eval "$2='${ALL_DEVICES}'"
			eval "$3='${WLAN_INCLUDED_DEVICES_NW_Two}'"
			eval "$4='${WLAN_MLD_LINK_INTERFACE_DEVICES_NW_Two}'"
		fi

		if [ -n "$map_nw_Three" ] && [ "$nw_name" = "$map_nw_Three" ]; then
			ALL_DEVICES=$WLAN_DEVICES_NW_Three
			if [ -n "$ETHER_DEVICES_NW_Three" ]; then
				[ -z "$ALL_DEVICES" ] || ALL_DEVICES="${ALL_DEVICES},"
				ALL_DEVICES="${ALL_DEVICES}${ETHER_DEVICES_NW_Three}"
			fi
			if [ -n "$WLAN_VLAN_DEVICES_NW_Three" ]; then
				[ -z "$ALL_DEVICES" ] || ALL_DEVICES="${ALL_DEVICES},"
				ALL_DEVICES="${ALL_DEVICES}${WLAN_VLAN_DEVICES_NW_Three}"
			fi
			if [ -n "$MLD_VLAN_DEVICES_NW_Three" ]; then
				[ -z "$ALL_DEVICES" ] || ALL_DEVICES="${ALL_DEVICES},"
				ALL_DEVICES="${ALL_DEVICES}${MLD_VLAN_DEVICES_NW_Three}"
			fi
			eval "$2='${ALL_DEVICES}'"
			eval "$3='${WLAN_INCLUDED_DEVICES_NW_Three}'"
			eval "$4='${WLAN_MLD_LINK_INTERFACE_DEVICES_NW_Three}'"
		fi

		if [ -n "$map_nw_BH" ] && [ "$nw_name" = "$map_nw_BH" ]; then
			ALL_DEVICES=$WLAN_DEVICES_NW_BH
			if [ -n "$ETHER_DEVICES_NW_BH" ]; then
				[ -z "$ALL_DEVICES" ] || ALL_DEVICES="${ALL_DEVICES},"
				ALL_DEVICES="${ALL_DEVICES}${ETHER_DEVICES_NW_BH}"
			fi
			if [ -n "$WLAN_VLAN_DEVICES_NW_BH" ]; then
				[ -z "$ALL_DEVICES" ] || ALL_DEVICES="${ALL_DEVICES},"
				ALL_DEVICES="${ALL_DEVICES}${WLAN_VLAN_DEVICES_NW_BH}"
			fi
			eval "$2='${ALL_DEVICES}'"
			eval "$3='${WLAN_INCLUDED_DEVICES_NW_BH}'"
			eval "$4='${WLAN_MLD_LINK_INTERFACE_DEVICES_NW_BH}'"
		fi
	fi
}

# hyfi_get_ifaces()
# input: $1 IEEE1905.1 managed bridge interface
# output: $2 List of ALL interface bound to the bridge
# output: $3 List of ALL WLAN devices/radios bound to the bridge
hyfi_get_ifaces() {
	local ieee1905managed="$1"

	WLAN_DEVICES=""
	WLAN_INCLUDED_DEVICES=""
	WLAN_MLD_LINK_INTERFACE_DEVICES=""
	WLAN_VLAN_DEVICES=""
	ETHER_DEVICES=""
	PLC_DEVICE=""

	config_load repacd
	config_get traffic_separation_enabled repacd TrafficSeparationEnabled '0'
	config_get traffic_separation_active repacd TrafficSeparationActive '0'
	config_get backhaul_network repacd NetworkBackhaul 'backhaul'
	config_get map_traffic_separation_active MAPConfig 'MapTrafficSeparationActive' '0'
	config_get map_nw_BH MAPConfig VlanNetworkBackHaul 'backhaul'
	config_get map_version MAPConfig 'MapVersionEnabled' '0'
	config_get map_fast_onboarding MAPConfig 'MapFastOnboarding' '0'

	if [ "$map_fast_onboarding" -eq 1 ]; then
		config_load network
		if [ "$build_name" = "image_rdkb" ]; then
			for i in `syscfg get network.alias` ; do
				__hyfi_get_all_device_ifaces $i
			done
		else
			config_foreach __hyfi_get_all_device_ifaces interface
		fi

		# Get all WLAN interfaces bound to the managed bridge
		hyfi_get_wlan_ifaces $ieee1905managed wlan_ifaces wlan_included_ifaces wlan_mld_included_ifaces
		WLAN_DEVICES=$wlan_ifaces
		WLAN_INCLUDED_DEVICES=$wlan_included_ifaces
		WLAN_MLD_LINK_INTERFACE_DEVICES=$wlan_mld_included_ifaces
	else
		hyfi_network_sync

		config_load network
		case "$board_name" in
			ap-sdx*)
				__hyfi_get_ether_ifaces_sdx interface $ieee1905managed
				;;
			*)
				if [ "$build_name" = "image_rdkb" ]; then
					__hyfi_get_ether_ifaces_rdkb interface $ieee1905managed
				else
					config_foreach __hyfi_get_ether_ifaces interface $ieee1905managed
				fi
				;;
		esac

		config_load wireless
		config_foreach __hyfi_get_wlan_ifaces wifi-iface $ieee1905managed
		config_load wireless
		config_foreach __hyfi_get_mld_link_ifaces wifi-iface $ieee1905managed

		if [ "$traffic_separation_enabled" -gt 0 ] && \
			[ "$traffic_separation_active" -gt 0 ] || \
				[ "$map_traffic_separation_active" -gt 0 ]; then
			case "$board_name" in
				ap-sdx*)
					config_load wireless
					config_foreach __hyfi_get_wlan_vlan_ifaces_sdx wifi-iface $ieee1905managed
					__hyfi_get_eth_vlan_ifaces_sdx $ieee1905managed
					;;
				*)
					if [ "$build_name" = "image_rdkb" ]; then
						__hyfi_get_ether_ifaces_rdkb interface $ieee1905managed
					else
						config_load network
						config_foreach __hyfi_get_wlan_vlan_ifaces interface $ieee1905managed
					fi
					;;
			esac
		fi

		__hyfi_get_plc_iface $ieee1905managed
	fi

	ALL_DEVICES=$WLAN_DEVICES
	if [ -n "$ETHER_DEVICES" ]; then
		[ -z "$ALL_DEVICES" ] || ALL_DEVICES="${ALL_DEVICES},"
		ALL_DEVICES="${ALL_DEVICES}${ETHER_DEVICES}"
	fi
	if [ -n "$PLC_DEVICE" ]; then
		[ -z "$ALL_DEVICES" ] || ALL_DEVICES="${ALL_DEVICES},"
		ALL_DEVICES="${ALL_DEVICES}${PLC_DEVICE}"
	fi
	if [ -n "$WLAN_VLAN_DEVICES" ]; then
		[ -z "$ALL_DEVICES" ] || ALL_DEVICES="${ALL_DEVICES},"
		ALL_DEVICES="${ALL_DEVICES}${WLAN_VLAN_DEVICES}"
	fi
	if [ -n "$MLD_VLAN_DEVICES" -a "$EZMESH_GLOBAL_FLAG" -eq 0 ]; then
		[ -z "$ALL_DEVICES" ] || ALL_DEVICES="${ALL_DEVICES},"
		ALL_DEVICES="${ALL_DEVICES}${MLD_VLAN_DEVICES}"
	fi
	eval "$2='${ALL_DEVICES}'"
	eval "$3='${WLAN_INCLUDED_DEVICES}'"
	eval "$4='${WLAN_MLD_LINK_INTERFACE_DEVICES}'"
}


# __hyfi_get_device_type()
# input: $1 IEEE1905.1 managed bridge interface
# input: $2 Device of managed bridge interface
# output: $3 Device type
__hyfi_get_device_type() {
    local config="$1"
    local interface_device="$2"
    local device_name=''
    local device_type=''

    config_get device_name "$config" name
    config_get device_type "$config" type

    if [ "$device_name" == "$interface_device" ]; then
        eval "$3=$device_type"
        return
    fi
}

# hyfi_network_get_device_type()
# input: $1 IEEE1905.1 managed bridge interface
# output: Print the Device type
hyfi_network_get_device_type() {
    local config="$1"
    local dev_type

    if [ "$openwrt_version_check" -eq 1 ]; then
        local interface_device
        config_get interface_device "$config" device
        config_load network
        config_foreach __hyfi_get_device_type device $interface_device dev_type
    else
        config_get dev_type "$config" type
    fi
    eval "$2=$dev_type"
}

__hyfi_iterate_networks() {
        local config="$1"
        local type ieee1905managed

        hyfi_network_get_device_type $config type

        [ -z "$type" -o ! "$type" = "bridge" ] && return

        config_get_bool ieee1905managed "$config" ieee1905managed

        [ -z "$ieee1905managed" ] && return

        if [ "$ieee1905managed" -eq "1" ]; then
                ieee1905managed_found=1
                if [ -n "$ieee1905managed_bridge" ]
                then
                        ieee1905managed_bridge2="$config"
                else
                        ieee1905managed_bridge="$config"
                fi
        fi
}

__hyfi_iterate_networks2() {
	local config="$1"
	local my_iface="$2"
	local ifnames iface type

	[ -n "$bound_bridge" ] && return

	hyfi_network_get_device_type $config type

	[ -z "$type" -o ! "$type" = "bridge" ] && return

	ifnames="$(hyfi_network_get_ifnames $config)"

	for iface in $ifnames; do
		if [ "$iface" = "$my_iface" ]; then
			bound_bridge=$brprefix$config
			return
		fi
	done
}

# hyfi_get_ieee1905_managed_iface()
# output: $1 IEEE1905.1 managed bridge interface
# output: $2 2nd IEEE1905.1 managed bridge interface
# output: $3 3rd IEEE1905.1 managed bridge interface
# Note: If no entry exists, the function will set the "lan"
# or "bridge0" interface as the default managed bridge
# based on the build info.
hyfi_get_ieee1905_managed_iface() {
        ieee1905managed_found=0
        ieee1905managed_bridge=""
        ieee1905managed_bridge2=""

        config_load network
        config_foreach __hyfi_iterate_networks interface
        eval "$1='$ieee1905managed_bridge'"
        eval "$2='$ieee1905managed_bridge2'"
        [ "$ieee1905managed_found" -eq "1" ] && return

        ieee1905managed_bridge="$primarybridgename"
        if [ "$enable_yocto_build" -ne 1 ]; then
            uci_set network $ieee1905managed_bridge ieee1905managed 1
            uci_commit network
        fi

        config_load network
        __hyfi_iterate_networks $ieee1905managed_bridge

        eval "$1='$ieee1905managed_bridge'"
}

# hyfi_strip_list
# input: $1 list of interfaces with attached type
# output: $2 same list with type stripped
hyfi_strip_list() {
	eval "$2='`echo $1 | sed 's/@[a-zA-Z0-9]*//g' | sed 's/:[A-Z]*,/ /g' | sed 's/:[A-Z]*//g' | sed 's/&[a-zA-Z0-9]*//g'`'"
}

# hyfi_get_bridge_from_iface()
# input: $1 interface name
# output: $2 bridge the interface is bound to
hyfi_get_bridge_from_iface() {
	bound_bridge=""
	local iface="$1"

	config_load network
	config_foreach __hyfi_iterate_networks2 interface $iface

	eval "$2='$bound_bridge'"
}

# hyfi_get_ieee1905_brguest_map()
# output: $1 IEEE1905.1 secondary network names
# output: $2 IEEE1905.1 backhaul network name
hyfi_get_ieee1905_brguest_map() {
	nw_names=""
	config_load repacd
	config_get num_vlan_supported MAPConfig 'NumberOfVLANSupported' '0'

	# Exclude lan from this list
	num_vlan_supported=$((num_vlan_supported-1))

	for i in One Two Three; do
		if [ "$num_vlan_supported" -eq 0 ]; then
			break
		fi

		if [ "$build_name" = "image_rdkb" ]; then
			config_load repacd
		fi
		config_get nw MAPConfig "VlanNetwork"$i '0'
		if [ "$enable_yocto_build" -eq 0 ]; then

			if [ "$build_name" = "image_rdkb" ]; then
				config_load network
			fi
			local type
			hyfi_network_get_device_type $nw type
			[ -z "$type" -o ! "$type" = "bridge" ] && return
		fi

		if [ -n "$nw_names" ]; then
			nw_names="$nw_names $nw"
		else
			nw_names="$nw"
		fi

		eval map_nw_$i=$nw
		num_vlan_supported=$((num_vlan_supported-1))
	done

	if [ "$build_name" = "image_rdkb" ]; then
		config_load repacd
	fi
	# Get BackHaul Network Name
	config_get nw_bh MAPConfig VlanNetworkBackHaul '0'

	eval "$1='$nw_names'"
	eval "$2='$nw_bh'"
}

hyfi_bring_vlan_iface_up() {
    local nw=$1 ifaces athiface vlanid vlan_iface
    local ifname vlan_athname vid vap_state

    ifaces=$(uci get network.$nw.ifname 2&> /dev/null)
    for ifname in $ifaces; do
        athiface=$(echo $ifname | grep ath)
        [ -z "$athiface" ] && continue
        vlan_iface=$(echo $ifname | cut -d "." -f 1)
        vlanid=$(echo $ifname | cut -d "." -f 2)
        athiface=$(echo $ifname | grep $vlan_iface | grep $vlanid)
        if [ -n "$athiface" ]; then
            vap_state=$(ifconfig -a $ifname 2&> /dev/null)
            if [ -z "$vap_state" ]; then
                vconfig add $vlan_iface $vlanid
                brctl addif "$brprefix$nw" $ifname
                ifconfig "$ifname" up
            fi
        fi
    done
}
