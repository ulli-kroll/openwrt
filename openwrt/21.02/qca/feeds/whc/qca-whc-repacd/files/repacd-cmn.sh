#!/bin/sh /etc/rc.common
# Copyright (c) 2015, 2017-2019, 2021 Qualcomm Technologies, Inc.
#
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# 2015 Qualcomm Atheros, Inc.
#
# All Rights Reserved.
# Qualcomm Atheros Confidential and Proprietary.


REPACD_DEBUG_OUTOUT=0

WSPLCD_MAP_DIR='/etc/wsplcd/map'
WSPLCD_MAP_BSS_POLICY_PATH="${WSPLCD_MAP_DIR}/bss-policy.conf"
WSPLCD_MAP_TEMPLATE_DIR="${WSPLCD_MAP_DIR}/templates"
SON_VERSION="son-10.0.0.0"
MAP_VERSION="easymesh-12.5.2.0"
RSTP_ROOTBRIDGE_PRIO=4096
RSTP_DEFBRIDGE_PRIO=32768

managed_network='' config_changed=0 sta_config_changed=0
config_re_mode='' resolved_re_mode='' resolved_re_submode=''
wsplcd_enabled=0 wsplcd_start=0 wsplcd_stop=0 wsplcd_restart=0
hyd_start=0 hyd_stop=0
rstp_enabled=0

network_guest=
traffic_separation_enabled=''
daisy_chain=0
bssid_resolve_state="resolving"
net_config_changed=0

map_version=0 map_ts_enabled=0 num_vlan_supported=0 map_enable_backhaul_WPS=0
map_single_r1r2_bh=0

#SLO Parameters
enable_slo=0
mldIndex=0
mldAddrInc=0
sta_mld=''
configured_mld_count=0
intf_up_delay_cnt=0

WSPLCD_INIT='/etc/init.d/wsplcd'
HYFI_BRIDGING_INIT='/etc/init.d/hyfi-bridging'

config_load 'repacd'
config_get_bool auto_config_enable repacd 'auto_config_enable' '0'
config_get_bool ezmesh repacd 'Ezmesh' '0'
config_get_bool ftauth repacd 'FTAuth' '0'
config_get_bool map_ftauth MAPConfig 'EnableFTAuth' '0'
config_get map_mobility_domain MAPConfig 'MobilityDomain' '0'
config_get ftauthbridge repacd 'FTAuthBridge' "$primarybridgename"
config_get_bool ezmesh6g repacd 'Ezmesh6GEnable' '0'
config_get enable_slo MAPConfig 'EnableSLO' 0
config_get enable_mlo MAPConfig 'EnableMLO' 0
config_get base_mld_mac_addr MAPConfig 'mld_mac_addr' ''
config_get mld_config MAPConfig 'MLDConfig' ''
#Config required to know whether hiddenvaps are being brought down from vlanmon
config_get hiddenvapdown 'MAPWiFiLink' 'hiddenvaps_down' '0'

    if [ "$ftauth" -eq 1 ] || [ "$map_ftauth" -eq 1 ]; then
        ft_256_key=$(echo $(date +05060708090a0b0c0d0e0f%s) % 4 |  awk '{print $1}')
    fi

    if [ "$ezmesh" -eq 1 ]; then
        HYD_INIT='/etc/init.d/ezmesh'
        MAP='ezmesh'
        MAPLBD='ezlbd'
    else
        HYD_INIT='/etc/init.d/hyd'
        MAP='hyd'
        MAPLBD='lbd'
    fi

LBD_INIT='/etc/init.d/lbd'
MCSD_INIT='/etc/init.d/mcsd'

. /lib/functions/whc-debug.sh
. /lib/functions/whc-iface.sh
. /lib/functions/whc-network.sh
. /lib/functions/repacd-netdet.sh
. /lib/functions/hyfi-iface.sh

# Write the provided log message to the correct output stream and syslog.
# If REPACD_DEBUG_OUTOUT is non-zero, it will be written to the console.
# Otherwise, it will be written to stdout.
__repacd_echo() {
    local stderr=''
    if [ "$REPACD_DEBUG_OUTOUT" -gt 0 ]; then
        echo "whc (repacd): $*" > /dev/console
        stderr='-s'
    else
        echo "whc (repacd): $*"
    fi

    logger $stderr -t repacd.init -p user.debug "$1"
}

# Determine whether the Multi-AP Topology Optimization algorithm is enabled.
#
# return: 0 if the algorithm is enabled; otherwise 1
__repacd_is_map_enabled() {
    local map_enabled

    config_load 'repacd'
    config_get_bool map_enabled MAPConfig 'Enable' '0'

    [ "$map_enabled" -gt 0 ]
    return $?
}

# Determine whether the Multi-AP Lite Mode is enabled.
#
# return: 0 if enabled; otherwise 1
__repacd_is_map_lite_enabled() {
    config_load 'repacd'
    config_get_bool map_lite_enabled MAPConfig 'EnableLiteMode' '0'

    [ "$map_lite_enabled" -gt 0 ]
    return $?
}

__repacd_check_version_compatibility() {
    local op_mode=$1
    local iface="ath0"
    local app_comp_num
    local driv_comp_num
    local driver_ver
    local disp_driv_ver

    driver_ver=$(cfg80211tool_mesh $iface get_mesh_version | grep -Eo '([0-9]+\.){3}[0-9]+')

    if [ -z "$driver_ver" ]; then
        __repacd_echo "ioctl failed: on interface $iface"
        __repacd_echo "Unable to perform Version Compatibility check !!!"
        return 1

    fi

    if [ "$op_mode" = "son" ]; then
        app_comp_num=$(echo $SON_VERSION | awk -F'.' '{print $3}')
        disp_driv_ver=$(echo $driver_ver | awk '{print $1}')
        driv_comp_num=$(echo $disp_driv_ver | awk -F'.' '{print $3}')

    elif [ "$op_mode" = "map" ]; then
        app_comp_num=$(echo $MAP_VERSION | awk -F'.' '{print $3}')
        disp_driv_ver=$(echo $driver_ver | awk '{print $2}')
        driv_comp_num=$(echo $disp_driv_ver | awk -F'.' '{print $3}')
    fi

    if [ -n "$driv_comp_num" -a -n "$app_comp_num" ]; then
        if [ $app_comp_num = $driv_comp_num ]; then
            __repacd_echo "Version Compatibility check PASSED"
        else
            __repacd_echo "Version Compatibility check FAILED"
            if [ -n "$disp_driv_ver" ]; then
                if [ "$op_mode" = "son" ]; then
                    __repacd_echo "REPACD Version : [$SON_VERSION] Driver Version : [son-$disp_driv_ver]"
                else
                    __repacd_echo "REPACD Version : [$MAP_VERSION] Driver Version : [easymesh-$disp_driv_ver]"
                fi
            fi
        fi
    fi

}

# Determine if the device is configured in gateway mode or not.
# Note that a device can still act as a CAP even if it is not in gateway
# mode, but this is detected later by the repacd-run.sh script.
# For WAN enabled gateway AP, check if wan interface name is configured
# For SDX-55 modem enabled gateway AP, check if wan protocol is set to 'rmnet'
# return: 0 if in gateway mode; otherwise non-zero
__repacd_gw_mode() {
    local wan_iface

    config_load network
    if [ "$openwrt_version_check" -eq 1 ]; then
        config_get wan_iface wan device
    else
        config_get wan_iface wan ifname
    fi
    config_get wan_proto wan proto 'dhcp'

    if [ -n "$wan_iface" ] || [ $wan_proto = "rmnet" ] || [ "$board_name" != "${board_name/ap-sdx*/}" ]; then
        return 0
    fi

    return 1
}

# Delete firewall rule
__repacd_delete_rule() {
    local config=$1 nw=$2 rule

    rule=$(uci show firewall.$config | grep -w $nw)
    [ -n "$rule" ] && uci delete firewall.$config
}

# Delete firewall rule if not required
__repacd_delete_firewall_rules() {
    local config=$1 rdnw rdtype

    config_get rdnw "$config" network
    config_get rdtype "$config" type
    [ "$rdnw" = "backhaul" ] && return
    [ "$rdtype" != "bridge" ] && return
    [ "$rdnw" = "$managed_network" ] && return

    config_load firewall
    config_foreach __repacd_delete_rule rule $rdnw
    config_foreach __repacd_delete_rule zone $rdnw
    config_foreach __repacd_delete_rule forwarding $rdnw
    uci_commit firewall
}

# Apply firewall rule based on the "allow_networks" config
# Input: $1-src network $2-dest network $3-Accpet/Reject
__repacd_apply_firewall_rules() {
    local  src_network=$1 dest_network=$2 action_cmd=$3
    local no_rule zone fwd rule rule_name

    [ -z "$src_network" -o "$src_network" = "backhaul" ] && return
    [ -z "$dest_network" -o "$dest_network" = "backhaul" ] && return

    config_load 'network'
    config_get ipaddr $dest_network ipaddr
    config_get type $src_network type
    [ -z "$type" -o $type != "bridge" ] && return
    config_get type $dest_network type
    [ -z "$type" -o $type != "bridge" ] && return
    no_rule=$(uci show firewall | grep zone | grep -w "$src_network")
    if [ -z "$no_rule" ]; then
        zone=$(uci add firewall zone)
        if [ -n "$zone" ]; then
            uci_set firewall "$zone" name "$src_network"
            uci add_list "firewall.$zone.network=$src_network"
            uci_set firewall "$zone" input 'REJECT'
            uci_set firewall "$zone" output 'ACCEPT'
            uci_set firewall "$zone" forward 'REJECT'
        fi
    fi
    no_rule=$(uci show firewall | grep -w "$src_network-forwarding-wan")
    if [ -z "$no_rule" ]; then
            fwd=$(uci add firewall forwarding)
            if [ -n "$fwd" ]; then
		rule_name="$src_network-forwarding-wan"
		uci_set firewall "$fwd" name "$rule_name"
                uci_set firewall "$fwd" src "$src_network"
                uci_set firewall "$fwd" dest 'wan'
            fi
    fi
    no_rule=$(uci show firewall | grep -w "$src_network-Allow-DHCP-request")
    if [ -z "$no_rule" ]; then
            rule=$(uci add firewall rule)
            if [ -n "$rule" ]; then
		rule_name="$src_network-Allow-DHCP-request"
                uci_set firewall "$rule" name "$rule_name"
                uci_set firewall "$rule" src "$src_network"
                uci_set firewall "$rule" src_port '67-68'
                uci_set firewall "$rule" dest_port '67-68'
                uci_set firewall "$rule" proto 'udp'
                uci_set firewall "$rule" target 'ACCEPT'
            fi
    fi
    no_rule=$(uci show firewall | grep -w "$src_network-Allow-DNS-queries")
    if [ -z "$no_rule" ]; then
            rule=$(uci add firewall rule)
            if [ -n "$rule" ]; then
		rule_name="$src_network-Allow-DNS-queries"
                uci_set firewall "$rule" name "$rule_name"
                uci_set firewall "$rule" src "$src_network"
                uci_set firewall "$rule" dest_port '53'
                uci_set firewall "$rule" proto 'tcpudp'
                uci_set firewall "$rule" target 'ACCEPT'
            fi
    fi
    no_rule=$(uci show firewall | grep -w "$src_network-$dest_network-forwarding")
    if [ -z "$no_rule" ]; then
            rule=$(uci add firewall rule)
            if [ -n "$rule" ]; then
                rule_name="$action_cmd-$src_network-$dest_network-forwarding"
                uci_set firewall "$rule" name "$rule_name"
                uci_set firewall "$rule" src "$src_network"
                uci_set firewall "$rule" dest $dest_network
                uci_set firewall "$rule" proto '0'
                uci_set firewall "$rule" family 'any'
                uci_set firewall "$rule" target "$action_cmd"
            fi
    fi
    no_rule=$(uci show firewall | grep -w "$src_network-$dest_network-Icmp")
    if [ -z "$no_rule" ]; then
	    rule=$(uci add firewall rule)
	    if [ -n "$rule" ]; then
		rule_name="$action_cmd-$src_network-$dest_network-Icmp"
		uci_set firewall "$rule" name "$rule_name"
		uci_set firewall "$rule" src "$src_network"
		uci_set firewall "$rule" proto "icmp"
		uci_set firewall "$rule" target "$action_cmd"
		uci_set firewall "$rule" family "ipv4"
		uci_set firewall "$rule" dest_ip "$ipaddr"
	    fi
    fi
    uci_commit firewall
}

# Check network to make decision accept / reject
# Input: $1 - repacd config $2 - network name
__repacd_is_allow() {
    local config=$1 nw_to_check=$2 allow_list nw

    config_get allow_list "$config" allow_networks

    for nw in $allow_list; do
	[ "$nw" = "$nw_to_check" ] && return 0
    done
    return 1
}

# Create firewall rules for the network based on the config
# "allow_networks" provided in repacd
# Input: repacd config
__repacd_set_firewall_rules_new() {
     local rdconfig=$1 rdnw rdtype allow_networks network_list
     local action_cmd="REJECT"

     config_get rdnw "$rdconfig" network
     config_get rdtype "$rdconfig" type
     [ "$rdnw" = "backhaul" ] && return
     [ "$rdtype" != "bridge" ] && return
     network_list=$(uci show network | grep bridge | grep type)
     for nw in $network_list; do
	nw_t=$(echo $nw | cut -d "." -f 2)
	[ "$rdnw" = "$nw_t" ] && [ "$rdnw" = "$managed_network" ] && continue
	if __repacd_is_allow $rdconfig $nw_t; then
	    action_cmd="ACCEPT"
	else
	    action_cmd="REJECT"
	fi
	__repacd_echo "src=$rdnw dst=$nw_t action=$action_cmd"
        __repacd_apply_firewall_rules $rdnw $nw_t "$action_cmd"
     done
}

# Create firewall rules for the given network if the rule does not exist.
# input: $1 network name
__repacd_set_firewall_rules() {
    local network=$1
    local no_rule
    local rule_name

    no_rule=$(uci show firewall | grep zone | grep "$network")

    if [ -z "$no_rule" ]; then
        local zone
        zone=$(uci add firewall zone)
        if [ -n "$zone" ]; then
            uci_set firewall "$zone" name "$network"
            uci add_list "firewall.$zone.network=$network"
            uci_set firewall "$zone" input 'REJECT'
            uci_set firewall "$zone" output 'ACCEPT'
            uci_set firewall "$zone" forward 'REJECT'

            local fwd
            fwd=$(uci add firewall forwarding)
            if [ -n "$fwd" ]; then
                uci_set firewall "$fwd" src "$network"
                uci_set firewall "$fwd" dest 'wan'
            fi

            local rule
            rule=$(uci add firewall rule)
            if [ -n "$rule" ]; then
                uci_set firewall "$rule" name 'Allow-DHCP-request'
                uci_set firewall "$rule" src "$network"
                uci_set firewall "$rule" src_port '67-68'
                uci_set firewall "$rule" dest_port '67-68'
                uci_set firewall "$rule" proto 'udp'
                uci_set firewall "$rule" target 'ACCEPT'
            fi

            rule=$(uci add firewall rule)
            if [ -n "$rule" ]; then
                uci_set firewall "$rule" name 'Allow-DNS-queries'
                uci_set firewall "$rule" src "$network"
                uci_set firewall "$rule" dest_port '53'
                uci_set firewall "$rule" proto 'tcpudp'
                uci_set firewall "$rule" target 'ACCEPT'
            fi

            rule=$(uci add firewall rule)
            if [ -n "$rule" ]; then
                rule_name="Block-$managed_network-$network-forwarding"
                uci_set firewall "$rule" name "$rule_name"
                uci_set firewall "$rule" src $managed_network
                uci_set firewall "$rule" dest "$network"
                uci_set firewall "$rule" proto '0'
                uci_set firewall "$rule" family 'any'
                uci_set firewall "$rule" target 'REJECT'
            fi

            rule=$(uci add firewall rule)
            if [ -n "$rule" ]; then
                rule_name="Block-$network-$managed_network-forwarding"
                uci_set firewall "$rule" name "$rule_name"
                uci_set firewall "$rule" src "$network"
                uci_set firewall "$rule" dest $managed_network
                uci_set firewall "$rule" proto '0'
                uci_set firewall "$rule" family 'any'
                uci_set firewall "$rule" target 'REJECT'
            fi
        fi
    fi
    uci_commit firewall
}

# Determine the configured RE mode, not applying any automatic mode
# switching rules.
#
# output: $1 variable in which to place the resolved mode
__repacd_get_config_re_mode() {
    local resolved_mode=$1
    local mode

    config_load 'repacd'
    config_get mode repacd 'ConfigREMode' 'auto'

    eval "$resolved_mode=$mode"
}

# Determine whether the provided mode is an automatic mode.
#
# input: $1 mode: the mode to check
# return: 0 if the mode is an auto mode; otherwise 1
__repacd_is_auto_re_mode() {
    local mode=$1

    if [ "$mode" = 'auto' ]; then
        return 0
    fi

    return 1
}

# Resolve the current mode in which to operate the range extender.
#
# This mode is either the configured mode (if it is set to a specific
# mode), the automatically derived RE mode based on association information,
# or the default range extender mode for use at startup.
#
# output: $1 - the variable into which to place the resolved mode
__repacd_get_re_mode() {
    local resolved_mode=$1
    local mode default_mode

    __repacd_get_config_re_mode config_re_mode

    if __repacd_is_auto_re_mode $config_re_mode; then
        config_get default_mode repacd 'DefaultREMode' 'qwrap'
        config_get mode repacd 'AssocDerivedREMode' "$default_mode"
    else
        mode=$config_re_mode
    fi

    # Get the Association derived sub-mode. If not derived, say "star".
    config_get resolved_re_submode repacd 'AssocDerivedRESubMode' 'star'

    eval "$resolved_mode=$mode"
}

# Determine if the range extension mode is set to WDS.
#
# return: 0 if the mode is set to WDS; otherwise 1 (meaing QWrap or ExtAP)
__repacd_is_wds_mode() {
    __repacd_get_re_mode resolved_re_mode

    case "$resolved_re_mode" in
        wds|WDS)
            return 0
        ;;

        *)
            return 1
        ;;
    esac
}

# Determine if the range extension mode is set to QWrap.
#
# return: 0 if the mode is set to QWrap; otherwise 1 (meaning WDS or ExtAP)
__repacd_is_qwrap_mode() {
    __repacd_get_re_mode resolved_re_mode
    case "$resolved_re_mode" in
        qwrap|QWRAP)
            return 0
        ;;

        *)
            return 1
    esac
}

# Determine if the range extension mode is set to SON.
# Note that if the hyd init script is missing, this will be considered as
# SON mode disabled.
#
# return: 0 if the mode is set to SON; otherwise 1 (meaing WDS or an
#         interoperable range extension mode)
__repacd_is_son_mode() {
    if [ ! -f $HYD_INIT ]; then
        return 1
    fi

    __repacd_get_re_mode resolved_re_mode
    case "$resolved_re_mode" in
        son|SON)
            return 0
        ;;

        *)
            return 1
        ;;
    esac
}

# Determine if DFS channels are to be blocked even for WDS mode.
#
# return: 0 if DFS channels should not be used; otherwise 1 (meaning they may
#         be used)
__repacd_is_block_dfs() {
    local block_dfs

    config_load 'repacd'
    config_get block_dfs repacd 'BlockDFSChannels' '0'

    if [ "$block_dfs" -gt 0 ]; then
        return 0
    else
        return 1
    fi
}

# Generate a suffix for the SSID using the last 3 bytes of the MAC address
# on the bridge that is being managed.
#
# output: $1 - variable into which to write the SSID suffix
__repacd_generate_ssid_suffix() {
    local generated_suffix
    generated_suffix=$(ifconfig $brprefix$managed_network | grep HWaddr | awk '{print $5}' | awk -F":" '{print $4$5$6}')
    eval "$1=$generated_suffix"
}

# Generate a random pre-shared key for use with WPA2
#
# output: $1 - variable into which to write the PSK
__repacd_generate_psk() {
    local generated_key
    generated_key=$(dd if=/dev/urandom bs=1 count=8 2> /dev/null | base64)
    eval "$1=$generated_key"
}

# Determine if the mode on the interface is a match.
# This does fuzzy matching in that multiple actual modes are said to match
# a given general mode.
#
# input: $1 general_mode: one of 'sta' or 'ap'
# input: $2 cur_mode: the currently configured mode
# return: 0 on a match; otherwise non-zero
__repacd_is_matching_mode() {
    local general_mode=$1
    local cur_mode=$2

    if [ "$general_mode" = 'sta' ]; then
        if [ "$cur_mode" = 'sta' ]; then
            return 0
        else
            return 1
        fi
    else
        if [ "$cur_mode" = 'ap' ] || [ "$cur_mode" = 'wrap' ]; then
            return 0
        else
            return 1
        fi
    fi
}

# Enumerate all of the devices and append them to the variable provided.
# input: $1 devices: variable to populate with the devices
__repacd_get_devices() {
    local devices=$1

    config_cb() {
        local type="$1"
        local section="$2"

        case "$type" in
            wifi-device)
        config_get hwmode "$section" hwmode
        config_get type "$section" type
        config_get band "$section" band
        if [ "$type" != 'mac80211' ] && [ "$hwmode" != '11ad' ]; then
            if [ "$ezmesh" -eq 1 ]; then
                if [ "$band" != 3 ] || [ "$ezmesh6g" -eq 1 ]; then
                    eval append $devices "$section"
                fi
            else
                eval append $devices "$section"
            fi
        fi
                ;;
        esac
    }
    config_load wireless
}

__repacd_get_mld_devices() {
    local mld_devices=$1

    config_cb() {
        local type="$1"
        local section="$2"

        case "$type" in
            wifi-mld)
                eval append $mld_devices "$section"
                ;;
        esac
    }
    config_load wireless
}

# Set the parameter in the wireless configuration section provided, recording
# if it actually needed to be changed.
# input: $1 iface_name: section name
# input: $2 param_name: the name of the parameter being set
# input: $3 param_val: the new value for the parameter
__repacd_update_vap_param() {
    local iface_name=$1 param_name=$2 param_val=$3
    local cur_val

    cur_val=$(uci get wireless.$iface_name.$param_name)
    if [ -z "$cur_val" ] || [ ! "$cur_val" = "$param_val" ]; then
        uci_set wireless "$iface_name" "$param_name" "$param_val"
        config_changed=1
    fi
}

# Set the FT parameter in the wireless configuration section provided
__repacd_init_vap_set_ft_params() {
    local config="$1"
    local name=$1 device=$2 base_ssid=$3 vap_count=$4 nw=$5 rdmode=$6
    local mode ifname nasid
    config_get mode "$name" mode
    config_get steeringdisabled "$name" SteeringDisabled
    config_get ifname "$name" ifname
    config_get device "$name" device
    config_get network "$config" network
    config_load $MAP
    config_get hyfi_mode 'config' 'Mode' 'HYROUTER'
    if [ "$ftauth" -eq 1 ] && [ "$mode" = "ap" ] && [ "$network" = "$ftauthbridge" ] && [ "$steeringdisabled" != 1 ] ; then
        __repacd_update_vap_param "$name" 'mobility_domain' "A1A1"
        __repacd_update_vap_param "$name" 'wps_pbc' 0
        __repacd_update_vap_param "$name" 'ft_auth' 1
        __repacd_update_vap_param "$name" 'wps_pbc_enable' 0
        __repacd_update_vap_param "$name" 'ft_over_ds' 0
        if [ "$hyfi_mode" = "HYROUTER" ] ; then
            __repacd_update_vap_param "$name" 'kh_key_hex' "$ft_256_key"
        fi
        nasid=FT"$(iw dev $ifname info | grep '\saddr' | awk -F " " '{print $2}' | sed 's/\://g')"
        ft_r1_key="$(iw dev $ifname info | grep '\saddr' | awk -F " " '{print $2}' | sed 's/\://g')"
        ft_r1_key_mesh="$(iw dev $ifname info | grep '\saddr' | awk -F " " '{print $2}')"
        __repacd_update_vap_param "$name" 'nasid' "$nasid"
        __repacd_update_vap_param "$name" 'r1_key_holder' "$ft_r1_key"
    fi
}

# Determine the SSID to apply to a specific VAP based on the current
# configuration of the WPS Push Button Configuration enhancement.
# input: $1 name: section name
# input: $2 device: name of the radio
# input: $3 base_ssid: the SSID from which to derive other ones
# input: $4 vap_count: additional vap count created in Device
# input: $5 nw: network name
# input: $6 rdmode: mode in repacd config
__repacd_init_vap_set_ssid_and_pbc() {
    local name=$1 device=$2 base_ssid=$3 vap_count=$4 nw=$5 rdmode=$6
    local mode
    local enable_wps_re_enhc global_suffix='' radio_suffix=''
    local start_time duration
    local auto_create_vaps

    # Grab the overall enable first
    config_get enable_wps_re_enhc qcawifi wps_pbc_extender_enhance '0'

    # Now VAP specific parameters
    config_get mode "$name" mode
    config_get auth_secret "$name" auth_secret
    config_get start_time "$name" wps_pbc_start_time
    config_get duration "$name" wps_pbc_duration
    config_get_bool repacd_security_unmanaged  "$name" repacd_security_unmanaged '0'
    config_get_bool auto_create_vaps "$device" repacd_auto_create_vaps '1'

    if [ "$enable_wps_re_enhc" -gt 0 ]; then
        # Only apply the SSID derivation rules on AP interfaces.
        if __repacd_is_matching_mode 'ap' $mode; then
            # Potentially two suffixes can be applied
            # First is set at the overall AP level.
            config_get global_suffix qcawifi wps_pbc_overwrite_ssid_suffix ''

            # Second is set at the radio level
            config_get radio_suffix "$device" wps_pbc_overwrite_ssid_band_suffix ''
        fi
    fi

    if [ "$repacd_security_unmanaged" -eq 0 ] ; then
        __repacd_update_vap_param "$name" 'ssid' "$base_ssid$global_suffix$radio_suffix"
    fi

    #Disabling WPS for all additional FH VAPs
    if [ "$vap_count" -eq 0 -a "$rdmode" != "ap_smart_monitor" -a -z "$auth_secret" ]; then
        __repacd_update_vap_param "$name" 'wps_pbc' 1
        __repacd_update_vap_param "$name" 'wps_pbc_enable' "$enable_wps_re_enhc"
        if [ "$traffic_separation_enabled" -gt 0 ]; then
	    if [ "$auto_config_enable" -eq 1 ]; then
		__repacd_update_vap_param "$name" 'wps_pbc_noclone' '0'
	    else
                __repacd_update_vap_param "$name" 'wps_pbc_noclone' '1'
	    fi
        fi

        # Without a custom config, set all interfaces to be enabled for 2 minutes.
        if [ -z "$start_time" ]; then
	    if [ "$traffic_separation_enabled" -gt 0 -a "$nw" = "backhaul" -a "$auto_config_enable" -eq 1 ]; then
                __repacd_update_vap_param "$name" 'wps_pbc_start_time' '61'
	    else
		__repacd_update_vap_param "$name" 'wps_pbc_start_time' 0
	    fi
        fi

        if [ -z "$duration" ]; then
	    if [ "$auto_config_enable" -eq 0 ]; then
                if [ "$traffic_separation_enabled" -gt 0 ] && [ "$auto_create_vaps" -eq 1 ]; then
                    __repacd_update_vap_param "$name" 'wps_pbc_duration' 60
                else
                    __repacd_update_vap_param "$name" 'wps_pbc_duration' 120
		fi
	    else
		if [ "$traffic_separation_enabled" -gt 0 -a "$nw" = "backhaul" ]; then
		    __repacd_update_vap_param "$name" 'wps_pbc_duration' '180'
		else
		    __repacd_update_vap_param "$name" 'wps_pbc_duration' '60'
		fi
            fi
        fi
    fi
}

# Set all of the configuration parameters for the given VAP.
# input: $1 name: section name
# input: $2 device: name of the radio
# input: $3 mode: whether to act as a STA or AP
# input: $4 ssid: the desired SSID for this VAP
# input: $5 encryption: the desired encryption mode for this VAP
# input: $6 pserver: dot1x primary server
# input: $7 pport: dot1x primary port
# input: $8 psecret: dot1x primary secret
# input: $9 sserver: secondary server address
# input: $10 sport: secondary server port
# input: $11 ssecret: secondary server secret
# input: $12 network: network bridge
# input: $13 rsn_preauth: rsn preauth value
# input: $14 rsn_preauth_interfaces: preauth interface name
# input: $15 wpa_group_rekey: group rekey period value
# input: $16 ieee80211w: ieee80211w value
# input: $17 group_mgmt_cipher: group management cipher value
# input: $18 suiteb: suiteb value
__repacd_init_vap_dot1x() {
    local name=$1 device=$2 mode=$3 ssid=$4 encryption=$5 pserver=$6 pport=$7 psecret=$8
    local sserver=$9 sport=${10} ssecret=${11} network=${12} rsn_preauth=${13}
    local rsn_preauth_interfaces=${14} wpa_group_rekey=${15} ieee80211w=${16}
    local group_mgmt_cipher=${17} suiteb=${18} ft_auth=${19} ft_256_key=${20} mobility_domain=${21}
    local config_mld=${22} cur_mode vap_count=0

    config_get_bool repacd_security_unmanaged "$name" repacd_security_unmanaged '0'
    __repacd_update_vap_param "$name" 'device' "$device"
    __repacd_update_vap_param "$name" 'network' $network
    __repacd_init_vap_set_ssid_and_pbc "$name" "$device" "$ssid" "$vap_count" "$network"

    if [ "$repacd_security_unmanaged" -eq 0 ] ; then
        if [ "$encryption" = "suiteb" ] && [ -n "$suiteb" ]; then
            __repacd_update_vap_param "$name" suite_b "$suiteb"
            __repacd_update_vap_param "$name" wpa_group_rekey "$wpa_group_rekey"
            __repacd_update_vap_param "$name" ieee80211w "$ieee80211w"
           __repacd_update_vap_param "$name" group_mgmt_cipher "$group_mgmt_cipher"
        else
            __repacd_update_vap_param "$name" 'encryption' "$encryption"
        fi
        __repacd_echo "auth_server = $pserver"
        __repacd_echo "auth_port = $pport"
        __repacd_echo "auth_secret = $psecret"
        __repacd_echo "auth_server2 = $sserver"
        __repacd_echo "auth_port2 = $sport"
        __repacd_echo "auth_secret2 = $ssecret"
        __repacd_update_vap_param "$name" 'auth_server' "$pserver"
        __repacd_update_vap_param "$name" 'auth_port' "$pport"
        __repacd_update_vap_param "$name" 'auth_secret' "$psecret"
        __repacd_update_vap_param "$name" 'auth_server2' "$sserver"
        __repacd_update_vap_param "$name" 'auth_port2' "$sport"
        __repacd_update_vap_param "$name" 'auth_secret2' "$ssecret"
        __repacd_update_vap_param "$name" 'rsn_preauth' "$rsn_preauth"
        __repacd_update_vap_param "$name" 'rsn_preauth_interfaces' "$rsn_preauth_interfaces"
        if [ "$ft_auth" -eq 1 ] || [ "$map_ftauth" -eq 1 ]; then
		if [ "$ft_auth" -eq 1 ]; then
			__repacd_update_vap_param "$name" 'ft_auth' "$ft_auth"
		fi
		if [ "$map_ftauth" -eq 1 ]; then
			__repacd_update_vap_param "$name" 'ft_auth' "$map_ftauth"
		fi
            __repacd_update_vap_param "$name" 'kh_key_hex' "$ft_256_key"
            __repacd_update_vap_param "$name" 'mobility_domain' "$mobility_domain"
        fi
    fi

    # Mode needs to be handled separately. If the device is already in one
    # of the AP modes and the init is requesting an AP mode, we leave it as
    # is. If it is already in the STA mode, we also leave it as is.
    config_get cur_mode "$name" 'mode'
    if ! __repacd_is_matching_mode "$mode" "$cur_mode"; then
        uci_set wireless "$name" 'mode' "$mode"
        config_changed=1
    fi

    local hwmode mld_role
    config_get hwmode "$device" hwmode

    if [ "$enable_slo" -eq 1 ] && [ "$config_mld" -eq 1 ]; then
        if [ "$hwmode" = '11bea' ] || [ "$hwmode" = '11beg' ]; then
            if [ "$mode" = 'ap' ]; then
                if __repacd_create_mld_section "$ssid" "$base_mld_mac_addr" "AP"; then
                    __repacd_update_vap_param "$name" 'mld' "mld$((mldIndex-1))"
                fi
            else
                __repacd_update_vap_param "$name" 'mld' "$sta_mld"
                uci_set wireless "$sta_mld" 'mld_ssid' "$ssid"
            fi
        fi
    fi
}


# Fetch the MLD section name from MLD Index if exists
#
# input: $1 config: section name
# input: $2 index_to_match: MLD Index to match the MLD Section
# output: $3 mld_section: MLD Section name
# output: $3 mld_role: MLD Role
__repacd_wifimon_get_mld_matching_index() {
    local config="$1"
    local index_to_match="$2"
    local MldIndex=255

    config_get role "$config" role
    MldIndex=$(echo "$config" | sed 's/[^0-9]*//g' )

    if [ "$MldIndex" == "$index_to_match" ]; then
        eval "$3=$config"
        eval "$4=$role"
        return
    fi
}

# Create new MLD section and associate it to a VAP
# input: $1 ssid: the desired SSID for this MLD section
# input: $2 mac: Input MAC address
# Note: caller of this function should take care of next
# desired MAC address increment and desired MLD section
# i.e. mldAddrInc and mldIndex
__repacd_create_mld_section() {
    local ssid=$1
    local mac="$2"
    local mld_role="$3"
    local mld_available=''
    local mld_available_role=''
    local staMldIndex=255
    local mld_found=0
    local mlo_skip_link_time

    config_load repacd
    config_get mlo_skip_link_time MAPConfig 'MLOSkipLinkTime' ''
    config_load wireless

    if [ "$mld_role" == "AP" ]; then
        configured_mld_count=0
        config_foreach __repacd_wifimon_get_configured_mld_count wifi-mld
#        __repacd_echo "Debug: mldIndex:$mldIndex configured_mld_count:$configured_mld_count"

        while [ "$mld_found" -eq 0 -a "$mldIndex" -lt "$configured_mld_count" ]; do
            mld_available=''
#            __repacd_echo "Debug: mldIndex:$mldIndex configured_mld_count:$configured_mld_count"
            # Check whether user already configured same MLD before creating new
            config_foreach __repacd_wifimon_get_mld_matching_index \
                wifi-mld "$mldIndex" mld_available mld_available_role

            if [ -n "$mld_available" ]; then
                # MLD already exists with this index, use it, if role matches
                # otherwise continue search
                __repacd_echo "MLD section found [mld$mldIndex]"
                # skip Non-AP MLD in this search
                if [ "$mld_role" == "$mld_available_role" ]; then
                    mldIndex=$((mldIndex+1))
                    return 0
                else
                    __repacd_echo "MLD Found. But MLD role did not match"
                    mldIndex=$((mldIndex+1))
                fi
            else
                # MLD not configured in this index, so this index can be used
                mld_found=$((mld_found+1))
            fi
        done
    fi

    if [ -z "$mac" ]; then
        __repacd_echo "Base MLD MAC Address not configured"
#        return 1
    fi

    uci set wireless."mld$mldIndex"=wifi-mld
    uci set wireless.mld"$mldIndex".mld_ssid="$ssid"
    uci set wireless.mld"$mldIndex".mld_ref="$mldIndex"

    if [ -n "$mld_role" ]; then
        uci set wireless.mld"$mldIndex".role="$mld_role"
    fi

    # Below option required to allow MLO STA to associate with Non-ML AP
    if [ "$mld_role" = 'Non-AP' ]; then
        uci set wireless.mld"$mldIndex".allow_non_ml='1'
        if [ "$mlo_skip_link_time" -gt 0 ]; then
            uci set wireless.mld"$mldIndex".mlo_skip_link_time="$mlo_skip_link_time"
        fi
    fi

    if [ -n "$mac" ]; then
        # remove colons Ex: 00:00:03:7F:FF:FF -> 0000037FFFFF
        mac_hex=$( echo "$mac" | tr -d ':' )

        # convert to decimal Ex: 58720255
        mac_dec=$( printf "%d\n" 0x$mac_hex )

        # Increment to next MAC address Ex: 58720256
        mac_dec_add=$( expr $mac_dec + $mldAddrInc )

        # convert to hex and prepend with zeros if required Ex: 000003800001
        mac_hex_out=$( printf "%X\n" $mac_dec_add | sed 's/^/000000000000/; s/^.\{1,12\}\(.\{12\}\)/\1/' )

        # insert colon back Ex: 00:00:03:80:00:01
        mac_out=$(echo $mac_hex_out | sed 's/\(..\)/\1:/g;s/:$//' )
        mldAddrInc=$((mldAddrInc+1))

        uci set wireless.mld"$mldIndex".mld_macaddr="$mac_out"
        __repacd_echo "Generate MLD MAC: mld mac:$mac_out"
    fi

    __repacd_echo "Generate MLD section: mld[$mldIndex]: mld ssid:[$ssid] Role:$mld_role Ref:$mldIndex"
    uci_commit wireless

    mldIndex=$((mldIndex+1))
	return 0
}

# Set all of the configuration parameters for the given VAP.
# input: $1 name: section name
# input: $2 device: name of the radio
# input: $3 mode: whether to act as a STA or AP
# input: $4 ssid: the desired SSID for this VAP
# input: $5 encryption: the desired encryption mode for this VAP
# input: $6 key: the desired passphrase for this VAP
# input: $7 vap_count: additional vap count created in Device
__repacd_init_vap() {
    local name=$1 device=$2 mode=$3 ssid=$4 encryption=$5 key=$6 vap_count=$7 config_mld=$8
    local cur_mode

    config_get_bool repacd_security_unmanaged "$name" repacd_security_unmanaged '0'
    __repacd_update_vap_param "$name" 'device' "$device"
    __repacd_update_vap_param "$name" 'network' $managed_network
    __repacd_init_vap_set_ssid_and_pbc "$name" "$device" "$ssid" "$vap_count" "$managed_network"

    if [ "$repacd_security_unmanaged" -eq 0 ] ; then
        __repacd_update_vap_param "$name" 'encryption' "$encryption"
        __repacd_update_vap_param "$name" 'key' "$key"
    fi

    # Mode needs to be handled separately. If the device is already in one
    # of the AP modes and the init is requesting an AP mode, we leave it as
    # is. If it is already in the STA mode, we also leave it as is.
    config_get cur_mode "$name" 'mode'
    if ! __repacd_is_matching_mode "$mode" "$cur_mode"; then
        uci_set wireless "$name" 'mode' "$mode"
        config_changed=1
    fi

    local hwmode mld_role
    config_get hwmode "$device" hwmode
    config_get enable_slo MAPConfig 'EnableSLO' 0

    if [ "$enable_slo" -eq 1 ] && [ "$config_mld" -eq 1 ]; then
        if [ "$hwmode" = '11bea' ] || [ "$hwmode" = '11beg' ]; then
            if [ "$mode" = 'ap' ]; then
                if __repacd_create_mld_section "$ssid" "$base_mld_mac_addr" "AP"; then
                    __repacd_update_vap_param "$name" 'mld' "mld$((mldIndex-1))"
                fi
            else
                __repacd_update_vap_param "$name" 'mld' "$sta_mld"
                uci_set wireless "$sta_mld" 'mld_ssid' "$ssid"
            fi
        fi
    fi
}

# Set all VAPs for the given network and mode to disabled or enabled (per
# the parameters).
#
# input: $1 config: section name
# input: $2 network: network for which to update VAPs
# input: $3 mode: sta or ap
# input: $4 disable_24g: 1 - disable, 0 - enable
# input: $5 disable_5g: 1 - disable, 0 - enable
# input-output: $6 change counter
__repacd_disable_vap() {
    local config="$1"
    local disable_24g="$4"
    local disable_5g="$5"
    local mode network disabled
    local changed="$6"

    config_get device "$config" device
    config_get hwmode "$device" hwmode
    config_get type "$device" type

    if [ "$hwmode" = '11ad' ] && [ "$type" = 'mac80211' ] ;then
        return
    fi
    config_get mode "$config" mode
    config_get network "$config" network
    config_get disabled "$config" disabled 0

    if [ "$2" = "$network" ] && __repacd_is_matching_mode "$3" "$mode"; then
        local desired_val
        if whc_is_5g_vap "$config"; then
            desired_val=$disable_5g
        else
            desired_val=$disable_24g
        fi

        if [ ! "$desired_val" = "$disabled" ]; then
            uci_set wireless "$config" disabled "$desired_val"
            changed=$((changed + 1))
            eval "$6='$changed'"
            __repacd_echo "Set VAP $config to Disabled=$desired_val"
        fi
    fi
}

# Change the wsplcd running mode based on the value provided.
#
# input: $1 new_mode: value to set wsplcd run mode to
# input: $2 new_deep_clone: whether to enable deep cloning (which copies the
#                       channel and locks the association to the CAP)
# input: $3 new_deep_clone_no_bssid: deep cloning without BSSID cloning
# input: $4 config_sta: whether to use the extension to configure a STA
#                       interface during cloning
# input: $5 map_enable: revision of MAP specification code is currently operating on
# input-output: $6 changed: count of the changes
__repacd_configure_wsplcd() {
    local new_mode=$1
    local new_deep_clone=$2
    local new_deep_clone_no_bssid=$3
    local new_config_sta=$4
    local new_map_enable=$5
    local changed="$6"
    local mode deep_clone deep_clone_no_bssid manage_vapind enabled
    local map_bss_conf

    if [ -f $WSPLCD_INIT ]; then
        config_load wsplcd
        config_get mode config 'RunMode'
        config_get deep_clone config 'DeepClone'
        config_get deep_clone_no_bssid config 'DeepCloneNoBSSID'
        config_get config_sta config 'ConfigSta'
        config_get map_enable config 'MapEnable'
        config_get map_bss_conf config 'MapGenericPolicyFile'
        config_get manage_vapind config 'ManageVAPInd'
        config_get_bool enabled config 'HyFiSecurity' 0

        if [ ! "$mode" = "$new_mode" ]; then
            uci_set wsplcd config 'RunMode' "$new_mode"
            changed=$((changed + 1))
            eval "$6='$changed'"
            uci_commit wsplcd
            __repacd_echo "Set wsplcd to $new_mode mode"
            wsplcd_restart=1
        fi

        if [ ! "$deep_clone" = "$new_deep_clone" ]; then
            uci_set wsplcd config 'DeepClone' "$new_deep_clone"
            changed=$((changed + 1))
            eval "$6='$changed'"
            uci_commit wsplcd
            __repacd_echo "Set wsplcd DeepClone=$new_deep_clone"
            wsplcd_restart=1
        fi

        if [ ! "$deep_clone_no_bssid" = "$new_deep_clone_no_bssid" ]; then
            uci_set wsplcd config 'DeepCloneNoBSSID' "$new_deep_clone_no_bssid"
            changed=$((changed + 1))
            eval "$6='$changed'"
            uci_commit wsplcd
            __repacd_echo "Set wsplcd DeepCloneNoBSSID=$new_deep_clone_no_bssid"
            wsplcd_restart=1
        fi

        if [ ! "$map_enable" = "$new_map_enable" ]; then
            uci_set wsplcd config 'MapEnable' "$new_map_enable"
            changed=$((changed + 1))
            eval "$6='$changed'"
            uci_commit wsplcd
            __repacd_echo "Set wsplcd MapEnable=$new_map_enable"
            wsplcd_restart=1
        fi

        if [ "$new_map_enable" -gt 0 ] && \
            [ ! "$map_bss_conf" = "$WSPLCD_MAP_BSS_POLICY_PATH" ]; then
            uci_set wsplcd config 'MapGenericPolicyFile' "$WSPLCD_MAP_BSS_POLICY_PATH"
            changed=$((changed + 1))
            eval "$6='$changed'"
            uci_commit wsplcd
            __repacd_echo "Set wsplcd MapGenericPolicyFile=$WSPLCD_MAP_BSS_POLICY_PATH"
            wsplcd_restart=1
        fi

        if [ ! "$manage_vapind" = "0" ]; then
            uci_set wsplcd config 'ManageVAPInd' "0"
            changed=$((changed + 1))
            eval "$6='$changed'"
            uci_commit wsplcd
            __repacd_echo "Set wsplcd ManageVAPInd=0"
            wsplcd_restart=1
        fi

        if [ ! "$enabled" -eq "$wsplcd_enabled" ]; then
            uci_set wsplcd config 'HyFiSecurity' $wsplcd_enabled
            changed=$((changed + 1))
            eval "$6='$changed'"
            uci_commit wsplcd

            __repacd_echo "Set wsplcd HyFiSecurity=$wsplcd_enabled"
            if [ "$wsplcd_enabled" -gt 0 ]; then
                __repacd_echo "Enabled security and configuration"
                wsplcd_start=1
            else
                __repacd_echo "Disabled security and configuration"
                wsplcd_stop=1
            fi
        fi

        if [ "$map_version" -gt 1 ]; then
            uci_set wsplcd config 'MapEnable' $map_version
            __repacd_echo "Set wsplcd MapEnable=$map_version"
            if [ "$map_ts_enabled" -gt 0 ]; then
                uci_set wsplcd config 'NumberOfVLANSupported' $num_vlan_supported
                uci_set wsplcd config 'Map2TrafficSepEnabled' $map_ts_enabled
                uci_set wsplcd config 'CombinedR1R2Backhaul' $map_single_r1r2_bh
            fi
            if [ "$ezmesh6g" -eq 1 ];then
                uci_set wsplcd config 'Ezmesh6GEnable' "1"
            fi
        fi
        uci_set wsplcd config 'EnableBackhaulWPS' $map_enable_backhaul_WPS
    fi
}

# Change whether mcsd is enabled and running or not
#
# input: $1 manage_mcsd: whether repacd is to manage the state of mcsd
# input: $2 enable_son: whether SON mode is enabled or not (where this could
#                       include MAP)
__repacd_config_mcsd() {
    local manage_mcsd=$1
    local enable_son=$2

    if [ "$manage_mcsd" -gt 0 ] && [ -f "$MCSD_INIT" ]; then
        if [ "$enable_son" -gt 0 ]; then
            uci_set mcsd config 'Enable' 0
            uci_commit mcsd
            # Stop mcsd and keep attempting to stop it in case it
            # is being restarted by a hotplug event.
            while pgrep mcsd;
            do
                /etc/init.d/mcsd stop
                sleep 2
            done
        else  # switching to non-SON mode, so enable mcsd
            uci_set mcsd config 'Enable' 1
            uci_commit mcsd
            /etc/init.d/mcsd start
        fi
    fi
}

# Change the hyd and/or lbd configuration based on the parameters provided
# and the allowed feature settings.
#
# input: $1 enable_steering: 1 - AP interfaces support steering;
#                            0 - they do not
# input: $2 disable_ap_steering: 1 - disable AP steering feature
# input: $3 enable_son: 1 - multi-AP SON mode should be enabled (so
#                           long as it is not prohibited by config);
#                       0 - multi-AP SON mode should not be enabled
# input: $4 son_mode: one of HYROUTER or HYCLIENT; only relevant if enable_son
#                     is 1 and SON mode is not prohibited by the config
# input-output: $5 changed: count of the changes
__repacd_configure_son() {
    local enable_steering=$1
    local disable_ap_steering=$2
    local enable_son=$3
    local son_mode="$4"
    local changed="$5"

    local enable_steering_mask enable_son_mask manage_mcsd
    config_load repacd
    config_get_bool enable_steering_mask repacd 'EnableSteering' 1
    config_get_bool enable_son_mask repacd 'EnableSON' 1
    config_get_bool manage_mcsd repacd 'ManageMCSD' 1

    # If the config does not permit steering or multi AP logic, force it
    # off.
    if [ "$enable_steering_mask" -eq 0 ]; then
        enable_steering=0
    fi

    if [ "$enable_son_mask" -eq 0 ]; then
        enable_son=0
    fi

    # If the package is not even installed, then we will fall back to
    # the uncoordinated steering mode (if enabled).
    if [ -f $HYD_INIT  ]; then
        local cur_mode hyd_enabled
        config_load $MAP
        config_get cur_mode config 'Mode'
        config_get hyd_enabled config 'Enable'
        config_get disable_steering config 'DisableSteering'

        if [ ! "$cur_mode" = "$son_mode" ]; then
            uci_set $MAP config 'Mode' "$son_mode"
            changed=$((changed + 1))
            eval "$5='$changed'"
            uci_commit $MAP
            __repacd_echo "Set hyd Mode=$son_mode"
        fi

        if [ ! "$disable_steering" = "$disable_ap_steering" ]; then
            uci_set $MAP config 'DisableSteering' "$disable_ap_steering"
            changed=$((changed + 1))
            eval "$5='$changed'"
            uci_commit $MAP
            __repacd_echo "Set hyd DisableSteering=$disable_ap_steering"
        fi

        if [ ! "$hyd_enabled" = "$enable_son" ]; then
            uci_set $MAP config 'Enable' "$enable_son"
            changed=$((changed + 1))
            eval "$5='$changed'"
            uci_commit $MAP

	    if [ "$multi_inst_enable" -eq 0 ]; then
            /etc/init.d/qrfs disable
            /etc/init.d/qrfs stop
	    fi

            __repacd_config_mcsd "$manage_mcsd" "$enable_son"

            # hyd should be started/stopped based on the hotplug hooks
            # it has installed.
            if [ "$enable_son" -gt 0 ]; then
                __repacd_echo "Enabled Wi-Fi SON mode"
                hyd_start=1
            else
                __repacd_echo "Disabled Wi-Fi SON mode"
                hyd_stop=1
            fi
        fi

        if [ "$enable_son" -gt 0 ]; then
            enable_steering=0
        fi
    fi

    if [ -f $LBD_INIT ]; then
        local lbd_enabled
        config_load $MAPLBD
        config_get lbd_enabled config 'Enable'

        if [ ! "$lbd_enabled" = "$enable_steering" ]; then
            uci_set $MAPLBD config 'Enable' "$enable_steering"
            changed=$((changed + 1))
            eval "$5='$changed'"
            uci_commit $MAPLBD

            # Start/stop is handled when Wi-Fi interfaces are reconfigured.
            if [ "$enable_steering" -gt 0 ]; then
                __repacd_echo "Enabled Wi-Fi steering"
            else
                __repacd_echo "Disabled Wi-Fi steering"
            fi
        fi
    fi
}

#Configure the bridge as rootbridge
#for controller/CAP
__repacd_set_rstp_rootbridge_prio() {
    local interface=$1
    local is_controller=$2
    local enable=0
    local brif type prio=$RSTP_DEFBRIDGE_PRIO

    config_get type "$interface" type
    [ -z "$type" -o ! "$type" = "bridge" ] && return

    network_get_physdev brif $interface

    [ -z "$brif" ] && return

    [ "$is_controller" = "HYROUTER" ] && prio=$RSTP_ROOTBRIDGE_PRIO

    err=`rstpctl showbridge $brif | cut -d' ' -f 6 `
    [ "$err" = '1002' ] && {    #Err_Bridge_RSTP_not_enabled
        echo "rstp stp enabling for $brif as err:$err" > /dev/console
        brctl stp $brif on
        rstpctl rstp $brif on
    }

    __repacd_echo "RSTP set rootbridgeprio=$prio for $brif"
    /sbin/rstpctl setbridgeprio $brif $prio
}

#Check if rstp is enabled
__is_rstp_enabled() {
    local enabled
    config_load rstp
    config_get_bool enabled global enable

    if [ "$enabled" = "1" ]; then
        rstp_enabled=1
    fi
    return 0
}

#For controller we need to set the bridgeprio as root whereas
#agent will have default bridgeprio through restart, no need to configure
__repacd_configure_rstp_root_bridgeprio() {
     local is_controller=$1
     local restart_required=$2

     . /lib/functions/network.sh
     config_load network
     config_foreach __repacd_set_rstp_rootbridge_prio interface $is_controller
 }

#Generic function to configure RSTP root bridge priority
# and manage start/stop/restart of RSTP as needed
#for both CAP & RE (SON), Controller & Agent (Ezmesh)
# Handles 3 scenarios where priority to be configured appropriately
# 1. Device Init/Boot up
#    - SON      [RE 32768(DP), CAP 4096(RP)]
#    - Ezmesh   [Agent 32768(DP), Controller 4096(RP)]
# 2. Auto Role config (Based on role change, priority to be updated)
#    - set_controller    [Controller 4096(RP)]
#    - set_agent         [Agent 32768]
#    As auto role internally calling below given restarts, priority
#    will be configured
# 3. Dynamic device mode change at runtime because of backhaul update
#   As there is a possibility of network restart from noncap to cap mode
#   rstpd would have restarted, priority update is needed.
#    - restart_in_cap_mode
#    - restart_in_noncap_mode
#    - restart_in_re_mode
#Start rstp and configure the bridge priority
__repacd_configure_rstp() {
    local is_controller
    config_load 'repacd'

    echo "RSTP Enabled, Configuring rstp" > /dev/console

    mode=$MAP
    config_load $mode
    config_get is_controller config 'Mode'

    #Handle restart, priority update
    # - Agent/RE (Default priority),
    # - CAP/Controller (Root Priority)
    {
        rstppid=`pidof rstpd`
        if [ "$rstppid" == '' ]; then
            echo "RSTP is enabled, but not running !!!" > /dev/console
            return 1
        fi

        __repacd_configure_rstp_root_bridgeprio $is_controller
    }

    return 0
}

# Set the option that indicates whether the DBDC repeater feature should be
# enabled or not.
#
# input: $1 config: section to update
# input: $2 1 - enable, 0 - disable
# input-output: $3 change counter
__repacd_config_dbdc_device() {
    local config="$1"
    local changed="$3"

    local dbdc_enable
    config_get hwmode "$config" hwmode
    config_get type "$config" type

    if [ "$hwmode" = '11ad' ] && [ "$type" = 'mac80211' ]; then
        return
    fi

    config_get dbdc_enable "$config" dbdc_enable 1
    if [ ! "$2" = "$dbdc_enable" ]; then
        uci_set wireless "$config" dbdc_enable "$2"
        changed=$((changed + 1))
        eval "$3='$changed'"
        __repacd_echo "Set radio $config to DBDC Enabled=$2"
    fi
}

# Change the configuration on the wifi-iface object to match what is desired.
# The values provided are determined by the caller based on the desired
# mode of operation (eg. QWrap/ExtAP or not).
#
# input: $1 config: section to update
# input: $2 network: only update if network matches this value
# input: $3 enable_wds: 1 - enable, 0 - disable
# input: $4 qwrap_ap: 1 - enable, 0 - disable
# input: $5 extap: 1 - enable, 0 disable
# input: $6 block_dfs_chan: 1 - block DFS channels, 0 - do not block them
# input: $7 enable_rrm: 1 - enable, 0 disable
# input: $8 re_scalingfactor: 0 - ignore, 1 to 100 valid for sta
# input: $9 default_root_dist: 0 - ignore, 255 - invalid root distance
# input: $10 cap_snr: 0 - disabled, 1-100 valid
# input-output: $11 change counter
__repacd_config_iface() {
    local config="$1"
    local device mode network enable_wds qwrap_ap extap block_dfs enable_rrm
    local re_scalingfactor root_distance cap_snr wps_pbc_skip
    local bssid="00:00:00:00:00:00"
    local num_changes=0
    local changed="${11}"
    local map_fast_onboarding
    map_fast_onboarding=$(uci get repacd.MAPConfig.MapFastOnboarding)

    config_get device "$config" device
    config_get hwmode "$device" hwmode
    config_get type "$device" type
    config_get band "$section" band

    if [ "$hwmode" = '11ad' ] && [ "$type" = 'mac80211' ] ;then
            if [ "$ezmesh" -eq 1 ]; then
                if [ "$band" = 3 ]; then
                    return
                fi
            else
                return
            fi
    fi

    config_get mode "$config" mode
    config_get network "$config" network
    config_get enable_wds "$config" wds '0'
    config_get qwrap_ap "$config" qwrap_ap '0'
    config_get extap "$config" extap '0'
    config_get block_dfs "$config" blockdfschan '0'
    config_get enable_rrm "$config" rrm '0'
    config_get re_scalingfactor "$config" re_scalingfactor '0'
    config_get root_distance "$config" root_distance '0'
    config_get cap_snr "$config" caprssi '0'
    config_get wps_pbc_skip "$config" wps_pbc_skip '0'

    if [ "$2" = "$network" ]; then
        if [ ! "$3" = "$enable_wds" ]  && [ "$mode" != "ap_smart_monitor" ]; then
            uci_set wireless "$config" wds "$3"
            num_changes=$((num_changes + 1))
            __repacd_echo "Set iface $config to WDS=$3"
        fi

        # When operating in WDS/SON mode (since we do not use the repeater
        # enhancement), set the option to skip activating WPS PBC on a
        # connected STA interface. This allows the WPS button to be pressed on
        # an RE without causing it to disconnect from its upstream device.
        if [ "$mode" = 'sta' ]; then
            if [ ! "$3" = "$wps_pbc_skip" ]; then
                uci_set wireless "$config" wps_pbc_skip "$3"
                num_changes=$((num_changes + 1))
                __repacd_echo "Set iface $config to wps_pbc_skip=$3"
            fi
        fi

        # These should only be set on AP interfaces.
        if __repacd_is_matching_mode 'ap' "$mode"; then
            if [ ! "$4" = "$qwrap_ap" ]; then
                uci_set wireless "$config" qwrap_ap "$4"
                num_changes=$((num_changes + 1))
                __repacd_echo "Set iface $config to QWrapAP=$4"
            fi

            # @todo If there are multiple 5 GHz radios, will need to figure
            #       out which can act as the backhaul.

            # Set the interface into wrap or vanilla AP mode as appropriate
            if whc_is_5g_radio $device; then
                if [ "$4" -gt 0 ]; then
                    if [ ! "$mode" = 'wrap' ]; then
                        uci_set wireless "$config" mode 'wrap'
                        num_changes=$((num_changes + 1))
                        __repacd_echo "Set iface $config mode to wrap"
                    fi
                else  # WDS or ExtAP mode
                    if [ ! "$mode" = 'ap' ]; then
                        uci_set wireless "$config" mode 'ap'
                        num_changes=$((num_changes + 1))
                        __repacd_echo "Set iface $config mode to ap"
                    fi
                fi
            fi

            if [ ! "$6" = "$block_dfs" ]; then
                uci_set wireless "$config" blockdfschan "$6"
                num_changes=$((num_changes + 1))
                __repacd_echo "Set iface $config to BlockDFSChan=$6"
            fi

            if [ ! "$7" = "$enable_rrm" ]; then
                uci_set wireless "$config" rrm "$7"
                num_changes=$((num_changes + 1))
                __repacd_echo "Set iface $config to RRM=$7"
            fi
        fi

        if [ ! "$5" = "$extap" ]; then
            uci_set wireless "$config" extap "$5"
            num_changes=$((num_changes + 1))
            __repacd_echo "Set iface $config to ExtAP=$5"
        fi

        if [ "$daisy_chain" -gt 0 ] && \
            __repacd_is_matching_mode 'sta' "$mode"; then

            # Need to resolve 2.4G & 5G BSSID, So until then configure invalid BSSID
            # to avoid association on 2.4G & 5G interface at bootup.
            # Check if bssid updated by wifimon/daisychain, needs restart
            if [ "$bssid_resolve_state" = "resolving" ]; then
                uci_set wireless "$config" bssid "$bssid"
                num_changes=$((num_changes + 1))
                __repacd_echo "Set iface $config to BSSID=$bssid"
            fi

            if [ ! "$8" = "$re_scalingfactor" ]; then
                uci_set wireless "$config" re_scalingfactor "$8"
                num_changes=$((num_changes + 1))
                __repacd_echo "Set iface $config to RE_ScalingFactor=$8"
            fi

            if [ ! "${9}" = "$cap_snr" ]; then
                uci_set wireless "$config" caprssi "${9}"
                num_changes=$((num_changes + 1))
                __repacd_echo "Set iface $config to caprssi=${9}"
            fi
        fi

        if [ "$num_changes" -gt 0 ]; then
            changed=$((changed + 1))
            eval "${10}='$changed'"
        fi
    fi
}


__repacd_bringup_interfaces() {
    local config="$1"
    local iface network disabled device 
    local interface
    config_get iface "$config" ifname
    config_get disabled "$config" disabled '0'
    config_get mode "$config" mode
    config_get network "$config" network
    config_get mld "$config" mld ''
    config_get hidden "$config" hidden
    interface="$(iw dev $iface info | grep ssid | awk -F " " '{print $2}')"

    while [ "$interface" = "Not-Associated" -a "$hidden" -eq 0 -a "$intf_up_delay_cnt" -lt 20 ]; do
        __repacd_echo "Interface iface:$iface is not up, wait for some time and try multi_up. cnt:$intf_up_delay_cnt"
        sleep 1
        intf_up_delay_cnt=$((intf_up_delay_cnt+1))
        interface=$(iwconfig $iface | grep Access | awk '{print$6}')
    done

    #if vlanmon brought down the hidden intf then only bringup itf with hidden=0
    #don't bringup intf which have hidden flag set
    if [ "$hiddenvapdown" -gt 0 ]; then
        if [ "$hidden" -eq 1 ]; then
            #skip checking for interface if assoc or not
            return
        elif [ -z "$interface" ] && [ "$hidden" -eq 0 ]; then
            __repacd_echo "iface $iface has interface $interface & hiddenvapdown is 1"
            if [ -n "$iface" -a "$disabled" -eq 0 -a "$mode" != "sta" ] && [ -z "$mld" ]; then
                __repacd_echo " ifconfig $iface up "
                ifconfig $iface up
                hapd $iface enable
            elif [ -n "$iface" -a "$disabled" -eq 0 -a "$mode" = "ap" ] && [ -n "$mld" ]; then
                __repacd_echo "wifi multi_up $mld"
                wifi multi_up $mld
            fi
        fi
    else
        __repacd_echo "Vlanmon did not bring down hidden vaps by default hiddenvapdown=0"
        #bringup all Not-Associated intf whether hidden set or not set
        if [ -z "$interface" ]; then
            __repacd_echo "iface $iface has interface $interface"
            if [ -n "$iface" -a "$disabled" -eq 0 -a "$mode" != "sta" ] && [ -z "$mld" ]; then
                __repacd_echo " ifconfig $iface up "
                ifconfig $iface up
                hapd $iface enable
            elif [ -n "$iface" -a "$disabled" -eq 0 -a "$mode" = "ap" ] && [ -n "$mld" ]; then
                __repacd_echo "wifi multi_up $mld"
                wifi multi_up $mld
            fi
        fi
    fi

}





# Restart wsplcd and the Wi-Fi interfaces based on configuration changes.
__repacd_restart_dependencies() {
    if [ "$wsplcd_stop" -gt 0 ]; then
        $WSPLCD_INIT stop
    fi

    if [ "$hyd_stop" -gt 0 ]; then
        $HYD_INIT stop
        $HYFI_BRIDGING_INIT stop
    fi

    # When hyd is being started, start the bridging hooks prior to restarting
    # the network to ensure any temporary loops are prevented.
    if [ "$hyd_start" -gt 0 ]; then
        $HYFI_BRIDGING_INIT start
    fi

    if [ "$config_changed" -gt 0 ]; then
        __repacd_echo "Restarting network stack..."

        # The below file gets created after wifi detect(config) from procd is complete.
        # By this time, all interfaces would be created.
        # Repacd should wait until this file is created & then do the whc_network_restart.
        while [ ! -f /tmp/.wifi-config-done ]; do
            sleep 1
        done

        whc_network_restart
    else
        __repacd_echo "No changes; not restarting network stack..."
        config_load repacd
        config_get traffic_separation_enabled repacd TrafficSeparationEnabled '0'
        config_get network_guest repacd NetworkGuest 'guest'
        config_get network_guestmac repacd GuestMac 'ath0'
        if [ "$traffic_separation_enabled" -gt 0 ]; then
            brguest=`ifconfig $network_guestmac | grep HWaddr | awk '{print $5}'`
            ifconfig $brprefix$network_guest hw ether "$brguest"
        fi
    fi

    # Perform Version compatibility check
    if __repacd_is_map_enabled; then
        __repacd_check_version_compatibility 'map'
    else
        __repacd_check_version_compatibility 'son'
    fi

    # To get the list of FT interfaces
    if [ "$ftauth" -eq 1 ] ; then
        config_load wireless
        config_foreach __repacd_init_vap_set_ft_params wifi-iface $1
        uci_set wsplcd config 'VapSpecificRestart' "1"
        uci_commit wsplcd
    fi

    if [ "$wsplcd_start" -gt 0 ]; then
        __repacd_echo "Starting wsplcd"
        $WSPLCD_INIT start
    elif [ "$wsplcd_restart" -gt 0 ]; then
        __repacd_echo "Restarting wsplcd"
        $WSPLCD_INIT restart
    fi

    if [ "$hyd_start" -gt 0 ]; then
        config_load wireless
        config_foreach __repacd_bringup_interfaces wifi-iface

        __repacd_echo "Starting $MAP"
        $HYD_INIT start
    fi

    __is_rstp_enabled
    [ "$rstp_enabled" -eq 1 ] && __repacd_configure_rstp
}

__repacd_set_fo() {
    local flag=$1
    uci set repacd.MAPConfig.MapFastOnboarding=$flag
    uci set repacd.MAPConfig.StartWithHighestPrefSTA=$flag
    uci set wsplcd.config.MapFastOnboarding=$flag
    uci set ezmesh.MAPConfigSettings.MapFastOnboarding=$flag
    uci commit
}

__stop_repacd_run() {
    PID=$(ps | grep repacd-run | grep -v grep | awk '{print $1}')
    if [ -n "$PID" ]; then
        kill -s SIGTERM $PID
        __repacd_echo "stopped repacd-run process $PID"
    fi
}
