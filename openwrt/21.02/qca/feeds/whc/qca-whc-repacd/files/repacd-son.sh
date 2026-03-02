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

RE_DEFAULT_RATE_ESTIMATE_SCALING_FACTOR='70'
RE_ROOT_AP_DISTANCE_INVALID='255'
vap_defconf=1
resolved_ssid='' resolved_enc='' resolved_key=''
resolved_sae_pass='' resolved_sae_group='' resolved_owe_group=''
guest_backhaul_iface=
network_backhaul="backhaul"
backhaul_ssid='' backhaul_key='' backhaul_enc=''
backhaul_sae_pass='' backhaul_sae_group='' backhaul_owe_group=''
def_backhaul_ssid="Backhaul" def_backhaul_key="1234567890" def_backhaul_enc="psk2+ccmp"
def_fh_enc="psk2+ccmp"
guest_ssid='' guest_enc='' guest_key=''
def_guest_ssid="Guest" def_guest_enc="none" def_guest_key=""
traffic_separation_active='' create_sta=0
manage_vap_ind=0
lan_vid=100 guest_vid=102
capsnr=0
Manage_front_and_back_hauls_ind=0
port_list="7878 8989 6666 9999"
used_list=""
def_lan=7777
def_guest=8888

eth_iface_wan="eth0"
eth_iface_lan="eth1"
eth_iface_wan_port=5
eth_iface_lan_port1=4
eth_iface_lan_port2=3

def_vlanid=100
brcount=0
net_ip=0 lan_ip=1 guest_ip=2 other_acc_ip=3
MAX_ACC_DEFAULT=2
MAX_ACC_MULTI_INST_SUPPORT=4

CFG_NAMES="ssid encryption key network wsplcd_unmanaged sae sae_groups sae_password owe_groups owe sae_pwe en_6g_sec_comp auth_server auth_port auth_secret rsn_preauth rsn_preauth_interfaces auth_server2 auth_port2 auth_secret2 suite_b wpa_group_rekey ieee80211w group_mgmt_cipher"

. /lib/functions/repacd-cmn.sh
. /lib/functions/hyfi-iface.sh

config_load 'repacd'
config_get def_backhaul_ssid repacd BackhaulSSID $def_backhaul_ssid
config_get_bool auto_config_enable repacd 'auto_config_enable' '0'
config_get_bool multi_inst_enable repacd 'multi_inst_enable' '0'
config_get_bool disable_sync repacd 'disable_sync' '1'
config_get managed_network repacd 'ManagedNetwork' 'lan'
config_get network_guest repacd NetworkGuest 'guest'
config_get network_employee repacd NetworkEmployee "employee"
config_get traffic_separation_enabled repacd TrafficSeparationEnabled '0'

config_load $MAP
config_get hyfi_mode 'config' 'Mode' 'HYROUTER'
# Set bridge_empty option for the given network.
# This option allows to create empty_bridge.
#
# input: $1 network name
__repacd_set_bridge_empty() {
    local name="$1"
    local bridge_empty

    config_load network

    config_get bridge_empty "$name" 'bridge_empty' 0
    if [ "$bridge_empty" -eq 0 ]; then
        uci_set network "$name" bridge_empty '1'
    fi

    uci_commit network
}

# Determine if additional network exist.
# Currently looking out for only guest network.
#
# input: $1 network name
# return: 0 if exist; otherwise non-zero
__repacd_check_additional_network_exist() {
    config_load network

    if [ -n "$network_guest" ]; then
        if __repacd_network_exist $network_guest; then
            return 0
        fi
    fi

    return 1
}

# Get backhaul interfaces for ethernet.
# output: $1 - variable into which we populate interface(eth0.102 or eth1.102).
__repacd_get_backhaul_ifaces_eth_guest() {
    local ifaces
    local ifaces_guest_intf
    ifaces=$(ifconfig 2>&1 | grep eth | grep $guest_vid)
    ifaces_guest_intf=$(echo "$ifaces" | cut -d ' ' -f1)
    eval "$1='$ifaces_guest_intf'"
}

__repacd_get_backhaul_ifaces_eth() {
    local eth_ifaces iface vlanid=$1
    eth_ifaces=$(ifconfig 2>&1 | grep eth | grep $vlanid)
    iface=$(echo "$eth_ifaces" | cut -d ' ' -f1)
    eval "$2='$iface'"
}


# Delete VLAN interfaces of backhaul vaps.
# input: $1 name of the interface config section
# input: $2 network name
# input: $3 VLAN id
# input: $4 mode 'sta' or 'ap'
# input-output: $5 change counter
__repacd_delete_vlan_interfaces() {
    local config=$1
    local id=$3
    local changed="$5"
    local ifname vlan_ifname
    local disabled mode bssid device hwmode

    config_get network "$config" network
    config_get ifname "$config" ifname
    config_get disabled "$config" disabled '0'
    config_get mode "$config" mode
    config_get device "$config" device
    config_get hwmode "$device" hwmode

    if [ "$hwmode" != "11ad" ]; then
        if [ "$network" = "$network_backhaul" ] && [ -n "$ifname" ] && \
            [ "$disabled" -eq 0 ]; then
            vlan_ifname=$(iwconfig 2>&1 | grep -o "$ifname.$id")
            if [ -n "$vlan_ifname" ]; then
               if __repacd_is_matching_mode "$4" "$mode"; then
                   vconfig rem "$ifname.$id"
                   brctl delif "br-$2" "$ifname.$id"
                   __repacd_delete_interface "$2" "$ifname.$id"
                   changed=$((changed + 1))
                   eval "$5='$changed'"
               fi
            fi
        fi
    fi
}

__repacd_delete_ethernet_vlan_interfaces() {
    local nw=$1
    local ether_ifaces ethiface
    local ifname vlan_ethname

    if [ "$nw" = "$managed_network" ]; then
        return
    fi
    ether_ifaces=$(uci get network.$nw.ifname 2&> /dev/null)
    for ifname in $ether_ifaces; do
	ethiface=$(echo $ifname | grep eth)
	if [ -n "$ethiface" ]; then
	    vlan_ethname=$(iwconfig 2>&1 | grep -o "$ethiface")
	    [ -z "$vlan_ethname" ] && continue
	    vconfig rem "$ethiface"
	    brctl delif "br-$nw" "$ethiface"
	    __repacd_delete_interface "$nw" "$ethiface"
	fi
    done
}

__repacd_create_ethernet_vlan_interfaces() {
    local nw=$1 backhaul_iface
    local vlanid=$2
    local ether_ifaces ethiface
    local ifname vlan_ethname
    local switch_iface eswitch_support switch_num switch_present

    if [ "$nw" = "$managed_network" ]; then
        return
    fi
    ether_ifaces=$(uci get network.$managed_network.ifname 2&> /dev/null)
    for ifname in $ether_ifaces; do
	ethiface=$(echo $ifname | grep eth)
	if [ -n "$ethiface" ]; then
	    vlan_ethname=$(iwconfig 2>&1 | grep -o "$ethiface.$vlanid")
	    [ -n "$vlan_ethname" ] && continue
	    vconfig add "$ethiface" "$vlanid"
	    brctl addif "br-$nw" "$ethiface.$vlanid"
	    ifconfig "$ethiface.$vlanid" up
	    __repacd_add_interface $nw "$ethiface.$vlanid"
	fi
    done
    __repacd_get_backhaul_ifaces_eth $vlanid backhaul_iface
    __hyfi_get_switch_iface switch_iface eswitch_support switch_num switch_present
    if [ -n "$backhaul_iface" -a -n "$switch_iface" ]; then
        if [ "$backhaul_iface" = "$eth_iface_wan.$vlanid" ]; then
            swconfig dev "switch$switch_num" vlan $vlanid set ports "0t ${eth_iface_wan_port}t ${eth_iface_lan_port1}t"
            swconfig dev "switch$switch_num" set apply
        elif [ "$backhaul_iface" = "$eth_iface_lan.$vlanid" ]; then
            swconfig dev "switch$switch_num" vlan $vlanid set ports "0t ${eth_iface_lan_port1}t ${eth_iface_lan_port2}t"
            swconfig dev "switch$switch_num" set apply
        fi
    fi
}

#Create Back haul interface for ethernet guest network and
#add it to the respective bridge
#input:$1 guest network name
#input-output:$2 change counter
__repacd_add_ethernet_vlan_interfaces() {
    local backhaul_ifaces_eth
    local network=$1
    local changed="$2"

    __repacd_get_backhaul_ifaces_eth_guest  backhaul_ifaces_eth
    if [ -n "$backhaul_ifaces_eth" ]; then
         __repacd_echo "Add VLAN ifaces for ethernet guest network support"
        if [ "$backhaul_ifaces_eth" = "$eth_iface_wan.$guest_vid" ] && [ "$network" = 'guest' ];then
             swconfig dev switch0 vlan $guest_vid set ports "0t ${eth_iface_wan_port}t ${eth_iface_lan_port1}t"
             swconfig dev switch0 set apply
             brctl addif "br-$network" "$backhaul_ifaces_eth"
             ifconfig "$backhaul_ifaces_eth" up
          __repacd_add_interface "$network" "$backhaul_ifaces_eth"
    elif [ "$backhaul_ifaces_eth" = "$eth_iface_lan.$guest_vid" ] && [ "$network" = 'guest' ];then
             swconfig dev switch0 vlan $guest_vid set ports "0t ${eth_iface_lan_port1}t ${eth_iface_lan_port2}t"
             swconfig dev switch0 set apply
             brctl addif "br-$network" "$backhaul_ifaces_eth"
             ifconfig "$backhaul_ifaces_eth" up
         __repacd_add_interface "$network" "$backhaul_ifaces_eth"
    else
             __repacd_echo "For Home network all the traffic is untagged,no vlan"
    fi
    #Do we really need to restart? revisit this
    changed=$((changed + 1))
    eval "$2='$changed'"
    fi

}

# Restart firewall.
__repacd_restart_firewall() {
    /etc/init.d/firewall restart
}

# Restart dnsmasq.
__repacd_restart_dnsmasq() {
    /etc/init.d/dnsmasq restart
}

# Check whether the given interface is the backhaul AP interface on
# the desired network and update the band specific interface names.
#
# input: $1 - config: the name of the interface config section
# input: $2 - network: the name of the network to which the AP interface
#                      must belong to be matched
# output: $3 - iface: the resolved backhaul AP interface name on 2.4 GHz (if found)
# output: $4 - iface: the resolved backhaul AP interface name on 5 GHz (if found)
# output: $5 - iface: the resolved backhaul AP interface name on 6 GHz (if found)
__repacd_wifi_check_and_get_backhaul_ap_iface() {
    local config="$1"
    local network_to_match="$2"
    local iface disabled mode device hwmode backhaul_ap

    config_get network "$config" network
    config_get iface "$config" ifname
    config_get disabled "$config" disabled '0'
    config_get mode "$config" mode
    config_get device "$config" device
    config_get backhaul_ap "$config" backhaul_ap '0'
    config_get hwmode "$device" hwmode
    config_get band "$device" band
    local band_freq=`iwlist $iface 2>&1 channel | grep "Current Frequency" | awk -F':' '{print $2}' | awk -F" " '{print $1}' | sed 's/\.//g' | sed -e "s/\<\([0-9]\{1,4\}\)\>/\10000/; s/\([0-9]\{4\}\)/\1/" | awk '{ print $1; }' | sed -n 's/\([0-9]\{4\}\).*/\1/p' `

    if [ "$hwmode" != "11ad" ]; then
        if [ "$network" = "$network_to_match" ] && [ -n "$iface" ] && [ "$mode" = "ap" ] \
            && [ "$backhaul_ap" -gt 0 ] && [ "$disabled" -eq 0 ]; then
            if [ "$hwmode" = "11bea" ] || [ "$hwmode" = "11axa" ] || [ "$hwmode" = "11ac" ] || [ "$hwmode" = "11na" ] || [ "$hwmode" = "11a" ]; then
                if [ "$band_freq" -gt 5950 ] && [ "$band_freq" -lt 7130 ]; then
                    eval "$5=$iface"
                fi
                if [ "$band_freq" -gt 5175 ] && [ "$band_freq" -lt 5900 ]; then
                    eval "$4=$iface"
                fi
            else
                eval "$3=$iface"
            fi
        fi
    fi
}

# Configure the otherband BSSIDs for both backhaul APs
# input: $1 - network: the name of the network to which the AP interface
#                      must belong to be matched
# output: None
__repacd_wifi_set_otherband_bssids() {
    local otherband_bssid24g1 otherband_bssid24g2 bssid_5g bssid_24g bssid_6g
    local otherband_bssid5g1 otherband_bssid5g2 otherband_bssid6g1 otherband_bssid6g2
    local backhaul_ap_iface_24g backhaul_ap_iface_5g backhaul_ap_iface_6g success=0 loop_count=30

    while [ "$success" -eq 0 ] && [ "$loop_count" -gt 0 ]; do
        config_load wireless
        config_foreach __repacd_wifi_check_and_get_backhaul_ap_iface wifi-iface "$1" \
            backhaul_ap_iface_24g backhaul_ap_iface_5g backhaul_ap_iface_6g
        if [ -n "$backhaul_ap_iface_5g" ] || [ -n "$backhaul_ap_iface_6g" ]; then
            if [ -n "$backhaul_ap_iface_5g" ] && [ -n "$backhaul_ap_iface_24g" ]; then
                bssid_5g=$(ifconfig "$backhaul_ap_iface_5g" | grep "HWaddr" | awk -F" " '{print $5}')
                bssid_24g=$(ifconfig "$backhaul_ap_iface_24g" | grep "HWaddr" | awk -F" " '{print $5}')
                otherband_bssid5g1=$(echo "$bssid_5g" | sed -e "s/://g" | cut -b 1-8)
                otherband_bssid5g2=$(echo "$bssid_5g" | sed -e "s/://g" | cut -b 9-12)
                otherband_bssid24g1=$(echo "$bssid_24g" | sed -e "s/://g" | cut -b 1-8)
                otherband_bssid24g2=$(echo "$bssid_24g" | sed -e "s/://g" | cut -b 9-12)
                cfg80211tool_mesh $backhaul_ap_iface_24g otherband_bssid 0x$otherband_bssid5g1 0x$otherband_bssid5g2
                cfg80211tool_mesh $backhaul_ap_iface_5g otherband_bssid 0x$otherband_bssid24g1 0x$otherband_bssid24g2
            fi
            if [ -n "$backhaul_ap_iface_6g" ]; then
                bssid_6g=$(ifconfig "$backhaul_ap_iface_6g" | grep "HWaddr" | awk -F" " '{print $5}')
                bssid_24g=$(ifconfig "$backhaul_ap_iface_24g" | grep "HWaddr" | awk -F" " '{print $5}')
                otherband_bssid6g1=$(echo "$bssid_5g" | sed -e "s/://g" | cut -b 1-8)
                otherband_bssid6g2=$(echo "$bssid_5g" | sed -e "s/://g" | cut -b 9-12)
                otherband_bssid24g1=$(echo "$bssid_24g" | sed -e "s/://g" | cut -b 1-8)
                otherband_bssid24g2=$(echo "$bssid_24g" | sed -e "s/://g" | cut -b 9-12)
                cfg80211tool_mesh $backhaul_ap_iface_6g otherband_bssid 0x$otherband_bssid24g1 0x$otherband_bssid24g2
                if [ -z "$backhaul_ap_iface_5g" ]; then
                cfg80211tool_mesh $backhaul_ap_iface_24g otherband_bssid 0x$otherband_bssid6g1 0x$otherband_bssid6g2
                fi
             fi
            success=1
        else
            sleep 1
            loop_count=$((loop_count - 1))
        fi
    done
}

# Enumerate all of the wifi interfaces and append
# only station devices to the variable provided.
# output: $1 devices: variable to populate with the station devices
__repacd_get_sta_devices() {
    local devices=$1

    config_cb() {
        local type="$1"
        local section="$2"

        case "$type" in
            wifi-iface)
                config_get mode "$section" mode
                config_get device "$section" device
                if [ "$mode" = 'sta' ];then
                    eval append $devices "$device"
                fi
            ;;
        esac
    }
    config_load wireless
}

# Configure the vap independent parameter for given wifi interface.
#
# input: $1 - config: interface config name to configure.
# input: $2 - devices: list of devices to match with provided interface.
__repacd_config_vap_ind() {
    local config=$1
    local devices=$2
    local match_found=0
    local device

    config_load wireless
    config_get device "$config" device

    for device_to_match in $devices; do
        if [ "$device" = "$device_to_match" ]; then
            if __repacd_is_son_mode; then
                # If in SON mode, STA vaps may be forced down based on
                # link strength so configure the VAPs in independent mode.
                # This will be used in conjunction with the other feature
                # that monitors the backhaul links and brings down the AP VAPs
                # if there is no backhaul for a sustained period of time.
                __repacd_update_vap_param "$config" 'athnewind' 1
            else
                __repacd_update_vap_param "$config" 'athnewind' 0
            fi
            match_found=1
        fi
    done

    # If the feature Manage Front-Haul VAPs independently based on
    # CAP's reachability is enabled, then set athnewind value to 1.
    if [ "$Manage_front_and_back_hauls_ind" -gt 0 ]; then
        __repacd_update_vap_param "$config" 'athnewind' 1

    # This is mainly for wsplcd optimization: if wsplcd detects athnewind
    # is set to 0, it will not restart VAP; otherwise it will bring down
    # the VAP, set athnewind to 0 and bring the VAP back up
    elif [ "$match_found" -eq 0 ]; then
        __repacd_update_vap_param "$config" 'athnewind' 0
    fi
}

# Configure the vap independent parameter for all VAPs
# only if repacd is allowed to configure this parameter.
# input: None
__repacd_config_independent_vaps() {
    local sta_devices=

    if [ "$manage_vap_ind" -gt 0 ]; then
        __repacd_get_sta_devices sta_devices
        __repacd_echo "Station device list = $sta_devices"

        config_foreach __repacd_config_vap_ind wifi-iface "$sta_devices"
    else
        # Pass empty device list, so that all vaps set to athnewind=0.
        config_foreach __repacd_config_vap_ind wifi-iface "$sta_devices"
    fi
}

# Get the configured Rate scaling factor.
# Gives default value in case of configuration miss or invalid value.
#
# output: $1 - rate_scaling_factor
__repacd_get_rate_scaling_factor() {
    local scaling_factor

    config_load 'repacd'
    config_get scaling_factor WiFiLink 'RateScalingFactor' $RE_DEFAULT_RATE_ESTIMATE_SCALING_FACTOR

    # If scaling factor out of limits, return the default value "70".
    if [ "$scaling_factor" -lt '1' ] || \
        [ "$scaling_factor" -gt '100' ]; then
        scaling_factor=$RE_DEFAULT_RATE_ESTIMATE_SCALING_FACTOR
    fi

    eval "$1=$scaling_factor"
}

# check vap is in default configuration by comparing ssid as OpenWrt
# and encryption as none,for now any one VAP with this configuration
# is enough for marking configuration as default configuration.
# Internal driver currently does not support 11ad so ignoring
# 11ad enabled devices for now.
# input: $1 config: section to update
# input: $2 network: Global variable vap_defconf.
# input-output:$2 is set to '0' in case we have any VAP
# with default config.

__repacd_check_default_vaps() {
    local config="$1"

    config_get device "$config" device
    config_get ssid  "$config" ssid
    config_get encryption "$config" encryption
    config_get hwmode "$device" hwmode
    config_get type "$device" type
    config_get_bool repacd_security_unmanaged "$config" repacd_security_unmanaged '0'

    if [ "$hwmode" = '11ad' ] && [ "$type" = 'mac80211' ] ;then
        return
    fi

    if [ "$repacd_security_unmanaged" -eq 1 ] ; then
        return
    fi

    if [ "$ssid" = "OpenWrt" ] && [ "$encryption" = "none" ] ;then
        eval "$2='0'"
    fi
}

# Determine if the VAPs are in the default configuration.
# For now this is defined simply as any VAP having the default SSID and no
# security enabled or that there are no VAPs whatsoever.
#
# return: 0 if in the default configuration; otherwise non-zero
__repacd_vaps_in_default_config() {
    local no_vaps

    # If there is no entry, uci will give an error message. We detect this
    # by looking for the string in the output.
    no_vaps=$(uci show wireless.@wifi-iface[-1] 2>&1 | grep 'Entry not found')
    [ -n "$no_vaps" ] && return 0

    config_load wireless
    config_foreach __repacd_check_default_vaps wifi-iface vap_defconf

    return $vap_defconf
}

# Reconfigure all managed VAPs and create new ones as appropriate. This is
# non-destructive in that all configuration parameters for a VAP that are not
# directly controlled by this script will not be modified.
#
# This should generally only be called when starting with a fresh
# configuration (eg. at first boot or due to a user request), as it will
# generate an SSID and passphrase. See __repacd_reset_existing_config() for
# the function used when the SSID and passphrase configured are to be
# preserved.
__repacd_reset_default_config() {
    config_load wireless
    config_foreach __repacd_resolve_vaps wifi-iface $managed_network

    # Use last three bytes of the MAC address to help make the SSID unique.
    local ssid_suffix
    __repacd_generate_ssid_suffix ssid_suffix

    # Generate a random password (which will likely be changed either through
    # cloning or by the end user).
    local random_psk
    __repacd_generate_psk random_psk

    __repacd_create_vaps "whc-$ssid_suffix" 'psk2+ccmp' "$random_psk"
    uci_commit wireless
}

__repacd_get_wifi_config() {
    local config=$1 rd_nw=$2 rd_mode=$3 wl_mode wl_nw

    config_get wl_mode "$config" mode
    config_get wl_nw "$config" network

    [ "$wl_mode" = "$rd_mode" -a "$wl_nw" = "$rd_nw"  ] && eval "$4='$config'"
}

__repacd_update_son_iface() {
    local rd_config=$1 wl_config network mode
    local cfgname="ssid encryption key network wsplcd_unmanaged mode sae sae_groups sae_password owe_groups owe auth_server auth_port auth_secret rsn_preauth rsn_preauth_interfaces auth_server2 auth_port2 auth_secret2 suite_b wpa_group_rekey ieee80211w group_mgmt_cipher"
    local name

    __repacd_get_config_value repacd "$rd_config" network mode

    [ "$mode" = "sta" -a "$network" = "backhaul" ] && return
    [ "$mode" = "ap_smart_monitor" ] && return
    config_load 'wireless'
    config_foreach __repacd_get_wifi_config wifi-iface $network $mode wl_config
    __repacd_config_copy "$cfgname" "$wl_config" "$rd_config" wireless repacd
}

__repacd_update_wifi_to_son_iface() {
    config_load 'repacd'
    config_get_bool auto_config_enable repacd 'auto_config_enable' '0'
    config_get_bool disable_sync repacd 'disable_sync' '1'

    [ "$auto_config_enable" -eq 0 ] && return
    [ "$disable_sync" -eq 1 ] && return
    config_foreach __repacd_update_son_iface son-iface
}

__repacd_update_config() {
    local file=$1 cfgname="$2" config=$3 arg=1 name value

    [ "$auto_config_enable" -eq 0 ] && return
    [ "$file" = "repacd" ] && [ "$disable_sync" -eq 1 ] && return
    for value in "$@"; do
        if [ "$value" = "$1" ] || [ "$value" = "$2" ] || [ "$value" = "$3" ]; then
            continue
        fi
        name=$(echo "$cfgname" | cut -d " " -f $arg)
        [ -z "$value" ] && uci delete "$file"."$config"."$name" 2&> /dev/null
        if [ "$name" = "sae_groups" ] || [ "$name" = "sae_password" ] || \
           [ "$name" = "owe_groups" ]; then
            uci delete "$file"."$config"."$name" 2&> /dev/null
            [ -n "$value" ] && uci add_list "$file"."$config"."$name"="$value"
        elif [ -n "$value" ]; then
            uci_set "$file" "$config" "$name" "$value"
        fi
        arg=`expr $arg + 1`
    done
    uci_commit "$file"
}

__repacd_get_default_value() {
    local config=$1 cfg_name="$2" nw random_psk mode ssid_suffix
    config_get nw "$config" network
    config_get mode "$config" mode "ap"
    case "$cfg_name" in
        ssid)
            __repacd_generate_ssid_suffix ssid_suffix
            if [ "$hyfi_mode" = "HYROUTER" ]; then
                eval "$3='whc-$ssid_suffix-$nw'"
            else
                eval "$3='whc-$ssid_suffix'"
            fi
        ;;
        encryption)
            if [ "$hyfi_mode" = "HYROUTER" ]; then
                if [ "$nw" = "$network_backhaul" ]; then
                    eval "$3='$def_backhaul_enc'"
                elif [ "$nw" = "$network_guest" ]; then
                    eval "$3='$def_guest_enc'"
                else
                    eval "$3='$def_fh_enc'"
                fi
            else
                eval "$3='none'"
            fi
        ;;
        key)
           if [ "$hyfi_mode" = "HYROUTER" ]; then
               if [ "$nw" = "$network_backhaul" ]; then
                   eval "$3='$def_backhaul_key'"
               else
                    __repacd_generate_psk random_psk
                    eval "$3='$random_psk'"
               fi
           else
                __repacd_generate_psk random_psk
                eval "$3='$random_psk'"
           fi
        ;;
        wsplcd)
            if [ "$nw" = "$network_backhaul" ]; then
                local overwrite_ap overwrite_sta
                config_load wireless
                config_get overwrite_ap 'qcawifi' 'wps_pbc_overwrite_ap_settings_all'
                config_get overwrite_sta 'qcawifi' 'wps_pbc_overwrite_sta_settings_all'
                if [ "$mode" = "ap" ] ; then
                    [ -n "$overwrite_ap" ] && eval "$3='$overwrite_ap'"
                elif [ "$mode" = "sta" ]; then
                    [ -n "$overwrite_sta" ] && eval "$3='$overwrite_sta'"
                fi
            fi
        ;;
        mode)
            eval "$3='ap'"
        ;;
        auth_server)
            eval "$3='192.168.100.50'"
        ;;
        auth_port)
           eval "$3='1812'"
        ;;
        rsn_preauth)
           eval "$3='1'"
        ;;
        rsn_preauth_interfaces)
            eval "$3='br-lan'"
        ;;
        wpa_group_rekey)
            eval "$3='300'"
        ;;
        ieee80211w)
            eval "$3='2'"
        ;;
        group_mgmt_cipher)
            eval "$3='BIP-GMAC-256'"
        ;;
    esac
}

__repacd_make_null() {
    local arg
    for arg in "$@"; do
        eval "$arg=''"
    done
}

__repacd_get_encryption_details() {
    local arg_enc=$1 arg_key=$2 arg_sae=$3 arg_sae_pass=$4
    local arg_sae_group=$5 arg_owe=$6 arg_owe_group=$7 arg_def_key=$8
    local arg_auth_server=$9 arg_auth_port=${10} arg_auth_secret=${11}
    local arg_rsn_preauth=${12} arg_rsn_preauth_interfaces=${13}
    local arg_auth_server2=${14} arg_auth_port2=${15} arg_auth_secret2=${16}
    local arg_suite_b=${17} arg_wpa_group_rekey=${18} arg_ieee80211w=${19}
    local arg_group_mgmt_cipher=${20}
    if [ "$arg_enc" = "ccmp" ]; then
        if [ -z "$arg_sae" -a -z "$arg_owe" ] ||
           [ -n "$arg_sae" -a -n "$arg_owe" ] ||
           [ "$arg_sae" -eq 0 -a "$arg_owe" -eq 0 ] ||
           [ "$arg_sae" -eq 1 -a "$arg_owe" -eq 1 ] ||
           [ -z "$arg_sae" -a "$arg_owe" -eq 0 ] ||
           [ -z "$arg_owe" -a "$arg_sae" -eq 0 ]; then
            arg_sae=1
            __repacd_make_null arg_owe arg_owe_group arg_auth_server arg_auth_port \
            arg_auth_secret arg_rsn_preauth arg_rsn_preauth_interfaces \
            arg_auth_server2 arg_auth_port2 arg_auth_secret2 arg_suite_b arg_wpa_group_rekey \
            arg_ieee80211w arg_group_mgmt_cipher
        elif [ "$arg_owe" -eq 1 ]; then
            __repacd_make_null arg_sae arg_sae_pass arg_sae_group arg_key arg_auth_server \
            arg_auth_port arg_auth_secret arg_rsn_preauth arg_rsn_preauth_interfaces \
            arg_auth_server2 arg_auth_port2 arg_auth_secret2 arg_suite_b arg_wpa_group_rekey \
            arg_ieee80211w arg_group_mgmt_cipher
            [ -z "$arg_owe_group" ] && arg_owe_group="19"
        fi
    elif [ "$arg_enc" = "psk2" ] || [ "$arg_enc" = "psk" ]; then
         [ -z "$arg_key" ] && arg_key="$arg_def_key"
        __repacd_make_null arg_sae arg_sae_pass arg_sae_group arg_key arg_owe arg_owe_group \
        arg_auth_server arg_auth_port arg_auth_secret arg_rsn_preauth arg_rsn_preauth_interfaces \
        arg_auth_server2 arg_auth_port2 arg_auth_secret2 arg_suite_b arg_wpa_group_rekey \
        arg_ieee80211w arg_group_mgmt_cipher
    elif [ "$arg_enc" = "none" ]; then
        __repacd_make_null arg_key arg_sae arg_sae_pass arg_sae_group arg_key arg_owe arg_owe_group \
        arg_auth_server arg_auth_port arg_auth_secret arg_rsn_preauth arg_rsn_preauth_interfaces \
        arg_auth_server2 arg_auth_port2 arg_auth_secret2 arg_suite_b arg_wpa_group_rekey \
        arg_ieee80211w arg_group_mgmt_cipher
    fi
    if [ "$arg_sae" -eq 1 ]; then
        [ -z "$arg_sae_group" ] && arg_sae_group="19"
        if [ "$arg_enc" = "ccmp" ]; then
            if [ -z "$arg_sae_pass" ] && [ -z "$arg_key" ]; then
                arg_sae_pass="$arg_def_key"
                arg_key="$arg_def_key"
            elif [ -z "$arg_sae_pass" ] && [ -n "$arg_key" ]; then
                arg_sae_pass="$arg_key"
            fi
             __repacd_make_null arg_key arg_owe arg_owe_group arg_auth_server arg_auth_port \
             arg_auth_secret arg_rsn_preauth arg_rsn_preauth_interfaces arg_auth_server2 \
             arg_auth_port2 arg_auth_secret2 arg_suite_b arg_wpa_group_rekey \
             arg_ieee80211w arg_group_mgmt_cipher
        elif [ "$arg_enc" = "psk2+ccmp" ] || [ "$arg_enc" = "psk+ccmp" ]; then
            if [ -z "$arg_sae_pass" ] && [ -z "$arg_key" ]; then
                arg_sae_pass="$arg_def_key"
                arg_key="$arg_def_key"
            elif [ -n "$arg_sae_pass" ] && [ -z "$arg_key" ]; then
                arg_key="$arg_sae_pass"
            elif [ -z "$arg_sae_pass" ] && [ -n "$arg_key" ]; then
                arg_sae_pass="$arg_key"
            elif [ "$arg_sae_pass" != "$arg_key" ]; then
                arg_sae_pass="$arg_key"
            fi
	    __repacd_make_null arg_owe arg_owe_group arg_auth_server arg_auth_port arg_auth_secret \
            arg_rsn_preauth arg_rsn_preauth_interfaces arg_auth_server2 arg_auth_port2 \
            arg_auth_secret2 arg_suite_b arg_wpa_group_rekey arg_ieee80211w arg_group_mgmt_cipher
        fi
    elif [ "$arg_enc" = "psk2+ccmp" ] || [ "$arg_enc" = "psk+ccmp" ]; then
        [ -z "$arg_key" ] && arg_key="$arg_def_key"
        __repacd_make_null arg_sae arg_sae_pass arg_sae_group arg_owe arg_owe_group arg_auth_server \
        arg_auth_port arg_auth_secret arg_rsn_preauth arg_rsn_preauth_interfaces arg_auth_server2 \
        arg_auth_port2 arg_auth_secret2 arg_suite_b arg_wpa_group_rekey arg_ieee80211w \
        arg_group_mgmt_cipher
    elif [ "$arg_enc" = "wpa2+ccmp" ]; then
         if [ -z "$arg_auth_secret" ] && [ -z "$arg_key" ]; then
             arg_auth_secret="$arg_def_key"
         elif [ -z "$arg_auth_secret" ] && [ -n "$arg_key" ]; then
             arg_auth_secret="$arg_key"
         fi
         __repacd_make_null arg_key arg_sae arg_sae_pass arg_sae_group arg_key arg_owe arg_owe_group \
         arg_suite_b arg_wpa_group_rekey arg_ieee80211w arg_group_mgmt_cipher
    elif [ -z "$arg_enc" ] && [ "$arg_suite_b" = "192" ]; then
        if [ -z "$arg_auth_secret" ] && [ -z "$arg_key" ]; then
             arg_auth_secret="$arg_def_key"
         elif [ -z "$arg_auth_secret" ] && [ -n "$arg_key" ]; then
             arg_auth_secret="$arg_key"
         fi
         __repacd_make_null arg_key arg_sae arg_sae_pass arg_sae_group arg_key arg_owe arg_owe_group
    fi
    eval "${21}='$arg_key'"
    eval "${22}='$arg_sae'"
    eval "${23}='$arg_sae_pass'"
    eval "${24}='$arg_sae_group'"
    eval "${25}='$arg_owe'"
    eval "${26}='$arg_owe_group'"
    eval "${27}='$arg_auth_server'"
    eval "${28}='$arg_auth_port'"
    eval "${29}='$arg_auth_secret'"
    eval "${30}='$arg_rsn_preauth'"
    eval "${31}='$arg_rsn_preauth_interfaces'"
    eval "${32}='$arg_auth_server2'"
    eval "${33}='$arg_auth_port2'"
    eval "${34}='$arg_auth_secret2'"
    eval "${35}='$arg_suite_b'"
    eval "${36}='$arg_wpa_group_rekey'"
    eval "${37}='$arg_ieee80211w'"
    eval "${38}='$arg_group_mgmt_cipher'"
}

__repacd_reset_default_config_new() {
    local cfgname="ssid encryption key wsplcd_unmanaged sae sae_groups sae_password owe_groups owe auth_server auth_port auth_secret rsn_preauth rsn_preauth_interfaces auth_server2 auth_port2 auth_secret2 suite_b wpa_group_rekey ieee80211w group_mgmt_cipher"
    local ssid encryption key wsplcd_unmanaged sae sae_groups sae_password owe_groups owe
    local config=$1 network mode is_match DEVICES device def_key
    local auth_server auth_port auth_secret rsn_preauth rsn_preauth_interfaces
    local auth_server2 auth_port2 auth_secret2 suite_b wpa_group_rekey ieee80211w
    local group_mgmt_cipher
    __repacd_get_config_value repacd "$config" network mode
    [ -z "$network" ] && return
    [ -z "$mode" ] && mode="ap"
    eval "${network}_${mode}_rdconfig=$config"
    __repacd_get_config_value repacd "$config" ssid encryption key sae_password sae_groups owe_groups \
    sae wsplcd_unmanaged owe auth_server auth_port auth_secret rsn_preauth rsn_preauth_interfaces \
    auth_server2 auth_port2 auth_secret2 suite_b wpa_group_rekey ieee80211w group_mgmt_cipher
    [ -z "$ssid" ] && __repacd_get_default_value "$config" "ssid" ssid
    [ -z "$encryption" -a -z "$suite_b" ] && __repacd_get_default_value "$config" "encryption" encryption
    [ -z "$key" ] && __repacd_get_default_value "$config" "key" def_key
    [ -z "$wsplcd_unmanaged" ] && __repacd_get_default_value "$config" "wsplcd_unmanaged" wsplcd_unmanaged
    [ -z "$auth_server" ] && __repacd_get_default_value "$config" "auth_server" auth_server
    [ -z "$auth_port" ] && __repacd_get_default_value "$config" "auth_port" auth_port
    [ -z "$rsn_preauth" ] && __repacd_get_default_value "$config" "rsn_preauth" rsn_preauth
    [ -z "$rsn_preauth_interfaces" ] && __repacd_get_default_value "$config" "rsn_preauth_interfaces" rsn_preauth_interfaces
    [ -z "$wpa_group_rekey" ] && __repacd_get_default_value "$config" "wpa_group_rekey" wpa_group_rekey
    [ -z "$ieee80211w" ] && __repacd_get_default_value "$config" "ieee80211w" ieee80211w
    [ -z "$group_mgmt_cipher" ] && __repacd_get_default_value "$config" "group_mgmt_cipher" group_mgmt_cipher

    __repacd_get_encryption_details "$encryption" "$key" "$sae" "$sae_password" "$sae_groups" \
         "$owe" "$owe_groups" "$def_key" "$auth_server" "$auth_port" "$auth_secret" "$rsn_preauth" \
         "$rsn_preauth_interfaces" "$auth_server2" "$auth_port2" "$auth_secret2" "$suite_b" \
         "$wpa_group_rekey" "$ieee80211w" "$group_mgmt_cipher" \
         key sae sae_password sae_groups owe owe_groups auth_server auth_port auth_secret \
         rsn_preauth rsn_preauth_interfaces auth_server2 auth_port2 auth_secret2 suite_b \
         wpa_group_rekey ieee80211w group_mgmt_cipher
    __repacd_get_devices DEVICES
    for device in $DEVICES; do
        __repacd_set_resolve_value "$device" "$network" "$mode" "$cfgname" "$ssid" "$encryption" "$key" \
        "$wsplcd_unmanaged" "$sae" "$sae_groups" "$sae_password" "$owe_groups" "$owe" "$auth_server" \
        "$auth_port" "$auth_secret" "$rsn_preauth" "$rsn_preauth_interfaces" "$auth_server2" \
        "$auth_port2" "$auth_secret2" "$suite_b" "$wpa_group_rekey" "$ieee80211w" "$group_mgmt_cipher"
    done
    config_foreach __repacd_resolve_wireless_config wifi-iface "$network" "$mode"
    __repacd_create_vaps_new "$network" "$mode"
}

# Enable all Wi-Fi devices
__repacd_enable_wifi() {
    local DEVICES=
    local device changed=0

    __repacd_get_devices DEVICES

    for device in $DEVICES; do
        config_get_bool disabled $device disabled
        [ -z "$disabled" ] && continue
        [ "$disabled" -eq 0 ] && continue

        uci_set wireless $device disabled '0'
        changed=1
    done

    if [ "$changed" -gt 0 ]; then
        uci_commit wireless
        config_changed=1
    fi
}

# Initialise all the additional that we created to support multi ssid traffic
# separation.
# input: $1 name: section name
# input: $2 device: name of the radio
# input: $3 mode: whether to act as a STA or AP
# input: $4 hwmode: radio hardware mode
# input: $5 network: desired network for this VAP
# input: $6 ssid: the desired SSID for this VAP
# input: $7 encryption: the desired encryption mode for this VAP
# input: $8 key: the desired passphrase for this VAP
__repacd_init_additional_vap() {
    local name=$1 device=$2 mode=$3 hwmode=$4 network=$5
    local ssid=$6 encryption=$7 key=$8
    local cur_mode repacd_security_unmanaged auto_create_vaps
    local enable_wds enable_rrm enable_qwrap_ap enable_extap block_dfs
    local rate_scaling_factor=0
    local num_changes=0
    local cur_key
    local is_owe

    __repacd_update_vap_param "$name" 'device' "$device"
    __repacd_update_vap_param "$name" 'network' "$network"

    config_get_bool repacd_security_unmanaged  "$name" repacd_security_unmanaged '0'
    if [ "$repacd_security_unmanaged" -eq 0 ] ; then
        __repacd_update_vap_param "$name" 'ssid' "$ssid"
        __repacd_update_vap_param "$name" 'encryption' "$encryption"

        config_get is_owe $name owe '0'
        # in mixed backhaul encryption eth unplug at RE will set the resolved encryption to
        # all repacd managed VAPs. There is a posibility for WPA2 VAP gets reconfigured to WPA3
        # so set sae if resolved encryption type is ccmp
        if [ "$encryption" = "ccmp" ] && [ "$is_owe" -eq 0 ]; then
            __repacd_update_vap_param "$name" 'sae' '1'
        fi

        config_get cur_key "$name" 'key'
        if [ ! "$cur_key" = "$key" ]; then
            __repacd_update_vap_param "$name" 'key' "$key"
        fi
    fi

    # using SON mode related settings to config backhaul VAPs.
    config_get_bool auto_create_vaps "$device" repacd_auto_create_vaps '1'
    if [ "$network" = "$network_backhaul" ] && [ "$auto_create_vaps" -eq 1 ]; then
        __repacd_update_vap_param "$name" 'wps_pbc_noclone' '0'
        __repacd_update_vap_param "$name" 'wps_pbc_enable' '1'
        __repacd_update_vap_param "$name" 'wps_pbc' '1'
        __repacd_update_vap_param "$name" 'wps_pbc_start_time' '61'
        __repacd_update_vap_param "$name" 'wps_pbc_duration' '120'
        __repacd_update_vap_param "$name" 'wds' '1'
        __repacd_update_vap_param "$name" 'extap' '0'
        config_load $MAP
        config_get hyfi_mode 'config' 'Mode' 'HYROUTER'
        if [ "$hyfi_mode" = "HYCLIENT" ]; then
            __repacd_update_vap_param "$name" 'root_distance' '255'
        fi
        if __repacd_is_matching_mode 'ap' "$mode"; then
            __repacd_update_vap_param "$name" 'qwrap_ap' '0'
            __repacd_update_vap_param "$name" 'rrm' '1'
            __repacd_update_vap_param "$name" 'hidden' '0'
            if __repacd_is_block_dfs; then
                __repacd_update_vap_param "$name" 'blockdfschan' '1'
            else
                __repacd_update_vap_param "$name" 'blockdfschan' '0'
            fi
        fi
    else
        __repacd_echo "Auto create VAPs disabled"
    fi
    if [ "$network" = "$network_guest" ] && [ "$auto_create_vaps" -eq 1 ]; then
        __repacd_update_vap_param "$name" 'rrm' '1'
    fi

    # Mode needs to be handled separately. If the device is already in one
    # of the AP modes and the init is requesting an AP mode, we leave it as
    # is. If it is already in the STA mode, we also leave it as is.
    config_get cur_mode "$name" 'mode'
    if ! __repacd_is_matching_mode "$mode" "$cur_mode"; then
        uci_set wireless "$name" 'mode' "$mode"
        config_changed=1
    fi
}

# Set all of the configuration parameters for the given VAP.
# input: $1 name: section name
# input: $2 device: name of the radio
# input: $3 mode: whether to act as a STA or AP
# input: $4 ssid: the desired SSID for this VAP
# input: $5 encryption: the desired encryption mode for this VAP
# input: $6 key: the desired passphrase for this VAP
# input: $7 vap_count: additional vap count created in Device
__repacd_init_vap_new() {
    local cfgname="device network mode ssid encryption key wsplcd_unmanaged sae sae_groups sae_password owe_groups owe en_6g_sec_comp sae_pwe auth_server auth_port auth_secret rsn_preauth rsn_preauth_interfaces auth_server2 auth_port2 auth_secret2 suite_b wpa_group_rekey ieee80211w group_mgmt_cipher"
    local config=$1 device=$2 mode=$3 ssid=$4 encryption=$5 key=$6 network=$7 wsplcd_unmanaged=$8
    local sae=$9 sae_groups=${10} sae_password=${11} en_6g_sec_comp=${12} sae_pwe=${13} owe_groups=${14}
    local owe=${15} auth_server=${16} auth_port=${17} auth_secrett=${18} rsn_preauth=${19} rsn_preauth_interfaces=${20}
    local auth_server2=${21} auth_port2=${22} auth_secret2=${23} suite_b=${24} wpa_group_rekey=${25} ieee80211w=${26}
    local group_mgmt_cipher=${27} cur_mode rdconfig

   __repacd_update_config wireless "$cfgname" "$config" "$device" "$network" "$mode" "$ssid" \
   "$encryption" "$key" "$wsplcd_unmanaged" "$sae" "$sae_groups" "$sae_password" \
   "$owe_groups" "$owe" "$en_6g_sec_comp" "$sae_pwe" "$auth_server" "$auth_port" \
   "$auth_secrett" "$rsn_preauth" "$rsn_preauth_interfaces" "$auth_server2" \
   "$auth_port2" "$auth_secret2" "$suite_b" "$wpa_group_rekey" "$ieee80211w" \
   "$group_mgmt_cipher"

   __repacd_init_vap_set_ssid_and_pbc "$config" "$device" "$ssid" 0 "$network" "$mode"

    rdconfig=$(eval "echo \$${network}_${mode}_rdconfig")
    __repacd_update_config repacd "$cfgname" "$rdconfig" "$device" "$network" "$mode" "$ssid" \
   "$encryption" "$key" "$wsplcd_unmanaged" "$sae" "$sae_groups" "$sae_password" \
   "$owe_groups" "$owe" "$en_6g_sec_comp" "$sae_pwe" "$auth_server" "$auth_port" \
   "$auth_secrett" "$rsn_preauth" "$rsn_preauth_interfaces" "$auth_server2" "$auth_port2" \
   "$auth_secret2" "$suite_b" "$wpa_group_rekey" "$ieee80211w" "$group_mgmt_cipher"

    config_get cur_mode "$config" 'mode'
    if ! __repacd_is_matching_mode "$mode" "$cur_mode"; then
        uci_set wireless "$config" 'mode' "$mode"
        config_changed=1
    fi
}

# Create the 4 VAPs needed (1 STA and 1 AP for each radio), with them all
# initially disabled. Three radio platforms are not currently handled.
#
# Note that if the VAPs already exist, they will be reconfigured as
# appropriate. Existing VAP section names are given by ${device}_ap and
# ${device}_sta global variables.
#
# input: $1 ssid: the SSID to use on all VAPs
# input: $2 encryption: the encryption mode to use on all VAPs
# input: $3 key: the pre-shared key to use on all VAPs
__repacd_create_vaps() {
    local ssid=$1
    local encryption=$2
    local key=$3
    local DEVICES=
    local backhaul_selected=0
    local additional_fh
    local repacd_security_unmanaged
    local is_owe
    local is_sae

    config_load $MAP
    config_load repacd

    config_get hyfi_mode 'config' 'Mode' 'HYROUTER'
    config_get additional_fh repacd 'AdditionalFHCount' '0'

    __repacd_get_devices DEVICES

    for device in $DEVICES; do
        local addi_vap=0
        if whc_is_5g_radio $device && [ "$backhaul_selected" -eq 0 ]; then
            # 5 GHz and we do not have a backhaul interface yet, so select
            # this one as the backhaul interface.
            #
            # @todo Consider which 5 GHz radio should be used for backhaul if
            #       there is more than one.
            backhaul_selected=1
        fi

        config_get_bool repacd_auto_create_vaps "$device" repacd_auto_create_vaps '1'
        uci_set wireless $device disabled '0'

        local name
        name=$(eval "echo \$${device}_ap")
        if [ -z "$name" ] && [ "$repacd_auto_create_vaps" -eq 1 ]; then
            # No existing entry; create a new one.
            name=$(uci add wireless wifi-iface)
            config_changed=1
        fi

        if [ -n "$name" ]; then
            # In case of auto config, this will be the fronthaul+backhaul AP VAP
            # However in manual config, this can be the fronthaul VAP
            # In manual cofig user sets this flag
            # We use repacd_auto_create_vaps to distinguish the two cases
            if [ "$repacd_auto_create_vaps" -eq 1 -a $traffic_separation_enabled -eq 0 ]; then
                uci_set wireless "$name" backhaul_ap '1'

                # wps_cred_add_sae parameter is needed for host wps enhc to
                # take care of overwriting credentials to Backhaul AP VAP
                if [ "$hyfi_mode" = "HYCLIENT" ]; then
                    __repacd_update_vap_param "$name" wps_cred_add_sae '1'
                fi
            fi

            __repacd_init_vap "$name" $device 'ap' "$ssid" "$encryption" "$key" "$addi_vap"

            config_get is_owe $name owe '0'
            config_get is_sae $name sae '0'
            config_get_bool repacd_security_unmanaged "$name" repacd_security_unmanaged '0'
            # in mixed backhaul encryption eth unplug at RE will set the resolved encryption to
            # all repacd managed VAPs. There is a posibility for WPA2 VAP gets reconfigured to WPA3
            # so set sae if resolved encryption type is ccmp
            if [ "$repacd_security_unmanaged" -eq 0 ] && [ "$encryption" = "ccmp" ] && [ "$is_owe" -eq 0 ]; then
                __repacd_update_vap_param "$name" 'sae' '1'
            fi
        fi

        addi_vap=1
        while [ $addi_vap -le $additional_fh ]; do
            name=$(eval "echo \$${device}_ap${addi_vap}")
            if [ -z "$name" ] && [ "$repacd_auto_create_vaps" -eq 1 ]; then
                # No existing entry; create a new one.
                name=$(uci add wireless wifi-iface)
                config_changed=1
                if [ -z "$encryption" ] || [ -z "$ssid" ]; then
                    local ssid_suffix
                    __repacd_generate_ssid_suffix ssid_suffix
                    __repacd_init_vap "$name" $device 'ap' "whc-${ssid_suffix}_${addi_vap}" "none" "$key" "$addi_vap"
                else
                    __repacd_init_vap "$name" $device 'ap' "${ssid}_${addi_vap}" "$encryption" "$key" "$addi_vap"
                fi

                __repacd_update_vap_param "$name" 'SteeringDisabled' 1
                # Additional/Extra VAPs encryption is set based on 1st VAPs encryption
                if [ "$repacd_security_unmanaged" -eq 0 ] && [ "$encryption" = "ccmp" ] || [ "$is_sae" -eq 1 ]; then
                    if [ -n "$ssid" ] && [ -n "$encryption" ]; then
                        if [ "$is_owe" -eq 0 ] && [ "$is_sae" -eq 1 ] && [ -n "$resolved_sae_pass" ]; then
                            __repacd_update_vap_param "$name" 'sae' '1'
                            uci add_list wireless."$name".sae_groups="$resolved_sae_group"
                            uci add_list wireless."$name".sae_password="$resolved_sae_pass"
                        else
                            __repacd_update_vap_param "$name" 'owe' '1'
                            uci add_list wireless."$name".owe_groups="$resolved_owe_group"
                        fi
                    fi
                fi
            fi
        addi_vap=$((addi_vap+1));
        done

        name=$(eval "echo \$${device}_sta")
        if [ -z "$name" ] && [ "$repacd_auto_create_vaps" -eq 1 ] && \
           [ "$traffic_separation_active" -eq 0 ] && [ "$hyfi_mode" = "HYCLIENT" ]; then
            # No existing entry; create a new one.
            name=$(uci add wireless wifi-iface)
            config_changed=1
        fi

        if [ -n "$name" ] && [ "$hyfi_mode" = "HYCLIENT" ]; then
            __repacd_init_vap "$name" $device 'sta' "$ssid" "$encryption" "$key" '0'

            config_get_bool repacd_security_unmanaged "$name" repacd_security_unmanaged '0'
            if [ "$repacd_security_unmanaged" -eq 0 ]; then
                uci delete wireless."$name".owe
                uci delete wireless."$name".owe_groups
                if [ "$encryption" = "ccmp" ]; then
                    uci delete wireless."$name".sae_groups
                    uci delete wireless."$name".sae_password
                    uci delete wireless."$name".key
                    if [ -n "$resolved_sae_pass" ] && [ -n "$resolved_sae_group" ]; then
                        uci_set wireless "$name" sae '1'
                        uci add_list wireless."$name".sae_groups="$resolved_sae_group"
                        uci add_list wireless."$name".sae_password="$resolved_sae_pass"
                    elif [ -n "$resolved_owe_group" ]; then
                        uci delete wireless."$name".sae
                        uci_set wireless "$name" owe '1'
                        uci add_list wireless."$name".owe_groups="$resolved_owe_group"
                    fi
                fi
            fi
        fi
    done
    uci_commit wireless
}

__repacd_create_vaps_new() {
    local network=$1 mode=$2
    local ssid encryption key wsplcd_unmanaged sae sae_groups sae_password owe_groups owe
    local auth_server auth_port auth_secret rsn_preauth rsn_preauth_interfaces
    local auth_server2 auth_port2 auth_secret2 suite_b wpa_group_rekey ieee80211w
    local group_mgmt_cipher
    local DEVICES=
    local en_6g_sec_comp sae_pwe band
    local skip_fh skipbh skipmon skipbh_ap skipbh_sta

    [ "$auto_config_enable" -eq 0 ] && return
    __repacd_get_devices DEVICES
    for device in $DEVICES; do
	config_get band $device band '0'
	config_get skipfh $device repacd_skipfh '0'
	config_get skipbh $device repacd_skipbh '0'
	config_get skipmon $device repacd_skipmon '0'
	config_get skipbh_ap $device repacd_skipbh_ap '0'
	config_get skipbh_sta $device repacd_skipbh_sta '0'

        if [ "$skipfh" -eq 1 ]; then
            [ "$network" != "backhaul" -a "$mode" != "ap_smart_monitor" ] && continue
        fi
        [ "$skipbh" -eq 1 -a "$network" = "backhaul" ] && continue
	[ "$skipmon" -eq 1 -a "$mode" = "ap_smart_monitor" ] && continue
	[ "$skipbh_ap" -eq 1 -a "$network" = "backhaul" -a "$mode" = "ap" ] && continue
	[ "$skipbh_sta" -eq 1 -a "$network" = "backhaul" -a "$mode" = "sta" ] && continue
        uci_set wireless $device disabled '0'

        local name

        name=$(eval "echo \$${device}_${network}_${mode}")
        if [ -z "$name" ] ; then
                name=$(uci add wireless wifi-iface)
                config_changed=1
        fi
        __repacd_get_resolve_value "$device" "$network" "$mode" ssid encryption key sae sae_password \
        sae_groups owe_groups wsplcd_unmanaged owe auth_server auth_port auth_secret rsn_preauth \
        rsn_preauth_interfaces auth_server2 auth_port2 auth_secret2 suite_b wpa_group_rekey \
        ieee80211w group_mgmt_cipher

	if [ "$band" -eq 3 ]; then
            if [ "$encryption" = "ccmp" ]; then
                [ -z "$en_6g_sec_comp" ] && en_6g_sec_comp=1
		[ -z "$sae_pwe" ] && sae_pwe=1
	    else
		[ -z "$en_6g_sec_comp" ] && en_6g_sec_comp=0
                [ -z "$sae_pwe" ] && sae_pwe=0
	    fi
	    [ "$network" = "$network_backhaul" ] && __repacd_update_vap_param "$name" 'ul_hyst' '3'
	fi
        if [ "$network" = "$network_backhaul" -a "$mode" != "ap_smart_monitor" ]; then
            __repacd_update_vap_param "$name" 'wps_pbc_noclone' '0'
            __repacd_update_vap_param "$name" 'wps_pbc_enable' '1'
            __repacd_update_vap_param "$name" 'wps_pbc' '1'
            __repacd_update_vap_param "$name" 'wps_pbc_start_time' '61'
            __repacd_update_vap_param "$name" 'wps_pbc_duration' '180'
            __repacd_update_vap_param "$name" 'wds' '1'
	    __repacd_update_vap_param "$name" 'rept_spl' '1'
            [ "$hyfi_mode" = "HYCLIENT" ] && __repacd_update_vap_param "$name" 'root_distance' '255'
            if __repacd_is_matching_mode 'ap' "$mode"; then
                __repacd_update_vap_param "$name" 'qwrap_ap' '0'
                __repacd_update_vap_param "$name" 'rrm' '1'
                __repacd_update_vap_param "$name" 'hidden' '0'
                if __repacd_is_block_dfs; then
                    __repacd_update_vap_param "$name" 'blockdfschan' '1'
                else
                    __repacd_update_vap_param "$name" 'blockdfschan' '0'
                fi
            fi
        fi

        if [ -n "$name" ]; then
            [ "$network" = "backhaul" -a "$mode" = "ap" ] && uci_set wireless "$name" backhaul_ap '1'
	    if [ "$mode" = "ap_smart_monitor" ]; then
		__repacd_update_vap_param "$name" 'disable' '0'
		__repacd_update_vap_param "$name" 'neighbourfilter' '1'
	    fi
	    [ -z "$mode" ] && mode="ap"
	    __repacd_init_vap_new "$name" $device "$mode" "$ssid" "$encryption" "$key" "$network" \
		"$wsplcd_unmanaged" "$sae" "$sae_groups" "$sae_password" "$en_6g_sec_comp" \
		"$sae_pwe" "$owe_groups" "$owe" "$auth_server" "$auth_port" "$auth_secret" \
                "$rsn_preauth" "$rsn_preauth_interfaces" "$auth_server2" "$auth_port2" \
                "$auth_secret2" "$suite_b" "$wpa_group_rekey" "$ieee80211w" "$group_mgmt_cipher"
        fi
    done
    uci_commit wireless
}

# Create the additional vaps required for multi ssid traffic separation.
# 1 guest vap and 1 spcl vap on each radio. Currently no sta vaps created.
# spcl vap on 2.4GHz band will be disabled.
# Note that if the VAPs already exist, they will be reconfigured as
# appropriate. Existing VAP section names are given by
# ${device}_(backhaul/guest)_ap and
# ${device}_(backhaul/guest)_sta global variables.
#
# input: $1 network: network to which this vap belongs
# input: $2 ssid: the SSID to use on all VAPs for the provided network
# input: $3 encryption: the encryption mode to use on all VAPs
# input: $4 key: the pre-shared key to use on all VAPs
__repacd_create_additional_vaps() {
    local ssid=$2
    local encryption=$3
    local key=$4
    local network=$1
    local DEVICES=
    local no_vlan_val
    local hwmode marker

    if [ "$network" = "$network_backhaul" ]; then
        marker="backhaul"
    elif [ "$network" = "$network_guest" ]; then
        marker="guest"
    fi

    config_get hyfi_mode 'config' 'Mode' 'HYROUTER'

    __repacd_get_devices DEVICES

    for device in $DEVICES; do
        config_get_bool repacd_auto_create_vaps "$device" repacd_auto_create_vaps '1'
        uci_set wireless $device disabled '0'
        config_get hwmode $device hwmode
        config_get no_vlan_val $device no_vlan '0'
        if [ "$no_vlan_val" -eq 0 ]; then
            uci_set wireless $device no_vlan '1'
        fi

        local name_managed
        name_managed=$(eval "echo \$${device}_ap")
        local name
        name=$(eval "echo \$${device}_${marker}_ap")
        if [ -z "$name" ] && [ "$repacd_auto_create_vaps" -eq 1 ]; then
            # No existing entry; create a new ap.
            name=$(uci add wireless wifi-iface)
            config_changed=1
        fi

        if [ -n "$name" ]; then
            if [ "$network" = "$network_backhaul" ]; then
                uci_set wireless "$name" rept_spl '1'
                if [ "$repacd_auto_create_vaps" -eq 1 ]; then
                    uci_set wireless "$name" backhaul_ap '1'

                    # wps_cred_add_sae parameter is required for host wps enhc to
                    # take care of overwriting credentials to Backhaul AP VAP
                    if [ "$hyfi_mode" = "HYCLIENT" ]; then
                        __repacd_update_vap_param "$name" wps_cred_add_sae '1'
                    fi
                fi

                if [ -n "$name_managed" ]; then
                    uci_set wireless "$name"_managed backhaul_ap '0'
                fi
            fi

            __repacd_init_additional_vap "$name" $device 'ap' "$hwmode" "$network" \
                                        "$ssid" "$encryption" "$key"
        fi

        if [ $create_sta -eq 1 ]; then
            name=$(eval "echo \$${device}_${marker}_sta")
            if [ -z "$name" ] && [ "$repacd_auto_create_vaps" -eq 1 ]; then
                # No existing entry; create a new sta.
                name=$(uci add wireless wifi-iface)
                config_changed=1
            fi

            if [ -n "$name" ]; then
                __repacd_init_additional_vap "$name" $device 'sta' "$hwmode" "$network" \
                                            "$ssid" "$encryption" "$key"
            fi
        fi
    done
    uci_commit wireless
}

# Reconfigure the STA vaps of managed network.
# After restarting in Non CAP mode we reconfigure the sta vaps
# to be part of backhaul network. Network,ssid and credential
# are changed other configuration remains same.
#
# input: $1 config: section name
# input: $2 : current network
# input: $3 : backhaul network
# input: $4 ssid_val: backhaul ssid
# input: $5 enc_val: backhaul encryption
# input: $6 key_val: backhaul key
__repacd_reconfig_sta_vaps() {
    local config=$1
    local network
    local ssid_val="$4"
    local enc_val="$5"
    local key_val="$6"

    config_get network "$config" network
    if [ "$2" = "$network" ] || [ "$3" = "$network" ]; then
        local mode device hwmode type_val disabled repacd_security_unmanaged

        config_get mode "$config" mode
        config_get device "$config" device
        config_get hwmode "$device" hwmode
        config_get type_val "$device" type
        config_get disabled "$config" disabled 0
        config_get_bool repacd_security_unmanaged  "$config" repacd_security_unmanaged '0'

        if [ "$hwmode" = '11ad' ] && [ "$type_val" = 'mac80211' ] ;then
            return
        fi

        if [ "$mode" = "sta" ]; then
            uci_set wireless "$config" rept_spl '1'
            __repacd_init_additional_vap "$config" $device 'sta' "$hwmode" "$3" \
                                            "$ssid_val" "$enc_val" "$key_val"

            if [ "$repacd_security_unmanaged" -eq 0 ]; then
            # On Eth unplug if STA has WPA3 encryption then copy WPA3 credentials from BH AP to STA VAP
                uci delete wireless."$config".owe_groups
                uci delete wireless."$config".owe
                if [ "$enc_val" = "ccmp" ]; then
                    uci delete wireless."$config".sae_groups
                    uci delete wireless."$config".sae_password
                    uci delete wireless."$config".key
                    if [ -n "$backhaul_sae_pass" ] && [ -n "$backhaul_sae_group" ]; then
                        uci_set wireless "$config" sae '1'
                        uci add_list wireless."$config".sae_groups="$backhaul_sae_group"
                        uci add_list wireless."$config".sae_password="$backhaul_sae_pass"
                    elif [ -n "$backhaul_owe_group" ]; then
                        uci delete wireless."$config".sae
                        uci_set wireless "$config" owe '1'
                        uci add_list wireless."$config".owe_groups="$backhaul_owe_group"
                    fi
                fi
            fi
        fi
    fi
}

__repacd_set_sta_vap() {
    local config=$1 state=$2 wlmode

    config_get wlmode "$config" mode
    [ -z "$wlmode" ] && return
    if [ "$wlmode" = "sta" ] ; then
	uci_set wireless "$config" disabled "$state"
	eval "$3='1'"
    fi
}

# Detect which VAPs are already configured and their corresponding SSID and
# passphrase.
#
# input: $1 config: section name
# input: $2 network: network for which to update VAPs
#
# Updates $resolved_ssid, $resolved_enc, and $resolved_key as appropriate.
__repacd_resolve_vaps() {
    local config="$1"
    local network
    local find_additional_ap=1
    local additional_fh
    local is_sae

    config_load repacd
    config_load wireless
    config_get network "$config" network
    config_get additional_fh repacd 'AdditionalFHCount' '0'

    if [ "$2" = "$network" ]; then
        local device mode ssid_val encryption_val key_val sae_pass sae_group

        config_get device "$config" device
        config_get mode "$config" mode
        config_get ssid_val "$config" ssid
        config_get encryption_val "$config" encryption
        config_get key_val "$config" key
        config_get disabled "$config" disabled 0
        config_get_bool repacd_security_unmanaged "$config" repacd_security_unmanaged '0'
        config_get hwmode "$device" hwmode
        config_get type "$device" type
        config_get sae_pass "$config" sae_password
        config_get sae_group "$config" sae_groups
        config_get owe_group "$config" owe_groups
        config_get is_sae "$config" sae '0'

        if [ "$hwmode" = '11ad' ] && [ "$type" = 'mac80211' ] ;then
             return
        fi

        local default_ap
        default_ap=$(eval "echo \$${device}_ap")
        # Remember the section name for this radio in this mode.
        if __repacd_is_matching_mode 'ap' "$mode"; then
            if [ -z "$default_ap" ]; then
                eval "${device}_ap=$config"
                find_additional_ap=0
            fi
        elif [ "$mode" = "sta" ]; then
            eval "${device}_sta=$config"
            find_additional_ap=0
        fi

        if [ "$find_additional_ap" -gt 0 -a "$network" = "$managed_network" ]; then
            addi_vap=1
            while [ $addi_vap -le $additional_fh ]; do
               default_ap=$(eval "echo \$${device}_ap${addi_vap}")
               # Remember the section name for this radio in this mode.
               if __repacd_is_matching_mode 'ap' "$mode"; then
                   if [ -z "$default_ap" ]; then
                       eval "${device}_ap${addi_vap}=$config"
                       break;
                   fi
               fi
               addi_vap=$((addi_vap+1))
            done
        fi

        # Do not store the credentials of additional FH & unmanaged FH
        if [ "$repacd_security_unmanaged" -eq 1 ] || [ "$find_additional_ap" -gt 0 ]; then
            return
        fi
        # Since there is really no way to know which SSID, encryption mode, or
        # passphrase to use, we will only store the first one unless we come
        # across an enabled STA interface in which case we will prefer that.
        # The reason for this is because if WPS is used without wsplcd, only
        # the STA interface will have the correct SSID and passphrase once the
        # process completes. When switching from pure client mode to RE mode,
        # we want to propagate those values to the AP interfaces and the other
        # STA interface.

        # Above is old behavior, New changes are to copy AP vap configuration
        # to STA vap interface to handle eth plug and unplug case. Initial
        # connection sta can be done with wps and CAP will clone proper BH config.
        # to RE via wsplcd.
        if [ -n "$ssid_val" ] && [ -z "$resolved_ssid" ] || [ "$mode" = "ap" ]; then
            if [ "$disabled" -eq 0 ]; then
                resolved_ssid="$ssid_val"
            fi
        fi

        if [ -n "$encryption_val" ] && [ -z "$resolved_enc" ] || [ "$mode" = "ap" ]; then
            if [ "$disabled" -eq 0 ]; then
                resolved_enc="$encryption_val"
            fi
        fi

        if [ -z "$resolved_key" ] || [ "$mode" = "ap" ]; then
            if [ "$disabled" -eq 0 ] && [ -n "$key_val" ]; then
                resolved_key="$key_val"
            fi
        fi

        if [ "$encryption_val" = "ccmp" ] || [ "$is_sae" -eq 1 ] && [ "$mode" = "ap" ]; then
            if [ -n "$sae_pass" ] && [ -n "$sae_group" ] && [ "$disabled" -eq 0 ]; then
                resolved_sae_pass="$sae_pass"
                resolved_sae_group="$sae_group"
            fi
        fi

        if [ "$encryption_val" = "ccmp" ] && [ "$mode" = "ap" ]; then
            if [ -n "$owe_group" ] && [ "$disabled" -eq 0 ]; then
                resolved_owe_group="$owe_group"
            fi
        fi
    fi
}

__repacd_get_repacd_config() {
    local config=$1 arg_nw=$2 arg_mode=$3 network mode

    __repacd_get_config_value repacd "$config" network mode
    [ "$arg_nw" = "$network" -a "$arg_mode" = "$mode" ] && eval "$4='$config'"
}

__repacd_get_config_value() {
    local file=$1 config=$2 name

    for name in "$@"; do
        [ "$name" = "$config" ] && continue
        value=$(uci get "$file"."$config"."$name" 2&> /dev/null)
        eval "$name='$value'"
    done
}

__repacd_set_resolve_value() {
    local device=$1 network=$2 mode=$3 cfgname=$4 value resvalue arg=1 name

    for value in "$@"; do
        if [ "$value" = "$1" ] || [ "$value" = "$2" ] || [ "$value" = "$3" ] ||
           [ "$value" = "$4" ]; then
            continue
        fi
        name=$(echo "$cfgname" | cut -d " " -f $arg)
        [ -n "$value" ] && eval "${device}_${network}_${mode}_${name}=$value"
        arg=`expr $arg + 1`
    done
}

__repacd_get_resolve_value() {
    local device=$1 network=$2 mode=$3 resvalue name
    for name in "$@"; do
        if [ "$name" = "$1" ] || [ "$name" = "$2" ] || [ "$name" = "$3" ]; then
            continue
        fi
        resvalue=$(eval "echo \$${device}_${network}_${mode}_${name}")
        eval "$name='$resvalue'"
    done
}

__repacd_resolve_wireless_config() {
    local config=$1
    local device network mode default_ap
    [ "$auto_config_enable" -eq 0 ] && return
    __repacd_get_config_value wireless "$config" network mode
    if [ "$2" = "$network" ] && [ "$3" = "$mode" ] ; then
        __repacd_get_config_value wireless "$config" device
        config_get hwmode "$device" hwmode
        config_get type "$device" type
        if [ "$hwmode" = '11ad' ] && [ "$type" = 'mac80211' ] ;then
             return
        fi
        default_ap=$(eval "echo \$${device}_${network}_${mode}")
        if [ -z "$default_ap" ]; then
             eval "${device}_${network}_${mode}=$config"
        fi
    fi
}

__repacd_resolve_vaps_new() {
    local cfgname="ssid encryption key wsplcd_unmanaged sae sae_groups sae_password owe_groups owe auth_server auth_port auth_secret rsn_preauth rsn_preauth_interfaces auth_server2 auth_port2 auth_secret2 suite_b wpa_group_rekey ieee80211w group_mgmt_cipher"
    local ssid encryption key wsplcd_unmanaged sae sae_groups sae_password owe_groups owe 
    local auth_server auth_port auth_secret rsn_preauth rsn_preauth_interfaces
    local auth_server2 auth_port2 auth_secret2 suite_b wpa_group_rekey ieee80211w
    local group_mgmt_cipher
    local config="$1" network mode DEVICES device default_ap config_name

    [ "$auto_config_enable" -eq 0 ] && return
    __repacd_get_config_value wireless "$config" network mode
    if [ "$2" = "$network" ] && [ "$3" = "$mode" ] ; then
        local device ssid encryption key sae_password sae_groups
	local wsplcd_unmanaged owe owe_groups sae default_ap
        local auth_server auth_port auth_secret rsn_preauth rsn_preauth_interface
        local auth_server2 auth_port2 auth_secret2 suite_b wpa_group_rekey ieee80211w
        local group_mgmt_cipher
        __repacd_get_config_value wireless "$config" device disabled
        config_get hwmode "$device" hwmode
        config_get type "$device" type

        if [ "$hwmode" = '11ad' ] && [ "$type" = 'mac80211' ] ;then
             return
        fi
        __repacd_get_config_value wireless "$config" ssid encryption key sae_password sae_groups owe_groups \
        sae wsplcd_unmanaged owe local auth_server auth_port auth_secret rsn_preauth rsn_preauth_interface \
        auth_server2 auth_port2 auth_secret2 suite_b wpa_group_rekey ieee80211w group_mgmt_cipher
        default_ap=$(eval "echo \$${device}_${network}_${mode}")
        if [ -z "$default_ap" ]; then
             eval "${device}_${network}_${mode}=$config"
        fi
	if [ -z "$disabled" ] || [ "$disabled" -eq 0 ]; then
	    eval "$4='1'"
            __repacd_set_resolve_value "$device" "$network" "$mode" "$cfgname" "$ssid" "$encryption" "$key" \
            "$wsplcd_unmanaged" "$sae" "$sae_groups" "$sae_password" "$owe_groups" "$owe" "$auth_server" \
            "$auth_port" "$auth_secret" "$rsn_preauth" "$rsn_preauth_interface" "$auth_server2" \
            "$auth_port2" "$auth_secret2" "$suite_b" "$wpa_group_rekey" "$ieee80211w" "$group_mgmt_cipher"
	fi
    fi
}

# Detect which additional VAPs are already configured.
#
# input: $1 config: section name
# input: $2 network: network for which to update VAPs
__repacd_resolve_additional_vaps() {
    local config="$1"
    local network marker
    local sae_pass sae_group

    config_get network "$config" network
    if [ "$2" = "$network" ]; then
        local device mode hwmode type_val encryption_val key_val

        if [ "$network" = "$network_backhaul" ]; then
            marker="backhaul"
        elif [ "$network" = "$network_guest" ]; then
            marker="guest"
        fi

        config_get device "$config" device
        config_get mode "$config" mode
        config_get ssid_val "$config" ssid
        config_get encryption_val "$config" encryption
        config_get key_val "$config" key
        config_get disabled "$config" disabled 0
        config_get hwmode "$device" hwmode
        config_get type_val "$device" type
        config_get sae_pass "$config" sae_password
        config_get sae_group "$config" sae_groups
        config_get owe_group "$config" owe_groups

        if [ "$hwmode" = '11ad' ] && [ "$type_val" = 'mac80211' ] ;then
             return
        fi

        # Remember the section name for this radio in this mode.
        if __repacd_is_matching_mode 'ap' "$mode"; then
            eval "${device}_${marker}_ap=$config"
        elif [ "$mode" = "sta" ]; then
            eval "${device}_${marker}_sta=$config"
        fi

        if [ -n "$ssid_val" ] && [ "$disabled" -eq 0 ]; then
            if [ "$network" = "$network_backhaul" ]; then
                if [ "$backhaul_ssid" = "$def_backhaul_ssid" ] || [ "$mode" = "ap" ]; then
                    backhaul_ssid="$ssid_val"
                fi
            elif [ "$network" = "$network_guest" ]; then
                if [ "$guest_ssid" = "$def_guest_ssid" ]; then
                    guest_ssid="$ssid_val"
                fi
            fi
        fi

        if [ -n "$encryption_val" ] && [ "$disabled" -eq 0 ]; then
            if [ "$network" = "$network_backhaul" ]; then
                if [ "$backhaul_enc" = "$def_backhaul_enc" ] || [ "$mode" = "ap" ]; then
                    backhaul_enc="$encryption_val"
                fi
            elif [ "$network" = "$network_guest" ]; then
                if [ "$guest_enc" = "$def_guest_enc" ]; then
                    guest_enc="$encryption_val"
                fi
            fi
        fi

        if [ -n "$key_val" ] && [ "$disabled" -eq 0 ]; then
            if [ "$network" = "$network_backhaul" ]; then
                if [ "$backhaul_key" = "$def_backhaul_key" ] || [ "$mode" = "ap" ]; then
                    backhaul_key="$key_val"
                fi
            elif [ "$network" = "$network_guest" ]; then
                if [ "$guest_key" = "$def_guest_key" ]; then
                    guest_key="$key_val"
                fi
            fi
        fi

        if [ -n "$sae_pass" ] && [ -n "$sae_group" ] && [ "$disabled" -eq 0 ] && [ "$mode" = "ap" ]; then
            if [ "$network" = "$network_backhaul" -a "$encryption_val" = "ccmp" ]; then
                backhaul_sae_pass="$sae_pass"
                backhaul_sae_group="$sae_group"
            fi
        fi

        if [ -n "$owe_group" ] && [ "$disabled" -eq 0 ] && [ "$mode" = "ap" ]; then
            if [ "$network" = "$network_backhaul" -a "$encryption_val" = "ccmp" ]; then
                backhaul_owe_group="$owe_group"
            fi
        fi
    fi
}

# Configure the additional VAPs needed to be consistent with the configuration that
# would be produced if starting from a default configuration. If any VAPs
# need to be created, use the SSID with suitable suffix, encryption mode, and passphrase
# from the managed network vaps
__repacd_reset_additional_config() {
    [ "$auto_config_enable" -eq 1 ] && return
    config_load wireless
    config_foreach __repacd_resolve_additional_vaps wifi-iface $network_guest
    config_foreach __repacd_resolve_additional_vaps wifi-iface $network_backhaul

    __repacd_create_additional_vaps $network_backhaul "$backhaul_ssid" "$backhaul_enc" \
                                   "$backhaul_key"
    __repacd_create_additional_vaps $network_guest "$guest_ssid" "$guest_enc" \
                                   "$guest_key"
}

# Configure the 4 VAPs needed to be consistent with the configuration that
# would be produced if starting from a default configuration. If any VAPs
# need to be created, carry over the SSID, encryption mode, and passphrase
# from one of the existing ones.
__repacd_reset_existing_config() {
    config_load wireless
    config_foreach __repacd_resolve_vaps wifi-iface $managed_network

    __repacd_create_vaps "$resolved_ssid" "$resolved_enc" "$resolved_key"
}

__repacd_reset_existing_config_new() {
    local cfgname="ssid encryption key network wsplcd_unmanaged mode sae sae_groups sae_password owe_groups owe auth_server auth_port auth_secret rsn_preauth rsn_preauth_interface auth_server2 auth_port2 auth_secret2 suite_b wpa_group_rekey ieee80211w group_mgmt_cipher"
    local config=$1 network mode is_match

    __repacd_get_config_value repacd "$config" network mode
    [ -z "$network" ] && return
    [ -z "$mode" ] && mode="ap"
    if [ "$disable_sync" -eq 0 ]; then
        __repacd_reset_default_config_new "$config"
    else
        config_load wireless
        config_foreach __repacd_resolve_vaps_new wifi-iface $network $mode is_match
        if [ "$is_match" -eq 1 ]; then
            __repacd_create_vaps_new "$network" "$mode"
        fi
    fi
}

# Set invalid bssid to make sure
# when there is change in RE role
# bssid is getting reset.
__repacd_delete_bssid() {
    local config="$1"
    local mode network disabled
    local bssid="00:00:00:00:00:00"

    config_get device "$config" device
    config_get hwmode "$device" hwmode
    config_get type "$device" type

    config_get mode "$config" mode
    config_get network "$config" network
    config_get disabled "$config" disabled 0

    if [ "$mode" = "sta" ]; then
        uci_set wireless "$config" bssid "$bssid"
        __repacd_echo "Set VAP $config to bssid=$bssid"
    fi
}

# Delete the bssid entry from the given STA interface.
#
# input: $1 config: section name
# input: $2 network: network being managed
# output: $3 config_changed: number of configurations changed
__repacd_delete_bssid_entry() {
    local config="$1"
    local network_to_match="$2"
    local changed="$3"

    local device hwmode network mode bssid

    config_get device "$config" device
    config_get hwmode "$device" hwmode
    config_get network "$config" network
    config_get type "$device" type

    if [ "$hwmode" = '11ad' ] && [ "$type" = 'mac80211' ] \
        || [ "$network" != "$network_to_match" ]; then
        return
    fi

    config_get mode "$config" mode

    if __repacd_is_matching_mode 'sta' "$mode"; then
        config_get bssid "$config" bssid ''
        if [ -n "$bssid" ]; then
            __repacd_echo "Deleting BSSID $bssid"
            uci delete "wireless.${config}.bssid"
            changed=$((changed + 1))
            eval "$3='$changed'"
        fi
    fi
}

# Change the configuration on the wifi-device object to match what is desired
# (either QWrap enabled or disabled based on the second argument).
#
# input: $1 config: section to update
# input: $2 1 - enable, 0 - disable
# input-output: $3 change counter
__repacd_config_qwrap_device() {
    local config="$1"
    local mode network
    local changed="$3"

    # @todo This will need to be updated for 3 radio configurations. The
    #       qwrap_enable should be set for the radio with the backhaul and
    #       qwrap_dbdc_enable should be set for the radios with only an AP
    #       interface.
    config_get hwmode "$config" hwmode
    config_get type "$config" type

    if [ "$hwmode" = '11ad' ] && [ "$type" = 'mac80211' ]; then
        return
    fi

    if whc_is_5g_radio "$1"; then
        local qwrap_enable
        config_get qwrap_enable "$config" qwrap_enable
        if [ ! "$2" = "$qwrap_enable" ]; then
            uci_set wireless "$config" qwrap_enable "$2"
            changed=$((changed + 1))
            eval "$3='$changed'"
            __repacd_echo "Set radio $config to QWrap Enabled=$2"
        fi
    else   # must be 2.4 GHz
        local qwrap_dbdc_enable
        config_get qwrap_dbdc_enable "$config" qwrap_dbdc_enable
        if [ ! "$2" = "$qwrap_dbdc_enable" ]; then
            uci_set wireless "$config" qwrap_dbdc_enable "$2"
            changed=$((changed + 1))
            eval "$3='$changed'"
            __repacd_echo "Set radio $config to QWrap DBDC Enabled=$2"
        fi
    fi
}

# Enable or disable the WPS Push Button Configuration Range Extender
# enhancement based on the current configuration.
# input: $1 force_cap_mode - whether to act as gateway connected even without
#                            a WAN interface
# input-output: $2 change count
__repacd_config_wps_pbc_enhc() {
    local force_gwcon_mode=$1
    local changed="$2"
    local cur_enable

    if __repacd_gw_mode || [ "$force_gwcon_mode" -gt 0 ] || \
       __repacd_is_wds_mode || __repacd_is_son_mode; then
        if [ "$traffic_separation_enabled" -gt 0 ]; then
            enable_wps_pbc_enhc=1
        else
            enable_wps_pbc_enhc=0
        fi
    else
        # Must be QWRAP or ExtAP mode, where we want distinct SSIDs for the
        # RE interfaces.
        enable_wps_pbc_enhc=1
    fi

    config_load wireless
    config_get cur_enable qcawifi wps_pbc_extender_enhance '0'

    if [ ! "$enable_wps_pbc_enhc" = "$cur_enable" ]; then
        # Create the section if it does not exist.
        uci set wireless.qcawifi=qcawifi
        uci_set wireless qcawifi wps_pbc_extender_enhance $enable_wps_pbc_enhc

        __repacd_echo "Set qcawifi.wps_pbc_extender_enhance=$enable_wps_pbc_enhc"
        changed=$((changed + 1))
        eval "$2='$changed'"
    fi
}

__repacd_config_son_iface() {
    local config=$1 config_changed
    local network mode
    config_get network "$config" network
    config_get mode "$config" mode
    if [ -n "$network" ]; then
        if [ "$traffic_separation_active" -eq 0 ] && \
           [ "$traffic_separation_enabled" -eq 0 ] && \
           [ "$managed_network" != "$network" ]; then
            return
        fi
        if [ "$mode" = "ap_smart_monitor" ]; then
            return
        fi
        config_load 'wireless'
        config_foreach __repacd_config_iface wifi-iface \
            $network $2 $3 $4 $5 $6 $7 $8 $9 config_changed
        eval "${10}='$config_changed'"
    fi
}

# Switch the device into acting as the CAP (main gateway).
# Also update the range extension mode as necessary.
#
# input: $1 is_cap: whether the device should act as the central
#                   AP or a secondary gateway connected AP
__repacd_config_gwcon_ap_mode() {
    local is_cap=$1
    local wsplcd_mode son_mode
    local rate_scaling_factor=0
    local default_root_dist=0

    # The WDS, VAP independent, and QWrap AP settings also need to be updated
    # based on the range extension mode.
    local enable_wds enable_rrm enable_qwrap_ap enable_extap
    local block_dfs enable_multi_ap disable_steering=0 config_sta=1
    local map_enable=0
    __repacd_get_config_re_mode config_re_mode
    __repacd_get_re_mode resolved_re_mode
    if __repacd_is_auto_re_mode $config_re_mode || __repacd_is_wds_mode || \
       __repacd_is_son_mode; then
        if __repacd_is_auto_re_mode $config_re_mode || \
           __repacd_is_son_mode; then
            __repacd_echo "Using SON mode for GW Connected AP"
            enable_multi_ap=1
        else   # Must be vanilla WDS
            __repacd_echo "Using WDS mode for GW Connected AP"
            enable_multi_ap=0
        fi

        enable_wds=1
        enable_rrm=1
        enable_qwrap_ap=0
        enable_extap=0

        # In WDS/SON modes, we let the OEM customize whether DFS channels
        # should be permitted.
        if __repacd_is_block_dfs; then
            block_dfs=1
        else
            block_dfs=0
        fi

        wsplcd_enabled=1
    else
        __repacd_echo "Using Non-WDS mode for GW Connected AP"
        enable_wds=0
        enable_multi_ap=0
        enable_rrm=0
        enable_qwrap_ap=0
        enable_extap=0

        # In QWrap/ExtAP mode, DFS channels should always be disallowed (as it
        # does not appear to suppor them currently). This may be able to
        # relaxed in the future.
        block_dfs=1

        # Since QWrap/ExtAP mode on the CAP is for mimicking a non-QTI AP, do
        # not run an IEEE P1905.1 registrar.
        wsplcd_enabled=0
    fi

    config_load repacd
    config_get bssid_resolve_state WiFiLink 'BSSIDResolveState' 'resolving'

    # In GW-connected AP mode, only the AP interfaces are enabled.
    local disable_24g_sta=1 disable_5g_sta=1 disable_24g_ap=0 disable_5g_ap=0
    config_load wireless
    config_foreach __repacd_disable_vap wifi-iface \
        $managed_network 'sta' $disable_24g_sta $disable_5g_sta config_changed
    config_foreach __repacd_disable_vap wifi-iface \
        $managed_network 'ap' $disable_24g_ap $disable_5g_ap config_changed

    # The QWrap parameters should always be set to 0 on the CAP.
    config_foreach __repacd_config_qwrap_device wifi-device \
        0 config_changed

    # Similarly, the DBDC repeater feature should be disabled on the
    # CAP.
    config_foreach __repacd_config_dbdc_device wifi-device \
        0 config_changed

    # If son_mode is HYCLIENT root distance whould be reest to 255
    # during restart_in_cap_mode to avoid island issue
    if [ "$is_cap" -gt 0 ]; then
        default_root_dist=0
    else
        default_root_dist=255
    fi


    # Now set up the interfaces in the right way.
    if [ "$auto_config_enable" -eq 0 ]; then
        config_foreach __repacd_config_iface wifi-iface \
            $managed_network $enable_wds $enable_qwrap_ap $enable_extap \
            $block_dfs $enable_rrm $rate_scaling_factor $default_root_dist $capsnr config_changed
        if [ "$traffic_separation_active" -gt 0 ]; then
            config_foreach __repacd_config_iface wifi-iface \
                $network_backhaul $enable_wds $enable_qwrap_ap $enable_extap \
                $block_dfs $enable_rrm $rate_scaling_factor $default_root_dist $capsnr config_changed
            config_foreach __repacd_config_iface wifi-iface \
                $network_guest $enable_wds $enable_qwrap_ap $enable_extap \
                $block_dfs $enable_rrm $rate_scaling_factor $default_root_dist $capsnr config_changed
        fi
    else
        config_load 'repacd'
        config_foreach __repacd_config_son_iface son-iface \
            $enable_wds $enable_qwrap_ap $enable_extap \
            $block_dfs $enable_rrm $rate_scaling_factor $default_root_dist $capsnr config_changed
    fi

    uci_commit wireless

    uci_set repacd repacd Role 'CAP'
    uci_commit repacd

    if [ "$is_cap" -gt 0 ]; then
        wsplcd_mode='REGISTRAR'
        son_mode='HYROUTER'
    else
        wsplcd_mode='ENROLLEE'
        son_mode='HYCLIENT'
    fi

    # Deep cloning is not relevant in registrar mode, but we set it to 1
    # anyways (since that is the default).
    __repacd_configure_wsplcd $wsplcd_mode 1 0 $config_sta $map_enable \
                              config_changed

    __repacd_configure_son $enable_wds $disable_steering $enable_multi_ap \
        $son_mode config_changed
}

# Switch the device to act in one of the NonCAP configurations.
# input: $1 disable_ap - whether to disable the AP interfaces
# input: $2 deep_clone - whether to use deep cloning in wsplcd
# input: $3 deep_clone_no_bssid - whether to use deep cloning without BSSID cloning in wsplcd
__repacd_config_noncap_mode() {
    local disable_ap=$1
    local deep_clone=$2
    local deep_clone_no_bssid=$3

    # The WDS and QWrap AP settings also need to be updated based on the
    # range extension mode.
    local enable_wds enable_qwrap_ap enable_extap enable_dbdc_repeater
    local block_dfs enable_rrm enable_multi_ap disable_steering=0 config_sta=1
    local map_enable=0
    local rate_scaling_factor=$RE_DEFAULT_RATE_ESTIMATE_SCALING_FACTOR
    local default_root_dist=$RE_ROOT_AP_DISTANCE_INVALID
    local disable_24g_sta=1 disable_5g_sta=0
    local disable_24g_ap=$disable_ap disable_5g_ap=$disable_ap
    if __repacd_is_wds_mode || __repacd_is_son_mode; then
        if __repacd_is_wds_mode; then
            __repacd_echo "Using WDS mode for NonCAP"
        else  # Must be SON mode
            __repacd_echo "Using SON mode for NonCAP"
        fi

        enable_wds=1
        enable_qwrap_ap=0
        enable_extap=0

        # Even if we are not operating in fully coordinated steering mode,
        # we should enable RRM for use in the uncoordinated steering
        # environment.
        enable_rrm=1

        # In WDS mode, we let the OEM customize whether DFS channels should
        # be permitted.
        if __repacd_is_block_dfs; then
            block_dfs=1
        else
            block_dfs=0
        fi

        wsplcd_enabled=1

        __repacd_get_rate_scaling_factor rate_scaling_factor
        config_get capsnr WiFiLink 'PreferCAPSNRThreshold5G' '0'
    else
        enable_wds=0

        # Until steering can be well supported on QWRAP/ExtAP, there is no real
        # need to have RRM enabled.
        enable_rrm=0

        # wsplcd needs WDS in order to work (as it sends/receives using the
        # bridge interface MAC address). Plus, it is not too likely that the
        # main AP will be running an IEEE P1905.1 registrar.
        wsplcd_enabled=0

        if [ "$disable_ap" -eq 0 ]; then
            if __repacd_is_qwrap_mode; then
                __repacd_echo "Using QWrap mode for NonCAP"
                enable_qwrap_ap=1
                enable_extap=0

            else
                __repacd_echo "Using ExtAP mode for NonCAP"
                enable_qwrap_ap=0
                enable_extap=1
            fi

            # In QWrap/ExtAP mode, DFS channels should always be disallowed
            # (as these modes may not support them properly).
            block_dfs=1
        else  # client mode
            if __repacd_is_qwrap_mode; then
                __repacd_echo "Using QWrap mode for NonCAP"
                enable_qwrap_ap=0
                enable_extap=0

                # @todo What mode should be used here? The STA interface is not
                #       even created if it is not in QWRAP or WDS. We could
                #       potentially consider standalone Proxy STA
                #       mode, but we need details on how to configure this.
            else
                __repacd_echo "Using ExtAP mode for NonCAP"
                enable_qwrap_ap=0
                enable_extap=1
            fi

            # We'll rely on the main AP to decide on DFS or not.
            block_dfs=0
        fi
    fi

    if __repacd_is_son_mode; then
        disable_24g_sta=0
        enable_dbdc_repeater=0
        enable_multi_ap=1
    else
        enable_multi_ap=0

        # Although currently in non-SON mode we do not enable both STA
        # interfaces, just in case we do in the future, set this flag. It
        # should make no difference if only one STA interface is active.
        enable_dbdc_repeater=1
    fi

    config_load repacd
    config_get bssid_resolve_state WiFiLink 'BSSIDResolveState' 'resolving'

    config_load wireless

    if [ "$traffic_separation_active" -gt 0 ]; then
        config_foreach __repacd_disable_vap wifi-iface \
            $network_backhaul 'sta' $disable_24g_sta $disable_5g_sta config_changed
    else
        config_foreach __repacd_disable_vap wifi-iface \
            $managed_network 'sta' $disable_24g_sta $disable_5g_sta config_changed
    fi
    config_foreach __repacd_disable_vap wifi-iface \
        $managed_network 'ap' "$disable_24g_ap" "$disable_5g_ap" config_changed

    # First set the special options for QWRAP and DBDC repeaters.
    config_foreach __repacd_config_qwrap_device \
        wifi-device $enable_qwrap_ap config_changed
    config_foreach __repacd_config_dbdc_device wifi-device \
        $enable_dbdc_repeater config_changed

    if [ "$auto_config_enable" -eq 0 ]; then
        config_foreach __repacd_config_iface wifi-iface \
            $managed_network $enable_wds $enable_qwrap_ap $enable_extap \
            $block_dfs $enable_rrm $rate_scaling_factor $default_root_dist $capsnr config_changed
        if [ "$traffic_separation_active" -gt 0 ]; then
            config_foreach __repacd_config_iface wifi-iface \
                $network_backhaul $enable_wds $enable_qwrap_ap $enable_extap \
                $block_dfs $enable_rrm $rate_scaling_factor $default_root_dist $capsnr config_changed
            config_foreach __repacd_config_iface wifi-iface \
                $network_guest $enable_wds $enable_qwrap_ap $enable_extap \
                $block_dfs $enable_rrm $rate_scaling_factor $default_root_dist $capsnr config_changed
        fi
    else
        config_load 'repacd'
        config_foreach __repacd_config_son_iface son-iface \
            $enable_wds $enable_qwrap_ap $enable_extap \
            $block_dfs $enable_rrm $rate_scaling_factor $default_root_dist $capsnr config_changed
    fi

    __repacd_config_independent_vaps

    uci_commit wireless

    uci_set repacd repacd Role 'NonCAP'
    uci_commit repacd

    __repacd_configure_wsplcd 'ENROLLEE' "$deep_clone" "$deep_clone_no_bssid" \
                              $config_sta $map_enable config_changed

    __repacd_configure_son $enable_wds $disable_steering $enable_multi_ap \
        'HYCLIENT' config_changed
}

# Switch the device into acting as a range extender.
# Also update the range extension mode as necessary.
__repacd_config_re_mode() {
    local disable_ap=0 deep_clone=1 deep_clone_no_bssid=0

    # We do deep cloning without BSSID for daisy chaining.
    [ "$daisy_chain" -gt 0 ] && deep_clone_no_bssid=1

    __repacd_config_noncap_mode $disable_ap $deep_clone $deep_clone_no_bssid
}

# Switch the device into acting as a pure client device (no AP interfaces
# enabled).
__repacd_config_client_mode() {
    local disable_ap=1 deep_clone=0
    __repacd_config_noncap_mode $disable_ap $deep_clone 0
}

__repacd_config_copy() {
    local cfgname=$1 src_config=$2 dst_config=$3 src_file=$4 dst_file=$5 mode=$6
    [ -n "$mode" ] && uci_set repacd "$dst_config" mode "$mode"
    for name in $cfgname; do
        value=$(uci get "$src_file"."$src_config"."$name" 2&> /dev/null) 
        [ -z "$value" ] && uci delete "$dst_file"."$dst_config"."$name" 2&> /dev/null
        if [ "$name" = "sae_groups" ] || [ "$name" = "sae_password" ] || \
           [ "$name" = "owe_groups" ]; then
            uci delete "$dst_file"."$dst_config"."$name" 2&> /dev/null
            [ -n "$value" ] && uci add_list "$dst_file"."$dst_config"."$name"="$value"
        elif [ -n "$value" ]; then
            uci_set "$dst_file" "$dst_config" "$name" "$value"
        fi
    done
    uci_commit "$dst_file"
}

__repacd_check_port_free() {
    local port_a=$1 port_b

    for port_b in $used_list; do
        [ "$port_a" = "$port_b" ] && return 1
    done
    return 0
}

__repacd_add_used_list() {
    used_list="$used_list$1 "
}

__repacd_alloc_port() {
    local port_c
    for port_c in $port_list; do
        if __repacd_check_port_free $port_c; then
            eval "$1='$port_c'"
            __repacd_add_used_list $port_c
            return
        fi
    done
}

__repacd_is_port_free() {
    local tport=$1 port_state
    port_state=`netstat -tulpn | grep LISTEN | grep "127.0.0.1:$tport"`
    [ -z "$port_state" ] && return 0
    return 1
}

# configure port for each and every network account
# Input: $1 - repacd config
# Input: $2 - network name
__repacd_config_port() {
    local config=$1 nw=$2 def_port lport

    [ -z "$nw" ] && config_get nw "$config" network
    if [ "$nw" = "lan" -o "$nw" = "Lan" -o "$nw" = "LAN" -o "$nw" = "$managed_network" ]; then
        def_port=$def_lan
    elif [ "$nw" = "guest" -o "$nw" = "Guest" -o "$nw" = "GUEST" -o "$nw" = "$network_guest" ]; then
        def_port=$def_guest
    else
        def_port=""
    fi
    config_get lport "$config" port $def_port
    if [ -z "$lport" ] ; then
        __repacd_alloc_port lport
        uci_set repacd "$config" port $lport
    fi
    if __repacd_is_port_free $lport; then
        uci_set repacd "$config" port $lport
        __repacd_echo "Portno:$lport assigned for $nw"
    else
        __repacd_echo "Portno:$lport is already in use.."
    fi
}

# Configure dhcp for each and every account
# Input: $1 - repacd config
# Input: $2 - network name
__repacd_config_dhcp() {
    local config=$1 nw=$2
    config_load 'repacd'
    config_get dhcp_start $config dhcp_start '100'
    config_get dhcp_limit $config dhcp_limit '150'
    config_get dhcp_ltime $config dhcp_leasetime "12h"
    config_get dhcp_dhcpv6 $config dhcp_dhcpv6 "server"
    config_get dhcp_ra $config dhcp_ra "server"
    config_get dhcp_ignore $config dhcp_ignore "1"

    if [ "$hyfi_mode" = "HYROUTER" ] ; then
	local is_nw_available
        [ "$nw" = "$managed_network" ] && return
        config_load 'dhcp'
	is_nw_available=$(uci show dhcp | grep $nw)
        [ -n "$is_nw_available" ] && uci delete dhcp.$nw
        uci set dhcp.$nw="dhcp"
        uci_set dhcp $nw interface "$nw"
        uci_set dhcp $nw start "$dhcp_start"
        uci_set dhcp $nw limit "$dhcp_limit"
        uci_set dhcp $nw leasetime "$dhcp_ltime"
        uci_set dhcp $nw dhcpv6 "$dhcp_dhcpv6"
        uci_set dhcp $nw ra "$dhcp_ra"
    elif [ "$hyfi_mode" = "HYCLIENT" ] ; then
	config_load 'dhcp'
	if [ "$nw" != "$managed_network" ] ; then
            uci delete dhcp.$nw
	    uci set dhcp.$nw="dhcp"
	fi
        uci_set dhcp $nw ignore "$dhcp_ignore"
    fi
    uci_commit dhcp
}

# Configure network, dhcp for each and every account
# Input: $1 - repacd config
__repacd_config_network() {
    local config=$1 nw def_ipaddr DEVICES device auto_config type
    local def_proto="" lport def_port="" mc_que ip6ass vid_conf
    local mode

    config_get nw "$config" network
    [ -z "$nw" ] && return
    config_get type "$config" type
    config_get mode "$config" mode
    if [ -z "$type" -o "$type" != "bridge" -o "$nw" = "backhaul" \
         -o "$mode" = "sta" -o "$mode" = "ap_smart_monitor" ]; then
	return
    fi
    var1=`echo $nw | tr '[A-Z]' '[a-z]'`

    if [ "$nw" = "$managed_network" ] || [ "$var1" = "lan" ] ; then
	net_ip=$lan_ip
	vid_conf="$lan_vid"
    elif [ "$nw" = "$network_guest" ] || [ "$var1" = "guest" ] ; then
	net_ip=$guest_ip
	vid_conf="$guest_vid"
    else
	net_ip=$other_acc_ip
	vid_conf=`expr $lan_vid + $other_acc_ip`
	other_acc_ip=`expr $other_acc_ip + 1`
    fi

    config_get lport "$config" port
    [ -z "$lport" ] && __repacd_config_port "$config" "$nw"
    def_ipaddr="192.168.$net_ip.1"
    config_get ip_addr "$config" ipaddr
    if [ -z "$ip_addr" ] ; then
        if [ "$disable_sync" -eq 0 ]; then
            uci_set repacd "$config" ipaddr $def_ipaddr
            uci_commit repacd
        fi
        ip_addr=$def_ipaddr
    fi

    config_get vid "$config" vid
    if [ -z "$vid" ]; then
        uci_set repacd "$config" vid $vid_conf
        uci_commit repacd
    fi
    config_get igmp "$config" igmp_snooping '1'
    config_get proto "$config" proto 'static'
    config_get netmask "$config" netmask '255.255.255.0'
    config_get ieee1905managed "$config" ieee1905managed '1'
    config_get force_link "$config" force_link '1'
    config_get family "$config" family 'ipv4'
    config_get mc_que "$config" multicast_querier '1'
    config_get ip6ass "$config" ip6assign '0'

    config_load network
    uci set network.$nw='interface'
    uci_set network $nw type 'bridge'

    if [ "$hyfi_mode" = "HYROUTER" ] ; then
	uci_set network $nw ipaddr $ip_addr
	def_proto="static"
    elif [ "$hyfi_mode" = "HYCLIENT" ] ; then
	if [ "$nw" = "$managed_network" ] || [ "$var1" = "lan" ] ; then
	    def_proto="dhcp"
	fi
    fi

    config_get proto "$config" proto "$def_proto"

    uci_set network $nw igmp_snooping $igmp
    uci_set network $nw multicast_querier $mc_que
    uci_set network $nw proto $proto
    uci_set network $nw netmask $netmask
    uci_set network $nw ieee1905managed $ieee1905managed
    uci_set network $nw force_link $force_link
    uci_set network $nw family $family
    uci_set network $nw ip6assign $ip6ass
    uci_commit network

    __repacd_config_dhcp $config $nw
}

# Deleting backhaul vlan interfaces
# Input: $1 - network name
__repacd_delete_bh_vlan_ifaces() {
    local nw=$1 ifaces athiface
    local ifname vlan_athname

    ifaces=$(uci get network.$nw.ifname 2&> /dev/null)
    for ifname in $ifaces; do
        athiface=$(echo $ifname | grep ath)
        if [ -n "$athiface" ]; then
            vlan_athname=$(iwconfig 2>&1 | grep -o "$athiface")
            [ -z "$vlan_athname" ] && continue
            vconfig rem "$athiface"
            brctl delif "br-$nw" "$athiface"
            __repacd_delete_interface "$nw" "$athiface"
        fi
    done
}

# Get network vlanid from the network config
# Input: $1 - network name
# Output: $2 - vlan id
__repacd_get_nw_vid() {
    local nw=$1 ifaces athiface
    local ifname vlan_athname vid

    ifaces=$(uci get network.$nw.ifname 2&> /dev/null)
    for ifname in $ifaces; do
        athiface=$(echo $ifname | grep ath)
	if [ -n "$athiface" ]; then
	    vid=$(echo $athiface | cut -d "." -f 2)
	    eval "$2='$vid'"
	fi
    done
}

# Create vlan config for all network available
# Input: $1 - repacd config
# Output: $2 - if network config changed
__repacd_config_vlan() {
    local config=$1 nw type eth_ifaces_t
    local net_config_changed=0 mode vlanid

    config_get type "$config" type
    config_get mode "$config" mode
    [ -z "$type" -o "$type" != "bridge" ] && return

    config_get nw "$config" network
    [ -z "$nw" ] && return

    __repacd_get_nw_vid $nw vlanid
    if [ -z "$vlanid" ] || [ "$disable_sync" -eq 0 ] ; then
        config_get vlanid "$config" vid
    fi

    [ -z "$vlanid" ] && return
    __repacd_delete_bh_vlan_ifaces $nw
    __repacd_delete_ethernet_vlan_interfaces $nw

    config_load wireless
    config_foreach __repacd_add_vlan_interfaces wifi-iface \
                $nw $vlanid 'both' net_config_changed
    __repacd_create_ethernet_vlan_interfaces $nw $vlanid
    eval "$2='$net_config_changed'"
}

# Check if backhaul, monitor, sta, fronthaul vap is present
# Input: $1 - repacd config
# Output: $2 - repacd config to copy backhaul
# Output: $3 - 1 if monitor vap is available
# Output: $4 - repacd config to copy sta
# Output: $5 - 1 if fronthaul vap is available
# Output: $6 - 1 if guest vap is available
__repacd_check_vap_state() {
    local config=$1 nw mode type

    config_get nw "$config" network
    config_get mode "$config" mode
    config_get port "$config" port
    config_get type "$config" type
    [ "$nw" = "backhaul" -a "$mode" != "sta" ] && eval "$2='$config'"
    [ "$mode" = "ap_smart_monitor" ] && eval "$3='1'"
    [ "$mode" = "sta" ] && eval "$4='$config'"

    if [ "$nw" = "$managed_network" ] && [ "$mode" != "ap_smart_monitor" ] && \
	[ "$mode" != "sta" ] && [ "$type" = "bridge" ]; then
	eval "$5='1'"
    fi
    if [ "$nw" = "$network_guest" ] && [ "$mode" != "ap_smart_monitor" ] && \
	[ "$mode" != "sta" ] && [ "$type" = "bridge" ]; then
        eval "$6='1'"
    fi
    if [ "$nw" = "$network_employee" ] && [ "$mode" != "ap_smart_monitor" ] && \
        [ "$mode" != "sta" ] && [ "$type" = "bridge" ]; then
        eval "$7='1'"
    fi
    if [ "$type" = "bridge" ]; then
       if [ "$brcount" -ge "$MAX_ACC_MULTI_INST_SUPPORT" ]; then
           uci delete repacd."$config"
       fi
       brcount=`expr $brcount + 1`
    fi
}

# Create backhaul config if it is not present in son-iface
__repacd_config_backhaul() {
    local config
    config_load repacd

    config=$(uci add repacd son-iface)
    uci_set repacd "$config" network "backhaul"
    uci_set repacd "$config" mode "ap"
    uci_set repacd "$config" backhaul_ap "1"
    uci_set repacd "$config" wds "1"
    uci commit repacd
    eval "$1='$config'"
}

# Create monitor vap if it is not present in son-iface
__repacd_config_monitor() {
    local config
    config_load repacd

    config=$(uci add repacd son-iface)
    uci_set repacd "$config" network "lan"
    uci_set repacd "$config" ssid "AEV_MON_AP"
    uci_set repacd "$config" mode "ap_smart_monitor"
    uci_set repacd "$config" encryption "none"
    uci_set repacd "$config" wsplcd_unmanaged "1"
    uci_set repacd "$config" neighbourfilter "1"
    uci_set repacd "$config" wds "1"
    uci commit repacd
    __repacd_echo "<AUTO-CONFIG> Smart monitor vap created"
}

# Configure port for each network as part of manual config
__repacd_config_port_manual() {
    local nw nw_t
    config_load repacd

    [ "$auto_config_enable" -eq 1 ] && return
    [ "$multi_inst_enable" -eq 0 ] && return
    network_list=$(uci show network | grep bridge | grep type)
    for nw in $network_list; do
	nw_t=$(echo $nw | cut -d "." -f 2)
	config=$(uci add repacd son-iface)
	uci_set repacd $config network $nw_t
	__repacd_config_port $config $nw_t
    done
    uci_commit repacd
}

# Check port in son-iface
# Input: $1 - repacd config
# Output: $2 - 1 if it is available
__repacd_check_port() {
    local config=$1

    config_get nw $config network
    config_get port $config port

    [ -z "$nw" -o -z "$port" ] && return

    eval "$2='1'"
}

# Check if port is already configured or not
# Output: return sucess if already configured
__repacd_is_port_configured() {
    local is_port
    config_load 'repacd'
    config_foreach __repacd_check_port son-iface is_port
    [ "$is_port" -eq 1 ] && return 0
    return 1
}

# Create hyt config for each interface
__repacd_config_hyt() {
cat > /usr/sbin/hyt << 'EOF1'
#hyt
. /lib/functions.sh
get_port() {
    local config=$1 lnw=$2 lport
    config_get lport "$config" port
    config_get nw "$config" network
    [ "$lnw" = "$nw" ] && eval "$3='$lport'"
}
config_load 'repacd'
config_foreach get_port son-iface $1 port
[ -z "$port" ]  && port=7777
telnet 127.0.0.1 $port
EOF1
}

# Create hyt for each network
__repacd_hyt() {
   local hytfile="/usr/sbin/hyt" is_conf
   [ "$multi_inst_enable" -eq 0 ] && return

   is_conf=`cat $hytfile | grep "#hyt"`
   [ -n "$is_conf" ] && return
   __repacd_config_hyt
}

# Create private fronthaul if it is not present in son-iface
__repacd_config_private_vap() {
    local config

    config=$(uci add repacd son-iface)
    uci_set repacd "$config" 'type' "bridge"
    uci_set repacd "$config" 'network' "$managed_network"
    uci_commit repacd
}

# Create Guest config if it is not present in son-iface
__repacd_config_guest_vap() {
    [ "$traffic_separation_enabled" -eq 0 ] && return
    config=$(uci add repacd son-iface)
    uci_set repacd "$config" 'type' "bridge"
    uci_set repacd "$config" 'network' "$network_guest"
    uci_commit repacd
}

# Create Employee config if it is not present in son-iface
__repacd_config_employee_vap() {
    [ "$traffic_separation_enabled" -eq 0 ] && return
    [ "$multi_inst_enable" -eq 0 ] && return
    config=$(uci add repacd son-iface)
    uci_set repacd "$config" 'type' "bridge"
    uci_set repacd "$config" 'network' "$network_employee"
    uci_commit repacd
}


# Check if network is present in the repacd config
# Input: $1 - repacd config
# Input: $2 - network name
# Output: $3 - 1 if success
__repacd_check_network() {
    local rdconfig=$1 rdnw rdtype network=$2

    config_get rdnw "$rdconfig" network
    config_get rdtype "$rdconfig" type
    if [ -n "$rdnw" ] && [ "$rdnw" = "$network" ] && \
       [ -n "$rdtype" ] && [ "$rdtype" = "bridge" ]; then
       eval "$3='1'"
    fi
}

# Check network if available in son-iface
# Input: $1 - network name
# Output: return success if network is not available in son-iface
__repacd_network_not_exist_in_son() {
    local nw=$1 status=0
    config_load 'repacd'
    config_foreach __repacd_check_network son-iface $nw status
    [ "$status" -eq 0 ] && return 0
    return 1
}

# If there is a mis-match between network and son-iface then
# the respective network will be removed from the networ config
# Input: $1 - network name
__repacd_sync_network() {
    local nw_t="$1" type ieee1905managed
    config_get type "$nw_t" type
    [ -z "$type" -o ! "$type" = "bridge" ] && return
    [ "$nw_t" = "$managed_network" ] && return
    config_get_bool ieee1905managed "$nw_t" ieee1905managed
    [ -z "$ieee1905managed" ] && return

    if __repacd_network_not_exist_in_son $nw_t; then
        ifconfig "br-$nw_t" down
        brctl delbr "br-$nw_t"
        uci delete network."$nw_t"
        uci delete dhcp."$nw_t"
        __repacd_echo "sync_network: deleting network.$nw_t"
        __repacd_echo "sync_network: deleting dhcp.$nw_t"
    fi
}

# This function will traverse through the son-iface and it will
# confirm the  nw and mode in the son-iface
# Input: $1 - repacd config $2 - network name $3 - ssid
# Output: $5 - Assign 1 if nw and mode are present in son-iface
__repacd_process_son_iface() {
    local rd_config=$1 w_nw=$2 w_mode=$3 w_ssid=$4 rd_nw rd_mode rd_ssid

    config_get rd_nw "$rd_config" network
    config_get rd_mode "$rd_config" mode 'ap'
    config_get rd_ssid "$rd_config" ssid

    [ -z "$rd_nw" ] && return
    [ -z "$rd_mode" ] && return

    if [ "$rd_nw" = "$w_nw" ] && [ "$rd_mode" = "$w_mode" ] ; then
	eval "$5='1'"
    fi
}

# This function will help to identify the missing nw in son-iface
# Input: $1 - network name $2 - mode $3 - ssid
# Output: return success if network and mode is not present in son-iface
__repacd_vap_not_exist_in_son() {
    local wi_nw=$1 wi_mode=$2 wi_ssid=$3 status=0
    config_load 'repacd'
    config_foreach __repacd_process_son_iface son-iface $wi_nw $wi_mode $wi_ssid status
    [ "$status" -eq 0 ] && return 0
    return 1
}

# If there is a mis-match between son-iface and wifi-iface then
# the respective config will be removed from the wireless config
# Input: $1 - wireless config
__repacd_process_wifi_iface() {
    local wl_config=$1 wl_nw wl_mode wl_ssid wl_device

    config_get wl_nw "$wl_config" network
    config_get wl_mode "$wl_config" mode
    config_get wl_ssid "$wl_config" ssid
    config_get wl_device "$wl_config" device
    [ -z "$wl_nw" ] && return
    [ -z "$wl_mode" ] && return
    if __repacd_vap_not_exist_in_son $wl_nw $wl_mode $wl_ssid ; then
        __repacd_echo "sync_wifi: Deleting:$wl_config $wl_nw $wl_mode $wl_ssid $wl_device from wifi-iface"
	uci_set wireless "$wl_config" disabled '1'
    else
	uci_set wireless "$wl_config" disabled '0'
    fi
}

# Sync between wifi-iface and son-iface
__repacd_sync() {
    config_load 'network'
    config_foreach __repacd_sync_network interface
    uci_commit network
    config_load 'wireless'
    config_foreach __repacd_process_wifi_iface wifi-iface
    uci_commit wireless
}

# Configure default networks to allow traffic
nw_list_fw=""
__repacd_config_allow_networks() {
    local config=$1 rd_config=$2
    local nw type
    config_get nw "$config" network
    config_get type "$config" type
    config_get mode "$config" mode
    [ "$nw" = "$managed_network" ] && return
    [ "$nw" = "$network_backhaul" ] && return
    [ "$mode" = "ap_smart_monitor" ] && return
    [ "$mode" = "sta" ] && return
    [ "$type" != "bridge" ] && return
    nw_list_fw="$nw_list_fw$nw "
}

__repacd_check_allow_nw() {
    local config=$1 nw_allow
    local nw type
    config_get nw "$config" network
    config_get type "$config" type
    config_get mode "$config" mode
    [ "$nw" = "$network_backhaul" ] && return
    [ "$mode" = "ap_smart_monitor" ] && return
    [ "$nw" != "$managed_network" ] && return
    config_get nw_allow "$config" allow_networks
    [ -z "$nw_allow" ] && eval "$2='$config'"
}

__repacd_allow_nw_for_managed_nw() {
    local rdconfig
    [ "$multi_inst_enable" -eq 0 ] && return
    config_load 'repacd'
    config_foreach __repacd_check_allow_nw son-iface rdconfig
    if [ -n "$rdconfig" ]; then
        config_load 'repacd'
        config_foreach __repacd_config_allow_networks son-iface "$rdconfig"
        uci_set repacd "$rdconfig" allow_networks "$nw_list_fw"
        uci_commit repacd
    fi
}

__repacd_update_sta_config() {
    local cfgname="ssid encryption key wsplcd_unmanaged sae sae_groups sae_password owe_groups owe auth_server auth_port auth_secret rsn_preauth rsn_preauth_interface auth_server2 auth_port2 auth_secret2 suite_b wpa_group_rekey ieee80211w group_mgmt_cipher"
    local ssid encryption key wsplcd_unmanaged sae sae_groups sae_password owe_groups owe
    local auth_server auth_port auth_secret rsn_preauth rsn_preauth_interface
    local auth_server2 auth_port2 auth_secret2 suite_b wpa_group_rekey ieee80211w
    local group_mgmt_cipher
    local is_match_ap is_match_sta bh_ssid bh_enc sta_ssid sta_enc skipbh skipbh_ap skipbh_sta
    local device DEVICES

    [ "$hyfi_mode" = "HYROUTER" ] && return
    config_load wireless
    config_foreach __repacd_resolve_vaps_new wifi-iface "$network_backhaul" "ap" is_match_ap
    config_load wireless
    config_foreach __repacd_resolve_vaps_new wifi-iface "$network_backhaul" "sta" is_match_sta
    if [ "$is_match_ap" -eq 1 ] && [ "$is_match_sta" -eq 1 ]; then
        __repacd_get_devices DEVICES
        for device in $DEVICES; do
            config_load wireless
            config_get skipbh $device repacd_skipbh '0'
            config_get skipbh_ap $device repacd_skipbh_ap '0'
            config_get skipbh_sta $device repacd_skipbh_sta '0'
            if [ "$skipbh" -eq 1 ] || [ "$skipbh_ap" -eq 1 ] || [ "$skipbh_sta" -eq 1 ]; then
                continue
            fi
            __repacd_get_resolve_value "$device" "$network_backhaul" "sta" ssid encryption
            sta_ssid="$ssid"
            sta_enc="$encryption"
            __repacd_get_resolve_value "$device" "$network_backhaul" "ap" ssid encryption key sae sae_password \
            sae_groups wsplcd_unmanaged auth_server auth_port auth_secret rsn_preauth rsn_preauth_interface \
            auth_server2 auth_port2 auth_secret2 suite_b wpa_group_rekey ieee80211w group_mgmt_cipher
            bh_ssid="$ssid"
            bh_enc="$encryption"
            if [ "$bh_ssid" != "$sta_ssid" ] || [ "$bh_enc" != "$sta_enc" ]; then
                __repacd_set_resolve_value "$device" "$network_backhaul" "sta" "$cfgname" "$ssid" "$encryption" "$key" \
                "$wsplcd_unmanaged" "$sae" "$sae_groups" "$sae_password" "$owe_groups" "$owe" "$auth_server" \
                "$auth_port" "$auth_secret" "$rsn_preauth" "$rsn_preauth_interface" "$auth_server2" "$auth_port2" \
                "$auth_secret2" "$suite_b" "$wpa_group_rekey" "$ieee80211w" "$group_mgmt_cipher"
                __repacd_create_vaps_new "$network_backhaul" "sta"
            fi
        done
    fi
}


__repacd_skip_fh() {
    local config=$1 device mode network skipfh default_ap

    if __repacd_vaps_in_default_config; then
        uci delete wireless.$config
    fi
}

# Auto config apply. This function will create backhaul vap, sta vap
# fronthaul vap - (private,guest), monitor vap if those vap configs are
# not available in repacd. It will apply the network config and
# port config by default. Default config will be applied in this function

__repacd_auto_config_new() {
    local device DEVICES bhconfig="" staconfig="" mon bridge=""
    local pvt_vap="" guest_vap="" emp_vap=""
    config_load 'repacd'

    if [ "$multi_inst_enable" -eq 1 ] ; then
        uci_set repacd repacd 'max_inst_supported' "$MAX_ACC_MULTI_INST_SUPPORT"
	uci_commit repacd
    fi
    if [ "$auto_config_enable" -eq 1 ]; then
	other_acc_ip=3
	brcount=0
	config_load wireless
	config_foreach __repacd_skip_fh wifi-iface
        if [ "$disable_sync" -eq 0 ]; then
            __repacd_sync
        fi

	if [ "$disable_sync" -eq 0 ]  || __repacd_vaps_in_default_config ; then
            __repacd_get_devices DEVICES
            for device in $DEVICES; do
                uci_set wireless "$device" repacd_auto_create_vaps '1'
            done
	    config_load 'repacd'
            config_foreach __repacd_check_vap_state son-iface bhconfig mon staconfig \
				pvt_vap guest_vap emp_vap
            if [ -z "$bhconfig" -a -z "$staconfig" ] ; then
	        __repacd_config_backhaul bhconfig
	    fi
	    if [ "$hyfi_mode" = "HYCLIENT" ]; then
                if [ -z "$staconfig" -a -n "$bhconfig" ] ; then
	            staconfig=$(uci add repacd son-iface)
	            __repacd_config_copy "$CFG_NAMES" "$bhconfig" "$staconfig" repacd repacd "sta"
                elif [ -n "$staconfig" -a -z "$bhconfig" ] ; then
	            bhconfig=$(uci add repacd son-iface)
	            __repacd_config_copy "$CFG_NAMES" "$staconfig" "$bhconfig" repacd repacd "ap"
		elif [ -n "$staconfig" -a -n "$bhconfig" ]; then
		    __repacd_config_copy "$CFG_NAMES" "$bhconfig" "$staconfig" repacd repacd "sta"
	        fi
	    fi
            [ -z "$mon" ] && __repacd_config_monitor
	    [ -z "$pvt_vap" ] && __repacd_config_private_vap
	    [ -z "$guest_vap" ] && __repacd_config_guest_vap
	    [ -z "$emp_vap" ] && __repacd_config_employee_vap
	    __repacd_allow_nw_for_managed_nw
            config_load 'repacd'
            config_foreach __repacd_config_network son-iface
	fi
    elif ! __repacd_is_port_configured; then
        __repacd_config_port_manual
    fi
    __repacd_hyt
}

# Perform the startup actions when operating in the original Wi-Fi SON mode
# (no Multi-AP SIG support).
__start_son() {
    local enabled map_enabled device_type
    local mode activate_ts is_backhaul=""
    local eth_mon_enabled is_backhaul is_monitor is_sta is_bridge

    config_changed=0
    net_config_changed=0
    activate_ts=0

    __repacd_auto_config_new
    __repacd_hyt
    __repacd_set_fo 0
    config_load 'repacd'
    config_get_bool enabled repacd 'Enable' '0'
    config_get traffic_separation_enabled repacd TrafficSeparationEnabled '0'
    config_get traffic_separation_active repacd TrafficSeparationActive '0'
    config_get daisy_chain WiFiLink DaisyChain '0'
    config_get backhaul_ssid repacd BackhaulSSID $def_backhaul_ssid
    config_get backhaul_enc repacd BackhaulEnc $def_backhaul_enc
    config_get backhaul_key repacd BackhaulKey $def_backhaul_key
    config_get network_guest repacd NetworkGuest 'guest'
    config_get guest_ssid repacd GuestSSID $def_guest_ssid
    config_get guest_enc repacd GuestEnc $def_guest_enc
    config_get guest_key repacd GuestKey $def_guest_key
    config_get guest_backhaul_iface repacd NetworkGuestBackhaulInterface 'both'
    config_get eth_mon_enabled repacd 'EnableEthernetMonitoring' '0'
    config_get manage_vap_ind WiFiLink 'ManageVAPInd' '0'
    config_get Manage_front_and_back_hauls_ind 'FrontHaulMgr' 'ManageFrontAndBackHaulsIndependently' '0'
    __repacd_get_config_re_mode config_re_mode

    [ "$enabled" -gt 0 ] || {
        return 1
    }

    __repacd_echo "starting WHC auto-configuration"

    # For now, we can only manage a single network.
    config_get managed_network repacd ManagedNetwork 'lan'
    __repacd_echo "Managed network: $managed_network"

    config_get device_type repacd DeviceType 'RE'
    __repacd_echo "Device type: $device_type"

    # Grab a lock to prevent any updates from being made by the daemon.
    whc_wifi_config_lock

    __repacd_config_wps_pbc_enhc 0 config_changed

    # Config_changed is not being gracefully handled in MIPS.
    # so commiting the change if wps_pbc_enhn is set
    if [ "$config_changed" -gt 0 ]; then
        uci_commit wireless
        config_changed=0
    fi

    if __repacd_vaps_in_default_config; then
	if [ "$auto_config_enable" -eq 0 ]; then
	    __repacd_reset_default_config
	else
	    config_load 'repacd'
            config_foreach __repacd_reset_default_config_new son-iface
	fi
        config_changed=1
        __repacd_echo "Reset $managed_network VAPs"
    else
        # Need to massage the configuration to make it consistent with the
        # expectations of repacd.
	if [ "$auto_config_enable" -eq 0 ]; then
	    __repacd_reset_existing_config
	else
	    config_load 'repacd'
            config_foreach __repacd_reset_existing_config_new son-iface
	fi
        __repacd_echo "Initialized $managed_network VAPs"
    fi
    __repacd_enable_wifi

    # create additional vaps if traffic separation enabled
    if __repacd_gw_mode || [ "$device_type" = 'RE' ] && [ "$gwcon_mode" != "CAP" ]; then
        if [ "$traffic_separation_enabled" -gt 0 ]; then
            if __repacd_is_qwrap_mode || \
               __repacd_is_son_mode; then
                if __repacd_check_additional_network_exist; then
		    if [ "$auto_config_enable" -eq 0 ]; then
                        __repacd_set_bridge_empty $network_guest
                        __repacd_set_firewall_rules $network_guest
		    else
			config_load 'network'
			config_foreach __repacd_set_bridge_empty interface
			config_load 'repacd'
			if [ "$disable_sync" -eq 0 ]; then
			    config_foreach __repacd_delete_firewall_rules son-iface
			fi
			config_load 'repacd'
			config_foreach __repacd_set_firewall_rules_new son-iface
		    fi
                    __repacd_reset_additional_config

                    if ! __repacd_gw_mode && __repacd_is_qwrap_mode && \
                       [ "$traffic_separation_active" -eq 0 ] && [  "$auto_config_enable" -eq 0 ]; then
                        local disable_24g_ap=1 disable_5g_ap=1

                        config_load wireless
                        config_foreach __repacd_disable_vap wifi-iface \
                            $network_backhaul 'ap' $disable_24g_ap $disable_5g_ap config_changed
                        config_foreach __repacd_disable_vap wifi-iface \
                            $network_guest 'ap' $disable_24g_ap $disable_5g_ap config_changed
                        uci_commit wireless
                    fi

                    uci_set repacd repacd NetworkBackhaul $network_backhaul
                    uci_commit repacd
                    activate_ts=1
                fi
            fi
        fi
    fi

    if __repacd_gw_mode; then
        # WAN group not empty; this device will act as CAP regardless of
        # the GatewayConnectedMode setting
        __repacd_config_gwcon_ap_mode 1
    elif [ "$device_type" = 'RE' ]; then
        # WAN group empty or non-existent
        # Switch to range extender mode

        # Clear the BSSIDs on fresh restart
        config_load wireless
        if [ "$traffic_separation_active" -gt 0 ]; then
            config_foreach __repacd_delete_bssid_entry wifi-iface $network_backhaul config_changed
        else
            config_foreach __repacd_delete_bssid_entry wifi-iface $managed_network config_changed
        fi
        uci_set repacd WiFiLink BSSIDResolveState 'resolving'
        uci_commit wireless
        uci_commit repacd
        __repacd_config_re_mode

        if [ "$activate_ts" -eq 1 -a "$auto_config_enable" -eq 0 ]; then
                config_load wireless
                config_foreach __repacd_reconfig_sta_vaps wifi-iface $managed_network $network_backhaul \
                    "$backhaul_ssid" "$backhaul_enc" "$backhaul_key"
                uci_commit wireless
        fi
    else
        # Must be a client device (that can opportunistically act as an RE).
        __repacd_config_client_mode
    fi


    whc_wifi_config_unlock

    __repacd_restart_dependencies
   if [ "$traffic_separation_enabled" -gt 0 ]; then
        __repacd_wifi_set_otherband_bssids $network_backhaul
   else
        __repacd_wifi_set_otherband_bssids $managed_network
   fi

    # create vlan interfaces required for traffic separation.
    if [ "$activate_ts" -eq 1 ]; then
            config_load network
            config_load wireless
	    if [ "$auto_config_enable" -eq 0 ]; then
                config_foreach __repacd_add_vlan_interfaces wifi-iface \
                    $managed_network $lan_vid 'both' net_config_changed
                config_foreach __repacd_add_vlan_interfaces wifi-iface \
                    $network_guest $guest_vid $guest_backhaul_iface net_config_changed
                uci_commit wireless

                __repacd_add_ethernet_vlan_interfaces $network_guest net_config_changed
                uci_commit network
	    else
	        config_load repacd
	        config_foreach __repacd_config_vlan son-iface net_config_changed
	        uci_commit network
	        uci_commit wireless
	    fi
            __repacd_restart_firewall

            uci_set repacd repacd TrafficSeparationActive '1'
            uci_commit repacd

            # stop/start hyd only if there is any change in netwrok config
            # due to addition of VLAN interfaces. If required VLAN iface already
            # present then we can avoid hyd stop/start. This will save some time
            # and avoid any delays after repacd restart.
            if [ "$net_config_changed" -gt 0 ] || [ "$config_changed" -gt 0 ]; then
                config_changed=0
                hyd_stop=1
                hyd_start=1
                wsplcd_stop=1
                wsplcd_start=1
                __repacd_restart_dependencies
            fi
    fi

    if [ "$eth_mon_enabled" -eq 1 ]; then
        # Make sure lldpd listens on wan and lan interfaces
        for int in wan lan; do
            if ! uci get lldpd.config.interface | grep $int > /dev/null; then
                uci add_list lldpd.config.interface=$int
            fi
        done
        __repacd_echo "Starting lldpd"
        repacd_netdet_lldpd_init start
    fi

    if ! __repacd_gw_mode || [ "$eth_mon_enabled" -eq 1 ]; then
        __stop_repacd_run

        # Start the script that monitors the link state.
        #
        # When in NonCAP mode, it will keep checking whether there is a link
        # to the gateway over ethernet. When in CAP mode, it will keep
        # checking the WAN/LAN ifaces.
        __repacd_echo "Starting  RE Placement and Auto-config Daemon"
        start-stop-daemon -S -x /usr/sbin/repacd-run.sh -b -- \
            "son" init $config_re_mode $resolved_re_mode $resolved_re_submode
    fi
}

# Force a restart into CAP mode using the SON algorithms.
#
# @see restart_in_cap_mode
__restart_in_cap_mode_son() {
    local gwcon_mode device_type activate_ts

    __repacd_auto_config_new
    __repacd_hyt

    config_load repacd

    config_get managed_network repacd ManagedNetwork 'lan'
    config_get gwcon_mode repacd GatewayConnectedMode 'AP'
    config_get device_type repacd DeviceType 'RE'
    config_get traffic_separation_enabled repacd TrafficSeparationEnabled '0'
    config_get traffic_separation_active repacd TrafficSeparationActive '0'
    config_get daisy_chain WiFiLink DaisyChain '0'
    config_get backhaul_ssid repacd BackhaulSSID $def_backhaul_ssid
    config_get backhaul_enc repacd BackhaulEnc $def_backhaul_enc
    config_get backhaul_key repacd BackhaulKey $def_backhaul_key
    config_get network_guest repacd NetworkGuest 'guest'
    config_get guest_ssid repacd GuestSSID $def_guest_ssid
    config_get guest_enc repacd GuestEnc $def_guest_enc
    config_get guest_key repacd GuestKey $def_guest_key
    config_get guest_backhaul_iface repacd NetworkGuestBackhaulInterface 'both'
    config_get manage_vap_ind WiFiLink 'ManageVAPInd' '0'
    __repacd_get_config_re_mode config_re_mode
    activate_ts=0
    net_config_changed=0
    __stop_repacd_run

    if [ "$gwcon_mode" = "CAP" ]; then
        # Explicitly being forced into CAP mode while gateway connected.
        # This could be a case where a device is being used as a pure bridge
        # due to another device acting as the gateway.
        __repacd_config_wps_pbc_enhc 1 config_changed
        __repacd_config_gwcon_ap_mode 1
    else
        # Operate just as a standalone AP. This assumes there is another
        # device in the network that operates as CAP.
        __repacd_config_wps_pbc_enhc 0 config_changed
        __repacd_config_gwcon_ap_mode 0
    fi
    if [ "$auto_config_enable" -eq 0 ]; then
        __repacd_reset_existing_config
    else
	config_load wireless
        config_load repacd
        config_foreach __repacd_reset_existing_config_new son-iface
    fi

    if [ "$traffic_separation_active" -gt 0 ]; then
        config_foreach __repacd_delete_bssid wifi-iface \
            $network_backhaul
    else
        config_foreach __repacd_delete_bssid wifi-iface \
            $managed_network
    fi

    if [ "$device_type" = 'RE' ] && [ "$gwcon_mode" != "CAP" ]; then
        if [ "$traffic_separation_enabled" -gt 0 ] && \
           __repacd_is_son_mode; then
            if __repacd_check_additional_network_exist; then
		if [ "$auto_config_enable" -eq 0 ]; then
                    # reset additional vaps if traffic separation enabled
                    local disable_24g_ap=0 disable_5g_ap=0
                    local disable_24g_sta=1 disable_5g_sta=1

                    config_load network
                    config_load wireless
                    config_foreach __repacd_disable_vap wifi-iface \
                        $network_backhaul 'ap' $disable_24g_ap $disable_5g_ap config_changed
                    config_foreach __repacd_disable_vap wifi-iface \
                        $network_guest 'ap' $disable_24g_ap $disable_5g_ap config_changed

                    if [ "$traffic_separation_active" -eq 1 ]; then
                        config_foreach __repacd_delete_vlan_interfaces wifi-iface \
                            $managed_network $lan_vid 'ap' net_config_changed
                        config_foreach __repacd_delete_vlan_interfaces wifi-iface \
                            $network_guest $guest_vid 'ap' net_config_changed
                        config_foreach __repacd_delete_vlan_interfaces wifi-iface \
                            $managed_network $lan_vid 'sta' net_config_changed
                        config_foreach __repacd_delete_vlan_interfaces wifi-iface \
                            $network_guest $guest_vid 'sta' net_config_changed
                        config_foreach __repacd_disable_vap wifi-iface \
                            $network_backhaul 'sta' $disable_24g_sta $disable_5g_sta config_changed
                    fi
                    uci_commit wireless
                    uci_commit network

                    __repacd_reset_additional_config
		else
		    config_load wireless
		    config_foreach __repacd_set_sta_vap wifi-iface "1" config_changed
		    uci_commit wireless
		fi
                activate_ts=1
            fi
        fi
    fi

    if [ "$wsplcd_enabled" -gt 0 ]; then
        wsplcd_restart=1
    fi

    __repacd_restart_dependencies
   if [ "$traffic_separation_enabled" -gt 0 ]; then
        __repacd_wifi_set_otherband_bssids $network_backhaul
   else
        __repacd_wifi_set_otherband_bssids $managed_network
   fi

   if [ "$activate_ts" -eq 1 ]; then
        config_load network
        config_load wireless
	if [ "$auto_config_enable" -eq 0 ]; then
            config_foreach __repacd_add_vlan_interfaces wifi-iface \
                $managed_network $lan_vid 'both' net_config_changed
            config_foreach __repacd_add_vlan_interfaces wifi-iface \
                $network_guest $guest_vid $guest_backhaul_iface net_config_changed
            uci_commit wireless

            __repacd_add_ethernet_vlan_interfaces $network_guest net_config_changed
            uci_commit network
	else
	    config_load repacd
	    config_foreach __repacd_config_vlan son-iface net_config_changed
	    uci_commit network
            uci_commit wireless
	fi
        __repacd_restart_firewall

        uci_set repacd repacd TrafficSeparationActive '1'
        uci_commit repacd

        if [ "$net_config_changed" -gt 0 ] || [ "$config_changed" -gt 0 ]; then
            config_changed=0
            hyd_stop=1
            hyd_start=1
            wsplcd_stop=1
            wsplcd_start=1
            __repacd_restart_dependencies
        fi
    fi

    if ! __repacd_gw_mode; then
        # Start the daemon that monitors link status in CAP mode, telling
        # the daemon that it is an auto config-triggered restart.
        #
        # In this mode, it will just keep checking that the link to the
        # gateway is still present on ethernet.
        start-stop-daemon -S -x /usr/sbin/repacd-run.sh -b -- \
            "son" CAP $config_re_mode $resolved_re_mode $resolved_re_submode \
            autoconf
    fi
}

# Force a restart into NonCAP mode using the SON algorithms.
#
# @see restart_in_noncap_mode
__restart_in_noncap_mode_son() {
    local device_type activate_ts
    __repacd_auto_config_new
    __repacd_hyt
    config_load repacd
    config_get managed_network repacd ManagedNetwork 'lan'
    config_get device_type repacd DeviceType 'RE'
    config_get gwcon_mode repacd GatewayConnectedMode 'AP'
    config_get traffic_separation_enabled repacd TrafficSeparationEnabled '0'
    config_get traffic_separation_active repacd TrafficSeparationActive '0'
    config_get daisy_chain WiFiLink DaisyChain '0'
    config_get backhaul_ssid repacd BackhaulSSID $def_backhaul_ssid
    config_get backhaul_enc repacd BackhaulEnc $def_backhaul_enc
    config_get backhaul_key repacd BackhaulKey $def_backhaul_key
    config_get network_guest repacd NetworkGuest 'guest'
    config_get guest_ssid repacd GuestSSID $def_guest_ssid
    config_get guest_enc repacd GuestEnc $def_guest_enc
    config_get guest_key repacd GuestKey $def_guest_key
    config_get guest_backhaul_iface repacd NetworkGuestBackhaulInterface 'both'
    config_get manage_vap_ind WiFiLink 'ManageVAPInd' '0'
    __repacd_get_config_re_mode config_re_mode
    activate_ts=0

    __stop_repacd_run

    net_config_changed=0


    # Apply the SSID and passphrase to all interfaces to ensure that if we are
    # switching into a SON mode where there are two STA interfaces, they all
    # have the right credentials. For the non-SON and WDS modes, this is
    # subject to the RE WPS enhancement rules.
    __repacd_config_wps_pbc_enhc 0 config_changed

    if [ "$auto_config_enable" -eq 0 ]; then
	__repacd_reset_existing_config
    else
	config_load wireless
	config_foreach __repacd_set_sta_vap wifi-iface "0" config_changed
	uci_commit wireless
        config_load repacd
        config_foreach __repacd_reset_existing_config_new son-iface
	__repacd_update_sta_config
    fi
    # Need to resolve the generic NonCAP role to the actual configuration.
    if [ "$device_type" = 'RE' ]; then
        __repacd_config_re_mode

        if [ "$gwcon_mode" != "CAP" ]; then
            if [ "$traffic_separation_enabled" -gt 0 ] && \
               __repacd_is_son_mode; then
               if __repacd_check_additional_network_exist; then
		    if [ "$auto_config_enable" -eq 0  ]; then
                        # reset additional vaps if traffic separation enabled
                        local disable_24g_ap=0 disable_5g_ap=0

                        config_load wireless
                        # Enable spcl AP VAPs on NON-CAP only if daisy chain is enabled.
                        # They are for multi hop support. We don't need them if daisy
                        # chain is disabled.
                        if [ "$daisy_chain" -gt 0 ]; then
                            config_foreach __repacd_disable_vap wifi-iface \
                                $network_backhaul 'ap' $disable_24g_ap $disable_5g_ap config_changed
                        fi
                        config_foreach __repacd_disable_vap wifi-iface \
                            $network_guest 'ap' $disable_24g_ap $disable_5g_ap config_changed
                        uci_commit wireless

                        __repacd_reset_additional_config

                        config_load wireless
                        config_foreach __repacd_reconfig_sta_vaps wifi-iface $managed_network $network_backhaul \
                            "$backhaul_ssid" "$backhaul_enc" "$backhaul_key" \
                            "$backhaul_sae_pwe"
                        uci_commit wireless
		    fi
                    activate_ts=1
               fi
            fi
        fi
    else
        __repacd_config_client_mode
    fi

    if [ "$wsplcd_enabled" -gt 0 ]; then
        wsplcd_restart=1
    fi

    __repacd_restart_dependencies
   if [ "$traffic_separation_enabled" -gt 0 ]; then
        __repacd_wifi_set_otherband_bssids $network_backhaul
   else
        __repacd_wifi_set_otherband_bssids $managed_network
   fi

    # create vlan interfaces required for traffic separation.
    if [ "$activate_ts" -eq 1 ]; then
        config_load network
        config_load wireless

        if [ "$auto_config_enable" -eq 0 ]; then
            config_foreach __repacd_add_vlan_interfaces wifi-iface \
                $managed_network $lan_vid 'both' net_config_changed
            config_foreach __repacd_add_vlan_interfaces wifi-iface \
                $network_guest $guest_vid $guest_backhaul_iface net_config_changed
            uci_commit wireless

            __repacd_add_ethernet_vlan_interfaces $network_guest net_config_changed
            uci_commit network
        else
	    config_load repacd
	    config_foreach __repacd_config_vlan son-iface net_config_changed
	    uci_commit network
            uci_commit wireless
        fi
        __repacd_restart_firewall

        uci_set repacd repacd TrafficSeparationActive '1'
        uci_commit repacd

        # stop/start hyd only if there is any change in netwrok config
        # due to addition of VLAN interfaces. If required VLAN iface already
        # present then we can avoid hyd stop/start. This will save some time
        # and avoid any unnecessary delays after repacd restart.
        if [ "$net_config_changed" -gt 0 ] || [ "$config_changed" -gt 0 ]; then
            config_changed=0
            hyd_stop=1
            hyd_start=1
            wsplcd_stop=1
            wsplcd_start=1
            __repacd_restart_dependencies
        fi
    fi

    if ! __repacd_gw_mode; then
        # Start the script that monitors the link state, telling the daemon that
        # it is an auto config-triggered restart.
        #
        # In this NonCAP mode, it will keep checking whether there is a link
        # to the gateway over ethernet.
        start-stop-daemon -S -x /usr/sbin/repacd-run.sh -b -- \
            "son" NonCAP $config_re_mode $resolved_re_mode \
            $resolved_re_submode autoconf
    fi
}

# Force a restart into Range Extender (RE) mode with the SON algorithms.
#
# @see restart_in_re_mode
__restart_in_re_mode_son() {
    config_load repacd
    config_get managed_network repacd ManagedNetwork 'lan'

    __stop_repacd_run
    # By resetting the configuration, this will apply the same SSID and
    # passphrase to all interfaces. Then enable the interfaces as appropriate
    # for RE mode.
    __repacd_config_wps_pbc_enhc 0 config_changed
    if [ "$auto_config_enable" -eq 0 ]; then
        __repacd_reset_existing_config
    else
        config_load repacd
        config_foreach __repacd_reset_existing_config_new son-iface
    fi
    __repacd_config_re_mode

    if [ "$wsplcd_enabled" -gt 0 ]; then
        wsplcd_restart=1
    fi

    __repacd_restart_dependencies
    __repacd_wifi_set_otherband_bssids $managed_network

    if ! __repacd_gw_mode; then
        # Start the script that monitors the link state, telling the daemon
        # that it is an auto config-triggered restart.
        #
        # In this Range Extender mode, it will keep checking whether there is
        # a link to the gateway over ethernet and that the Wi-Fi link is
        # sufficient to continue operating as an RE.
        start-stop-daemon -S -x /usr/sbin/repacd-run.sh -b -- \
            "son" RE $config_re_mode $resolved_re_mode $resolved_re_submode \
            autoconf
    fi
}


