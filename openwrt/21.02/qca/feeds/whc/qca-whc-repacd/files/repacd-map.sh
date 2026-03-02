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

map_primary_nw='' map_backhaul_nw=''
map_country=0
map_primary_vlan=0
map_bsta_max_preference=0
anqp_ele="272:34108cfdf0020df1f7000000733000030101"
repacdPID=$(ps | grep repacd-run.sh | grep -v grep | awk '{print$1}')

# DPP config
map_dpp_enabled=0
dpp_sta_found=0
dpp_sta_iface=''
dpp_config_key=0
dpp_key_len=0
map_num_radio=0
dpp_gas_comeback_delay=0
enable_anqp_interworking=0
MAP_TEMP_INTF_LIST="/tmp/mapTempIntfList"
MAP_TEMP_INTF_LIST_WSPLCD="/tmp/mapTempIntfListWsplcd"

# Fast Onboarding
map_fast_onboarding=0
dot1x_support=0
map_use_high_pref_sta=0
current_sta='' preferred_sta=''
preferred_mld_sta=0
dummy_vaps_enable=0

# Ezmesh 6G enable
map_6g_enable=0
enable_mlo=0
is_current_sta_mlo=0
enable_single_netdev=0

#Increasing MAP_MAX_ADDITIONA_VAP needs changes also in all scheme template file
MAP_MAX_ADDITIONA_VAP=3
#Increasng MAP_MAX_MLD_COUNT needs changes in all scheme template file
MAP_MAX_MLD_COUNT=8

kernel_version=`uname -r |cut -d. -f1`
#Multibsta variable
number_of_bsta=0
number_of_mlo_radio=0
number_of_non_11be_radio=0
total_bsta_mld_count=0
mld_bsta_group=''
sta_configured=0
MBsta_mlo_sta_iface_list=''
MBsta_non_mlo_sa_iface_list=''

MBsta_BHssid=''
MBsta_BHkey=''
MBsta_BHencryption=''
MBsta_mld_for_eth=''
MBsta_multi_up_required=0
MBsta_connected_bhssid=''
MBsta_connected_bhkey=''
MBsta_connected_bhencryption=''
MBsta_mlo_bstagroup=''
MBsta_nonmlo_bstagroup=''

#Hardcoding guest bridge based on sigma-dut changes and update board_name globally
guest_bridge="bridge20"
board_name=""
[ -f /tmp/sysinfo/sdk_board_name  ] && {
        board_name=ap$(cat /tmp/sysinfo/sdk_board_name | awk -F 'ap' '{print $2}')
}


. /lib/functions/repacd-cmn.sh
. /lib/functions/hyfi-iface.sh

config_load 'repacd'
config_get IsActive8021x MAPConfig 'IsActive8021x'
config_get dot1x_support MAPConfig  'Enable8021x'
if [ -z "$IsActive8021x" ]; then
    uci set repacd.MAPConfig.IsActive8021x=$dot1x_support
    uci commit repacd
fi

__repacd_str_handle_special_char() {
    local fronthaul_ssid
    fronthaul_ssid=$1
    fronthaul_ssid=${fronthaul_ssid//\\/\\\\\\\\}
    fronthaul_ssid=${fronthaul_ssid//,/\\\\,}
    fronthaul_ssid=${fronthaul_ssid//&/\\&}
    fronthaul_ssid=${fronthaul_ssid//!/\\!}
    eval $2='$fronthaul_ssid'
}

#Check if interface is ethernet interface
#input:$1 interface name
#return 1 if ethernet interface else 0
__repacd_is_eth_interface() {
    local iface_name=$1
    local ifaces_eth

    ifaces_eth=$(ifconfig 2>&1 | grep eth)
    ifaces=$(echo "$ifaces_eth" | cut -d ' ' -f1)
    for iface in $ifaces; do
        if [ "$iface" = "$iface_name" ]; then
            return 1
        fi
    done

    return 0
}

# Set egress and ingress priority map per VLAN interface
__repacd_map_set_egress_ingress_per_intf() {
    local ifname="$1"
    local vlan_id="$2"

    vconfig set_egress_map "$ifname.$vlan_id" 0 0
    vconfig set_egress_map "$ifname.$vlan_id" 1 1
    vconfig set_egress_map "$ifname.$vlan_id" 2 2
    vconfig set_egress_map "$ifname.$vlan_id" 3 3
    vconfig set_egress_map "$ifname.$vlan_id" 4 4
    vconfig set_egress_map "$ifname.$vlan_id" 5 5
    vconfig set_egress_map "$ifname.$vlan_id" 6 6
    vconfig set_egress_map "$ifname.$vlan_id" 7 7
    vconfig set_ingress_map "$ifname.$vlan_id" 0 0
    vconfig set_ingress_map "$ifname.$vlan_id" 1 1
    vconfig set_ingress_map "$ifname.$vlan_id" 2 2
    vconfig set_ingress_map "$ifname.$vlan_id" 3 3
    vconfig set_ingress_map "$ifname.$vlan_id" 4 4
    vconfig set_ingress_map "$ifname.$vlan_id" 5 5
    vconfig set_ingress_map "$ifname.$vlan_id" 6 6
    vconfig set_ingress_map "$ifname.$vlan_id" 7 7
}

# The below logic is based on C bitmap like implementation
# 8 4 2 1 - where 1 -> 2G, 2 -> 5G, 4 -> 5GL, 8 -> 6G
# Combinations of these values will support all the combination
# where vap param needs to be enabled or disabled
# input 1 : $name
# input 2 : band in which vap is configured (2.4G/5G/6G)
# input 3 : vap repacd configuration, based on which vap param is updated
# input 4 : vap param to be updated
# input 5 : Current value of the vap param
__repacd_map_set_vap_param_from_config() {
    local name=$1
    local current_band_vap=$2
    local vap_param_config=$3
    local vap_param=$4
    local current_vap_param=$5

    if [ "$vap_param_config" == "0" ]; then
        local param_in_conf
        param_in_conf=$(uci get wireless.$name.$vap_param)
        if [ "$param_in_conf" == "1" ];then
            __repacd_update_vap_param "$name" "$vap_param" 0
        fi
    elif [ "$current_band_vap" == "2G" ]; then
        case $vap_param_config in
            1|3|5|7|9|11|13|15)
                __repacd_update_vap_param "$name" "$vap_param" 1
                ;;
            2|4|6|8|10|12|14)
                if [ "$current_vap_param" -eq 1 ]; then
                    __repacd_update_vap_param "$name" "$vap_param" 0
                fi
                ;;
        esac
    elif [ "$current_band_vap" == "5G" ]; then
        case $vap_param_config in
            2|3|6|7|10|11|14|15)
                __repacd_update_vap_param "$name" "$vap_param" 1
                ;;
            1|4|5|8|9|12|13)
                if [ "$current_vap_param" -eq 1 ]; then
                    __repacd_update_vap_param "$name" "$vap_param" 0
                fi
                ;;
        esac
    elif [ "$current_band_vap" == "5GL" ]; then
        case $vap_param_config in
            4|5|6|7|12|13|14|15)
                __repacd_update_vap_param "$name" "$vap_param" 1
                ;;
            1|2|3|8|9|10|11)
                if [ "$current_vap_param" -eq 1 ]; then
                    __repacd_update_vap_param "$name" "$vap_param" 0
                fi
                ;;
        esac
    elif [ "$current_band_vap" == "6G" ]; then
        case $vap_param_config in
            8|9|10|11|12|13|14|15)
                __repacd_update_vap_param "$name" "$vap_param" 1
                ;;
            1|2|3|4|5|6|7)
                if [ "$current_vap_param" -eq 1 ]; then
                    __repacd_update_vap_param "$name" "$vap_param" 0
                fi
                ;;
        esac
    elif [ "$current_band_vap" == "6GL" ]; then
        case $vap_param_config in
            8|9|10|11|12|13|14|15)
                __repacd_update_vap_param "$name" "$vap_param" 1
                ;;
            1|2|3|4|5|6|7)
                if [ "$current_vap_param" -eq 1 ]; then
                    __repacd_update_vap_param "$name" "$vap_param" 0
                fi
                ;;
        esac
    fi

}

# Create necessary VLAN interfaces for the backhaul vaps and add the
# created VLAN interfaces to the given network.
# VLAN interfaces are created by concatenating interface name and vlan id.
# input: $1 config
# input: $2 network name
# input: $3 VLAN id
__repacd_map_add_vlan_backhaul() {
    local config="$1"
    local nw_name="$2"
    local vlan_id="$3"
    local iface network disabled device

    config_get ifname "$config" ifname
    config_get network "$config" network
    config_get disabled "$config" disabled '0'
    config_get mode "$config" mode
    config_get MapBSSType "$config" MapBSSType '0'
    config_get mld "$config" mld ''

    if [ -n "$ifname" -a "$disabled" -eq 0 -a "$network" = "$map_backhaul_nw" \
            -a "$mode" = "ap" ]; then
        # if r2 STA Assoc DisAllowed do not create vlan
        if [ $(($((MapBSSType&4)) >> 2)) -eq 1 ]; then
            return
        fi

        if [ "$enable_single_netdev" -eq 1 ]; then
            if [ -n "$mld" ]; then
                ifname=$mld
            fi
        fi

        if [ "$map_single_r1r2_bh" -eq 1 ]; then
            if [ "$nw_name" = "$map_primary_nw" ]; then
                __repacd_add_interface "$nw_name" "$ifname"
            fi
        fi

        __repacd_echo "Apply Vlan $ifname $vlan_id $brprefix$nw_name"
        __repacd_add_interface "$nw_name" "$ifname.$vlan_id"
    fi
}

# Set ingress and egress priorty maps on necessary VLAN interfaces for the backhaul vaps
# to the given network.
# We get VLAN interfaces by concatenating interface name and vlan id.
# input: $1 config
# input: $2 network name
# input: $3 VLAN id
__repacd_map_set_egress_ingress_backhaul() {
    local config="$1"
    local nw_name="$2"
    local vlan_id="$3"
    local iface network disabled device

    config_get ifname "$config" ifname
    config_get network "$config" network
    config_get disabled "$config" disabled '0'
    config_get mode "$config" mode
    config_get MapBSSType "$config" MapBSSType '0'

    if [ -n "$ifname" -a "$disabled" -eq 0 -a "$network" = "$map_backhaul_nw" \
            -a "$mode" = "ap" ]; then
        # if r2 STA Assoc DisAllowed do not set.
        if [ $(($((MapBSSType&4)) >> 2)) -eq 1 ]; then
            return
        fi

        __repacd_echo "Set egress/ingress port for $ifname $vlan_id $brprefix$nw_name"
        __repacd_map_set_egress_ingress_per_intf $ifname $vlan_id
    fi
}

# Create necessary VLAN interfaces for ethernet interfaces and add the
# created VLAN interfaces to the given network.
# VLAN interfaces are created by concatenating interface name and vlan id.
# input: $1 network name
# input: $2 VLAN id
__repacd_map_add_vlan_ethernet() {
    local network="$1"
    local vlan_id="$2"

    iface_list="$(hyfi_network_get_ifnames $map_primary_nw)"

    for ifname in $iface_list; do
        __repacd_is_eth_interface $ifname
        is_eth_iface=$?
        if [ "$is_eth_iface" -eq 0 ]; then
            continue
        fi

        __repacd_echo "Apply Vlan $ifname $vlan_id $brprefix$network"
        __repacd_add_interface "$network" "$ifname.$vlan_id"

        swconfig dev switch0 vlan $vlan_id set ports "0t 1t 2t 3t 4t"
        swconfig dev switch0 vlan $vlan_id set ports "0t 1t 2t 3t 4t"
        swconfig dev switch0 set apply
    done
}

# Set egress and ingress priorty maps on necessary VLAN interfaces for the ethernet ports
# to the given network.
# We get VLAN interfaces by concatenating interface name and vlan id.
# input: $1 network name
# input: $2 VLAN id
__repacd_map_set_egress_ingress_ports_ethernet() {
    local network="$1"
    local vlan_id="$2"

    iface_list="$(hyfi_network_get_ifnames $map_primary_nw)"

    for ifname in $iface_list; do
        __repacd_is_eth_interface $ifname
        is_eth_iface=$?
        if [ "$is_eth_iface" -eq 0 ]; then
            continue
        fi

        __repacd_echo "Set egress/ingress priority map for $ifname $vlan_id $brprefix$network"
        __repacd_map_set_egress_ingress_per_intf $ifname $vlan_id
    done
}

# Create necessary VLAN interfaces for Easy Mesh
__repacd_map_apply_vlan_config() {
    __repacd_echo "Enable Easy Mesh Traffic Separation"
    local num_vlan=$num_vlan_supported

    # Create new bridge based on num Vlan configured by user
    config_load network
    for i in Primary One Two Three; do
        config_get nw_name MAPConfig "VlanNetwork"$i '0'
        config_get vlan_id MAPConfig "VlanIDNw"$i '0'

        if [ "$num_vlan" -eq 0 ]; then
            break
        fi

        if [ "$vlan_id" -eq 0 ]; then
            return
        fi

        # Configure backHaul BSS with VLAN for primary and secondary networks
        config_load wireless
        config_foreach __repacd_map_add_vlan_backhaul wifi-iface $nw_name $vlan_id

        # Configure ethernet interface with VLAN for secondary networks
        if [ "$i" != "Primary" ]; then
            __repacd_map_add_vlan_ethernet $nw_name $vlan_id
        fi

        num_vlan=$((num_vlan-1))
    done
}

# Set engress/ingress priorty maps
__repacd_map_set_egress_ingress_ports() {
    __repacd_echo "Setting egress and ingress priority map"
    local num_vlan=$num_vlan_supported

    config_load network
    for i in Primary One Two Three; do
        config_get nw_name MAPConfig "VlanNetwork"$i '0'
        config_get vlan_id MAPConfig "VlanIDNw"$i '0'

        if [ "$num_vlan" -eq 0 ]; then
            break
        fi

        if [ "$vlan_id" -eq 0 ]; then
            return
        fi

        # Configure egress ingress priority maps for backHaul BSS
        # with VLAN for primary and secondary networks
        config_load wireless
        config_foreach __repacd_map_set_egress_ingress_backhaul wifi-iface $nw_name $vlan_id

        # Configure egress ingress priority maps for ethernet interface
        # with VLAN for secondary networks
        if [ "$i" != "Primary" ]; then
            __repacd_map_set_egress_ingress_ports_ethernet $nw_name $vlan_id
        fi

        num_vlan=$((num_vlan-1))
    done
}

# Create firewall rules for the given network if the rule does not exist.
# input: $1 nw_index network index
__repacd_set_firewall_dhcp_rules_map() {
    local nw_index=$1
    local network no_rule
    local dhcp_key dhcp_value dhcp_setting

    config_load repacd
    config_get network MAPConfig "VlanNetwork"$nw_index '0'
    config_get dhcp_setting MAPConfig "dhcpSettingsNw"$nw_index '0'

    # Set firewall rules for network
    __repacd_set_firewall_rules $network

    no_rule=$(uci show dhcp | grep zone | grep "$network")
    if [ -z "$no_rule" ]; then
        # DHCP Settings
        uci set dhcp.$network=dhcp
        uci set dhcp.$network.interface=$network
        uci set dhcp.$network.dhcpv6=server
        uci set dhcp.$network.ra=server

        for setting in $dhcp_setting
        do
            dhcp_key=$(echo "$setting" | cut -d '=' -f1)
            dhcp_value=$(echo "$setting" | cut -d '=' -f2)
            uci set dhcp.$network.$dhcp_key=$dhcp_value
        done

        uci_commit dhcp
    fi
}

# Delete interface and device section from network file
__repacd_delete_vlan_network_device() {
    local device="$1"
    local network_name=''
    local bridge='' nw='' br='' matched_nw_name=''

    config_get network_name "$device" name
    bridges=$(uci show network | grep "device=")

    for br in $bridges; do
       matched_nw_name=$(echo $br | cut -d "'" -f2)
       nw=$(echo $br | cut -d "." -f 2)
        if [ "$map_primary_nw" != "$nw" ]; then
            if [ "$network_name" == "$matched_nw_name" ]; then
                uci delete network.$nw
                uci delete "network.$device"
            fi
        fi
    done
}


# Delete guest bridges, firewall and DHCP settings for guest networks
__repacd_map_delete_traffic_sep_config() {
    local is_controller=$1
    local bridges dhcp_uci br nw
    local ifaces_eth iface_wan ifaces
    local first_config_required

    config_load 'repacd'
    config_get_bool first_config_required MAPConfig 'FirstConfigRequired' '0'

    # On reset delete all network configurations other than
    # primary

    if [ "$openwrt_version_check" -eq 1 ]; then
        config_load 'network'
        config_foreach __repacd_delete_vlan_network_device device
        uci commit network
    else
        bridges=$(uci show network | grep bridge)
        for br in $bridges; do
            nw=$(echo $br | cut -d "." -f 2)
            if [ "$map_primary_nw" != "$nw" ]; then
                uci delete network.$nw
            fi
        done
    fi


    # Clean Primary ifname list so any vlan configuration will
    # be erased and add ethernet interface to ifname
    if [ "$openwrt_version_check" -eq 1 ]; then
        local ifnames_config
        hyfi_network_get_device_config "$map_primary_nw" ifnames_config

        uci delete network.$ifnames_config.ports
    else
        uci_set network "$map_primary_nw" ifname ' '
    fi

    ifaces_eth=$(ifconfig -a 2>&1 | grep eth)
    if [ "$openwrt_version_check" -eq 1 ]; then
        iface_wan=$(uci get network.wan.device)
    else
        iface_wan=$(uci get network.wan.ifname)
    fi
    ifaces=$(echo "$ifaces_eth" | cut -d ' ' -f1)
    for iface in $ifaces; do
        # Delete interface that is vlan configured
        echo "$iface" | grep '\.' >/dev/null 2>&1
        if [ "$?" -eq "0" ]; then
            continue
        fi

        # Add ethernet lan interface to primary ifname if link is detected
        if [ "$iface" != "$iface_wan" ]; then
            __repacd_add_interface $map_primary_nw $iface
        fi
    done

    # Reset Firewall and DHCP Settings on Controller
    if [ "$is_controller" -eq 1 ]; then
        if [ "$gwcon_mode" = 'CAP' ]; then
            local tsbridge=0
            firewalls=$(uci show firewall | grep zone | grep name)
            for fw in $firewalls; do
                nw=$(echo $fw | cut -d "=" -f 2 | cut -d "'" -f 2)
                if [ "$map_primary_nw" != "$nw" -o "wan" != "$nw" ]; then
                    tsbridge=1
                fi
            done
            if [ "$tsbridge" -eq 0 ]; then
                # If no firewall zone rule exists for additional bridge,
                # copy default firewall config
                cp /rom/etc/config/firewall /etc/config/firewall
            fi
         else
             # Copy default firewall config
             cp /rom/etc/config/firewall /etc/config/firewall
         fi

       # On reset delete all dhcp configurations other than
       # primary
       dhcp_uci=$(uci show dhcp | grep interface)
       for br in $dhcp_uci; do
           nw=$(echo $br | cut -d "." -f 2)
           if [ "$map_primary_nw" != "$nw" -a "wan" != "$nw" ]; then
               uci delete dhcp.$nw
           fi
       done
    fi

    if [ "$map_ts_enabled" -eq 0 ] || [ "$first_config_required" -eq 1 ]; then
        __repacd_echo "Easy Mesh Traffic Separation Disabled"
        uci_set repacd MAPConfig 'MapTrafficSeparationActive' 0
        uci_set $MAP MultiAP 'NumberOfVLANSupported' 0
        uci_set $MAP MultiAP 'Map2TrafficSepEnabled' 0
        uci_set wsplcd config 'NumberOfVLANSupported' 0
        uci_set wsplcd config 'Map2TrafficSepEnabled' 0
        uci_set $MAP MultiAP 'Map8021QPCP' 0
        uci_set $MAP MultiAP 'FronthaulSSIDPrimary' ''
        uci_set $MAP MultiAP 'VlanIDNwPrimary' 0
        uci_set $MAP MultiAP 'FronthaulSSIDNwOne' ''
        uci_set $MAP MultiAP 'VlanIDNwOne' 0
        uci_set $MAP MultiAP 'FronthaulSSIDNwTwo' ''
        uci_set $MAP MultiAP 'VlanIDNwTwo' 0
        uci_set $MAP MultiAP 'FronthaulSSIDNwThree' ''
        uci_set $MAP MultiAP 'VlanIDNwThree' 0

        uci_set $MAP MultiAP 'AdditionalFHssid1' ''
        uci_set $MAP MultiAP 'AdditionalFHkey1' ''
        uci_set $MAP MultiAP 'AdditionalFHauth1' ''

        uci_set $MAP MultiAP 'AdditionalFHssid2' ''
        uci_set $MAP MultiAP 'AdditionalFHkey2' ''
        uci_set $MAP MultiAP 'AdditionalFHauth2' ''

        uci_set $MAP MultiAP 'AdditionalFHssid3' ''
        uci_set $MAP MultiAP 'AdditionalFHkey3' ''
        uci_set $MAP MultiAP 'AdditionalFHauth3' ''

    fi
}

# Add or remove bridges based on user configuration. If traffic separation is
# enabled create bridge with network name, IP address and proto as specified
# by user. Add interface and vlanID for secondary bridge based on primary lan
# ifname list. If traffic separation is disabled remove all bridges except primary
# bridge
__repacd_map_reset_default_bridge_config() {
    local is_controller=$1
    local num_vlan=$num_vlan_supported
    config_load 'ezmesh'
    config_get_bool external_controller MultiAP 'ExternalController' '0'

    if [ "$external_controller" != "1" ]; then
        __repacd_map_delete_traffic_sep_config $is_controller
    fi

    if [ "$map_ts_enabled" -eq 0 ]; then
        __repacd_echo "Easy Mesh Traffic Separation Disabled"
        # Set eth0 mac address for R2 profile but traffic separation policy as 0
            __repacd_echo "set eth0 mac address for br-lan"
            __repacd_map_set_bridge_mac
        return
    fi

    # Create new bridge based on num Vlan configured by user
    config_load network
    for i in One Two Three; do
        if [ "$num_vlan" -eq 0 -o "$num_vlan" -eq 1 ]; then
            break
        fi

        config_get nw_name MAPConfig "VlanNetwork"$i '0'
        config_get br_proto MAPConfig "BridgeProtoNw"$i '0'
        config_get br_ip MAPConfig "BridgeIPAddressNw"$i '0'

        __repacd_echo "Creating Bridge : $brprefix$nw_name"
        __repacd_echo "Bridge Proto : $br_proto"
        __repacd_echo "Bridge IP : $br_ip"

        if [ "$openwrt_version_check" -eq 1 ]; then
            local network_section=$(uci add network device)
            uci_set network $network_section name "$brprefix$nw_name"
            uci_set network $network_section type "bridge"
        fi

        uci set network.$nw_name=interface

        if [ "$openwrt_version_check" -eq 1 ]; then
            uci_set network "$nw_name" device "$brprefix$nw_name"
        fi
        uci_set network "$nw_name" proto $br_proto
        if [ "$br_proto" = "static" ]; then
            uci_set network "$nw_name" ipaddr $br_ip
        fi
        uci_set network "$nw_name" netmask "255.255.255.0"
        uci_set network "$nw_name" force_link 1
        uci_set network "$nw_name" family ipv4
        uci_set network "$nw_name" bridge_empty 1

        if [ "$openwrt_version_check" -eq 0 ]; then
            uci_set network "$nw_name" type "bridge"
            uci_set network "$nw_name" ifname ' '
        fi

        # Set firewall settings on controller for guest nw
        if [ "$is_controller" -eq 1 ]; then
            __repacd_set_firewall_dhcp_rules_map $i
        fi

        uci_commit network
        num_vlan=$((num_vlan-1))
    done

    # set bridge MAC
    __repacd_map_set_bridge_mac
}

__map_set_bridge_mac()
{
    local config="$1"
    local dev_name="$2"
    local br_mac="$3"

    config_get name "$config" name

    if [ -n "$dev_name" -a "$name" == "$dev_name" ]; then
        uci_set network "$config" macaddr $br_mac
        uci commit network
    fi
}

# Set bridge MAC
__repacd_map_set_bridge_mac() {
    local num_vlan=$num_vlan_supported
    local ifaces_eth iface_wan ifaces
    local nw_name br_mac sameMAC

    config_get nw_name MAPConfig "VlanNetworkPrimary" '0'
    config_get sameMAC MAPConfig "TSUseSameBridgeMAC" '0'

    br_mac=$(ifconfig eth0 | grep HWaddr | awk '{print $5}')
    if [ "$openwrt_version_check" -eq 0 ]; then
        uci_set network "$nw_name" macaddr $br_mac
    else
        config_load network
        config_foreach __map_set_bridge_mac device $brprefix$nw_name "$br_mac"
    fi

    ifconfig $brprefix$nw_name hw ether "$br_mac"

    # Create new bridge based on num Vlan configured by user
    config_load network
    for i in One Two Three; do
        if [ "$num_vlan" -eq 0 -o "$num_vlan" -eq 1 ]; then
            break
        fi

        config_get nw_name MAPConfig "VlanNetwork"$i '0'
        if [ "$i" = "One" -a "$sameMAC" -eq 0 ]; then
            br_mac=$(ifconfig eth1 | grep HWaddr | awk '{print $5}')
        fi

        # Wifi Interface should be up by now
        if [ "$i" = "Two" -a "$sameMAC" -eq 0 ]; then
            br_mac=$(uci show wireless | grep macaddr | grep wifi0 | cut -d '=' -f2 |
                         sed 's/^.\(.*\).$/\1/')
        fi

        if [ "$i" = "Three" -a "$sameMAC" -eq 0 ]; then
            br_mac=$(uci show wireless | grep macaddr | grep wifi1 | cut -d '=' -f2 |
                         sed 's/^.\(.*\).$/\1/')
        fi

        if [ "$openwrt_version_check" -eq 0 ]; then
            uci_set network "$nw_name" macaddr $br_mac
        else
            config_load network
            config_foreach __map_set_bridge_mac device $brprefix$nw_name "$br_mac"
        fi

        ifconfig $brprefix$nw_name hw ether "$br_mac"
        uci_commit network
        num_vlan=$((num_vlan-1))
    done
}

# Iterate the MLD section name from MLD Role
# And increment number of mld configured
#
# input: $1 config: section name
# input: $2 role_to_match: MLD Role to match the MLD Section
# output: $3 mld_section: MLD Section name
__repacd_wifimon_get_mld_matching_role() {
    local config="$1"
    local role_to_match="$2"
    local role

    config_get role "$config" role

    if [ "$role" == "$role_to_match" ]; then
        configured_mld_count=$((configured_mld_count+1))
        eval "$3=$config"
    fi
}

# Iterate the MLD section name
# And increment number of mld configured
#
# input: $1 config: section name
__repacd_wifimon_get_configured_mld_count() {
    configured_mld_count=$((configured_mld_count+1))
}

# Generate MLD section based on MLDConfig and available MLD section
#
# If MLD section is pre-configured, then use it for MLO
# otherwise create new AP MLD section based on MLDConfig parameter and
# create new STA MLD section based on BackhaulMld parameter
#
# If Non-AP MLD is not present, then create new Non-AP MLD section
# and remaining MLD is created as AP MLD based on MLDConfig parameter
#
# If Non-AP MLD is present but BackhaulMld parameter is different,
# then MLD mentioned in BackhaulMld parameter will be changed to Non-AP MLD
# and Non-AP MLD already present will be change to AP MLD
__repacd_generate_mld_section() {
    local mld_config mld_count mld_dev_index=1
    local mld_available
    local mld_device=''
    configured_mld_count=0

    config_load repacd
    config_get mld_config MAPConfig 'MLDConfig' ''
    config_get mld_mac_addr MAPConfig 'mld_mac_addr' ''
    config_get backhaul_mld MAPConfig 'BackhaulMld' ''

    if [ -z "$mld_config" ]; then
        # MLDConfig is not configured, could be a Agent or SLO mode
        return
    fi
    config_load wireless
    config_foreach __repacd_wifimon_get_configured_mld_count wifi-mld

    mld_count=$(eval "echo $mld_config | grep -o mld | wc -l")
    __repacd_echo "Available MLD section: $configured_mld_count"
    __repacd_echo "Required MLD section: $mld_count"

    mldIndex=0
    while [ $mld_dev_index -le $mld_count ]; do
        __repacd_create_mld_section "mld_default_ssid" "$mld_mac_addr" "AP"
        mld_dev_index=$((mld_dev_index+1))
    done
}

__repacd_update_mld() {
    local MLD=
    local fronthaul_ssid backhaul_ssid fronthaul_mld backhaul_mld
    local map_version map_ts_enabled num_vlan_supported additional_vaps
    local fh_vap_count ad_vap_index num_vlan ad_vap

    __repacd_get_mld_devices MLD
    __repacd_echo "MLD Section Found: $MLD"
    config_load repacd
    config_get fronthaul_ssid MAPConfig 'FronthaulSSID' ''
    config_get backhaul_ssid MAPConfig 'BackhaulSSID' ''
    config_get fronthaul_mld MAPConfig 'FronthaulMld' ''
    config_get backhaul_mld MAPConfig 'BackhaulMld' ''
    config_get map_version MAPConfig 'MapVersionEnabled'
    config_get map_ts_enabled MAPConfig 'MapTrafficSeparationEnable' '0'
    config_get num_vlan_supported MAPConfig 'NumberOfVLANSupported' '0'

    if [ -n "$fronthaul_ssid" ] && [ -n $fronthaul_mld ]; then
        uci_set wireless $fronthaul_mld mld_ssid "$fronthaul_ssid"
    fi

    if [ -n $backhaul_ssid ] && [ -n $backhaul_mld ]; then
        uci_set wireless $backhaul_mld mld_ssid $backhaul_ssid
    fi

    if [ "$map_version" -gt 1 -a "$map_ts_enabled" -gt 0 ]; then
        num_vlan=$num_vlan_supported
        fh_vap_count=0
        for i in One Two Three; do
            # if 1 then VLAN is configured on Primary VLAN
            if [ "$num_vlan" -eq 0 -o "$num_vlan" -eq 1 ]; then
                break
            fi

            config_get fronthaul_mld MAPConfig "FronthaulMldNw"$i ''
            config_get fronthaul_ssid MAPConfig "FronthaulSSIDNw"$i ''

            if [ -n "$fronthaul_ssid" ] && [ -n $fronthaul_mld ]; then
                uci_set wireless $fronthaul_mld mld_ssid "$fronthaul_ssid"
            fi
            num_vlan=$((num_vlan-1))
        done
    fi

    ad_vap_index=1
    while [ $ad_vap_index -le $MAP_MAX_ADDITIONA_VAP ]; do
        config_get fronthaul_ssid MAPConfig "AdditionalFHssid"$ad_vap_index ''
        config_get fronthaul_mld MAPConfig "AdditionalFHmld"$ad_vap_index ''
        if [ -n "$fronthaul_ssid" ]; then
            __repacd_echo "Additional FH MLD Found: $fronthaul_mld"
            if [ -n "$fronthaul_ssid" ] && [ -n $fronthaul_mld ]; then
                uci_set wireless $fronthaul_mld mld_ssid "$fronthaul_ssid"
            fi
        fi
        ad_vap_index=$((ad_vap_index+1))
    done

    uci_commit wireless
}

# Delete all managed VAPs and re-create them according to the current
# configuration.
#
# Unlike the above, this is destructive in that all VAPs not marked as
# repacd_security_unmanaged will be deleted. If this is not desired, set the
# FirstConfigRequired flag to 0 to prevent this step from taking place. In
# this case, the VAPs must be created manually in a manner that matches the
# expectations of this script.
#
# input: $1 - is_controller: whether this device is acting as the controller
__repacd_reset_map_default_config() {
    local is_controller=$1

    local fronthaul_ssid fronthaul_key backhaul_ssid backhaul_key backhaul_authtype
    local backhaul_suffix smartmonitor_ssid
    local fh_vap_count vlan_id nw_name num_vlan vlan_nw_postfix
    local map_r1_sta_assoc_disallow map_r2_sta_assoc_disallow
    local vap_bh_bss_type
    local ad_vap_index
    local fronthaul_mld backhaul_mld
    local auth_primary_server auth_primary_port auth_primary_secret
    local auth_secondary_server auth_secondary_port auth_secondary_secret
    local rsn_preauth rsn_preauth_interfaces wpa_group_rekey ieee80211w
    local group_mgmt_cipher suiteb
    local fh_skip_vap fhNw_skip_vap fhNwOne_skip_vap fhNwTwo_skip_vap fhNwThree_skip_vap
    local fh_hidden fhNw_hidden fhNwOne_hidden fhNwTwo_hidden fhNwThree_hidden
    # Default values
    vlan_id=0 map_r1_sta_assoc_disallow=0 map_r2_sta_assoc_disallow=0
    vap_bh_bss_type=$((map_r1_sta_assoc_disallow|map_r2_sta_assoc_disallow))
    fh_vap_count=1 nw_name="$primarybridgename"

    config_load repacd
    config_get fronthaul_ssid MAPConfig 'FronthaulSSID' ''
    config_get fronthaul_key MAPConfig 'FronthaulKey' ''
    config_get fronthaul_authtype MAPConfig 'FronthaulAuthType' 0
    config_get backhaul_ssid MAPConfig 'BackhaulSSID' ''
    config_get backhaul_key MAPConfig 'BackhaulKey' ''
    config_get backhaul_suffix MAPConfig 'BackhaulSuffix' ''
    config_get backhaul_authtype MAPConfig 'BackhaulAuthType' 0
    config_get enable_mlo MAPConfig 'EnableMLO' 0
    config_get fronthaul_mld MAPConfig 'FronthaulMld' ''
    config_get backhaul_mld MAPConfig 'BackhaulMld' ''
    config_get fh_skip_vap MAPConfig 'Fronthaul_skipvap' '0'
    config_get fh_hidden MAPConfig 'Fronthaul_hidden' '0'
    config_get dummy_vaps_enable MAPConfig "EnableCloningOptimization" 0

    config_get smartmonitor_ssid MAPConfig 'SmartMonitorSSID' 'ap_smart_monitor'

    if [ "$map_version" -gt 1 ]; then
        config_get map_r1_sta_assoc_disallow MAPConfig 'R1AgentAssocDisAllowed' '0'
        config_get map_r2_sta_assoc_disallow MAPConfig 'R2AgentAssocDisAllowed' '0'
        vap_bh_bss_type=$(($((map_r1_sta_assoc_disallow<<3))|$((map_r2_sta_assoc_disallow<<2))))
        if [ "$map_ts_enabled" -gt 0 ]; then
            config_get map_primary_vlan MAPConfig 'VlanIDNwPrimary' '0'
            nw_name=$map_primary_nw
        fi
    fi

    config_load wireless
    config_foreach __repacd_delete_managed_vaps wifi-iface $managed_network
    uci_commit wireless

    if [ "$is_controller" -eq 1 ]; then
        config_foreach __repacd_delete_mlds wifi-mld
        uci_commit wireless
    fi

    # Clear out the environment of all CONFIG_ variables. Seems like there
    # should be some way to do this in UCI.
    unset $(set | grep '^CONFIG_' | awk -F'=' '{print $1}')

    # Reload to reset our variable state after the deletion
    config_load wireless

    # Generate mld section based on MLDConfig parameter
    if [ "$enable_slo" -eq 1 ]; then
        __repacd_generate_mld_section
        __repacd_echo "Next available MLD Index: $mldIndex"
    fi

    if [ "$enable_mlo" -eq 1 ]; then
        __repacd_update_mld
    fi

    if [ -z "$fronthaul_ssid" ]; then
        local ssid_suffix
        __repacd_generate_ssid_suffix ssid_suffix
        fronthaul_ssid="mapsig-${ssid_suffix}"
        __repacd_generate_psk fronthaul_key
    fi

    if [ -z "$backhaul_ssid" ]; then
        backhaul_ssid="${fronthaul_ssid}${backhaul_suffix}"

        if [ "$fronthaul_ssid" = "$backhaul_ssid" ]; then
            backhaul_key="$fronthaul_key"
        else
            __repacd_generate_psk backhaul_key
        fi
    fi

    if [ "$is_controller" -eq 0 -a "$dummy_vaps_enable" -gt 0 ];then
        fronthaul_ssid="map-${ssid_suffix}-FH"
        [ -z "$backhaul_ssid" ] && backhaul_ssid="map-${ssid_suffix}-BH"
    fi

    if [ "$map_fast_onboarding" -eq 1 -a "$is_controller" -eq 0 ] && [ "$MBsta_onboarding" -eq 1 ]; then
        __repacd_echo "Creating Multibsta & each sta on first index since MBsta_onboarding $MBsta_onboarding enabled"
        __repacd_create_vaps_map_fast_onboard "$is_controller" "$fronthaul_ssid" "$fronthaul_key" \
                                              "$backhaul_ssid" "$backhaul_key" "$backhaul_authtype"
    fi
    # Create Primary/Private VAP in each radio - with regular or overried credential
    __repacd_create_vaps_map "$is_controller" "$fronthaul_ssid" "$fronthaul_key" \
                             "$backhaul_ssid" "$backhaul_key" "$smartmonitor_ssid" \
                             "$fh_vap_count" "$vap_bh_bss_type" "$map_primary_vlan" "$nw_name" \
                             "$fronthaul_authtype" "$backhaul_authtype" "$vlan_nw_postfix" \
                             "$fronthaul_mld" "$backhaul_mld" "$fh_skip_vap" "$fh_hidden"

    config_load 'ezmesh'
    config_get_bool dualalid config 'DualAlid' '0'
    config_load 'repacd'
    config_get_bool enable_virtual_ap  MAPConfig 'EnableVirtualAP' '0'
    config_get virtualSSID MAPConfig 'VirtualSSID' 'VirtualAP'
    config_get virtual  MAPConfig 'VirtualDevice' 'wifi0'
    config_get fhNwOne_skip_vap MAPConfig 'FronthaulNwOne_skipvap' '0'
    config_get fhNwTwo_skip_vap MAPConfig 'FronthaulNwTwo_skipvap' '0'
    config_get fhNwThree_skip_vap MAPConfig 'FronthaulNwThree_skipvap' '0'
    config_get fhNwOne_hidden MAPConfig 'FronthaulNwOne_hidden' '0'
    config_get fhNwTwo_hidden MAPConfig 'FronthaulNwTwo_hidden' '0'
    config_get fhNwThree_hidden MAPConfig 'FronthaulNwThree_hidden' '0'

    local currentband;
    config_get currentband $virtual band '0'

    if [ "$enable_virtual_ap" -eq 1 ] && [ "$dualalid" -eq 1 ]; then
        name=$(uci add wireless wifi-iface)
        config_changed=1
        local config_slo_virtual_ap=1
        __repacd_init_vap "$name" $virtual 'ap' "$virtualSSID" \
                            "$smartmonitor_encryption" 'none' '0' "$config_slo_virtual_ap"
        __repacd_update_vap_param "$name" 'disable' 0
        __repacd_update_vap_param "$name" 'hidden' 1
        __repacd_update_vap_param "$name" 'map' "$map_version"
        __repacd_update_vap_param "$name" 'wsplcd_unmanaged' 1
        __repacd_update_vap_param "$name" 'repacd_security_unmanaged' 1
        __repacd_update_vap_param "$name" 'wps_pbc' 0
        __repacd_update_vap_param "$name" 'virtualAP' 1
        if [ "$currentband" -eq 3  ]; then
            __repacd_update_vap_param "$name" 'sae' 1
            __repacd_update_vap_param "$name" 'en_6g_sec_comp' 0
        fi
    fi
    uci_commit wireless

    # Create Secondary/Guest VAPs in each radio - with regular or overried credential
    if [ "$map_version" -gt 1 -a "$map_ts_enabled" -gt 0 ] && [ "$is_controller" -gt 0 -o "$dummy_vaps_enable" -gt 0 ]; then
        num_vlan=$num_vlan_supported
        for i in One Two Three; do
            # if 1 then VLAN is configured on Primary VLAN
            if [ "$num_vlan" -eq 0 -o "$num_vlan" -eq 1 ]; then
                break
            fi

            if [ "$i" == "One" ]; then
                fhNw_skip_vap=$fhNwOne_skip_vap
                fhNw_hidden=$fhNwOne_hidden
            elif [ "$i" == "Two" ];then
                fhNw_skip_vap=$fhNwTwo_skip_vap
                fhNw_hidden=$fhNwTwo_hidden
            elif [ "$i" == "Three" ];then
                fhNw_skip_vap=$fhNwThree_skip_vap
                fhNw_hidden=$fhNwThree_hidden
            fi

            config_get fronthaul_ssid MAPConfig "FronthaulSSIDNw"$i ''
            config_get fronthaul_mld MAPConfig "FronthaulMldNw"$i ''
            config_get fronthaul_key MAPConfig "FronthaulKeyNw"$i ''
            config_get fronthaul_authtype MAPConfig "FronthaulAuthTypeNw"$i 0
            config_get vlan_id MAPConfig "VlanIDNw"$i '0'
            config_get nw_name MAPConfig "VlanNetwork"$i ''

            if [ "$dot1x_support" -eq 1 ]; then
                config_get auth_primary_server MAPConfig "AuthPrimaryServerNw"$i ''
                config_get auth_primary_port MAPConfig "AuthPrimaryPortNw"$i ''
                config_get auth_primary_secret MAPConfig "AuthPrimarySecretNw"$i ''
                config_get auth_secondary_server MAPConfig "AuthSecondaryServerNw"$i ''
                config_get auth_secondary_port MAPConfig "AuthSecondaryPortNw"$i ''
                config_get auth_secondary_secret MAPConfig "AuthSecondarySecretNw"$i ''
                config_get rsn_preauth MAPConfig "RSN_PreAuthNw"$i ''
                config_get rsn_preauth_interfaces MAPConfig "RSN_PreAuth_InterfacesNw"$i ''
                config_get wpa_group_rekey MAPConfig "WPA_Group_ReKeyNw"$i ''
                config_get ieee80211w MAPConfig "IEEE80211WNw"$i ''
                config_get group_mgmt_cipher MAPConfig "Group_MGMT_CipherNw"$i ''
                config_get suiteb MAPConfig "SuiteBNw"$i ''
            fi

            vlan_nw_postfix="Nw"$i
            if [ -z "$fronthaul_ssid" -a "$dummy_vaps_enable" -eq 0 ]; then
                local ssid_suffix
                __repacd_generate_ssid_suffix ssid_suffix
                fronthaul_ssid="mapsig-${ssid_suffix}-$i"
                __repacd_generate_psk fronthaul_key
            fi

            __repacd_echo "Fronthaul SSID Network $i : $fronthaul_ssid"
            __repacd_echo "Fronthaul Key Network $i : $fronthaul_key"
            __repacd_echo "Fronthaul AuthType $i: $fronthaul_authtype"
            __repacd_echo "Fronthaul Vlan ID $i : $vlan_id"
            __repacd_echo "Fronthaul Network$i Name : $nw_name"

            fh_vap_count=$((fh_vap_count+1))
            # Create Guest VAPs in each radio - using regular or overried credential
            __repacd_create_vaps_map "$is_controller" "$fronthaul_ssid" "$fronthaul_key" \
                         "$backhaul_ssid" "$backhaul_key" "$smartmonitor_ssid" \
                         "$fh_vap_count" "$vap_bh_bss_type" "$vlan_id" "$nw_name" \
                         "$fronthaul_authtype" "$backhaul_authtype" "$vlan_nw_postfix" \
                         "$fronthaul_mld" "$backhaul_mld" \
                         "${fhNw_skip_vap}" "$fhNw_hidden" \
                         "$auth_primary_server" "$auth_primary_port" "$auth_primary_secret" \
                         "$auth_secondary_server" "$auth_secondary_port" "$auth_secondary_secret" \
                         "$rsn_preauth" "$rsn_preauth_interfaces" "$wpa_group_rekey" "$ieee80211w" \
                         "$group_mgmt_cipher" "$suiteb" "$map_ftauth" "$ft_256_key" "$map_mobility_domain"

            num_vlan=$((num_vlan-1))
        done
    fi


    # Create Additinal VAPs in each radio - with regular or overried credential
    __repacd_create_additional_FH_vaps_map "$is_controller"
    if [ "$map_fast_onboarding" -eq 1 -a "$is_controller" -eq 0 ] && [ "$MBsta_onboarding" -eq 0 ]; then
        __repacd_create_vaps_map_fast_onboard "$is_controller" "$fronthaul_ssid" "$fronthaul_key" \
                                              "$backhaul_ssid" "$backhaul_key" "$backhaul_authtype"
    fi

}

# Delete all of the VAPs for the given network that are marked as unmanaged.
#
# input: $1 config: section being considered
# input: $2 network: managed network name
__repacd_delete_managed_vaps() {
    local config="$1"
    local network repacd_security_unmanaged mode
    local vlan_nw_name backhaul_nw_name
    local virtualAP

    config_get network "$config" network
    config_get mode "$config" mode
    config_get_bool repacd_security_unmanaged "$config" repacd_security_unmanaged '0'
    config_get_bool virtualAP "$config" virtualAP '0'

    if [ "$2" = "$network" ] && [ "$repacd_security_unmanaged" -eq 0 ]; then
        uci delete "wireless.$config"
    fi

    if [ $mode = "ap_smart_monitor" ]; then
        uci delete "wireless.$config"
    fi

    if [ $virtualAP -eq 1 ]; then
        uci delete "wireless.$config"
    fi

    if [ "$map_version" -gt 1 ]; then
        for i in One Two Three; do
            config_get vlan_nw_name MAPConfig "VlanNetwork"$i ''
            if [ "$vlan_nw_name" = "$network" ] && [ "$repacd_security_unmanaged" -eq 0 ]; then
                uci delete "wireless.$config"
            fi
        done

        config_get backhaul_nw_name MAPConfig "VlanNetworkBackHaul" ''
        if [ "$backhaul_nw_name" = "$network" ] && [ "$repacd_security_unmanaged" -eq 0 ]; then
            uci delete "wireless.$config"
        fi
    fi
}

# Delete mlds when FirstConfigRequired flag is set to 1
__repacd_delete_mlds() {
    __repacd_echo "Deleting mld: $config"
    local config="$1"
    uci delete "wireless.$config"
}

# For Traffic Separation on EasyMesh Device we do not support combined
# Profile-1 and Profile-2 backhaul. Check if Profile-1 or Profile-2 STA
# Assoc is DisAllowed and create backHaul BSS accordingly.

# If Profile-1 Assoc is DisAllowed create 1 backhaul BSS and update MapBSSType
# for Profiel-1 Assoc DisAllowed
# If Profile-2 Assoc is DisAllowed create 1 backhaul BSS and update MapBSSType
# for Profiel-2 Assoc DisAllowed
# If both Profile-1 and Profile-2 Assoc is allowed then create 2 backhaul BSS
# Mark one as Profile-1 Assoc Disallowed and the other as Profile-2 Assoc DisAllowed
# If Fronthaul and backHaul are on the same VAP then split such a VAP to 1 frontaul
# and 2 backhaul VAP with above configuration
#
# input: $1 name: Current VAP config
# input: $2 device: the radio to which the VAP belongs
# input: $3 backhaul_ssid: the SSID to use on all backhaul VAPs
# input: $4 backhaul_key: the PSK for the backhaul, or the empty string
#                         for open mode
# input: $5 encryption: encryption
# input: $6 vap_bss_type: current vap attribute
# input: $7 vap_bh_bss_type: carries backhaul BSS attribute
# input: $8 backhaul_authtype: backhaul authentication type
# input: $9 config_slo_fh_mld: SLO backhaul mld config flag
# input: $9 config_slo_bh_mld: SLO fronthaul mld config flag
__reapcd_create_additional_backhaul_map() {
    local name=$1
    local device=$2
    local backhaul_ssid=$3
    local backhaul_key=$4
    local backhaul_encryption=$5
    local vap_bss_type=$6
    local vap_bh_bss_type=$7
    local backhaul_authtype=$8
    local config_slo_fh_mld=$9
    local config_slo_bh_mld=${10}

    local bss_attribute currentband
    local MAPeapolOverBridge

    local currentband;
    config_get currentband $device band '0'
    config_get MAPeapolOverBridge MAPConfig "MapEapolOverBridge" '0'
    config_get currentband $device band '0'

    # Split shared vap into 1 FH and 2 BH VAPs
    if [ "$vap_bss_type" -eq 96 ]; then
        # Mark VAP as fronthaul
        __repacd_update_vap_param "$name" 'MapBSSType' 32
        __repacd_update_vap_param "$name" 'network' $map_primary_nw
        if [ "$currentband" -eq 3  ]; then
            __repacd_update_vap_param "$name" 'en_6g_sec_comp' 0
        fi

        # Add new backhaul VAP
        name=$(uci add wireless wifi-iface)
        config_changed=1

        __repacd_init_vap "$name" $device 'ap' "$backhaul_ssid" \
                          $backhaul_encryption "$backhaul_key" '0' "$config_slo_fh_mld"
        __repacd_update_vap_param "$name" 'map' 1
        __repacd_update_vap_param "$name" 'MapBSSType' 64
        __repacd_update_vap_param "$name" 'wps_pbc' 0
        if [ "$currentband" -eq 3  ]; then
            __repacd_update_vap_param "$name" 'en_6g_sec_comp' 0
        fi


        if [ $backhaul_authtype -eq 1 -o $backhaul_authtype -eq 12 -o $backhaul_authtype -eq 15 -o $backhaul_authtype -eq 17 ]; then
            __repacd_update_vap_param "$name" 'sae' 1
        fi

        if [ $backhaul_authtype -eq 15 ] || [ $backhaul_authtype -eq 17 ]; then
            __repacd_update_vap_param "$name" 'wpa_key_mgmt' "SAE SAE-EXT-KEY"
            __repacd_update_vap_param "$name" 'add_sha256' "0"
            __repacd_update_vap_param "$name" 'add_sha384' "0"
            __repacd_update_vap_param "$name" 'group_mgmt_cipher' "BIP-GMAC-256"
            __repacd_update_vap_param "$name" 'sae_password' "$backhaul_key"
            uci add_list wireless."$name".sae_groups="19"
            uci commit repacd
        fi
    fi

    # update backhaul BSS based on DisAllow Bit
    if [ "$vap_bh_bss_type" -eq 8 ]; then
        # vap_bh_bss_type 8, Profile-1 Backhaul STA association disallowed
        bss_attribute=$((vap_bh_bss_type|64))
        __repacd_update_vap_param "$name" 'MapBSSType' $bss_attribute
        if [ "$map_primary_vlan" -gt 0 ]; then
            __repacd_update_vap_param "$name" 'network' $map_backhaul_nw
            [ "$is_controller" -eq 1 ] && __repacd_update_vap_param "$name" 'map8021qvlan' $map_primary_vlan
            if [ "$MAPeapolOverBridge" -gt 0 ]; then
                __repacd_update_vap_param "$name" 'vlan_bridge' "$brprefix$map_primary_nw"
            fi
        fi
    elif [ "$vap_bh_bss_type" -eq 4 ]; then
        # vap_bh_bss_type 4, Profile-2 Backhaul STA association disallowed
        bss_attribute=$((vap_bh_bss_type|64))
        __repacd_update_vap_param "$name" 'MapBSSType' $bss_attribute
        __repacd_update_vap_param "$name" 'network' $map_primary_nw

        # Disable the SAE authentication for R1 Agent BH BSS
        if [ $backhaul_authtype -eq 1 -o $backhaul_authtype -eq 12 -o $backhaul_authtype -eq 15 -o $backhaul_authtype -eq 17 ]; then
            __repacd_update_vap_param "$name" 'sae' 0
            __repacd_update_vap_param "$name" 'encryption' "psk2+ccmp"
        fi

        if [ "$map_primary_vlan" -gt 0 ]; then
            [ "$is_controller" -eq 1 ] && __repacd_update_vap_param "$name" 'map8021qvlan' $map_primary_vlan
        fi
    elif [ "$vap_bh_bss_type" -eq 0 ]; then
        if [ "$vap_bss_type" -eq 96 -a "$map_single_r1r2_bh" -eq 1 -a "$vlan_id" -gt 0 ]; then
            __repacd_update_vap_param "$name" 'network' $map_backhaul_nw
            [ "$is_controller" -eq 1 ] && __repacd_update_vap_param "$name" 'map8021qvlan' $map_primary_vlan
            if [ "$MAPeapolOverBridge" -gt 0 ]; then
                __repacd_update_vap_param "$name" 'vlan_bridge' "$brprefix$map_primary_nw"
            fi
            return
        fi

        # Update current bhBSS for r1 Agent assoc disallowed
        bss_attribute=$((vap_bh_bss_type|64|8))
        __repacd_update_vap_param "$name" 'MapBSSType' $bss_attribute
        __repacd_update_vap_param "$name" 'network' $map_primary_nw
        if [ "$map_primary_vlan" -gt 0 ]; then
            __repacd_update_vap_param "$name" 'network' $map_backhaul_nw
            [ "$is_controller" -eq 1 ] && __repacd_update_vap_param "$name" 'map8021qvlan' $map_primary_vlan
            if [ "$MAPeapolOverBridge" -gt 0 ]; then
                __repacd_update_vap_param "$name" 'vlan_bridge' "$brprefix$map_primary_nw"
            fi
        fi


        if [ $backhaul_authtype -eq 1 -o $backhaul_authtype -eq 12 -o $backhaul_authtype -eq 15 -o $backhaul_authtype -eq 17 ]; then
            __repacd_update_vap_param "$name" 'sae' 1
        fi

        if [ $backhaul_authtype -eq 15 ] || [ $backhaul_authtype -eq 17 ]; then
            __repacd_update_vap_param "$name" 'wpa_key_mgmt' "SAE SAE-EXT-KEY"
            __repacd_update_vap_param "$name" 'add_sha256' "0"
            __repacd_update_vap_param "$name" 'add_sha384' "0"
            __repacd_update_vap_param "$name" 'group_mgmt_cipher' "BIP-GMAC-256"
            __repacd_update_vap_param "$name" 'sae_password' "$key"
            uci add_list wireless."$name".sae_groups="19"
            uci commit repacd
        fi

        # Create One More bhBSS for r1 Agents
        name=$(uci add wireless wifi-iface)
        config_changed=1

        __repacd_init_vap "$name" $device 'ap' "$backhaul_ssid" \
                          $backhaul_encryption "$backhaul_key" '0' '1'

        # Disable the SAE authentication for R1 Agent BH BSS
        if [ $backhaul_authtype -eq 1 -o $backhaul_authtype -eq 12 -o $backhaul_authtype -eq 15 -o $backhaul_authtype -eq 17 ]; then
            __repacd_update_vap_param "$name" 'sae' 0
            __repacd_update_vap_param "$name" 'encryption' "psk2+ccmp"
        fi


        if [ "$currentband" -eq 3 ]; then
            __repacd_update_vap_param "$name" 'en_6g_sec_comp' 0
        fi

        __repacd_update_vap_param "$name" 'network' $map_primary_nw
        __repacd_update_vap_param "$name" 'map' $map_version
        # Update current bhBSS for r2 Agent assoc disallowed
        bss_attribute=$((vap_bh_bss_type|64|4))
        __repacd_update_vap_param "$name" 'MapBSSType' $bss_attribute
        __repacd_update_vap_param "$name" 'wps_pbc' 0
    fi
}

__repacd_create_additional_FH_vaps_map() {
    local is_controller=$1
    local fronthaul_ssid fronthaul_key fronthaul_authtype fronthaul_encryption additional_fh_vaps
    local ad_vap_index ad_vap device_5GL override_enabled device_6GL
    local additional_skip_vap additional1_skip_vap additional2_skip_vap additional3_skip_vap
    local additional_hidden additional1_hidden additional2_hidden additional3_hidden
    local config_slo_fh_mld=1

    config_get override_enabled MAPConfig OverrideVapConfigEnable
    config_get device_5GL MAPConfig Radio5GL 0
    config_get device_6GL MAPConfig Radio6GL 0
    config_get additional1_skip_vap MAPConfig AdditionalFH1_skipvap '0'
    config_get additional2_skip_vap MAPConfig AdditionalFH2_skipvap '0'
    config_get additional3_skip_vap MAPConfig AdditionalFH3_skipvap '0'
    config_get additional1_hidden MAPConfig AdditionalFH1_hidden '0'
    config_get additional2_hidden MAPConfig AdditionalFH2_hidden '0'
    config_get additional3_hidden MAPConfig AdditionalFH3_hidden '0'
    config_get map_version MAPConfig 'MapVersionEnabled'
    local DEVICES=
    __repacd_get_devices DEVICES

    for device in $DEVICES; do
        local repacd_auto_create_vaps create_ctrl_fbss
        local currentband band_string
        config_get currentband $device band '0'

        config_get_bool repacd_auto_create_vaps "$device" repacd_auto_create_vaps '1'
        config_get_bool create_ctrl_fbss "$device" repacd_create_ctrl_fbss '1'
        ad_vap_index=1

        if whc_is_5g_radio "$device"; then
            if [ "$currentband" -eq 3 ]; then
                if [ $device != $device_6GL ]; then
                    band_string="6G"
                else
                    band_string="6GL"
                fi
            elif [ $device != $device_5GL ]; then
                band_string="5G"
            else
                band_string="5GL"
            fi
        elif ! whc_is_5g_radio "$device"; then
            band_string="2G"
        fi

        while [ $ad_vap_index -le $MAP_MAX_ADDITIONA_VAP ]; do
            if [ "$override_enabled" -eq 1 ]; then
                config_get fronthaul_ssid MAPConfig "AdditionalFHOverride"${band_string}ssid${ad_vap_index}
                config_get fronthaul_key MAPConfig "AdditionalFHOverride"${band_string}key${ad_vap_index}
                config_get fronthaul_authtype MAPConfig "AdditionalFHOverride"${band_string}auth${ad_vap_index}
                config_get fronthaul_mld MAPConfig "AdditionalFHOverride"${band_string}mld${ad_vap_index}
            else
                config_get fronthaul_ssid MAPConfig "AdditionalFH"ssid${ad_vap_index}
                config_get fronthaul_key MAPConfig "AdditionalFH"key${ad_vap_index}
                config_get fronthaul_authtype MAPConfig "AdditionalFH"auth${ad_vap_index}
                config_get fronthaul_mld MAPConfig "AdditionalFH"mld${ad_vap_index}
            fi

            if [ -z "$fronthaul_ssid" ]; then
               break;
            fi
            if [ $ad_vap_index -eq 1 ]; then
                additional_skip_vap=$additional1_skip_vap
                additional_hidden=$additional1_hidden
            elif [ $ad_vap_index -eq 2 ]; then
                additional_skip_vap=$additional2_skip_vap
                additional_hidden=$additional2_hidden
            elif [ $ad_vap_index -eq 3 ]; then
                additional_skip_vap=$additional3_skip_vap
                additional_hidden=$additional3_hidden
            fi
            vap_index_to_store=$ad_vap_index
            ad_vap_index=$((ad_vap_index+1))
            fronthaul_encryption='none'
            [ -n "$fronthaul_key" ] && fronthaul_encryption='psk2+ccmp'
            [ -n "$fronthaul_key" -a "$fronthaul_authtype" -eq 12 ] && fronthaul_encryption='ccmp'
            [ -n "$fronthaul_key" -a "$fronthaul_authtype" -eq 13 ] && fronthaul_encryption='gcmp-256'
            [ -n "$fronthaul_key" -a "$fronthaul_authtype" -eq 14 ] && fronthaul_encryption='ccmp'
            [ -n "$fronthaul_key" -a "$fronthaul_authtype" -eq 15 ] && fronthaul_encryption='psk2+gcmp-256'
            [ -n "$fronthaul_key" -a "$fronthaul_authtype" -eq 16 ] && fronthaul_encryption='ccmp+gcmp-256'
            [ -n "$fronthaul_key" -a "$fronthaul_authtype" -eq 17 ] && fronthaul_encryption='psk2+ccmp+gcmp-256'

            if [ "$currentband" -eq "3" -a "$kernel_version" -eq "6" ]; then
                fronthaul_encryption='ccmp'
                if [ "$fronthaul_authtype" -eq 13 ]; then
                    fronthaul_encryption='gcmp-256'
                elif [ "$fronthaul_authtype" -eq 14 ]; then
                    fronthaul_encryption='ccmp'
                elif [ "$fronthaul_authtype" -eq 15 ]; then
                    fronthaul_encryption='psk2+gcmp-256'
                elif [ "$fronthaul_authtype" -eq 16 ]; then
                    fronthaul_encryption='ccmp+gcmp-256'
                elif [ "$fronthaul_authtype" -eq 17 ]; then
                    fronthaul_encryption='psk2+ccmp+gcmp-256'
                fi
            fi

            if [ "$repacd_auto_create_vaps" -gt 0 ]; then
                if [ "$is_controller" -eq 1 ] || [ "$create_ctrl_fbss" -gt 0 ]; then
                    name=$(uci add wireless wifi-iface)
                    config_changed=1
                    config_slo_fh_mld=1
                    if [ -n "$fronthaul_mld" ]; then
                        if __repacd_is_mld_vap "$fronthaul_mld" "$band_string"; then
                            config_slo_fh_mld=0
                        fi
                    fi

                    __repacd_init_vap "$name" $device 'ap' "$fronthaul_ssid" \
                                      "$fronthaul_encryption" "$fronthaul_key" '0' "$config_slo_fh_mld"
                    __repacd_update_vap_param "$name" 'map' 1
                    __repacd_update_vap_param "$name" 'MapBSSType' 32
                    __repacd_update_vap_param "$name" 'additional_fh_vap_index' $vap_index_to_store
                    if [ "$currentband" -eq 3  ]; then
                        __repacd_update_vap_param "$name" 'en_6g_sec_comp' 0
                    fi
                    __repacd_map_set_vap_param_from_config "$name" "$band_string" "$additional_skip_vap" \
                                                            "map_SkipVapRestart"
                    __repacd_map_set_vap_param_from_config "$name" "$band_string" "$additional_hidden" \
                                                            "hidden"

                    #steering & wps is disabled by default for additional_FH
                    __repacd_update_vap_param "$name" 'wps_pbc' 0
                    __repacd_update_vap_param "$name" 'SteeringDisabled' 1

                    if [ "$enable_mlo" -eq 1 -a -n "$fronthaul_mld" ]; then
                        if __repacd_is_mld_vap "$fronthaul_mld" "$band_string"; then
                            __repacd_echo "Attaching Additional fronthaul vap to  mld $fronthaul_mld"
                            __repacd_update_vap_param "$name" 'mld' $fronthaul_mld
                        fi
                    fi

                fi
                if [ "$map_version" -gt 1 ]; then
                    __repacd_update_vap_param "$name" 'map' $map_version
                    __repacd_update_vap_param "$name" 'network' $map_primary_nw
                    if [ "$is_controller" -gt 0 -o "$dummy_vaps_enable" -gt 0 ] && [ "$fronthaul_authtype" -eq 1 ]; then
                        __repacd_update_vap_param "$name" 'sae' 1
                        [ "$currentband" -eq 3  ] && __repacd_update_vap_param "$name" 'en_6g_sec_comp' 0
                    fi
                    if [ "$currentband" -eq "3" -a "$kernel_version" -eq "6" ] || [ "$fronthaul_authtype" -eq 12 ] || \
                       [ "$fronthaul_authtype" -eq 13 ] || [ "$fronthaul_authtype" -eq 14 ] || [ "$fronthaul_authtype" -eq 15 ] || \
                       [ "$fronthaul_authtype" -eq 16 ] || [ "$fronthaul_authtype" -eq 17 ]; then
                        __repacd_update_vap_param "$name" 'sae' 1
                        #Currently we are not using sae password in default config.
                        #sae_password should be supported in EasyMesh via vendor TLV
#                        uci add_list wireless."$name".sae_password="$fronthaul_key"
#                        uci add_list wireless."$name".sae_groups="19"
                        if [ "$currentband" -eq 3 ]; then
                            __repacd_update_vap_param "$name" 'sae_pwe' 1
                            if [ "$fronthaul_authtype" -eq 12 -o "$fronthaul_authtype" -eq 14 ]; then
                                __repacd_update_vap_param "$name" 'en_6g_sec_comp' 1
                            fi
                        else
                            __repacd_update_vap_param "$name" 'sae_pwe' 2
                            __repacd_update_vap_param "$name" 'en_6g_sec_comp' 0
                        fi
                    fi
                    if [ "$map_dpp_enabled" -eq 1 ]; then
                        __repacd_update_vap_param "$name" 'dpp_map' 1
                        __repacd_update_vap_param "$name" 'dpp_configurator_connectivity' 1
                        __repacd_update_vap_param "$name" 'disablecoext' 1
                        __repacd_update_vap_param "$name" 'gas_comeback_delay' $dpp_gas_comeback_delay
                    fi
                    if [ "$fronthaul_authtype" -eq 13 ] || [ "$fronthaul_authtype" -eq 14 ] || \
                       [ "$fronthaul_authtype" -eq 15 ] || [ "$fronthaul_authtype" -eq 16 ] || \
                       [ "$fronthaul_authtype" -eq 17 ]; then
                        if [ "$fronthaul_authtype" -eq 15 ] || [ "$fronthaul_authtype" -eq 17 ]; then
                            __repacd_update_vap_param "$name" 'wpa_key_mgmt' "WPA-PSK SAE SAE-EXT-KEY"
                        else
                            __repacd_update_vap_param "$name" 'wpa_key_mgmt' "SAE SAE-EXT-KEY"
                        fi
                        __repacd_update_vap_param "$name" 'add_sha256' "0"
                        __repacd_update_vap_param "$name" 'add_sha384' "0"
                        __repacd_update_vap_param "$name" 'group_mgmt_cipher' "BIP-GMAC-256"
                        __repacd_update_vap_param "$name" 'sae_password' "$key"
                        uci add_list wireless."$name".sae_groups="19"
                        uci commit repacd
                    fi
                fi
            fi
        done
    done
}

# Create the VAPs needed for Multi-AP SIG Topology Optimization with them all
# initially disabled.
#
# input: $1 is_controller: whether this device is acting as the controller
# input: $2 fronthaul_ssid: the SSID to use on all fronthaul VAPs
# input: $3 fronthaul_key: the PSK for the fronthaul, or the empty string
#                          for open mode
# input: $4 backhaul_ssid: the SSID to use on all backhaul VAPs
# input: $5 backhaul_key: the PSK for the backhaul, or the empty string
#                         for open mode
__repacd_create_vaps_map_fast_onboard() {
    local is_controller=$1
    local fronthaul_ssid=$2
    local fronthaul_key=$3
    local backhaul_ssid=$4
    local backhaul_key=$5
    local backhaul_authtype=$6
    local fronthaul_encryption backhaul_encryption
    local shared_vaps=0 num_vlan_supported
    local mloenabled create_ctrl_bbss

    config_load repacd
    config_get num_vlan_supported MAPConfig 'NumberOfVLANSupported' '0'

    if [ "$fronthaul_ssid" = "$backhaul_ssid" ]; then
        shared_vaps=1
    fi

    if [ -n "$fronthaul_key" ]; then
        fronthaul_encryption='psk2+ccmp'
    else
        fronthaul_encryption='none'
    fi


    local DEVICES=
    __repacd_get_devices DEVICES

    # Create STA Vaps
    for device in $DEVICES; do
        local repacd_auto_create_vaps repacd_create_bsta repacd_bsta_pref
        local create_backhaul_sta
        config_get_bool repacd_auto_create_vaps "$device" repacd_auto_create_vaps '1'
        config_get_bool repacd_create_bsta "$device" repacd_create_bsta '1'
        config_get repacd_bsta_pref "$device" repacd_map_bsta_preference '0'
        uci_set wireless $device disabled '0'
        config_get mloenabled "$device" mloenabled '0'

        if [ -n "$backhaul_key" ]; then
            backhaul_encryption='psk2+ccmp'
            #Enable SAE only mode for pure bakchaul BSS
            if [ "$map_version" -gt 1 ] && [ "$is_controller" -gt 0 -o "$dummy_vaps_enable" -eq 1 ] && \
                  [ "$shared_vaps" -eq 0 ]; then
                if [ "$backhaul_authtype" -eq 1 ]; then
                    __repacd_update_vap_param "$name" 'sae' 1
                    uci delete wireless."$name".sae_pwe
                    uci delete wireless.$name.wpa_key_mgmt
                    uci delete wireless.$name.add_sha256
                    uci delete wireless.$name.add_sha384
                    uci delete wireless.$name.group_mgmt_cipher
                    uci del_list wireless."$name".sae_groups="19"
                elif [ "$backhaul_authtype" -eq 12 ]; then
                    backhaul_encryption='ccmp'
                    __repacd_update_vap_param "$name" 'sae' 1
                    uci delete wireless.$name.wpa_key_mgmt
                    uci delete wireless.$name.add_sha256
                    uci delete wireless.$name.add_sha384
                    uci delete wireless.$name.group_mgmt_cipher
                    uci del_list wireless."$name".sae_groups="19"
                elif [ "$backhaul_authtype" -eq 17 ]; then
                    backhaul_encryption='psk2+ccmp+gcmp-256'
                    __repacd_update_vap_param "$name" 'sae' 1
                    __repacd_update_vap_param "$name" 'wpa_key_mgmt' "SAE SAE-EXT-KEY"
                    __repacd_update_vap_param "$name" 'add_sha256' "0"
                    __repacd_update_vap_param "$name" 'add_sha384' "0"
                    __repacd_update_vap_param "$name" 'group_mgmt_cipher' "BIP-GMAC-256"
                    __repacd_update_vap_param "$name" 'sae_password' "$backhaul_key"
                    uci add_list wireless."$name".sae_groups="19"
                elif [ "$backhaul_authtype" -eq 15 ]; then
                    backhaul_encryption='psk2+gcmp-256'
                    __repacd_update_vap_param "$name" 'sae' 1
                    __repacd_update_vap_param "$name" 'wpa_key_mgmt' "SAE SAE-EXT-KEY"
                    __repacd_update_vap_param "$name" 'add_sha256' "0"
                    __repacd_update_vap_param "$name" 'add_sha384' "0"
                    __repacd_update_vap_param "$name" 'group_mgmt_cipher' "BIP-GMAC-256"
                    __repacd_update_vap_param "$name" 'sae_password' "$backhaul_key"
                    uci add_list wireless."$name".sae_groups="19"
                else
                    __repacd_update_vap_param "$name" 'sae' 0
                    uci delete wireless."$name".sae_password
                    uci delete wireless."$name".sae_groups
                    uci delete wireless."$name".sae_pwe
                    uci delete wireless.$name.wpa_key_mgmt
                    uci delete wireless.$name.add_sha256
                    uci delete wireless.$name.add_sha384
                    uci delete wireless.$name.group_mgmt_cipher
                    uci del_list wireless."$name".sae_groups="19"
                fi
            fi
        else
            backhaul_encryption='none'
        fi

        local currentBand=$(cfg80211tool $device g_oper_reg_info | awk -F "=" '{print $4}' | cut -c-1)
        if [ "$currentBand" -eq "6" -a "$kernel_version" -eq "6" ]; then
            backhaul_encryption="ccmp"
            if [ "$backhaul_authtype" -eq 17 ]; then
                backhaul_encryption='psk2+ccmp+gcmp-256'
            elif [ "$backhaul_authtype" -eq 15 ]; then
                backhaul_encryption='psk2+gcmp-256'
            fi
        fi

        # Create one bSTA per radio
        local create_backhaul_sta bhmloenabled
        config_get_bool create_backhaul_sta "$device" repacd_create_backhaul_sta '0'
        config_get_bool bhmloenabled "$device" map_mbsta_bhmlo_enabled '0'

        __repacd_echo "User Configs: device $device bhmloenabled $bhmloenabled  create_ctrl_bbss $create_ctrl_bbss repacd_auto_create_vaps $repacd_auto_create_vaps repacd_create_bsta $repacd_create_bsta"
        if [ "$MBsta_onboarding" -eq 1 ]; then
            __repacd_echo "CREATING MULTIPLE STA IFACES"
            if [ "$repacd_auto_create_vaps" -gt 0 ] && [ "$repacd_create_bsta" -gt 0 ] && [ "$create_backhaul_sta" -eq 1 ]; then
                if [ -n "$mld_bsta_group" ]; then
                    if [ "$bhmloenabled" -eq 1 ]; then
                        __repacd_echo "STA MLD $mld_bsta_group"
                        sta_mld="$mld_bsta_group"
                    else
                        sta_configured=$((sta_configured+1))
                        sta_mld="mld$sta_configured"
                        __repacd_echo "STA MLD $sta_mld"
                    fi
                else
                    #SLO Case: Assign sta mld incrementing counter mld0,mld1...
                    sta_mld="mld$sta_configured"
                    sta_configured=$((sta_configured+1))
                fi
            # Create one bSTA per radio
            __repacd_create_sta_vap_map  "$device" "$is_controller" "$backhaul_ssid" \
                                    "$backhaul_key" "$backhaul_encryption" \
                                    "$backhaul_authtype"
            fi
        else
            __repacd_echo "CREATING ONLY ONE BSTA"
            if [ "$repacd_auto_create_vaps" -gt 0 ] && [ "$repacd_create_bsta" -gt 0 ] \
                    && ! whc_is_5g_radio "$device"; then
                    __repacd_create_sta_vap_map  "$device" "$is_controller" "$backhaul_ssid" \
                                                "$backhaul_key" "$backhaul_encryption" \
                                                "$backhaul_authtype"
            fi
        fi
    done
}

# Create STA VAPs needed for Multi-AP SIG Topology Optimization with them all
# initially disabled.
#
__repacd_create_sta_vap_map() {
    local device=$1
    local is_controller=$2
    local backhaul_ssid=$3
    local backhaul_key=$4
    local backhaul_encryption=$5
    local MBsta_upfrontmlo
    local backhaul_authtype=$6

    config_get MBsta_upfrontmlo MAPConfig 'MultibSTAConfigureMLObSTAUpfront' '0'
    # Create a bSTA interface
    name=$(uci add wireless wifi-iface)
    config_changed=1

    __repacd_init_vap "$name" $device 'sta' "$backhaul_ssid" \
                      "$backhaul_encryption" "$backhaul_key" '0' "$enable_slo"
    __repacd_update_vap_param "$name" 'map' 1
    __repacd_update_vap_param "$name" 'MapBSSType' 128

    config_get IndependentRepeaterMode MAPConfig EnableIndependentRepeaterMode '1'
    if [ "$IndependentRepeaterMode" -eq 1 ]; then
        __repacd_update_vap_param "$name" 'athnewind' 1
    fi
    __repacd_update_vap_param "$name" 'wps_state' 1
    __repacd_update_vap_param "$name" 'wps_pbc_skip' 1

    if [ "$backhaul_authtype" -eq 1 -o "$backhaul_authtype" -eq 12 ]; then
        __repacd_update_vap_param "$name" 'sae' 1
    fi
    if [ "$map_dpp_enabled" -eq 1 ]; then
        __repacd_update_vap_param "$name" 'dpp' 1
        __repacd_update_vap_param "$name" 'dpp_map' 1
        __repacd_update_vap_param "$name" 'disablecoext' 1
    fi

    # Enable 802.11k support so that the controller can ask for
    # beacon measurements from the bSTA
    __repacd_update_vap_param "$name" 'rrm' 1
    __repacd_update_vap_param "$name" 'rrm_capie' 1

    # update network name if traffic separation is enabled
    if [ "$map_version" -gt 1 ]; then
        __repacd_update_vap_param "$name" 'map' $map_version
        __repacd_update_vap_param "$name" 'network' $map_primary_nw
        if [ "$MBsta_onboarding" -eq 1 ] && [ "$map_ts_enabled" -gt 0 ]; then
            __repacd_update_vap_param "$name" 'network' $map_backhaul_nw
        fi
        if [ "$map_ts_enabled" -gt 0 -a "$is_controller" -eq 1 -a "$vlan_id" -gt 0 ]; then
            __repacd_update_vap_param "$name" 'network' $map_backhaul_nw
        fi
    fi

    if [ "$map_fast_onboarding" -eq 0 ] || [ "$map_use_high_pref_sta" -eq 0 ]; then
        # For initial onboarding, 2.4 GHz is the selected bSTA radio
        # (unless it is given a preference value of 0, in which case
        # no radio is marked as selected and we let the preference values
        # determine it entirely).
        if whc_is_5g_vap "$name" || [ "$repacd_bsta_pref" -eq 0 ]; then
            uci_set wireless $device repacd_map_bsta_selected '0'
        else
            uci_set wireless $device repacd_map_bsta_selected '1'
        fi
    fi
}

# Get the 802.1x vap encryption details
#
# input: $1 fronthaul_authtype: fronthaul repacd encryption type
# output: $2 fronthaul wireless encryption type
__repacd_get_dot1x_details () {
    local fronthaul_authtype=$1

    if [ "$dot1x_support" -eq 0 ]; then
        return
    fi

    if [ "$fronthaul_authtype" -eq 2 ]; then
        eval "$2=wpa2+ccmp"
        eval "$3=0x0010"
        eval "$4=0x0008"
    elif [ "$fronthaul_authtype" -eq 3 ]; then
        eval "$2=wpa2+tkip"
        eval "$3=0x0010"
        eval "$4=0x0004"
    elif [ "$fronthaul_authtype" -eq 4 ]; then
        eval "$2=wpa2+tkip+ccmp"
        eval "$3=0x0010"
        eval "$4=0x000C"
    elif [ "$fronthaul_authtype" -eq 5 ]; then
        eval "$2=wpa+tkip"
        eval "$3=0x0008"
        eval "$4=0x0004"
    elif [ "$fronthaul_authtype" -eq 6 ]; then
        eval "$2=wpa+ccmp"
        eval "$3=0x0008"
        eval "$4=0x0008"
    elif [ "$fronthaul_authtype" -eq 7 ]; then
        eval "$2=wpa+tkip+ccmp"
        eval "$3=0x0008"
        eval "$4=0x000C"
    elif [ "$fronthaul_authtype" -eq 8 ]; then
        eval "$2=wpa-mixed+tkip"
        eval "$3=0x0200"
        eval "$4=0x0004"
    elif [ "$fronthaul_authtype" -eq 9 ]; then
        eval "$2=wpa-mixed+ccmp"
        eval "$3=0x0200"
        eval "$4=0x0008"
    elif [ "$fronthaul_authtype" -eq 10 ]; then
        eval "$2=wpa-mixed+tkip+ccmp"
        eval "$3=0x0200"
        eval "$4=0x000C"
    elif [ "$fronthaul_authtype" -eq 11 ]; then
        eval "$2=suiteb"
        eval "$3=0x0400"
        eval "$4=0"
    else
        eval "$2="
        eval "$3="
        eval "$4=0x0008"
    fi
}

# Check whether the VAP is part of a MLD or not
#
# input: $1 mld_device: mld device index to configure
# input: $2 band: band information of the vap
__repacd_is_mld_vap() {
    local mld_device=$1
    local band=$2
    local mld_dev mld_dev_index=1
    local mld_band mld_group mld_config mld_count

    config_get mld_config MAPConfig 'MLDConfig' ''
    mld_count=$(eval "echo $mld_config | grep -o mld | wc -l")
    while [ $mld_dev_index -le $mld_count ]; do
        mld_group=$(eval "echo $mld_config | awk 'BEGIN { FS = \",\" }; { print \$$mld_dev_index }'")
        if [ -z "$mld_group" ]; then
            break;
        fi
        mld_dev=$(eval "echo $mld_group | awk 'BEGIN { FS = \":\" }; { print \$1 }'")
        mld_band=$(eval "echo $mld_group | awk 'BEGIN { FS = \":\" }; { print \$2 }'")
        if [ "$mld_dev" == "$mld_device" ]; then
            for i in $mld_band; do
                if [ "$i" == "$band" ]; then
                    #found radio device
                    return 0
                fi
            done
        fi
        mld_dev_index=$((mld_dev_index+1))
    done
    return 1
}

# Create the VAPs needed for Multi-AP SIG Topology Optimization with them all
# initially disabled.
#
# input: $1 is_controller: whether this device is acting as the controller
# input: $2 fronthaul_ssid: the SSID to use on all fronthaul VAPs
# input: $3 fronthaul_key: the PSK for the fronthaul, or the empty string
#                          for open mode
# input: $4 backhaul_ssid: the SSID to use on all backhaul VAPs
# input: $5 backhaul_key: the PSK for the backhaul, or the empty string
#                         for open mode
# input: $6 smartmonitor_ssid: the SSID to use on all smart monitor VAPs
# input: $7 fh_vap_count: the count of Fronthaul VAPs that are created on a radio
# input: $8 vap_bh_bss_type: carries backhaul BSS attribute
# input: $9 vlan_id: fronthaul vlan ID
# input: $10 nw_name: network to which the fronthaul belongs to
# input: $11 fronthaul_authtype: the authentication type used for fronthaul
# input: $12 backhaul_authtype: the authentication type used for backhaul
__repacd_create_vaps_map() {
    local is_controller=$1
    local fronthaul_ssid=$2
    local fronthaul_key=$3
    local backhaul_ssid=$4
    local backhaul_key=$5
    local smartmonitor_ssid=$6
    local fh_vap_count=$7
    local vap_bh_bss_type=$8
    local vlan_id=$9
    local nw_name=${10}
    local fronthaul_authtype=${11}
    local backhaul_authtype=${12}
    local vlan_nw_postfix=${13}
    local fronthaul_mld=${14}
    local backhaul_mld=${15}
    local fh_skip_vap=${16}
    local fh_hidden=${17}
    local auth_primary_server=${18}
    local auth_primary_port=${19}
    local auth_primary_secret=${20}
    local auth_secondary_server=${21}
    local auth_secondary_port=${22}
    local auth_secondary_secret=${23}
    local rsn_preauth=${24}
    local rsn_preauth_interfaces=${25}
    local wpa_group_rekey=${26}
    local ieee80211w=${27}
    local group_mgmt_cipher=${28}
    local suiteb=${29}
    local ft_auth=${30}
    local ft_256_key=${31}
    local mobility_domain=${32}

    local fronthaul_encryption backhaul_encryption fh_dot1x_encryption
    local bss_auth_type bss_encr_type
    local smartmonitor_encryption='none'
    local enable_smart_monitor_mode
    local bss_attribute
    local override_enabled override_value device_5GL device_band device_6GL
    local MAPeapolOverBridge

    # Whether the same VAP is used for fronthaul and backhaul or not.
    local shared_vaps=0
    config_load repacd
    config_get dummy_vaps_enable MAPConfig "EnableCloningOptimization" 0
    config_get MAPeapolOverBridge MAPConfig "MapEapolOverBridge" '0'
    config_get override_enabled MAPConfig OverrideVapConfigEnable
    config_get device_5GL MAPConfig Radio5GL 0
    config_get device_6GL MAPConfig Radio6GL 0
    config_get enable_wps_on_backhaul MAPConfig 'EnableBackhaulWPS' '0'

    __repacd_echo " cv: auth_primary_secret = $auth_primary_secret "
    __repacd_echo " cv: auth_secondary_secret = $auth_secondary_secret "

    if [ "$fronthaul_ssid" = "$backhaul_ssid" ]; then
        shared_vaps=1
    fi

    local DEVICES=
    __repacd_get_devices DEVICES

    for device in $DEVICES; do
        local repacd_auto_create_vaps repacd_create_bsta repacd_bsta_pref
        local create_ctrl_fbss create_ctrl_bbss neighbourfilter set_monrxfilter
        local disable wsplcd_unmanaged repacd_security_unmanaged
        local smart_monitor_state
        local no_vlan_val overrideSSID=''
        local currentband useOnlyOverride
        local config_slo_fh_mld=1 config_slo_bh_mld=1
        config_get_bool repacd_auto_create_vaps "$device" repacd_auto_create_vaps '1'
        config_get_bool repacd_create_bsta "$device" repacd_create_bsta '1'
        config_get repacd_bsta_pref "$device" repacd_map_bsta_preference '0'
        config_get_bool create_ctrl_fbss "$device" repacd_create_ctrl_fbss '1'
        config_get_bool create_ctrl_bbss "$device" repacd_create_ctrl_bbss '1'
        config_get_bool smart_monitor_state "$device" repacd_create_ctrl_fbss '0'
        config_get currentband $device band '0'
        uci_set wireless $device disabled '0'

        if [ "$map_version" -gt 1 -a "$map_ts_enabled" -gt 0 ]; then
            config_get no_vlan_val $device no_vlan '0'
            if [ "$no_vlan_val" -eq 0 ]; then
                uci_set wireless $device no_vlan '1'
            fi
        fi

        if [ -n "$fronthaul_key" ]; then
            fronthaul_encryption='psk2+ccmp'
            [ "$fronthaul_authtype" -eq 12 ] && fronthaul_encryption='ccmp'
            [ "$fronthaul_authtype" -eq 13 ] && fronthaul_encryption='gcmp-256'
            [ "$fronthaul_authtype" -eq 14 ] && fronthaul_encryption='ccmp'
            [ "$fronthaul_authtype" -eq 15 ] && fronthaul_encryption='psk2+gcmp-256'
            [ "$fronthaul_authtype" -eq 16 ] && fronthaul_encryption='ccmp+gcmp-256'
            [ "$fronthaul_authtype" -eq 17 ] && fronthaul_encryption='psk2+ccmp+gcmp-256'
        else
            fronthaul_encryption='none'
        fi

        if [ "$dot1x_support" -eq 1 ]; then
            __repacd_get_dot1x_details $fronthaul_authtype fh_dot1x_encryption \
            bss_auth_type bss_encr_type
            if [ -n "$fh_dot1x_encryption" ]; then
                fronthaul_encryption="$fh_dot1x_encryption"
                fronthaul_key=''
            fi
        fi

        if [ -n "$backhaul_key" ]; then
            backhaul_encryption='psk2+ccmp'
            #Enable SAE only mode for pure bakchaul BSS
            if [ "$map_version" -gt 1 ] && [ "$is_controller" -gt 0 -o "$dummy_vaps_enable" -eq 1 ] && \
                  [ "$shared_vaps" -eq 0 ]; then
                if [ "$backhaul_authtype" -eq 12 ]; then
                    backhaul_encryption='ccmp'
                elif [ "$backhaul_authtype" -eq 17 ]; then
                    backhaul_encryption='psk2+ccmp+gcmp-256'
                elif [ "$backhaul_authtype" -eq 15 ]; then
                    backhaul_encryption='psk2+gcmp-256'
                fi
            fi
        else
            backhaul_encryption='none'
        fi

        if [ "$currentband" -eq "3" -a "$kernel_version" -eq "6" ]; then
            backhaul_encryption="ccmp"
            fronthaul_encryption='ccmp'
            if [ "$backhaul_authtype" -eq 17 ]; then
                backhaul_encryption='psk2+ccmp+gcmp-256'
            elif [ "$backhaul_authtype" -eq 15 ]; then
                backhaul_encryption='psk2+gcmp-256'
            fi
            if [ "$fronthaul_authtype" -eq 13 ]; then
                fronthaul_encryption='gcmp-256'
            elif [ "$fronthaul_authtype" -eq 14 ]; then
                fronthaul_encryption='ccmp'
            elif [ "$fronthaul_authtype" -eq 15 ]; then
                fronthaul_encryption='psk2+gcmp-256'
            elif [ "$fronthaul_authtype" -eq 16 ]; then
                fronthaul_encryption='ccmp+gcmp-256'
            elif [ "$fronthaul_authtype" -eq 17 ]; then
                fronthaul_encryption='psk2+ccmp+gcmp-256'
            fi
        fi

        # Create one bSTA per radio
        if [ "$fh_vap_count" -eq 1 ] && [ "$repacd_auto_create_vaps" -gt 0 ] && \
               [ "$repacd_create_bsta" -gt 0 ] && [ "$map_fast_onboarding" -eq 0 ] && \
               ! whc_is_5g_radio "$device"; then

            __repacd_create_sta_vap_map  "$device" "$is_controller" "$backhaul_ssid" \
                                         "$backhaul_key" "$backhaul_encryption" \
                                         "$backhaul_authtype"
        fi

        if whc_is_5g_radio "$device"; then
            if [ "$currentband" -eq 3 ]; then
                if [ $device != $device_6GL ]; then
                    device_band="6G"
                else
                    device_band="6GL"
		fi
            elif [ $device != $device_5GL ]; then
                device_band="5G"
            else
                device_band="5GL"
            fi
        elif ! whc_is_5g_radio "$device"; then
            device_band="2G"
        fi

        config_slo_fh_mld=1
        if [ -n "$fronthaul_mld" ]; then
            if __repacd_is_mld_vap "$fronthaul_mld" "$device_band"; then
                config_slo_fh_mld=0
            fi
        fi
        config_slo_bh_mld=1
        if [ -n "$backhaul_mld" ]; then
            if __repacd_is_mld_vap "$backhaul_mld" "$device_band"; then
                config_slo_bh_mld=0
            fi
        fi

        config_get overrideSSID MAPConfig FHOverrideSSID"$vlan_nw_postfix""$device_band"
        config_get useOnlyOverride MAPConfig ForceOverrideCredential 0

        # If not acting as the controller, we will let the Multi-AP
        # Configuration procedure create the BSSes. However, due to
        # limitations in wsplcd, we have to create a BSS on each radio to
        # ensure AP Auto-Config takes place. If this is removed, then repacd
        # can be updated to only do this on the controller again.
        if [ "$repacd_auto_create_vaps" -gt 0 ]; then
            # Only create the fBSS on agents (to ensure wsplcd works) or on
            # the controller if so configured.
            if [ "$is_controller" -eq 0 -a "$dummy_vaps_enable" -eq 0 ] || [ "$create_ctrl_fbss" -gt 0 ] \
                && ! [ $useOnlyOverride -eq 1 -a -z "$overrideSSID" ]; then

                if [ "$dummy_vaps_enable" -eq 1 -a -z "$overrideSSID" -a -z "$fronthaul_ssid" ]; then
                   #Do not create Network/Vlan VAPs if both override and primary ssid is not mentioned
                   continue
                fi

                # Create the fBSS (which may also be a bBSS)
                name=$(uci add wireless wifi-iface)
                config_changed=1
                if [ "$override_enabled" -eq "1" ]; then
                    if [ "$fh_vap_count" -eq "1" -o -n "$vlan_nw_postfix" ]; then
                        config_get override_value MAPConfig FHOverrideSSID"$vlan_nw_postfix""$device_band"
                        if [ -n "$override_value" ]; then
                            fronthaul_ssid=$override_value
                            config_get fronthaul_key MAPConfig FHOverrideKey"$vlan_nw_postfix""$device_band"
                            config_get fronthaul_authtype MAPConfig FHOverrideAuthType"$vlan_nw_postfix""$device_band"
                            fronthaul_encryption='none'
                            [ -n "$fronthaul_key" ] && fronthaul_encryption='psk2+ccmp'
                            [ "$fronthaul_authtype" -eq 12 ] && fronthaul_encryption='ccmp'
                            [ "$fronthaul_authtype" -eq 13 ] && fronthaul_encryption='ccmp+gcmp-256'
                        else
                            fronthaul_ssid=$2
                            fronthaul_key=$3
                            fronthaul_authtype=${11}
                            fronthaul_encryption='none'
                            [ -n "$fronthaul_key" ] && fronthaul_encryption='psk2+ccmp'
                            [ "$fronthaul_authtype" -eq 12 ] && fronthaul_encryption='ccmp'
                            [ "$fronthaul_authtype" -eq 13 ] && fronthaul_encryption='ccmp+gcmp-256'
                        fi
                    fi
                fi

                if [ "$dot1x_support" -eq 1 ] && [ -n "$fh_dot1x_encryption" ]; then
                    __repacd_init_vap_dot1x "$name" $device 'ap' "$fronthaul_ssid" \
                    "$fronthaul_encryption" "$auth_primary_server" "$auth_primary_port" \
                    "$auth_primary_secret" "$auth_secondary_server" "$auth_secondary_port" \
                    "$auth_secondary_secret" $nw_name "$rsn_preauth" "$rsn_preauth_interfaces" \
                    "$wpa_group_rekey" "$ieee80211w" "$group_mgmt_cipher" "$suiteb" "$ft_auth" "$ft_256_key" \
                    "$mobility_domain" "$config_slo_fh_mld"
                else
                    __repacd_init_vap "$name" $device 'ap' "$fronthaul_ssid" \
                                      $fronthaul_encryption "$fronthaul_key" '0' "$config_slo_fh_mld"
                fi
                __repacd_update_vap_param "$name" 'map' 1

                if [ "$map_version" -gt 1 ]; then
                    __repacd_update_vap_param "$name" 'map' $map_version
                    __repacd_update_vap_param "$name" 'network' $map_primary_nw

                    if [ -n "$fh_dot1x_encryption" ]; then
                        __repacd_update_vap_param "$name" 'network' $nw_name
                    fi

                    if [ "$is_controller" -gt 0 -o "$dummy_vaps_enable" -eq 1 ] && [ "$fronthaul_authtype" -eq 1 ]; then
                        __repacd_update_vap_param "$name" 'sae' 1
                    fi

                    if [ "$currentband" -eq "3" -a "$kernel_version" -eq "6" ] || [ "$fronthaul_authtype" -eq 12 ] || \
                                                                                  [ "$fronthaul_authtype" -eq 13 ]; then
                        __repacd_update_vap_param "$name" 'sae' 1
#                        uci add_list wireless."$name".sae_password="$fronthaul_key"
#                        uci add_list wireless."$name".sae_groups="19"
                        if [ "$currentband" -eq 3 ]; then
                            __repacd_update_vap_param "$name" 'sae_pwe' 1
                        else
                            __repacd_update_vap_param "$name" 'sae_pwe' 2
                        fi
                    fi

                    if [ "$fronthaul_authtype" -eq 13 ] || [ "$fronthaul_authtype" -eq 14 ] || \
                       [ "$fronthaul_authtype" -eq 15 ] || [ "$fronthaul_authtype" -eq 16 ] || \
                       [ "$fronthaul_authtype" -eq 17 ]; then
                        if [ "$fronthaul_authtype" -eq 15 ] || [ "$fronthaul_authtype" -eq 17 ]; then
                            __repacd_update_vap_param "$name" 'wpa_key_mgmt' "WPA-PSK SAE SAE-EXT-KEY"
                        else
                            __repacd_update_vap_param "$name" 'wpa_key_mgmt' "SAE SAE-EXT-KEY"
                        fi
                        __repacd_update_vap_param "$name" 'add_sha256' "0"
                        __repacd_update_vap_param "$name" 'add_sha384' "0"
                        __repacd_update_vap_param "$name" 'group_mgmt_cipher' "BIP-GMAC-256"
                        __repacd_update_vap_param "$name" 'sae_password' "$key"
                        uci add_list wireless."$name".sae_groups="19"
                        uci commit repacd
                    fi

                    # Enable wps_pbc only on one fronthaul
                    if [ "$fh_vap_count" -gt 1 ]; then
                        __repacd_update_vap_param "$name" 'wps_pbc' 0
                    fi

                    if [ "$map_ts_enabled" -gt 0 ] && [ "$is_controller" -eq 1 -o "$dummy_vaps_enable" -gt 0 ]; then
                        __repacd_update_vap_param "$name" 'network' $nw_name
                        [ "$is_controller" -eq 1 ] && __repacd_update_vap_param "$name" 'mapVlanID' $vlan_id
                        [ "$is_controller" -eq 1 ] && __repacd_update_vap_param "$name" 'map8021qvlan' $map_primary_vlan
                    fi

                    if [ "$map_dpp_enabled" -eq 1 ]; then
                        __repacd_update_vap_param "$name" 'dpp_map' 1
                        __repacd_update_vap_param "$name" 'dpp_configurator_connectivity' 1
                        __repacd_update_vap_param "$name" 'disablecoext' 1
                        __repacd_update_vap_param "$name" 'gas_comeback_delay' $dpp_gas_comeback_delay
                    fi
                fi

                if [ "$shared_vaps" -gt 0 ]; then
                    # fBSS uses same VAP as bBSS
                    __repacd_update_vap_param "$name" 'MapBSSType' 96
                    if [ "$map_ts_enabled" -gt 0 -a "$is_controller" -eq 1 ]; then
                        __reapcd_create_additional_backhaul_map "$name $device" \
                                                                "$fronthaul_ssid" "$fronthaul_key" \
                                                                "$fronthaul_encryption" "96" \
                                                                "$vap_bh_bss_type" "$fronthaul_authtype" \
                                                                "$config_slo_fh_mld" "$config_slo_bh_mld"
                    fi
                else
                    # Distinct VAPs for backhaul and fronthaul
                    __repacd_update_vap_param "$name" 'MapBSSType' 32
                fi

                if [ "$enable_mlo" -eq 1 -a -n "$fronthaul_mld" ]; then
                    if __repacd_is_mld_vap "$fronthaul_mld" "$device_band"; then
                        __repacd_echo "Attaching fronthaul vap to  mld $fronthaul_mld"
                        __repacd_update_vap_param "$name" 'mld' $fronthaul_mld
                    fi
                fi

                if [ "$currentband" -eq 3 ]; then
                    __repacd_update_vap_param "$name" 'en_6g_sec_comp' 0
                    [ "$fronthaul_authtype" -eq 12 ] && __repacd_update_vap_param "$name" 'en_6g_sec_comp' 1
                fi
                __repacd_map_set_vap_param_from_config "$name" "$device_band" "$fh_skip_vap" "map_SkipVapRestart"
                __repacd_map_set_vap_param_from_config "$name" "$device_band" "$fh_hidden" "hidden"

                # Create smart monitor VAP per each radio on the agent and for
                # every fronthaul BSS on the controller
                # Check if smart monitor mode is enabled
                config_load 'repacd'
                config_get_bool enable_smart_monitor_mode MAPConfig 'EnableSmartMonitorMode' '0'
                if [ "$enable_smart_monitor_mode" -eq 1 ] && [ "$smart_monitor_state" -eq 1 ] && [ "$fh_vap_count" -eq 1 ]; then
                    name=$(uci add wireless wifi-iface)
                    config_changed=1

                    __repacd_init_vap "$name" $device 'ap_smart_monitor' "$smartmonitor_ssid" \
                                       $smartmonitor_encryption '0' '0'
                    __repacd_update_vap_param "$name" 'neighbourfilter' 1
                    __repacd_update_vap_param "$name" 'set_monrxfilter' 1
                    __repacd_update_vap_param "$name" 'disable' 0
                    __repacd_update_vap_param "$name" 'wsplcd_unmanaged' 1
                    __repacd_update_vap_param "$name" 'repacd_security_unmanaged' 1
                    __repacd_update_vap_param "$name" 'wps_pbc' 0
                fi
            fi

            # Now create the bBSS, but only on the controller if configured (and
            # only if it is meant to be a unique BSS). The agent will have any
            # bBSSes created via the AP Auto-Configuration process.
            if [ "$is_controller" -gt 0 ] && [ "$create_ctrl_bbss" -gt 0 ] && \
                [ "$shared_vaps" -eq 0 ] && [ "$fh_vap_count" -eq 1 ] || \
                [ "$dummy_vaps_enable" -gt 0 -a "$fh_vap_count" -eq 1 -a "$shared_vaps" -eq 0 -a "$create_ctrl_bbss" -gt 0 ]; then
                name=$(uci add wireless wifi-iface)
                config_changed=1

                __repacd_init_vap "$name" $device 'ap' "$backhaul_ssid" \
                    $backhaul_encryption "$backhaul_key" '0' "$config_slo_bh_mld"
                __repacd_update_vap_param "$name" 'map' 1
                __repacd_update_vap_param "$name" 'MapBSSType' 64
                __repacd_echo "Set root distance to 255 for backhaul AP vaps"
                [ $is_controller -eq 0 ] && __repacd_update_vap_param "$name" 'root_distance' 255
                # Need to force this disabled per the Multi-AP SIG spec
                __repacd_update_vap_param "$name" 'wps_pbc' 0
                [ $enable_wps_on_backhaul -eq 1 -a $create_ctrl_fbss -eq 0 ] && __repacd_update_vap_param "$name" 'wps_pbc' 1

                if [ "$enable_mlo" -eq 1 -a -n "$backhaul_mld" ]; then
                    if __repacd_is_mld_vap "$backhaul_mld" "$device_band"; then
                        __repacd_echo "Attaching backhaul vap to mld $backhaul_mld"
                        __repacd_update_vap_param "$name" 'mld' $backhaul_mld
                    fi
                fi

                if [ "$map_version" -gt 1 ]; then
                    __repacd_update_vap_param "$name" 'map' $map_version
                    __repacd_update_vap_param "$name" 'network' $map_primary_nw

                    if [ "$map_dpp_enabled" -eq 1 ]; then
                        __repacd_update_vap_param "$name" 'dpp_map' 1
                        __repacd_update_vap_param "$name" 'dpp_configurator_connectivity' 1
                        __repacd_update_vap_param "$name" 'disablecoext' 1
                        __repacd_update_vap_param "$name" 'gas_comeback_delay' $dpp_gas_comeback_delay
                    fi

                    if [ "$is_controller" -gt 0 -o "$dummy_vaps_enable" -eq 1 ] && [ "$backhaul_authtype" -eq 1 ]; then
                        __repacd_update_vap_param "$name" 'sae' 1
                    fi

                    if [ "$currentband" -eq "3" -a "$kernel_version" -eq "6" ] || \
                            [ $backhaul_authtype -eq 12 -o $backhaul_authtype -eq 15 -o $backhaul_authtype -eq 17 ]; then
                        __repacd_update_vap_param "$name" 'sae' 1
                        if [ "$currentband" -eq 3 ]; then
                            __repacd_update_vap_param "$name" 'sae_pwe' 1
                        else
                            __repacd_update_vap_param "$name" 'sae_pwe' 2
                        fi
                    fi

                    if [ "$currentband" -eq 3 ]; then
                        __repacd_update_vap_param "$name" 'en_6g_sec_comp' 0
                        # Mark 6G backhaul BSS as Tx VAP
                        __repacd_update_vap_param "$name" 'mbss_tx_vdev' 1
                    fi

                    if [ $backhaul_authtype -eq 15 ] || [ $backhaul_authtype -eq 17 ]; then
                        __repacd_update_vap_param "$name" 'wpa_key_mgmt' "SAE SAE-EXT-KEY"
                        __repacd_update_vap_param "$name" 'add_sha256' "0"
                        __repacd_update_vap_param "$name" 'add_sha384' "0"
                        __repacd_update_vap_param "$name" 'group_mgmt_cipher' "BIP-GMAC-256"
                        __repacd_update_vap_param "$name" 'sae_password' "$key"
                        uci add_list wireless."$name".sae_groups="19"
                        uci commit repacd
                    fi

                    if [ "$map_ts_enabled" -gt 0 -a "$vlan_id" -gt 0 ]; then
                        if [ "$map_single_r1r2_bh" -eq 1 ]; then
                            bss_attribute=$((vap_bh_bss_type|64))
                            __repacd_update_vap_param "$name" 'MapBSSType' $bss_attribute
                            if [ "$map_primary_vlan" -gt 0 ]; then
                                __repacd_update_vap_param "$name" 'network' $map_backhaul_nw
                                [ "$is_controller" -eq 1 ] && __repacd_update_vap_param "$name" 'map8021qvlan' $map_primary_vlan
                                if [ "$MAPeapolOverBridge" -gt 0 ]; then
                                    __repacd_update_vap_param "$name" 'vlan_bridge' "$brprefix$map_primary_nw"
                                fi
                            fi
                        else
                            __reapcd_create_additional_backhaul_map "$name" "$device" \
                                                                    "$backhaul_ssid" "$backhaul_key" \
                                                                    "$backhaul_encryption" "64" \
                                                                    "$vap_bh_bss_type" "$backhaul_authtype" \
                                                                    "$config_slo_fh_mld" "$config_slo_bh_mld"
                        fi
                    fi

                    if [ "$map_ts_enabled" -gt 0 -a "$vlan_id" -gt 0 -a "$dummy_vaps_enable" -gt 0 ]; then
                        # Dummy BH VAPs network is set to backhaul network type
                        # so that BH VAPs credentials is updated by hostapd reload_config and VAP restart is avoided
                        __repacd_update_vap_param "$name" 'network' $map_backhaul_nw
                    fi
                fi
            fi
        fi
    done
    sleep 2
    uci_commit wireless
}

# Determine the radio on which the bSTA should be allocated.
#
# If a radio is marked as selected (using the repacd_map_bsta_selected config
# option), it will be used. If instead none is marked, the radio with the
# highest repacd_map_bsta_pref value will be used.
#
# input: $1 config: section name
# output: $2 selected_radio: the radio that is marked as selected
# output: $3 preferred_radio: the radio with the highest preference
__repacd_resolve_bsta_radio() {
    local config="$1"

    local bsta_selected='' bsta_preference=''
    config_get bsta_selected "$config" repacd_map_bsta_selected 0
    config_get bsta_preference "$config" repacd_map_bsta_preference

    if [ "$bsta_selected" -gt 0 ]; then
        eval "$2=$config"
    fi

    # Radios with no preference set are ignored. This is meant to indicate
    # the OEM never wants to use that radio.
    if [ -n "$bsta_preference" ]; then
        if [ "$bsta_preference" -gt "$map_bsta_max_preference" ]; then
            eval "$3=$config"
            map_bsta_max_preference="$bsta_preference"
        fi
    fi
}

# Update the radio on which the bSTA VAP is allocated based on the setting
# at the radio level.
#
# input: $1 config: section name
# input: $2 selected_radio: name of the radio on which to run the bSTA
# input: $3 network: network for which to update VAPs
# input-output: $4 change counter
__repacd_update_map_bsta_radio() {
    local config="$1"
    local selected_radio="$2"
    local changed="$4"
    local MLObSTAconfigured="$5"

    local device hwmode type iface currentband
    config_get device "$config" device
    config_get hwmode "$device" hwmode
    config_get type "$device" type
    config_get iface "$config" ifname
    config_get currentband "$selected_radio" band '0'

    if [ "$hwmode" = '11ad' ] && [ "$type" = 'mac80211' ] ;then
        return
    fi

    local mode network disabled bssid
    config_get mode "$config" mode
    config_get network "$config" network
    config_get disabled "$config" disabled 0
    config_get bssid "$config" bssid
    config_get mld "$config" mld
    config_get ssid "$config" ssid
    if [ "$3" = "$network" ] && __repacd_is_matching_mode "sta" "$mode"; then
        if [ "$map_fast_onboarding" -eq 1 ]; then
            __repacd_echo "Current STA iface $iface"
            if [ -n "$iface" ]; then
                current_sta="$device $iface"
                if [ -n "$mld" ]; then
                    is_current_sta_mlo=1
                fi
            fi
	    if [ -n "$iface" ] && [ "$disabled" -eq 1 ]; then
		current_sta=''
            fi
        fi
        if [ "$MLObSTAconfigured" -eq 1 ]; then
            __repacd_echo "MLD bSTA is enabled and skip radio device update"
            if [ -z "$iface" -o ! -e "/sys/class/net/"$iface"" ]; then
                if [ "$map_fast_onboarding" -eq 1 ]; then
                    preferred_mld_sta=$((preferred_mld_sta + 1))
                    preferred_sta="$mld"
                    sta_config_changed=$((sta_config_changed + 1))
                fi
            fi
            __repacd_echo "Current STA disabled $disabled"
            if [ "$disabled" -eq 1 ]; then
                uci_set wireless "$config" disabled 0
                if [ "$map_fast_onboarding" -eq 1 ]; then
                    preferred_mld_sta=$((preferred_mld_sta + 1))
                    preferred_sta="$mld"
                    sta_config_changed=$((sta_config_changed + 1))
                fi
                __repacd_echo "Set VAP $config to Disabled=0"
            fi

            return
        fi
        if [ ! "$device" = "$selected_radio" ]; then
            uci_set wireless "$config" device "$selected_radio"
            if [ "$map_6g_enable" -eq 1  ]; then
                config_get currentband "$selected_radio" band '0'
                if [ "$currentband" -eq 3  ]; then
                    uci_set wireless "$config" sae 1
                    uci_set wireless "$config" en_6g_sec_comp 0
                fi
            fi
            if [ "$map_fast_onboarding" -eq 1 ]; then
                uci_set wireless "$config" network "$managed_network"
                preferred_sta="$selected_radio"
                sta_config_changed=$((sta_config_changed + 1))
            else
                changed=$((changed + 1))
                eval "$4='$changed'"
            fi
            __repacd_echo "Set VAP $config to device=$selected_radio"

            if [ -n "$bssid" ]; then
                uci delete "wireless.${config}.bssid"
                __repacd_echo "Removed BSSID from VAP $config due to radio change"
            fi
        fi
        if [ -n "$sta_mld" ]; then
            uci_set wireless "$sta_mld" 'mld_ssid' "$ssid"
            __repacd_echo "Updating Non-AP MLD with backhaul SSID $ssid"
        fi
        config_get hwmode_new "$selected_radio" hwmode
        if [ "$hwmode_new" = '11bea' ] || [ "$hwmode_new" = '11beg' ]; then
            if [ -n "$sta_mld" ]; then
                uci_set wireless "$config" mld "$sta_mld"
                uci_set wireless "$sta_mld" 'mld_ssid' "$ssid"
            fi
        else
            if [ -n "$mld" ]; then
                uci delete wireless."$config".mld
            fi
        fi

        __repacd_echo "Current STA disabled $disabled"
        if [ "$disabled" -eq 1 ]; then
            uci_set wireless "$config" disabled 0
            if [ "$map_fast_onboarding" -eq 1 ]; then
                sta_config_changed=$((sta_config_changed + 1))
            else
                changed=$((changed + 1))
                eval "$4='$changed'"
            fi
            __repacd_echo "Set VAP $config to Disabled=0"
        fi
    fi
}

# Update the radio on which the bSTA VAP is allocated based on the setting
# at the radio level.
#
# input: $1 config: section name
# input: $2 selected_radio: name of the radio on which to run the bSTA
# input: $3 network: network for which to update VAPs
# input-output: $4 change counter
__repacd_update_map_bsta_radio_Multi_bsta() {
    local config="$1"
    local selected_radio="$2"
    local changed="$4"
    local MLObSTAconfigured="$5"

    local device hwmode type iface currentband
    config_get device "$config" device
    config_get hwmode "$device" hwmode
    config_get type "$device" type
    config_get iface "$config" ifname
    config_get currentband "$selected_radio" band '0'

    if [ "$hwmode" = '11ad' ] && [ "$type" = 'mac80211' ] ;then
        return
    fi

    local mode network disabled bssid mld ssid
    config_get mode "$config" mode
    config_get network "$config" network
    config_get disabled "$config" disabled 0
    config_get bssid "$config" bssid
    config_get mld "$config" mld
    config_get ssid "$config" ssid
    if [ "$3" = "$network" ] && [ "$device" = "$selected_radio" ] && __repacd_is_matching_mode "sta" "$mode"; then
        if [ "$map_fast_onboarding" -eq 1 ]; then
            __repacd_echo "Current STA iface $iface"
            if [ -n "$iface" ]; then
                current_sta="$device $iface"
                if [ -n "$mld" ]; then
                    is_current_sta_mlo=1
                fi
            fi
            if [ -n "$iface" ] && [ "$disabled" -eq 1 ]; then
                current_sta=''
            fi
        fi
        if [ "$MLObSTAconfigured" -eq 1 ]; then
            __repacd_echo "MLD bSTA is enabled and skip radio device update"
            if [ -z "$iface" -o ! -e "/sys/class/net/"$iface"" ]; then
                if [ "$map_fast_onboarding" -eq 1 ]; then
                    preferred_mld_sta=$((preferred_mld_sta + 1))
                    preferred_sta="$mld"
                    sta_config_changed=$((sta_config_changed + 1))
                fi
            fi
            __repacd_echo "Current STA disabled $disabled"
            if [ "$disabled" -eq 1 ]; then
                uci_set wireless "$config" disabled 0
                if [ "$map_fast_onboarding" -eq 1 ]; then
                    preferred_mld_sta=$((preferred_mld_sta + 1))
                    preferred_sta="$mld"
                    sta_config_changed=$((sta_config_changed + 1))
                fi
                __repacd_echo "Set VAP $config to Disabled=0"
            fi

            return
        fi

        uci_set wireless "$config" device "$selected_radio"
        if [ "$map_6g_enable" -eq 1  ]; then
            config_get currentband "$selected_radio" band '0'
            if [ "$currentband" -eq 3  ]; then
                uci_set wireless "$config" sae 1
                uci_set wireless "$config" en_6g_sec_comp 0
            fi
        fi
        if [ "$map_fast_onboarding" -eq 1 ]; then
            uci_set wireless "$config" network "$managed_network"
            preferred_sta="$selected_radio"
            sta_config_changed=$((sta_config_changed + 1))
        else
            changed=$((changed + 1))
            eval "$4='$changed'"
        fi
        __repacd_echo "Set VAP $config to device=$selected_radio"

        if [ -n "$bssid" ]; then
            uci delete "wireless.${config}.bssid"
            __repacd_echo "Removed BSSID from VAP $config due to radio change"
        fi

        config_get hwmode_new "$selected_radio" hwmode
        if [ "$hwmode_new" = '11bea' ] || [ "$hwmode_new" = '11beg' ]; then
            __repacd_echo "STA mld already assigned"
        else
            if [ -n "$mld" ]; then
                uci delete wireless."$config".mld
            fi
        fi

        __repacd_echo "Current STA disabled $disabled"
        if [ "$disabled" -eq 1 ]; then
            uci_set wireless "$config" disabled 0
            if [ "$map_fast_onboarding" -eq 1 ]; then
                sta_config_changed=$((sta_config_changed + 1))
            else
                changed=$((changed + 1))
                eval "$4='$changed'"
            fi
            __repacd_echo "Set VAP $config to Disabled=0"
        fi
    fi
    #if non preferred radio is 6g then set security compliance flag
    if [ "$map_6g_enable" -eq 1 ] && __repacd_is_matching_mode "sta" "$mode"; then
        config_get currentband "$device" band '0'
        local backhaul_authtype
        config_get backhaul_authtype MAPConfig BackhaulAuthType 0
        if [ "$backhaul_authtype" -eq 1 ]; then
            uci_set wireless "$config" sae 1
        fi
        if [ "$currentband" -eq 3 ]; then
            uci_set wireless "$config" en_6g_sec_comp 0
        fi
    fi
}

__repacd_map_get_last_vap() {
    local config="$1"
    local sta_iface="$2"
    local last_iface="$3"
    local mode network disabled bssid
    local device hwmode type iface

    config_get device "$config" device
    config_get hwmode "$device" hwmode
    config_get type "$device" type
    config_get iface "$config" ifname
    config_get mode "$config" mode
    config_get disabled "$config" disabled 0

    if [ "$hwmode" = '11ad' ] && [ "$type" = 'mac80211' ] ;then
        return
    fi

    if [ "$device" != "$preferred_sta" ]; then
        return
    fi

    if __repacd_is_matching_mode "sta" "$mode"; then
        eval $sta_iface="$iface"
    else
        eval $last_iface="$iface"
    fi
}

__repacd_map_get_new_sta() {
    local sta_vap last_vap
    local rIdx vapIdx

    config_load wireless
    config_foreach __repacd_map_get_last_vap wifi-iface sta_vap last_vap

    __repacd_echo "sta: $sta_vap , last vap: $last_vap"
    if [ "$sta_vap" != "$last_vap" ]; then
        rIdx=$(echo $last_vap | cut -c 4)
        vapIdx=$(echo $last_vap | cut -c 5)
        vapIdx=$((vapIdx + 1))
        preferred_sta="$preferred_sta ath$rIdx$vapIdx"
    fi
}

# Change the hyd running mode based on the value provided.
#
# input: $1 map_version: value to set hyd run mode to based on version
# input: $2 is_controller: device mode
__repacd_configure_hyd_map() {
    local version=$map_version
    local is_controller=$is_controller
    local num_vlan_supported map_single_r1r2_bh map_primary_nw
    local fronthaul_ssid primary_vlan_id pcp additional_fh
    local vlan_id override_enabled
    config_get override_enabled MAPConfig OverrideVapConfigEnable 0

    if [ "$version" -ge 1 ]; then
        # Set Forwarding Mode to SINGLE to disable HA, HD
        uci_set $MAP hy 'ForwardingMode' 'SINGLE'
        uci_set $MAP MultiAP 'MapVersion' $version

        if [ "$map_ts_enabled" -gt 0 ]; then
            config_get num_vlan_supported MAPConfig 'NumberOfVLANSupported' '0'
            config_get map_single_r1r2_bh MAPConfig 'CombinedR1R2Backhaul'
            config_get map_primary_nw MAPConfig 'VlanNetworkPrimary' ''

            uci_set repacd MAPConfig 'MapTrafficSeparationActive' 1
            uci_set $MAP MultiAP 'Map2TrafficSepEnabled' 1
            uci_set $MAP MultiAP 'NumberOfVLANSupported' $num_vlan_supported
            uci_set $MAP MultiAP 'CombinedR1R2Backhaul' $map_single_r1r2_bh
            uci_set $MAP MultiAP 'VlanNetworkPrimary' $map_primary_nw

            if [ "$is_controller" -gt 0 ]; then
                config_get fronthaul_ssid MAPConfig FronthaulSSID
                config_get primary_vlan_id MAPConfig 'VlanIDNwPrimary' '0'
                config_get pcp MAPConfig 'Map8021QPCP' '0'

                config_get additional_fhssid1 MAPConfig AdditionalFHssid1
                config_get additional_fhkey1 MAPConfig AdditionalFHkey1
                config_get additional_fhauth1 MAPConfig AdditionalFHauth1

                config_get additional_fhssid2 MAPConfig AdditionalFHssid2
                config_get additional_fhkey2 MAPConfig AdditionalFHkey2
                config_get additional_fhauth2 MAPConfig AdditionalFHauth2

                config_get additional_fhssid3 MAPConfig AdditionalFHssid3
                config_get additional_fhkey3 MAPConfig AdditionalFHkey3
                config_get additional_fhauth3 MAPConfig AdditionalFHauth3

                uci_set $MAP MultiAP 'FronthaulSSIDPrimary' "$fronthaul_ssid"
                uci_set $MAP MultiAP 'Map8021QPCP' $pcp
                uci_set $MAP MultiAP 'VlanIDNwPrimary' $primary_vlan_id

                uci_set $MAP MultiAP 'AdditionalFHssid1' "$additional_fhssid1"
                uci_set $MAP MultiAP 'AdditionalFHkey1' "$additional_fhkey1"
                uci_set $MAP MultiAP 'AdditionalFHauth1' $additional_fhauth1

                uci_set $MAP MultiAP 'AdditionalFHssid2' "$additional_fhssid2"
                uci_set $MAP MultiAP 'AdditionalFHkey2' "$additional_fhkey2"
                uci_set $MAP MultiAP 'AdditionalFHauth2' $additional_fhauth2

                uci_set $MAP MultiAP 'AdditionalFHssid3' "$additional_fhssid3"
                uci_set $MAP MultiAP 'AdditionalFHkey3' "$additional_fhkey3"
                uci_set $MAP MultiAP 'AdditionalFHauth3' $additional_fhauth3

                local num_vlan=$num_vlan_supported
                for i in One Two Three; do
                    config_get fronthaul_ssid MAPConfig "FronthaulSSIDNw"$i ''
                    config_get vlan_id MAPConfig "VlanIDNw"$i '0'

                    if [ "$num_vlan" -eq 0 -o "$num_vlan" -eq 1 ]; then
                        break
                    fi

                    if [ -z "$fronthaul_ssid" ]; then
                        if [ "$override_enabled" -eq "1" ]; then
                            for j in One Two Three; do
                                for band in 2G 5G 5GL 6G 6GL; do
                                   config_get fronthaul_ssid MAPConfig "FHOverrideSSIDNw"$j$band ''
                                   uci_set $MAP MultiAP 'FronthaulSSIDNw'$j "$fronthaul_ssid"
                                   break
                                done
                            done
                        fi
                    else
                        uci_set $MAP MultiAP 'FronthaulSSIDNw'$i "$fronthaul_ssid"
                    fi
                    uci_set $MAP MultiAP 'VlanIDNw'$i $vlan_id

                    num_vlan=$((num_vlan-1))
                done
            fi
        fi
    fi

    uci_commit $MAP
}

# Set the country
#
# input: $1 config: section to update
# input: $2 country to update
__repacd_config_set_device_country() {
    local config="$1"
    local country="$2"
    config_get hwmode "$config" hwmode
    config_get type "$config" type

    if [ "$hwmode" = '11ad' ] && [ "$type" = 'mac80211' ]; then
        return
    fi

    uci_set wireless "$config" country "$country"
    uci_commit wireless
    __repacd_echo "Set radio $config to country=$country"
}

# Enable Radio
#
# input: $1 config: section to update
__repacd_config_set_radio_enable() {
    local config="$1"
    local country="$2"
    local skip_radio=''
    config_get hwmode "$config" hwmode
    config_get type "$config" type
    config_get channel "$config" channel
    config_get skip_radio MAPConfig '6GRadio'
    config_get map_version MAPConfig 'MapVersionEnabled'

    if [ "$hwmode" = '11ad' ] && [ "$type" = 'mac80211' ]; then
        return
    fi

    if [ "$skip_radio" = "$config" ]; then
        # set the band as 3 for 6GHz radio device
        uci_set wireless "$config" band 3
        uci_commit wireless
        if [ "$map_version" -lt 6 ]; then
            map_num_radio=$((map_num_radio + 1))
            return
        fi
    fi

    uci_set wireless "$config" disabled 0

    # Configure 2G radio
    if [ "$map_version" -ge 6 ]; then
        if [ "$channel" -le 14 ]; then
            uci_set wireless "$config" htmode EHT40
        fi
    fi

    # Configure 5GL and 5GH radios
    if [ "$channel" -gt 14 ]; then
        if [ "$map_version" -ge 6 ]; then
            uci_set wireless "$config" htmode EHT80
        else
            uci_set wireless "$config" htmode HT40
        fi
    fi

    # Configure 6G radio
    if [ "$map_version" -ge 6 ]; then
        if [ "$skip_radio" = "$config" ]; then
            uci_set wireless "$config" htmode EHT160
        fi
    fi
    if [ "$map_version" -lt 6 ]; then
        if [ "$hwmode" != '11axg' ] && [ "$hwmode" != '11axa' ]; then
            if [ "$channel" -le 14 ]; then
                uci_set wireless "$config" hwmode 11axg
            else
                uci_set wireless "$config" hwmode 11axa
            fi
        fi
    fi

    uci_commit wireless

    map_num_radio=$((map_num_radio + 1))
}

__repacd_config_update_mld_section_maplite() {
    local config="$1"
    local mld='' ssid='' mld_mac_addr='' indx=0 ifacedevice='' hwmode=''
    config_get ifacedevice "$config" device
    config_get map_version MAPConfig 'MapVersionEnabled'
    config_get hwmode "$ifacedevice" hwmode
    config_get mld "$config" mld
    config_get ssid "$config" ssid

    if [ "$map_version" -ge 6 ]; then
        if [ "$hwmode" != '11axg' ] && [ "$hwmode" != '11axa' ] && [ -z "$mld" ]; then
            # setting mld for 11be radios as non mlo 11be vaps are not supported
            __repacd_create_mld_section "$ssid" "$mld_mac_addr" "AP"
            indx=$((mldIndex-1))
            uci_set wireless "$config" mld mld$indx
            __repacd_echo "created mld$indx and attached to $ifacedevice VAP:$ssid"
        fi
    fi
}


# Disable vaps of Radio that is marked to be skipped and update mld for 11be iface
# input: $1 config: section to update
__repacd_config_update_radio_vaps() {
    local config="$1"
    local skip_radio='' ifacedevice=''
    config_get skip_radio MAPConfig '6GRadio'
    config_get ifacedevice "$config" device
    config_get map_version MAPConfig 'MapVersionEnabled'

    if [ "$ifacedevice" = "$skip_radio" ]; then
        if [ "$map_version" -lt 6 ]; then
            uci_set wireless "$config" disabled 1
            uci_commit wireless
            return
        fi
    fi
}

# Change the configuration on the wifi-iface object to match what is desired.
# This function is for Multi-AP mode.
#
# input: $1 config: section to update
# input: $2 network: only update if network matches this value
# input: $3 block_dfs_chan: 1 - block DFS channels, 0 - do not block them
# input-output: $4 change counter
__repacd_config_iface_map() {
    local config=$1
    local enable_wds=1 qwrap_ap=0 extap=0 enable_rrm=1 re_scalingfactor=0
    local default_root_dist=0 cap_snr=0

    __repacd_config_iface "$1" "$2" $enable_wds $qwrap_ap $extap "$3" $enable_rrm \
                          $re_scalingfactor $cap_snr "$4"

    if [ "$map_version" -ge 2 ]; then
        config_get mode "$config" mode

        config_load 'wsplcd'
        config_get map2SetMboOcePmf config 'Map2EnableMboOcePmf'

        # These should only be set on AP interfaces.
        if __repacd_is_matching_mode 'ap' "$mode"; then
            # Set Mbo, Oce and PMF
            if [ "$map2SetMboOcePmf" -eq 1 ]; then
                uci_set wireless "$config" mbo 1
                uci_set wireless "$config" oce 1
                uci_set wireless "$config" ieee80211w 1
            fi

            if [ "$enable_anqp_interworking" -eq 1 ]; then
                # Set Interworking and ANQP
                uci_set wireless "$config" interworking 1
                uci del_list wireless."$config".anqp_elem="$anqp_ele"
                uci add_list wireless."$config".anqp_elem="$anqp_ele"
            fi
        fi

        # Update config when Traffic Separation is enabled
        if [ "$map_ts_enabled" -eq 1 ]; then
            config_load network
            for i in Primary One Two Three; do
                config_get nw_name MAPConfig "VlanNetwork"$i '0'
                __repacd_config_iface "$1" "$nw_name" $enable_wds $qwrap_ap $extap "$3" \
                                      $enable_rrm $re_scalingfactor \
                                      $cap_snr "$4"
            done
            __repacd_config_iface "$1" "$map_backhaul_nw" $enable_wds $qwrap_ap $extap "$3" \
                                  $enable_rrm $re_scalingfactor $cap_snr "$4"
        fi
    fi
}

# Generate the Multi-AP BSS instantiation config file for wsplcd based on
# the selected template and SSID settings.
# input-output: $1 change counter
__repacd_generate_map_bss_conf() {
    local changed="$1"

    local template_filename
    local template_path
    local fronthaul_ssid fronthaul_key
    local backhaul_ssid backhaul_key
    local fronthaul_authtype_str="0x0020" backhaul_authtype_str="0x0020"
    local backhaul_r2_authtype_str="0x0020"
    local fronthaul_authtype backhaul_authtype
    local primary_vlan_id pcp
    local band override_enabled device_5GL device_6GL
    local is_controller=1
    local auth_primary_server auth_primary_port auth_primary_secret
    local auth_secondary_server auth_secondary_port auth_secondary_secret
    local rsn_preauth rsn_preauth_interfaces wpa_group_rekey ieee80211w
    local group_mgmt_cipher suiteb
    local fronthaul_mld backhaul_mld
    local mld_dev mld_dev_index=1 use_bh2_for_mlo=0
    local mld_band mld_group mld_count

    config_load repacd
    config_get template_filename MAPConfig BSSInstantiationTemplate
    config_get fronthaul_ssid MAPConfig FronthaulSSID
    config_get fronthaul_key MAPConfig FronthaulKey
    config_get backhaul_ssid MAPConfig BackhaulSSID
    config_get backhaul_key MAPConfig BackhaulKey
    config_get fronthaul_authtype MAPConfig FronthaulAuthType 0
    config_get backhaul_authtype MAPConfig BackhaulAuthType 0
    config_get override_enabled MAPConfig OverrideVapConfigEnable 0
    config_get device_5GL MAPConfig Radio5GL
    config_get device_6GL MAPConfig Radio6GL
    config_get use_5GL_for_5G MAPConfig BssPolicyCfgForceFullBandToUse5GLCfg 0
    config_get force_5GL_cfg MAPConfig BssPolicyCfgForce5GLCfg 0
    config_get enable_mlo MAPConfig 'EnableMLO' 0
    config_get fronthaul_mld MAPConfig 'FronthaulMld' ''
    config_get backhaul_mld MAPConfig 'BackhaulMld' ''

    config_get map_version MAPConfig 'MapVersionEnabled'
    config_get map_ts_enabled MAPConfig 'MapTrafficSeparationEnable' '0'
    config_get num_vlan_supported MAPConfig 'NumberOfVLANSupported' '0'

    # Config Required for 8021x support
    config_get dot1x_support MAPConfig  'Enable8021x'
    config_get mld_config MAPConfig 'MLDConfig' ''

    config_get useSpecBasedDefinitions MAPConfig 'useSpecBasedDefinitions' 0

    mld_count=$(eval "echo $mld_config | grep -o mld | wc -l")

    if [ $use_5GL_for_5G -eq 1 -a $force_5GL_cfg -eq 1 ]; then
        use_5GL_for_5G=0
        force_5GL_cfg=0
    fi

    if [ -z "${template_filename}" ]; then
        __repacd_echo "BSS instantiation template not specified"
        return 1
    fi

    template_path="${WSPLCD_MAP_TEMPLATE_DIR}/${template_filename}"
    if [ ! -r "${template_path}" ]; then
        __repacd_echo "BSS instantiation template ${template_path} not readable"
        return 1
    fi

    __repacd_echo "Using wsplcd BSS instantiation template: ${template_path}"

    if [ "$map_version" -ge 2 ]; then
        __repacd_configure_hyd_map $map_version $is_controller
    fi

    if [ "$map_version" -ge 2 ]; then
        if [ $fronthaul_authtype -gt 0 ]; then
            if [ "$useSpecBasedDefinitions" -eq 0 ]; then
                fronthaul_authtype_str="0x0060"
                [ $fronthaul_authtype -eq 12 ] && fronthaul_authtype_str="0x0040"
                [ $fronthaul_authtype -eq 13 ] && fronthaul_authtype_str="0x0800"
                [ $fronthaul_authtype -eq 14 ] && fronthaul_authtype_str="0x1000"
                [ $fronthaul_authtype -eq 15 ] && fronthaul_authtype_str="0x0820"
                [ $fronthaul_authtype -eq 16 ] && fronthaul_authtype_str="0x1800"
                [ $fronthaul_authtype -eq 17 ] && fronthaul_authtype_str="0x1820"
            else
                fronthaul_authtype_str="0x0820"
                [ $fronthaul_authtype -eq 12 ] && fronthaul_authtype_str="0x0800"
                [ $fronthaul_authtype -eq 13 ] && fronthaul_authtype_str="0x0040"
                [ $fronthaul_authtype -eq 14 ] && fronthaul_authtype_str="0x0100"
                [ $fronthaul_authtype -eq 15 ] && fronthaul_authtype_str="0x0060"
                [ $fronthaul_authtype -eq 16 ] && fronthaul_authtype_str="0x0140"
                [ $fronthaul_authtype -eq 17 ] && fronthaul_authtype_str="0x0160"
            fi
        fi

        if [ "$useSpecBasedDefinitions" -eq 0 ]; then
            # WPA2 mode (psk)
            if [ $backhaul_authtype -eq 0 ]; then
                backhaul_authtype_str="0x0020"
            # WPA3+Mixed mode (sae-psk)
            elif [ $backhaul_authtype -eq 1 ]; then
                backhaul_authtype_str="0x0060"
            # WPA3 only mode (sae)
            elif [ $backhaul_authtype -eq 12 ]; then
                backhaul_authtype_str="0x0040"
            elif [ $backhaul_authtype -eq 15 ]; then
                backhaul_authtype_str="0x0820"
            elif [ $backhaul_authtype -eq 17 ]; then
                backhaul_authtype_str="0x1820"
            fi
        else
            # WPA2 mode (psk)
            if [ $backhaul_authtype -eq 0 ]; then
                backhaul_authtype_str="0x0020"
            # WPA3+Mixed mode (sae-psk)
            elif [ $backhaul_authtype -eq 1 ]; then
                backhaul_authtype_str="0x0820"
            # WPA3 only mode (sae)
            elif [ $backhaul_authtype -eq 12 ]; then
                backhaul_authtype_str="0x0800"
            elif [ $backhaul_authtype -eq 15 ]; then
                backhaul_authtype_str="0x0060"
            elif [ $backhaul_authtype -eq 17 ]; then
                backhaul_authtype_str="0x0160"
            fi
        fi

        if [ "$map_version" -ge 3 ]; then
            if [ "$map_dpp_enabled" -eq 1 ]; then
                fronthaul_authtype_str="0x00E0"
                backhaul_authtype_str="0x00E0"
            fi
        fi
    fi

    if [ -z "$fronthaul_key" ]; then
        fronthaul_key="NULL"
        fronthaul_authtype_str="0x0001"
    fi

    if [ -z $backhaul_key ]; then
        backhaul_key="NULL"
        backhaul_authtype_str="0x0001"
        backhaul_r2_authtype_str="0x0001"
    fi

    local tempfile
    tempfile=$(mktemp)
    cp "${template_path}" "${tempfile}"

    if [ -n "$device_5GL" ] && [ "$use_5GL_for_5G" == "1" ]; then
        sed -i -e '/Generic-Policy: 2,115,120,121,127/s/5G/5GL/g' "${tempfile}"
        # Fix 6GL condition as separate
        sed -i -e '/Generic-Policy: 1,131,136,F/s/6G/6GL/g' "${tempfile}"
    fi

    local ssid_with_escape_char key_with_escape_char
    __repacd_str_handle_special_char "$fronthaul_ssid" ssid_with_escape_char
    fronthaul_ssid="$ssid_with_escape_char"
    __repacd_str_handle_special_char "$fronthaul_key" key_with_escape_char
    fronthaul_key="$key_with_escape_char"

    ssid_with_escape_char='' key_with_escape_char=''
    __repacd_str_handle_special_char "$backhaul_ssid" ssid_with_escape_char
    backhaul_ssid="$ssid_with_escape_char"
    __repacd_str_handle_special_char "$backhaul_key" key_with_escape_char
    backhaul_key="$key_with_escape_char"

    # Some of these replacements may not be used, but this covers all of the
    # cases of variables that need to be replaced.
    #
    # Note that in case the variable contains the sed delimeter, it needs to
    # be escaped to avoid a sed error.
    if [ -z "$fronthaul_ssid" ]; then
        sed -i -e "s!__FH_SSID_REPLACE__!"teardown"!g" "${tempfile}"
    fi
    sed -i -e "s!__FH_SSID_REPLACE__!${fronthaul_ssid/!/\!}!g" \
           -e "s!__FH_KEY_REPLACE__!${fronthaul_key/!/\!}!g" \
           -e "s!__BH_SSID_REPLACE__!${backhaul_ssid/!/\!}!g" \
           -e "s!__BH_KEY_REPLACE__!${backhaul_key/!/\!}!g" \
           -e "s!__FHBH_SSID_REPLACE__!${fronthaul_ssid/!/\!}!g" \
           -e "s!__FHBH_KEY_REPLACE__!${fronthaul_key/!/\!}!g" \
           -e "s!__FH_AUTH_TYPE_REPLACE__!${fronthaul_authtype_str/!/\!}!g" \
           -e "s!__FHBH_AUTH_TYPE_REPLACE__!${fronthaul_authtype_str/!/\!}!g" \
           "${tempfile}"

    # Update template of FH override and Additional-FH override
    if [ "$override_enabled" -eq "1" ]; then
        for band in 2G 5G 5GL 6G 6GL; do
            config_get fronthaul_ssid MAPConfig 'FHOverrideSSID'$band ''
            config_get fronthaul_key MAPConfig 'FHOverrideKey'$band ''
            config_get fronthaul_authtype MAPConfig 'FHOverrideAuthType'$band ''
            #use 5G config if 5G low band is not specified
            if [ "$force_5GL_cfg" == "0" -a "$band" = "5GL" -a -z "$device_5GL" ]; then
                config_get fronthaul_ssid MAPConfig 'FHOverrideSSID5G' ''
                config_get fronthaul_key MAPConfig 'FHOverrideKey5G' ''
                config_get fronthaul_authtype MAPConfig 'FHOverrideAuthType5G' ''
            fi

            if [ -z "$fronthaul_ssid" ]; then
                sed -i -e "s!__FHOR"$band"_SSID_REPLACE__!"teardown"!g" "${tempfile}"
            else
                fronthaul_authtype_str="0x0020"
                if [ "$map_version" -ge 2 ]; then
                    if [ $fronthaul_authtype -gt 0 ]; then
                        if [ "$useSpecBasedDefinitions" -eq 0 ]; then
                            fronthaul_authtype_str="0x0060"
                            [ $fronthaul_authtype -eq 12 ] && fronthaul_authtype_str="0x0040"
                            [ $fronthaul_authtype -eq 13 ] && fronthaul_authtype_str="0x0800"
                            [ $fronthaul_authtype -eq 14 ] && fronthaul_authtype_str="0x1000"
                            [ $fronthaul_authtype -eq 15 ] && fronthaul_authtype_str="0x0820"
                            [ $fronthaul_authtype -eq 16 ] && fronthaul_authtype_str="0x1800"
                            [ $fronthaul_authtype -eq 17 ] && fronthaul_authtype_str="0x1820"
                        else
                            fronthaul_authtype_str="0x0820"
                            [ $fronthaul_authtype -eq 12 ] && fronthaul_authtype_str="0x0800"
                            [ $fronthaul_authtype -eq 13 ] && fronthaul_authtype_str="0x0040"
                            [ $fronthaul_authtype -eq 14 ] && fronthaul_authtype_str="0x0100"
                            [ $fronthaul_authtype -eq 15 ] && fronthaul_authtype_str="0x0060"
                            [ $fronthaul_authtype -eq 16 ] && fronthaul_authtype_str="0x0140"
                            [ $fronthaul_authtype -eq 17 ] && fronthaul_authtype_str="0x0160"
                        fi

                    fi

                    if [ "$map_version" -ge 3 ]; then
                        if [ "$map_dpp_enabled" -eq 1 ]; then
                            fronthaul_authtype_str="0x00E0"
                        fi
                    fi
                fi
                if [ -z $fronthaul_key ]; then
                    fronthaul_key="NULL"
                    fronthaul_authtype_str="0x0001"
                fi
                ssid_with_escape_char=''
                __repacd_str_handle_special_char "$fronthaul_ssid" ssid_with_escape_char
                fronthaul_ssid="$ssid_with_escape_char"
                key_with_escape_char=''
                 __repacd_str_handle_special_char "$fronthaul_key" key_with_escape_char
                fronthaul_key="$key_with_escape_char"

                sed -i -e "s!__FHOR"$band"_SSID_REPLACE__!${fronthaul_ssid/!/\!}!g" \
                       -e "s!__FHOR"$band"_KEY_REPLACE__!${fronthaul_key/!/\!}!g" \
                       -e "s!__FHOR"$band"_AUTH_TYPE_REPLACE__!${fronthaul_authtype_str/!/\!}!g" \
                       "${tempfile}"
            fi

            ad_vap_index=1
            while [ $ad_vap_index -le $MAP_MAX_ADDITIONA_VAP ]; do
                config_get fronthaul_ssid MAPConfig "AdditionalFHOverride"${band}ssid${ad_vap_index} ''
                config_get fronthaul_key MAPConfig "AdditionalFHOverride"${band}key${ad_vap_index} ''
                config_get fronthaul_authtype MAPConfig "AdditionalFHOverride"${band}auth${ad_vap_index} ''

                if [ -z "$fronthaul_ssid" ]; then
                   sed -i -e "s!__FHA"$ad_vap_index"OR"$band"_SSID_REPLACE__!"teardown"!g" "${tempfile}"
                else
                   fronthaul_authtype_str="0x0020"
                   if [ "$map_version" -ge 2 ]; then
                       if [ $fronthaul_authtype -gt 0 ]; then
                            if [ "$useSpecBasedDefinitions" -eq 0 ]; then
                                fronthaul_authtype_str="0x0060"
                                [ $fronthaul_authtype -eq 12 ] && fronthaul_authtype_str="0x0040"
                                [ $fronthaul_authtype -eq 13 ] && fronthaul_authtype_str="0x0800"
                                [ $fronthaul_authtype -eq 14 ] && fronthaul_authtype_str="0x1000"
                                [ $fronthaul_authtype -eq 15 ] && fronthaul_authtype_str="0x0820"
                                [ $fronthaul_authtype -eq 16 ] && fronthaul_authtype_str="0x1800"
                                [ $fronthaul_authtype -eq 17 ] && fronthaul_authtype_str="0x1820"
                            else
                                fronthaul_authtype_str="0x0820"
                                [ $fronthaul_authtype -eq 12 ] && fronthaul_authtype_str="0x0800"
                                [ $fronthaul_authtype -eq 13 ] && fronthaul_authtype_str="0x0040"
                                [ $fronthaul_authtype -eq 14 ] && fronthaul_authtype_str="0x0100"
                                [ $fronthaul_authtype -eq 15 ] && fronthaul_authtype_str="0x0060"
                                [ $fronthaul_authtype -eq 16 ] && fronthaul_authtype_str="0x0140"
                                [ $fronthaul_authtype -eq 17 ] && fronthaul_authtype_str="0x0160"
                            fi
                        fi

                       if [ "$map_version" -ge 3 ]; then
                           if [ "$map_dpp_enabled" -eq 1 ]; then
                               fronthaul_authtype_str="0x00E0"
                           fi
                       fi
                   fi

                   if [ -z "$fronthaul_key" ]; then
                       fronthaul_key="NULL"
                       fronthaul_authtype_str="0x0001"
                   fi

                   ssid_with_escape_char=''
                   __repacd_str_handle_special_char "$fronthaul_ssid" ssid_with_escape_char
                   fronthaul_ssid="$ssid_with_escape_char"
                   key_with_escape_char=''
                   __repacd_str_handle_special_char "$fronthaul_key" key_with_escape_char
                   fronthaul_key="$key_with_escape_char"

                   local tmp_ssid_replace="__FHA"$ad_vap_index"OR"$band"_SSID_REPLACE__"
                   sed -i -e "s!${tmp_ssid_replace}!${fronthaul_ssid/!/\!}!g" \
                       -e "s!__FHA"$ad_vap_index"OR"$band"_KEY_REPLACE__!${fronthaul_key/!/\!}!g" \
                       -e "s!__FHA"$ad_vap_index"OR"$band"_AUTH_TYPE_REPLACE__!${fronthaul_authtype_str/!/\!}!g" \
                       "${tempfile}"
                fi
                ad_vap_index=$((ad_vap_index+1))
            done
        done
    fi

    # Update template of Additional-FH
    ad_vap_index=1
    while [ $ad_vap_index -le $MAP_MAX_ADDITIONA_VAP ]; do
        config_get fronthaul_ssid MAPConfig "AdditionalFH"ssid${ad_vap_index} ''
        config_get fronthaul_key MAPConfig "AdditionalFH"key${ad_vap_index} ''
        config_get fronthaul_authtype MAPConfig "AdditionalFH"auth${ad_vap_index} ''

        if [ -z "$fronthaul_ssid" -o "$override_enabled" -eq "1" ]; then
           sed -i -e "s!__FHA"$ad_vap_index"_SSID_REPLACE__!"teardown"!g" "${tempfile}"
        else
           __repacd_echo "BSS-CONF_FILE: $ad_vap_index. ssid:$fronthaul_ssid key:$fronthaul_key auth:$authtype"

           fronthaul_authtype_str="0x0020"
           if [ "$map_version" -ge 2 ]; then
               if [ $fronthaul_authtype -gt 0 ]; then
                    if [ "$useSpecBasedDefinitions" -eq 0 ]; then
                        fronthaul_authtype_str="0x0060"
                        [ $fronthaul_authtype -eq 12 ] && fronthaul_authtype_str="0x0040"
                        [ $fronthaul_authtype -eq 13 ] && fronthaul_authtype_str="0x0800"
                        [ $fronthaul_authtype -eq 14 ] && fronthaul_authtype_str="0x1000"
                        [ $fronthaul_authtype -eq 15 ] && fronthaul_authtype_str="0x0820"
                        [ $fronthaul_authtype -eq 16 ] && fronthaul_authtype_str="0x1800"
                        [ $fronthaul_authtype -eq 17 ] && fronthaul_authtype_str="0x1820"
                    else
                        fronthaul_authtype_str="0x0820"
                        [ $fronthaul_authtype -eq 12 ] && fronthaul_authtype_str="0x0800"
                        [ $fronthaul_authtype -eq 13 ] && fronthaul_authtype_str="0x0040"
                        [ $fronthaul_authtype -eq 14 ] && fronthaul_authtype_str="0x0100"
                        [ $fronthaul_authtype -eq 15 ] && fronthaul_authtype_str="0x0060"
                        [ $fronthaul_authtype -eq 16 ] && fronthaul_authtype_str="0x0140"
                        [ $fronthaul_authtype -eq 17 ] && fronthaul_authtype_str="0x0160"
                    fi
                fi

               if [ "$map_version" -ge 3 ]; then
                   if [ "$map_dpp_enabled" -eq 1 ]; then
                       fronthaul_authtype_str="0x00E0"
                   fi
               fi
           fi

           if [ -z "$fronthaul_key" ]; then
               fronthaul_key="NULL"
               fronthaul_authtype_str="0x0001"
           fi

           ssid_with_escape_char=''
           __repacd_str_handle_special_char "$fronthaul_ssid" ssid_with_escape_char
           fronthaul_ssid="$ssid_with_escape_char"
           key_with_escape_char=''
           __repacd_str_handle_special_char "$fronthaul_key" key_with_escape_char
           fronthaul_key="$key_with_escape_char"

           sed -i -e "s!__FHA"$ad_vap_index"_SSID_REPLACE__!${fronthaul_ssid/!/\!}!g" \
               -e "s!__FHA"$ad_vap_index"_KEY_REPLACE__!${fronthaul_key/!/\!}!g" \
               -e "s!__FHA"$ad_vap_index"_AUTH_TYPE_REPLACE__!${fronthaul_authtype_str/!/\!}!g" \
               "${tempfile}"
        fi
        ad_vap_index=$((ad_vap_index+1))
    done

    if [ "$map_ts_enabled" -eq 0 ]; then
        sed -i -e "s!__BH_AUTH_TYPE_REPLACE__!${backhaul_authtype_str/!/\!}!g" \
                "${tempfile}"
    fi

    if [ "$map_version" -ge 2 ]; then
        if [ "$map_ts_enabled" -gt 0 ]; then
            config_get map_r1_sta_assoc_disallow MAPConfig 'R1AgentAssocDisAllowed' '0'
            config_get map_r2_sta_assoc_disallow MAPConfig 'R2AgentAssocDisAllowed' '0'
            config_get primary_vlan_id MAPConfig 'VlanIDNwPrimary' '0'
            config_get pcp MAPConfig 'Map8021QPCP' '0'
        fi

        if [ "$map_ts_enabled" -eq 0 ]; then
            __repacd_echo "Traffic Separation Disabled"
            __repacd_echo "Remove Vlan Config and Secondary Networks"
            map_r1_sta_assoc_disallow=0
            map_r2_sta_assoc_disallow=0
            primary_vlan_id=0
            pcp=0

            sed -i -e "s!__R1_BSTA_ASSOC_DISALLOW_BH1__!${map_r1_sta_assoc_disallow/!/\!}!g" \
                -e "s!__R2_BSTA_ASSOC_DISALLOW_BH1__!${map_r2_sta_assoc_disallow/!/\!}!g" \
                "${tempfile}"

            # Mark second backhaul config to teardown to remove
            sed -i -e "s!__BH2_SSID_REPLACE__!"teardown"!g" \
            "${tempfile}"
        elif [ "$map_r1_sta_assoc_disallow" -gt 0 -o "$map_r2_sta_assoc_disallow" -gt 0 ]; then
            sed -i -e "s!__R1_BSTA_ASSOC_DISALLOW_BH1__!${map_r1_sta_assoc_disallow/!/\!}!g" \
                -e "s!__R2_BSTA_ASSOC_DISALLOW_BH1__!${map_r2_sta_assoc_disallow/!/\!}!g" \
                "${tempfile}"

            # map_r2_sta_assoc_disallow is enabled update authtype as WPA2
            if [ "$map_r2_sta_assoc_disallow" -gt 0 ]; then
                sed -i -e "s!__BH_AUTH_TYPE_REPLACE__!${backhaul_r2_authtype_str}!}!g" \
                "${tempfile}"
            else
                sed -i -e "s!__BH_AUTH_TYPE_REPLACE__!${backhaul_authtype_str/!/\!}!g" \
                "${tempfile}"
            fi

            # Mark second backhaul config to teardown to remove
            sed -i -e "s!__BH2_SSID_REPLACE__!"teardown"!g" \
            "${tempfile}"
        elif [ "$map_r1_sta_assoc_disallow" -eq 0 -a "$map_r2_sta_assoc_disallow" -eq 0 ]; then
            # 2 backhaul are created . 1 with r1 STA assoc disAllowed another with r2
            # STA assoc disAllowed
            if [ "$map_single_r1r2_bh" -eq 1 ]; then
                sed -i -e "s!__R1_BSTA_ASSOC_DISALLOW_BH1__!0!g" \
                    -e "s!__R2_BSTA_ASSOC_DISALLOW_BH1__!0!g" \
                    -e "s!__BH_AUTH_TYPE_REPLACE__!${backhaul_authtype_str/!/\!}!g" \
                    "${tempfile}"

                # Mark second backhaul config to teardown to remove
                sed -i -e "s!__BH2_SSID_REPLACE__!"teardown"!g" \
                    "${tempfile}"
            else
                use_bh2_for_mlo=1
                # Always update the Authtype as WPA2 for R1 agent assoc allowed backhaul BSS
                sed -i -e "s!__R1_BSTA_ASSOC_DISALLOW_BH1__!0!g" \
                    -e "s!__R2_BSTA_ASSOC_DISALLOW_BH1__!1!g" \
                    -e "s!__BH_AUTH_TYPE_REPLACE__!${backhaul_r2_authtype_str}!g" \
                    "${tempfile}"

                sed -i -e "s!__R1_BSTA_ASSOC_DISALLOW_BH2__!1!g" \
                    -e "s!__R2_BSTA_ASSOC_DISALLOW_BH2__!0!g" \
                    -e "s!__BH2_SSID_REPLACE__!${backhaul_ssid/!/\!}!g" \
                    -e "s!__BH2_AUTH_TYPE_REPLACE__!${backhaul_authtype_str/!/\!}!g" \
                    "${tempfile}"
            fi
        fi

        # Update primary information in bss.conf
        sed -i -e "s!__PRIMARY_VLAN__!${primary_vlan_id/!/\!}!g" \
            -e "s!__PCP__!${pcp/!/\!}!g" \
            "${tempfile}"

        # Create new bridge based on num Vlan configured by user
        config_load network
        local num_vlan=$num_vlan_supported

        if [ "$override_enabled" -eq "1" ]; then
            for i in One Two Three; do
                config_get vlan_id MAPConfig "VlanIDNw"$i '0'
                for band in 2G 5G 5GL 6G 6GL; do
                    config_get fronthaul_ssid MAPConfig "FHOverrideSSIDNw"$i$band ''
                    config_get fronthaul_key MAPConfig "FHOverrideKeyNw"$i$band ''
                    config_get fronthaul_authtype MAPConfig "FHOverrideAuthTypeNw"$i$band 0
                    if [ "$force_5GL_cfg" == "0" -a "$band" = "5GL" -a -z "$device_5GL" ]; then
                        config_get fronthaul_ssid MAPConfig "FHOverrideSSIDNw"$i"5G" ''
                        config_get fronthaul_key MAPConfig "FHOverrideKeyNw"$i"5G" ''
                        config_get fronthaul_authtype MAPConfig "FHOverrideAuthTypeNw"$i"5G" 0
                    fi

                    fronthaul_authtype_str="0x0020"
                    if [ $fronthaul_authtype -gt 0 ]; then
                        if [ "$useSpecBasedDefinitions" -eq 0 ]; then
                            fronthaul_authtype_str="0x0060"
                            [ $fronthaul_authtype -eq 12 ] && fronthaul_authtype_str="0x0040"
                            [ $fronthaul_authtype -eq 13 ] && fronthaul_authtype_str="0x0800"
                            [ $fronthaul_authtype -eq 14 ] && fronthaul_authtype_str="0x1000"
                            [ $fronthaul_authtype -eq 15 ] && fronthaul_authtype_str="0x0820"
                            [ $fronthaul_authtype -eq 16 ] && fronthaul_authtype_str="0x1800"
                            [ $fronthaul_authtype -eq 17 ] && fronthaul_authtype_str="0x1820"
                        else
                            fronthaul_authtype_str="0x0820"
                            [ $fronthaul_authtype -eq 12 ] && fronthaul_authtype_str="0x0800"
                            [ $fronthaul_authtype -eq 13 ] && fronthaul_authtype_str="0x0040"
                            [ $fronthaul_authtype -eq 14 ] && fronthaul_authtype_str="0x0100"
                            [ $fronthaul_authtype -eq 15 ] && fronthaul_authtype_str="0x0060"
                            [ $fronthaul_authtype -eq 16 ] && fronthaul_authtype_str="0x0140"
                            [ $fronthaul_authtype -eq 17 ] && fronthaul_authtype_str="0x0160"
                        fi
                    fi
                    if [ "$map_version" -ge 3 ]; then
                        if [ "$map_dpp_enabled" -eq 1 ]; then
                            fronthaul_authtype_str="0x00E0"
                        fi
                    fi
                    if [ -z "$fronthaul_key" ]; then
                        fronthaul_key="NULL"
                        fronthaul_authtype_str="0x0001"
                    fi
                    if [ -z "$fronthaul_ssid" -o "$num_vlan" -le 1 ]; then
                        fronthaul_ssid="teardown"
                    fi
                    if [ "$map_ts_enabled" -eq 0 ]; then
                        fronthaul_ssid="teardown"
                    fi

                    ssid_with_escape_char=''
                    __repacd_str_handle_special_char "$fronthaul_ssid" ssid_with_escape_char
                    fronthaul_ssid="$ssid_with_escape_char"
                    key_with_escape_char=''
                    __repacd_str_handle_special_char "$fronthaul_key" key_with_escape_char
                    fronthaul_key="$key_with_escape_char"

                    local tmp_nwssid_replace="__FHOR"$band"_SSID_REPLACE_NW_"$i"__"
                    sed -i -e "s!${tmp_nwssid_replace}!${fronthaul_ssid/!/\!}!g" \
                        -e "s!__FHOR"$band"_KEY_REPLACE_NW_"$i"__!${fronthaul_key/!/\!}!g" \
                        -e "s!__FHOR"$band"_AUTH_TYPE_REPLACE_NW_"$i"__!${fronthaul_authtype_str/!/\!}!g" \
                        -e "s!__VLAN_ID_NW_"$i"__!${vlan_id/!/\!}!g" \
                        "${tempfile}"
                done
                num_vlan=$((num_vlan-1))
            done
        fi


        local num_vlan=$num_vlan_supported
        local fh_dot1x_encryption bss_auth_type bss_encr_type
        bss_encr_type="0x0008"
        for i in One Two Three; do
            config_get fronthaul_ssid MAPConfig "FronthaulSSIDNw"$i ''
            config_get fronthaul_key MAPConfig "FronthaulKeyNw"$i ''
            config_get fronthaul_authtype MAPConfig "FronthaulAuthTypeNw"$i 0
            config_get vlan_id MAPConfig "VlanIDNw"$i '0'

            fronthaul_authtype_str="0x0020"
            if [ $fronthaul_authtype -gt 0 ]; then
                if [ "$useSpecBasedDefinitions" -eq 0 ]; then
                    fronthaul_authtype_str="0x0060"
                    [ $fronthaul_authtype -eq 12 ] && fronthaul_authtype_str="0x0040"
                    [ $fronthaul_authtype -eq 13 ] && fronthaul_authtype_str="0x0800"
                    [ $fronthaul_authtype -eq 14 ] && fronthaul_authtype_str="0x1000"
                    [ $fronthaul_authtype -eq 15 ] && fronthaul_authtype_str="0x0820"
                    [ $fronthaul_authtype -eq 16 ] && fronthaul_authtype_str="0x1800"
                    [ $fronthaul_authtype -eq 17 ] && fronthaul_authtype_str="0x1820"
                else
                    fronthaul_authtype_str="0x0820"
                    [ $fronthaul_authtype -eq 12 ] && fronthaul_authtype_str="0x0800"
                    [ $fronthaul_authtype -eq 13 ] && fronthaul_authtype_str="0x0040"
                    [ $fronthaul_authtype -eq 14 ] && fronthaul_authtype_str="0x0100"
                    [ $fronthaul_authtype -eq 15 ] && fronthaul_authtype_str="0x0060"
                    [ $fronthaul_authtype -eq 16 ] && fronthaul_authtype_str="0x0140"
                    [ $fronthaul_authtype -eq 17 ] && fronthaul_authtype_str="0x0160"
                fi
            fi

            if [ "$map_version" -ge 3 ]; then
                if [ "$map_dpp_enabled" -eq 1 ]; then
                    fronthaul_authtype_str="0x00E0"
                fi
            fi

            if [ -z "$fronthaul_key" ]; then
                fronthaul_key="NULL"
                fronthaul_authtype_str="0x0001"
            fi

            if [ -z "$fronthaul_ssid" -o "$num_vlan" -le 1 ]; then
                fronthaul_ssid="teardown"
            fi

            if [ "$map_ts_enabled" -eq 0 ]; then
                fronthaul_ssid="teardown"
            fi

            if [ "$dot1x_support" -eq 1 ]; then
                __repacd_get_dot1x_details $fronthaul_authtype fh_dot1x_encryption \
                bss_auth_type bss_encr_type

                if [ -n "$fh_dot1x_encryption" -a -n "$bss_auth_type" ]; then
                    fronthaul_authtype_str="$bss_auth_type"
                fi

                config_get auth_primary_server MAPConfig "AuthPrimaryServerNw"$i ''
                config_get auth_primary_port MAPConfig "AuthPrimaryPortNw"$i ''
                config_get auth_primary_secret MAPConfig "AuthPrimarySecretNw"$i ''
                config_get auth_secondary_server MAPConfig "AuthSecondaryServerNw"$i ''
                config_get auth_secondary_port MAPConfig "AuthSecondaryPortNw"$i ''
                config_get auth_secondary_secret MAPConfig "AuthSecondarySecretNw"$i ''
                config_get rsn_preauth MAPConfig "RSN_PreAuthNw"$i ''
                config_get rsn_preauth_interfaces MAPConfig "RSN_PreAuth_InterfacesNw"$i ''
                config_get wpa_group_rekey MAPConfig "WPA_Group_ReKeyNw"$i ''
                config_get ieee80211w MAPConfig "IEEE80211WNw"$i ''
                config_get group_mgmt_cipher MAPConfig "Group_MGMT_CipherNw"$i ''
                config_get suiteb MAPConfig "SuiteBNw"$i ''
            fi

            ssid_with_escape_char=''
            __repacd_str_handle_special_char "$fronthaul_ssid" ssid_with_escape_char
            fronthaul_ssid="$ssid_with_escape_char"
            key_with_escape_char=''
            __repacd_str_handle_special_char "$fronthaul_key" key_with_escape_char
            fronthaul_key="$key_with_escape_char"

            sed -i -e "s!__FH_SSID_REPLACE_NW_"$i"__!${fronthaul_ssid/!/\!}!g" \
                -e "s!__FH_KEY_REPLACE_NW_"$i"__!${fronthaul_key/!/\!}!g" \
                -e "s!__FH_AUTH_TYPE_REPLACE_NW_"$i"__!${fronthaul_authtype_str/!/\!}!g" \
                -e "s!__FH_ENCR_TYPE_REPLACE_NW_"$i"__!${bss_encr_type/!/\!}!g" \
                -e "s!__VLAN_ID_NW_"$i"__!${vlan_id/!/\!}!g" \
                "${tempfile}"

            if [ -z "$auth_primary_server" ]; then
                auth_primary_server="teardown"
            fi
            if [ -z "$suiteb" ]; then
                suiteb="teardown"
            fi

            sed -i -e "s!__FH_PRIMARY_SERVER_IP_NW_"$i"__!${auth_primary_server/!/\!}!g" \
                -e "s!__FH_PRIMARY_PORT_NW_"$i"__!${auth_primary_port/!/\!}!g" \
                -e "s!__FH_PRIMARY_SECRET_NW_"$i"__!${auth_primary_secret/!/\!}!g" \
                -e "s!__FH_SECONDARY_SERVER_IP_NW_"$i"__!${auth_secondary_server/!/\!}!g" \
                -e "s!__FH_SECONDARY_PORT_NW_"$i"__!${auth_secondary_port/!/\!}!g" \
                -e "s!__FH_SECONDARY_SECRET_NW_"$i"__!${auth_secondary_secret/!/\!}!g" \
                -e "s!__FH_RSN_PREAUTH_NW_"$i"__!${rsn_preauth/!/\!}!g" \
                -e "s!__FH_RSN_PREAUTH_INTERFACES_NW_"$i"__!${rsn_preauth_interfaces/!/\!}!g" \
                -e "s!__FH_MAP_FTAUTH_NW_"$i"__!${map_ftauth/!/\!}!g" \
                -e "s!__FH_FT_256_KEY_NW_"$i"__!${ft_256_key/!/\!}!g" \
                -e "s!__FH_MOBILITY_DOMAIN_NW_"$i"__!${map_mobility_domain/!/\!}!g" \
                "${tempfile}"

            sed -i -e "s!__FH_SUITEB_NW_"$i"__!${suiteb/!/\!}!g" \
                -e "s!__FH_GROUP_MGMT_CIPHER_NW_"$i"__!${group_mgmt_cipher/!/\!}!g" \
                -e "s!__FH_WPA_GROUP_REKEY_NW_"$i"__!${wpa_group_rekey/!/\!}!g" \
                -e "s!__FH_IEEE8021W_NW_"$i"__!${ieee80211w/!/\!}!g" \
                "${tempfile}"
            num_vlan=$((num_vlan-1))
        done
    fi
    if [ "$override_enabled" -eq "0" ]; then
        for band in 2G 5G 5GL 6G 6GL; do
            sed -i -e "s!__FHOR"$band"_SSID_REPLACE__!"teardown"!}!g" \
                -e "s!__FHOR"$band"_SSID_REPLACE_NW_One!"teardown"!}!g" \
                -e "s!__FHOR"$band"_SSID_REPLACE_NW_Two__!"teardown"!}!g" \
                -e "s!__FHOR"$band"_SSID_REPLACE_NW_Three__!"teardown"!}!g" \
                -e "s!__FHA1OR"$band"_SSID_REPLACE__!"teardown"!}!g" \
                -e "s!__FHA2OR"$band"_SSID_REPLACE__!"teardown"!}!g" \
                -e "s!__FHA3OR"$band"_SSID_REPLACE__!"teardown"!}!g" \
                "${tempfile}"
        done
    fi

    if [ "$enable_mlo" -eq 1 ]; then
        bh_str="BH"
        if [ "$map_version" -gt 1 ]; then
           bh_str="BH1"
           #in two BH AP case, mlo grouping is supported only on R2 Backhaul and not in R1 backhaul
           [ "$use_bh2_for_mlo" -eq 1 ] && bh_str="BH2"
        fi

        if [ -n $backhaul_mld ]; then
            backhaul_mld=`echo $backhaul_mld | awk '{print toupper($0)}'`
            sed -i -e "s!__"$backhaul_mld"_SSID_KEY_REPLACE__!"$bh_str"!}!g" \
                "${tempfile}"
        fi

        if [ "$override_enabled" -eq "0" ]; then
            if [ -n $fronthaul_mld ]; then
                fronthaul_mld=`echo $fronthaul_mld | awk '{print toupper($0)}'`
                sed -i -e "s!__"$fronthaul_mld"_SSID_KEY_REPLACE__!"FH"!}!g" \
                "${tempfile}"
            fi

            if [ "$map_version" -gt 1 -a "$map_ts_enabled" -gt 0 ]; then
                num_vlan=$num_vlan_supported
                fh_vap_count=0
                for i in One Two Three; do
                    # if 1 then VLAN is configured on Primary VLAN
                    if [ "$num_vlan" -eq 0 -o "$num_vlan" -eq 1 ]; then
                        break
                    fi

                    config_get fronthaul_mld MAPConfig "FronthaulMldNw"$i ''

                    fh_vap_count=$((fh_vap_count+1))
                    if [ -n $fronthaul_mld ]; then
                        fronthaul_mld=`echo $fronthaul_mld | awk '{print toupper($0)}'`
                        sed -i -e "s!__"$fronthaul_mld"_SSID_KEY_REPLACE__!"FH""${fh_vap_count}"!}!g" \
                                 "${tempfile}"
                    fi

                    num_vlan=$((num_vlan-1))
                done
            fi

            ad_vap_index=1
            while [ $ad_vap_index -le $MAP_MAX_ADDITIONA_VAP ]; do
                config_get fronthaul_mld MAPConfig "AdditionalFHmld${ad_vap_index}" ''
                if [ -n "$fronthaul_mld" ]; then
                    __repacd_echo "Additional FH MLD Found: $fronthaul_mld"
                    fronthaul_mld=`echo $fronthaul_mld | awk '{print toupper($0)}'`
                    sed -i -e "s!__"$fronthaul_mld"_SSID_KEY_REPLACE__!"FHA""$ad_vap_index"!}!g" \
                        "${tempfile}"
                fi
                ad_vap_index=$((ad_vap_index+1))
            done
        else
            #MLO override
            #Fronthaul VAP override config
            local vap_sh ml_vap_list
            mld_dev_index=0
            config_get fronthaul_mld MAPConfig 'FronthaulMld' ''
            fronthaul_mld=`echo $fronthaul_mld | awk '{print toupper($0)}'`
            config_get mld_config MAPConfig 'MLDConfig' ''
            OR_short_name=$(cat ${tempfile} | grep -v teardown | grep SSID:\ FHOR | awk '{print $2}' | cut -d "," -f1)

            while [ $mld_dev_index -le $mld_count ]; do
                mld_group=$(eval "echo $mld_config | awk 'BEGIN { FS = \",\" }; { print \$$((mld_dev_index+1)) }'")

                if [ -z "$mld_group" ]; then
                    break;
                fi

                mld_dev=$(eval "echo $mld_group | awk 'BEGIN { FS = \":\" }; { print \$1 }'")
                mld_dev=`echo $mld_dev | awk '{print toupper($0)}'`
                mld_band=$(eval "echo $mld_group | awk 'BEGIN { FS = \":\" }; { print \$2 }'")

                for bd in $mld_band; do
                    for vap_sh in $OR_short_name; do
                        if [ "FHOR$bd" == "$vap_sh" ]; then
                            ml_vap_list="$ml_vap_list,$vap_sh"
                        fi
                    done
                done

                if [ -n "$ml_vap_list" ] && [ "$fronthaul_mld" == "$mld_dev" ]; then
                    sed -i -e "s!,__"$fronthaul_mld"_SSID_KEY_REPLACE__!$ml_vap_list!}!g" \
                              "${tempfile}"
                fi
                mld_dev_index=$((mld_dev_index+1))
            done

            #MLO Guest-1,2 & 3 VAP override config
            vap_sh='' ml_vap_list=''
            config_get mld_config MAPConfig 'MLDConfig' ''
            if [ "$map_version" -gt 1 -a "$map_ts_enabled" -gt 0 ]; then
                num_vlan=$num_vlan_supported
                for i in One Two Three; do
                    # if 1 then VLAN is configured on Primary VLAN
                    if [ "$num_vlan" -eq 0 -o "$num_vlan" -eq 1 ]; then
                        break
                    fi

                    config_get fronthaul_mld MAPConfig "FronthaulMldNw"$i ''
                    [ -z $fronthaul_mld ] && continue;
                    fronthaul_mld=`echo $fronthaul_mld | awk '{print toupper($0)}'`

                    [ "$i" == "One" ] && fh_prefix="FH1OR"
                    [ "$i" == "Two" ] && fh_prefix="FH2OR"
                    [ "$i" == "Three" ] && fh_prefix="FH3OR"

                    mld_dev_index=0 ml_vap_list=''
                    OR_short_name=$(cat ${tempfile} | grep -v teardown | grep SSID:\ FH | awk '{print $2}' | cut -d "," -f1)

                    while [ $mld_dev_index -le $mld_count ]; do
                        mld_group=$(eval "echo $mld_config | awk 'BEGIN { FS = \",\" }; { print \$$((mld_dev_index+1)) }'")

                        if [ -z "$mld_group" ]; then
                            break;
                        fi

                        mld_dev=$(eval "echo $mld_group | awk 'BEGIN { FS = \":\" }; { print \$1 }'")
                        mld_dev=`echo $mld_dev | awk '{print toupper($0)}'`
                        mld_band=$(eval "echo $mld_group | awk 'BEGIN { FS = \":\" }; { print \$2 }'")

                        if [ "$mld_dev" != "$fronthaul_mld" ]; then
                            mld_dev_index=$((mld_dev_index+1))
                            continue;
                        fi

                        for bd in $mld_band; do
                            for vap_sh in $OR_short_name; do
                                if [ "$fh_prefix""$bd" == "$vap_sh" ]; then
                                    ml_vap_list="$ml_vap_list,$vap_sh"
                                fi
                            done
                        done

                        if [ -n "$ml_vap_list" ] && [ "$fronthaul_mld" == "$mld_dev" ]; then
                            sed -i -e "s!,__"$fronthaul_mld"_SSID_KEY_REPLACE__!$ml_vap_list!}!g" \
                                      "${tempfile}"
                        fi
                        mld_dev_index=$((mld_dev_index+1))
                    done
                    num_vlan=$((num_vlan-1))
                done
            fi

            #MLO additional VAP override config
            mld_dev_index=0
            config_get mld_config MAPConfig 'MLDConfig' ''

            while [ $mld_dev_index -le $mld_count ]; do
                mld_group=$(eval "echo $mld_config | awk 'BEGIN { FS = \",\" }; { print \$$((mld_dev_index+1)) }'")

                if [ -z "$mld_group" ]; then
                    break;
                fi

                mld_dev=$(eval "echo $mld_group | awk 'BEGIN { FS = \":\" }; { print \$1 }'")
                mld_dev=`echo $mld_dev | awk '{print toupper($0)}'`
                mld_band=$(eval "echo $mld_group | awk 'BEGIN { FS = \":\" }; { print \$2 }'")

                fronthaul_mld='' ml_vap_list=''
                for bd in $mld_band; do
                    config_get additional_VAP_ovr MAPConfig "AdditionalFHOverride"$bd ''
                    ad_vap_index=1
                    while [ $ad_vap_index -le $MAP_MAX_ADDITIONA_VAP ]; do
                        ad_vap=$(eval "echo $additional_VAP_ovr | awk 'BEGIN { FS = \",\" }; { print \$$ad_vap_index }'")
                        if [ -n "$ad_vap" ]; then
                            fronthaul_mld=$(eval "echo $ad_vap | awk 'BEGIN { FS = \":\" }; { print \$4 }'")
                            __repacd_echo "Additional FH MLD Found: $fronthaul_mld"
                            if [ -n $fronthaul_mld ]; then
                                fronthaul_mld=`echo $fronthaul_mld | awk '{print toupper($0)}'`
                            fi
                        fi
                        if [ "$fronthaul_mld" == "$mld_dev" ]; then
                            break;
                        fi
                        ad_vap_index=$((ad_vap_index+1))
                    done

                    [ $ad_vap_index -le $MAP_MAX_ADDITIONA_VAP ] && ml_vap_list=$ml_vap_list,FHA$ad_vap_index"OR"$bd
                done

                if [ -n "$ml_vap_list" ] && [ "$fronthaul_mld" == "$mld_dev" ]; then
                    sed -i -e "s!,__"$fronthaul_mld"_SSID_KEY_REPLACE__!$ml_vap_list!}!g" \
                              "${tempfile}"
                fi
                mld_dev_index=$((mld_dev_index+1))
            done
        fi
        #End of MLO Override

        mld_dev_index=0
        config_get mld_config MAPConfig 'MLDConfig' ''
        while [ $mld_dev_index -le $mld_count ]; do
            mld_group=$(eval "echo $mld_config | awk 'BEGIN { FS = \",\" }; { print \$$mld_dev_index }'")
            if [ -z "$mld_group" ]; then
                break;
            fi
            mld_dev=$(eval "echo $mld_group | awk 'BEGIN { FS = \":\" }; { print \$1 }'")
            mld_dev=`echo $mld_dev | awk '{print toupper($0)}'`
            mld_band=$(eval "echo $mld_group | awk 'BEGIN { FS = \":\" }; { print \$2 }'")
            for bd in $mld_band; do
                sed -i -e "s/\<${mld_dev}${bd}\>/"${mld_dev}"/g" "${tempfile}"
            done
            mld_dev_index=$((mld_dev_index+1))
        done
    fi

    # Get short hand name from config file and remove unused profiles
    mld_SHN=$(cat ${tempfile} | grep MLO: | awk '{print $2}' \
                 | cut -d "," -f1 | awk '{$1=$1};1')
    for mldsh in $mld_SHN; do
        for band in 2G 5G 5GL 6G 6GL; do
            sed -i -e "s/,${mldsh}${band},/,/g" "${tempfile}"
            sed -i -e ":a;G;s/,${mldsh}${band}\n/\n/g" "${tempfile}"
            sed -i -e '/^$/d' "${tempfile}"
        done
    done

    # Remove empty MLO policy
    mlo_policy=$(cat ${tempfile} | grep MLO-Policy | grep -v ",MLD")

    # store old delimiter
    oldifs="$IFS"
    IFS=$'\n'
    for line in $mlo_policy; do
        sed -i -e  /${line}/d "${tempfile}"
    done
    # restore old delimiter
    IFS="$oldifs"

    # Remove the unused MLO config
    mld_count=0
    while [ $mld_count -lt $MAP_MAX_MLD_COUNT ]; do
        sed -i -e "s!__MLD"$mld_count"_SSID_KEY_REPLACE__!"teardown"!}!g" \
                "${tempfile}"
        mld_count=$((mld_count+1))
    done

    # Get short hand name from config file and remove unused profiles
    config_SHN=$(cat ${tempfile} | grep teardown | awk '{print $2}' \
                     | cut -d "," -f1 | awk '{$1=$1};1')
    for shName in $config_SHN; do
        sed -i -e "s/,${shName},/,/g" "${tempfile}"
        sed -i -e ":a;G;s/,${shName}\n/\n/g" "${tempfile}"
        sed -i -e '/^$/d' "${tempfile}"
    done
    # Remove config marked as teardown
    sed -i '/teardown/d' "${tempfile}"

    # if both primary & override credentials are available then override is preferred
    for band in 2G 5G 5GL 6G 6GL; do
        sed -i -e "s!FH,FHOR"$band"!FHOR"$band"!g" \
               -e "s!FH1,FH1OR"$band"!FH1OR"$band"!g" \
               -e "s!FH2,FH2OR"$band"!FH2OR"$band"!g" \
               -e "s!FH3,FH3OR"$band"!FH3OR"$band"!g" \
               "${tempfile}"
    done

    if [ ! -r "${WSPLCD_MAP_BSS_POLICY_PATH}" ] || \
        ! cmp -s "${tempfile}" "${WSPLCD_MAP_BSS_POLICY_PATH}"; then
        # New file differs from old. Move it into place and update the change count.
        # Check if the build is yocto and overwrite the map bss policy config file
        # instead of moving, in order to handle SELinux policy enforcements.
        if [ "$enable_yocto_build" -gt 0 ]; then
            cat "${tempfile}" > "${WSPLCD_MAP_BSS_POLICY_PATH}"
            # Remove the tempfile as all the contents of the tempfile have been
            # written into map bss policy config file
            rm -f "${tempfile}"
        else
            mv -f "${tempfile}" "${WSPLCD_MAP_BSS_POLICY_PATH}"
        fi
        changed=$((changed + 1))
        eval "$1='$changed'"
    else
        # No change, so just remove the temporary file
        rm -f "${tempfile}"
    fi

    return 0
}

# Switch the device into acting as a gateway connected AP (no bSTA).
#
# Pre-condition: RE mode has already been checked to be SON
#
# input: $1 is_controller: whether the device should act as the controller
#                          or just a gateway connected AP
# input: $2 standalone_controller: if a controller, whether the device should
#                                  act as a standalone controller
__repacd_config_gwcon_map_ap() {
    local is_controller=$1
    local wsplcd_mode son_mode
    local rate_scaling_factor=0
    local default_root_dist=0

    # The WDS, VAP independent, and QWrap AP settings also need to be updated
    # based on the range extension mode.
    local enable_wds=1 enable_multi_ap=1 disable_steering=0
    local deep_clone=0 deep_clone_no_bssid=0 config_sta=0 map_enable=1
    local block_dfs

    local pre_config_change

    __repacd_echo "Using SON mode for GW Connected AP"

    # In WDS/SON modes, we let the OEM customize whether DFS channels
    # should be permitted.
    if __repacd_is_block_dfs; then
        block_dfs=1
    else
        block_dfs=0
    fi

    wsplcd_enabled=1

    local disable_24g_sta disable_5g_sta disable_24g_ap disable_5g_ap
    if [ "$is_controller" -gt 0 ] && [ "$standalone_controller" -gt 0 ]; then
        __repacd_echo "Disabling all interfaces for standalone controller"
        disable_24g_sta=1
        disable_5g_sta=1
        disable_24g_ap=1
        disable_5g_ap=1
    else
        # In GW-connected AP mode, only the AP interfaces are enabled.
        disable_24g_sta=1
        disable_5g_sta=1
        disable_24g_ap=0
        disable_5g_ap=0
    fi

    if [ "$map_fast_onboarding" -eq 1 ]; then
        pre_config_change=$config_changed
    fi

    config_load wireless
    #Not to disable STA interfaces in Ethernet if Mbsta_onboarding is enabled
    if [ "$MBsta_onboarding" -eq 0 ]; then
        config_foreach __repacd_disable_vap wifi-iface \
            $managed_network 'sta' $disable_24g_sta $disable_5g_sta config_changed
    fi
    config_foreach __repacd_disable_vap wifi-iface \
        $managed_network 'ap' $disable_24g_ap $disable_5g_ap config_changed

    if [ "$map_fast_onboarding" -eq 1 ]; then
        if [ "$MBsta_onboarding" -eq 0 ]; then
            if [ "$disable_24g_sta" -eq 1 ] || [ "$disable_5g_sta" -eq 1 ]; then
                if [ "$config_changed" -ne "$pre_config_change" ]; then
                    config_changed=$pre_config_change
                    sta_config_changed=$((sta_config_changed + 1))
                fi
            fi
        fi
    fi

    # Now set up the interfaces in the right way.
    if [ "$standalone_controller" -eq 0 ]; then
        config_foreach __repacd_config_iface_map wifi-iface \
            $managed_network $block_dfs config_changed
    fi

    uci_commit wireless

    uci_set repacd repacd Role 'CAP'
    if [ "$is_controller" -gt 0 ]; then
        uci_set $MAP MultiAP EnableController 1

        # Standalone controller does not act as an Agent
        if [ "$standalone_controller" -gt 0 ]; then
            uci_set $MAP MultiAP EnableAgent 0
        else
            uci_set $MAP MultiAP EnableAgent 1
        fi

        # Force the remote association tracking on for the controller, as the
        # steering is centralized.
        uci_set $MAPLBD StaDB TrackRemoteAssoc 1
    else
        uci_set $MAP MultiAP EnableController 0
        uci_set $MAP MultiAP EnableAgent 1

        # For an agent, use whatever the current remote association tracking
        # setting is. There is still an advantage to tracking remote
        # associations as it allows the bridging tables to be cleaned up
        # properly when a Topology Notification is missed.
    fi

    if [ "$map_version" -ge 2 ]; then
        if [ "$map_fast_onboarding" -eq 1 ]; then
            pre_config_change=$config_changed
        fi

        __repacd_configure_hyd_map $map_version $is_controller
        config_load wireless
        if [ "$MBsta_onboarding" -eq 0 ]; then
            config_foreach __repacd_disable_vap wifi-iface \
                            $map_backhaul_nw 'sta' $disable_24g_sta $disable_5g_sta config_changed
        fi
        if [ "$map_fast_onboarding" -eq 1 ]; then
            if [ "$MBsta_onboarding" -eq 0 ]; then
                if [ "$disable_24g_sta" -eq 1 ] || [ "$disable_5g_sta" -eq 1 ]; then
                    if [ "$config_changed" -ne "$pre_config_change" ]; then
                    config_changed=$pre_config_change
                    sta_config_changed=$((sta_config_changed + 1))
                    fi
                fi
            fi
        fi
    fi

    # This generally should nto be needed when operating in gateway connected
    # AP mode. Until there is a case where we need it, we'll leave it disabled.
    uci_set repacd FrontHaulMgr ManageFrontAndBackHaulsIndependently 0
    uci_commit repacd

    uci_commit $MAP
    uci_commit $MAPLBD

    if [ "$is_controller" -gt 0 ]; then
        wsplcd_mode='REGISTRAR'
        son_mode='HYROUTER'

        if ! __repacd_generate_map_bss_conf config_changed; then
            return 1
        fi
    else
        wsplcd_mode='ENROLLEE'
        son_mode='HYCLIENT'
    fi

    if [ "$map_fast_onboarding" -eq 1 ]; then
        pre_config_change=$config_changed
    fi

    # No deep cloning with the MAP algorithms
    __repacd_configure_wsplcd $wsplcd_mode $deep_clone $deep_clone_no_bssid \
                              $config_sta $map_version config_changed

    __repacd_configure_son $enable_wds $disable_steering $enable_multi_ap \
                           $son_mode config_changed

    if [ "$map_fast_onboarding" -eq 1 ] && [ "$config_changed" -ne "$pre_config_change" ]; then
        config_changed=$pre_config_change
    fi

    return 0
}

# Switch the device to act as a range extender.
__repacd_config_map_re() {
    # The WDS and QWrap AP settings also need to be updated based on the
    # range extension mode.
    local enable_wds=1 block_dfs
    local enable_multi_ap=1 disable_steering=0
    local deep_clone=0 deep_clone_no_bssid=0 config_sta=0 map_enable=1
    local wifi_not_req=0

    local disable_24g_ap=0 disable_5g_ap=0
    local pre_config_change

    # We let the OEM customize whether DFS channels should be permitted.
    if __repacd_is_block_dfs; then
        block_dfs=1
    else
        block_dfs=0
    fi

    # We let the daemon start wsplcd once it has a stable bSTA association.
    wsplcd_enabled=0
    wsplcd_stop=1

    MLObSTAconfigured=0
    config_load repacd
    config_get_bool MLObSTAconfigured MAPConfig 'MLObSTAconfigured' '0'
    __repacd_echo "MLObSTAconfigured [$MLObSTAconfigured]"

    config_load wireless

    # How the bSTA interface is managed is dependent on the selected and
    # preference values.
    local selected_radio='' preferred_radio=''
    map_bsta_max_preference=0
    config_foreach __repacd_resolve_bsta_radio wifi-device \
        selected_radio preferred_radio

    if [ -z "$selected_radio" ]; then
        selected_radio="$preferred_radio"
    fi

    current_sta=''
    preferred_sta=''
    if [ "$MBsta_onboarding" -eq 0 ]; then
        __repacd_echo "Using $selected_radio for bSTA"
        config_foreach __repacd_update_map_bsta_radio wifi-iface \
                    $selected_radio $managed_network config_changed "$MLObSTAconfigured"
        if [ "$map_version" -ge 2 -a "$map_ts_enabled" -eq 1 ]; then
            config_foreach __repacd_update_map_bsta_radio wifi-iface \
                        $selected_radio $map_backhaul_nw config_changed "$MLObSTAconfigured"
        fi
    else
        __repacd_echo "Using $selected_radio for bSTA"
        config_foreach __repacd_update_map_bsta_radio_Multi_bsta wifi-iface \
                    $selected_radio $managed_network config_changed "$MLObSTAconfigured"
        if [ "$map_version" -ge 2 -a "$map_ts_enabled" -eq 1 ]; then
            config_foreach __repacd_update_map_bsta_radio_Multi_bsta wifi-iface \
                        $selected_radio $map_backhaul_nw config_changed "$MLObSTAconfigured"
        fi
    fi

    config_foreach __repacd_disable_vap wifi-iface \
        $managed_network 'ap' $disable_24g_ap $disable_5g_ap config_changed

    config_foreach __repacd_config_iface_map wifi-iface \
        $managed_network $block_dfs config_changed

    uci_commit wireless

    uci_set repacd repacd Role 'NonCAP'
    uci_set repacd FrontHaulMgr ManageFrontAndBackHaulsIndependently 0
    uci_commit repacd

    uci_set $MAP MultiAP EnableController 0
    uci_set $MAP MultiAP EnableAgent 1
    uci_commit $MAP

    if [ "$map_version" -ge 2 ]; then
        __repacd_configure_hyd_map $map_version $is_controller
    fi

    if [ "$map_fast_onboarding" -eq 1 ]; then
        pre_config_change=$config_changed
    fi

    __repacd_configure_wsplcd 'ENROLLEE' $deep_clone $deep_clone_no_bssid \
                              $config_sta $map_version config_changed

    __repacd_configure_son $enable_wds $disable_steering $enable_multi_ap \
                           'HYCLIENT' config_changed

    if [ "$map_fast_onboarding" -eq 1 ] && [ "$config_changed" -ne "$pre_config_change" ]; then
        config_changed=$pre_config_change
    fi
}

__repacd_map_check_backhaul_vaps() {
    local config="$1"
    local iface network disabled device
    local isCacInProgress

    config_get iface "$config" ifname
    config_get disabled "$config" disabled '0'
    config_get mode "$config" mode
    config_get MapBSSType "$config" MapBSSType '0'
    config_get network "$config" network
    config_get hidden "$config" hidden '0'
    config_get mld "$config" mld ''

    if [ -n "$iface" -a "$disabled" -eq 0 -a "$mode" = "ap" ] && [ "$hidden" != 1 ] && [ $((MapBSSType & 0x40)) -eq 64 ]; then
        local bitRate=$(repacdcli $iface get_bitrate)
        if [ "$bitRate" -eq 0 -o -z "$bitRate" ]; then
            local isAcsInProgress=$(cfg80211tool $iface get_acs_state)
            isAcsInProgress=${isAcsInProgress#*:}
            isCacInProgress=$(cfg80211tool $iface get_cac_state)
            isCacInProgress=${isCacInProgress#*:}
            if [ "$isCacInProgress" -eq 0 -a -n "$mld" ] && [ "$isAcsInProgress" -eq 0 ] && [ -n "$mld"]; then
                __repacd_echo "Iface $iface has invalid Bit Rate $bitRate post cac.
                                Do multi_up for $mld"
                wifi multi_up $mld
                sleep 5
            elif [ "$isCacInProgress" -eq 0 ] && [ "$isAcsInProgress" -eq 0 ] && [ -z "$mld"]; then
                ifconfig $iface down
                hapd $iface disable
                sleep 2
                ifconfig $iface up
                hapd $iface enable
            fi
        fi
    elif [ -n "$iface" -a "$disabled" -eq 0 -a "$mode" = "ap_smart_monitor" ]; then
        local bitRate=$(repacdcli $iface get_bitrate)
        if [ "$bitRate" -eq 0 -o -z "$bitRate" ]; then
            __repacd_echo "Smart monitor vap: $iface has invalid Bit Rate $bitRate"
            ifconfig $iface down
            hapd $iface disable
            sleep 2
            ifconfig $iface up
            hapd $iface enable
        fi
    fi
}

__repacd_check_one_iface_ready_multi() {
    local config=$1 is_nw_available
    local network mode disabled device dev_disabled ssid essid

    config_get iface "$config" ifname
    config_get network "$config" network
    config_get mode "$config" mode
    config_get disabled "$config" disabled '0'
    config_get ssid "$config" ssid
    config_get device "$config" device
    config_get dev_disabled "$device" disabled '0'
    config_load network
    config_get_bool ieee1905managed "$network" ieee1905managed

    if [ "$network" != "backhaul" ]; then
        [ -z "$ieee1905managed" -o "$ieee1905managed" -eq 0 ] && return
    fi

    [ "$dev_disabled" -gt 0 ] && return

    [ "$disabled" -gt 0 ] && return

    is_nw_available=$(uci show network | grep $network | grep type | grep bridge)

    [ -z "$is_nw_available" -a "$network" != "backhaul" ] && return

    if [ -n "$iface"  ]; then
        essid=$(repacdcli "$iface" get_essid)
    fi
    if [ -z "$iface" ]; then
        eval "$2='1'"
        __repacd_echo "some interface(s) are not ready: $ssid"
    elif [ -z "$essid" ]; then
        eval "$2='1'"
        __repacd_echo "some interface(s) are not ready: $iface :: ESSID=$essid"
    else
        ifconfig $iface 2>/dev/null |grep $iface
        retval=$?
        if [ $retval -ne 0 ] ; then
            eval "$2='1'"
            __repacd_echo "some interface($iface) are not ready: $ssid"
        fi
    fi
}

#Function to identify 11ax radios in the device
__repacd_number_of_non_11be_radio() {
    local config="$1"
    local hwmode

    config_get hwmode "$config" hwmode
    config_get_bool create_ctrl_bbss "$config" repacd_create_ctrl_bbss '1'

    if [ "$hwmode" != "11bea" ] && [ "$hwmode" != "11beg" ]; then
        __repacd_echo "HWMODE NON 11BE DETCETED"
        if [ "$create_ctrl_bbss" -eq 1 ]; then
            number_of_non_11be_radio=$((number_of_non_11be_radio+1)) #Only increment if we need to create sta vap on 11ax radio
            __repacd_echo "Number of Non 11be Radios: $number_of_non_11be_radio"
            uci set repacd.MAPConfig.number_of_non_11be_radio="$number_of_non_11be_radio"
            uci commit repacd
        fi
    fi
}

#Function will check the MLDConfig and find the
#BH mlo enabled bands. Mark them in wireless
#with map_mbsta_bhmlo_enabled so we can differentiate
#between FH & BH MLO devices
__repacd_MBsta_map_bhmlo_group() {
    local band=$1
    local config="$2"
    local mld_dev mld_dev_index=1 i
    local mld_band mld_group BH_AP_mld_dev mld_count
    local mld_device

    config_get mld_device MAPConfig 'BackhaulMld' ''
    config_get BH_AP_mld_dev MAPConfig 'MLDConfig' ''

    mld_count=$(eval "echo $BH_AP_mld_dev | grep -o mld | wc -l")
    __repacd_echo " backhaul_mld $mld_device band $band config $config"
    while [ $mld_dev_index -le $mld_count ]; do
        __repacd_echo " mld_dev_index $mld_dev_index, mld_count $mld_count"
        mld_group=$(eval "echo $BH_AP_mld_dev | awk 'BEGIN { FS = \",\" }; { print \$$mld_dev_index }'")
        if [ -z "$mld_group" ]; then
            break;
        fi
        mld_dev=$(eval "echo $mld_group | awk 'BEGIN { FS = \":\" }; { print \$1 }'")
        mld_band=$(eval "echo $mld_group | awk 'BEGIN { FS = \":\" }; { print \$2 }'")
        __repacd_echo "mld_dev $mld_dev, mld_band $mld_band"
        if [ "$mld_dev" == "$mld_device" ]; then
            for i in $mld_band; do
                if [ "$i" == "$band" ]; then
                    uci_set wireless "$config" map_mbsta_bhmlo_enabled 1
                    uci_commit wireless
                    __repacd_echo "using band:$band as one of the link in BSTA MLO group"
                    return
                fi
            done
        fi
        mld_dev_index=$((mld_dev_index+1))
    done
}

#Function will check for each band in the MLDConfig
#to match it with the BH mld grouping
__repacd_MBsta_find_BHmlo_group() {
    local config="$1"
    local device_band device backhaul_mld
    local device_6GL device_5GL currentband

    config_get device_6GL MAPConfig Radio6GL '0'
    config_get device_5GL MAPConfig Radio5GL '0'
    config_get currentband "$config" band '0'
    config_get backhaul_mld MAPConfig 'BackhaulMld' ''

    device="$config"
    if [ -z "$backhaul_mld" ]; then
        __repacd_echo "Error! No Backhaul MLD configured is Empty!"
        return
    fi
    __repacd_echo "device $device, backhaul_mld $backhaul_mld"
    if whc_is_5g_radio "$device"; then
        if [ "$currentband" -eq 3 ]; then
            if [ "$device" != "$device_6GL" ]; then
                device_band="6G"
            else
                device_band="6GL"
            fi
        elif [ "$device" != "$device_5GL" ]; then
            device_band="5G"
        else
            device_band="5GL"
        fi
    elif ! whc_is_5g_radio "$device"; then
        device_band="2G"
    fi
    __repacd_MBsta_map_bhmlo_group "$device_band" "$config"
}

#Function which will determine how many bsta vaps
#to be created, and MLD section based on how many
#radio's are 11be non 11be
#Input: wifi-iface
__repacd_MBsta_mldgroups_to_create() {
    local config="$1"
    local bhmloenabled device create_ctrl_bbss
    local hwmode create_backhaul_sta

    config_get_bool bhmloenabled "$config" map_mbsta_bhmlo_enabled '0'
    config_get device "$config" device
    config_get_bool create_ctrl_bbss "$config" repacd_create_ctrl_bbss '1'
    config_get_bool create_backhaul_sta "$config" repacd_create_backhaul_sta '0'
    config_get hwmode "$config" hwmode
    __repacd_echo "Populating Bsta vap counter Info: config $config bhmloenabled $bhmloenabled hwmode $hwmode device $device create_backhaul_sta: $create_backhaul_sta create_ctrl_bbss $create_ctrl_bbss"
    if [ "$hwmode" != "11bea" ] && [ "$hwmode" != "11beg" ]; then
        __repacd_echo "HWMODE NON!!BE DETCETED"
        if [ "$create_ctrl_bbss" -eq 1 ]; then
            number_of_non_11be_radio=$((number_of_non_11be_radio+1)) #Only increment if we need to create sta vap on 11ax radio
            __repacd_echo "Number of Non 11be Radios: $number_of_non_11be_radio"
            uci set repacd.MAPConfig.number_of_non_11be_radio="$number_of_non_11be_radio"
            uci commit repacd
        fi
    fi
    if [ "$bhmloenabled" -eq 1 ] && [ "$create_backhaul_sta" -eq 1 ]; then
        number_of_bsta=$((number_of_bsta+1))
        number_of_mlo_radio=$((number_of_mlo_radio+1))
    elif [ "$bhmloenabled" -eq 0 ] && [ "$create_backhaul_sta" -eq 1 ]; then
        number_of_bsta=$((number_of_bsta+1))
    fi

}

#Store STA ifaces in appropriate varaibles
#Input: Wifi-iface
#Output: Populates ath interfaces in MBsta_mlo_sta_iface_list
#& MBsta_mlo_sta_iface_list variables
__repacd_initialize_sta_ifaces_list() {
    local config="$1"
    local device mode iface cur_band bhmloenabled
    local mld

    config_get device "$config" device
    config_get mode "$config" mode
    config_get iface "$config" ifname
    config_get cur_band "$device" band
    config_get_bool bhmloenabled "$device" map_mbsta_bhmlo_enabled '0'
    config_get mld "$config" mld

   if [ "$mode" = "sta" ]; then
        if [ "$bhmloenabled" -eq 1 ]; then
            MBsta_mlo_sta_iface_list="$MBsta_mlo_sta_iface_list $iface"
            if [ -z "$MBsta_mlo_bstagroup" ]; then
                MBsta_mlo_bstagroup="$mld"
            fi
        else
            MBsta_non_mlo_sta_iface_list="$MBsta_non_mlo_sta_iface_list $iface"
            if [ -z "$MBsta_nonmlo_bstagroup" ]; then
                if [ -n "$mld" ]; then
                    MBsta_nonmlo_bstagroup="$mld"
                else
                    #Non 11 be
                    MBsta_nonmlo_bstagroup="$device $iface"
                fi
            fi
        fi
    fi
}

#Function to fetch the bhssid & key
#from BH ap vaps
#Input: Wifi-iface
#Output: Populates MBsta_BHssid &
#MBsta_BHkey variables
__repacd_get_bhssid() {
    local config="$1"

    local mode network ssid key
    local device MapBSSType
    local encryption

    config_get key "$config" key
    config_get mode "$config" mode
    config_get network "$config" network
    config_get ssid "$config" ssid
    config_get MapBSSType "$config" MapBSSType '0'
    config_get encryption "$config" encryption

    if [ "$mode" = "ap" ]; then
        #BSSType-64 (MapR1), 72 (MapR2), network - backhaul for Vlan enabled
        if [ "$MapBSSType" = "64" ] || [ "$MapBSSType" = "72" ] || [ "$network" = "backhaul" ]; then
            MBsta_BHssid="$ssid"
            MBsta_BHkey="$key"
            MBsta_BHencryption="$encryption"
            __repacd_echo "ssid $MBsta_BHssid key $MBsta_BHkey MBsta_BHencryption $MBsta_BHencryption"
            return
        fi
    fi
}

#Function to set or reset option ignore_bstavap
#in wireless based on Ethernet or Wifi BH
#Input: Wifi-iface
#Output: Sets ignore_bstavap option in sta if Eth BH
#Resets ignore_bstavap option if Wifi BH
#Sets the BHssid & BHkey in sta configs when
#needed
__repacd_set_reset_ignore_bstavap_in_wireless() {
    local config="$1"
    local mode iface ssid mld device hwmode
    local key encryption currentband

    config_get mode "$config" mode
    config_get iface "$config" ifname
    config_get ssid "$config" ssid
    config_get mld "$config" mld
    config_get device "$config" device
    config_get hwmode "$device" hwmode
    config_get key "$config" key
    config_get encryption "$config" encryption
    config_get currentband "$device" band '0'

    if [ "$mode" != "sta" ]; then
        return
    fi

    if [ "$start_role" = "CAP" ]; then
        uci_set wireless "$config" ignore_bstavap 1
        uci_set wireless "$config" wps_pbc '0'
        uci_commit wireless
    else
        uci_set wireless "$config" ignore_bstavap 0
        uci_set wireless "$config" wps_pbc '1'
        local onboardingDone=0
        local bhSsid=''
        config_get onboardingDone MAPConfig OnboardingDone
        config_get bhSsid MAPConfig 'BackhaulSSID' ''
        config_get MBsta_connected_bhssid MAPConfig 'MultibSTAConnectedBackhaulSSID'
        __repacd_echo "iface:$iface, ssid: $ssid, onboardingDone $onboardingDone"
        __repacd_echo "SSID of successfull connected node: $MBsta_connected_bhssid"
        #Below condition will be true when we onboard in ethernet out of box
        #MBsta_connected_bhssid is updated based on wpa_state of sta vap.
        #wpa_state=COMPLETED, means Agent successfully connected to a node
        if [ "$onboardingDone" -eq 1 ] && [ -z "$MBsta_connected_bhssid" ]; then
            config_load wireless
            config_foreach __repacd_get_bhssid wifi-iface
            __repacd_echo "MBsta_BHssid $MBsta_BHssid MBsta_BHkey $MBsta_BHkey"
            if [ "$ssid" != "$MBsta_BHssid" ] || [ "$key" != "$MBsta_BHkey" ] || [ "$encryption" != "$MBsta_BHencryption" ]; then
                uci_set wireless "$config" ssid "$MBsta_BHssid"
                uci_set wireless "$config" key "$MBsta_BHkey"
                uci_set wireless "$config" encryption "$MBsta_BHencryption"
                if [ "$MBsta_BHencryption" = "ccmp" ] || [ "$MBsta_BHencryption" = "psk2+ccmp" ]; then
                    uci_set wireless "$config" sae 1
                fi
                if [ "$currentband" -eq 3 ]; then
                    __repacd_echo "currentband: $currentband"
                    uci_set wireless "$config" 'en_6g_sec_comp' 0
                fi

                uci_commit wireless
                __repacd_echo "Set the bhssid $MBsta_BHssid key $MBsta_BHkey to wireless for iface $iface"
            fi
        fi
        uci_commit wireless
    fi
}

# Perform the startup actions when operating with Multi-AP SIG Topology
# Optimization.
#
# Pre-condition: repacd has already been determined to be enabled and in
#                MAP mode
#
# input: $1 - ether_gwcon: whether the device has been determined to be
#                          connected to the gateway via Ethernet
# input: $2 - start_role: the startup role for the device
# input: $3 - autoconf: boolean indicating whether this was a start
#                       initiated due to an auto config change
__start_map() {
    STARTTIME=$(date +%s)
    local ether_gwcon=$1
    local start_role=$2
    local autoconf=$3

    local enabled gwcon_mode first_config_required manage_mcsd
    local traffic_separation_enabled ethernet_monitoring_enabled
    local enable_steering standalone_controller
    local use_single_line_multi_up is_fallback_nonDPP

    config_changed=0
    map_dpp_enabled=0
    dpp_sta_found=0

    config_load 'repacd'
    config_get_bool enabled repacd 'Enable' '0'
    config_get gwcon_mode repacd GatewayConnectedMode 'AP'
    config_get_bool first_config_required MAPConfig 'FirstConfigRequired' '0'
    config_get_bool manage_mcsd repacd 'ManageMCSD' 1
    config_get_bool standalone_controller MAPConfig 'StandaloneController' '0'
    config_get map_country MAPConfig 'MapCountry'
    config_get map_single_r1r2_bh MAPConfig 'CombinedR1R2Backhaul'
    config_get use_pb_for_dpp MAPConfig 'DPPOnboardingWithPB' '0'

    # Certain features are not supported with Multi-AP (at least not yet)
    config_get traffic_separation_enabled repacd TrafficSeparationEnabled '0'
    config_get ethernet_monitoring_enabled repacd EnableEthernetMonitoring '0'
    config_get enable_steering repacd EnableSteering '1'

    # Config required for EasyMesh Rev2 Traffic Separation
    config_get map_version MAPConfig 'MapVersionEnabled' '1'
    config_get num_vlan_supported MAPConfig 'NumberOfVLANSupported' '0'
    config_get map_ts_enabled MAPConfig 'MapTrafficSeparationEnable' '0'
    config_get map_enable_backhaul_WPS MAPConfig 'EnableBackhaulWPS' '0'
    config_get map_primary_nw MAPConfig 'VlanNetworkPrimary' ''
    config_get map_backhaul_nw MAPConfig 'VlanNetworkBackHaul' ''

    # Config Required for DPP
    config_get onboarding_type MAPConfig 'OnboardingType'
    config_get dump_security_logs MAPConfig 'DPPDumpSecurityLog' '0'
    config_get dpp_gas_comeback_delay MAPConfig 'DPPGasComebackDelay'
    config_get enable_anqp_interworking MAPConfig 'EnableANQPInterworking'

    # Config Required for Fast Onboarding
    config_get map_fast_onboarding MAPConfig 'MapFastOnboarding'
    config_get map_use_high_pref_sta MAPConfig 'StartWithHighestPrefSTA'
    config_get use_single_line_multi_up MAPConfig 'MapUseSingleMultiUp'

    # Config required for 6G support
    config_get_bool map_6g_enable repacd 'Ezmesh6GEnable' '0'

    # Config Required for 8021x support
    config_get dot1x_support MAPConfig  'Enable8021x'

    # Config required for MLO support
    config_get enable_mlo MAPConfig 'EnableMLO' 0

    # Config required for SLO support
    config_get enable_slo MAPConfig 'EnableSLO' 0

    # Config required for Single Netdev support
    config_get enable_single_netdev MAPConfig 'EnableSingleNetdev' 0

    #Config required to know whether hiddenvaps are being brought down from vlanmon
    config_get hiddenvapdown 'MAPWiFiLink' 'hiddenvaps_down' '0'
    config_load wireless
    MBsta_non_mlo_sta_iface_list=''
    MBsta_mlo_sta_iface_list=''
    config_foreach __repacd_initialize_sta_ifaces_list wifi-iface
    config_get MBsta_onboarding MAPConfig 'MultibSTAOnboarding' 0

    if [ "$enable_slo" -eq 1 -o "$enable_mlo" -eq 1 ]; then
        enable_single_netdev=1
    fi

    config_get_bool maplite_enabled MAPConfig 'EnableLiteMode' '0'

    config_get useSpecBasedDefinitions MAPConfig 'useSpecBasedDefinitions' 0

    if [ "$map_version" -gt 0 ] && [ "$maplite_enabled" -ne 1 ]; then
        uci_set repacd MAPConfig 'MapFastOnboarding' 1
        uci_commit repacd
        uci_set wsplcd config 'MapFastOnboarding' "1"
        uci_commit wsplcd
        uci set ezmesh.MAPConfigSettings.MapFastOnboarding='1'
        uci commit
        __repacd_echo "Enabling Fast Onboarding by default for MAP Version $map_version"
    fi

    config_load repacd
    config_get_bool map_fast_onboarding MAPConfig 'MapFastOnboarding' '0'

    if [ "$onboarding_type" == "dpp" ]; then
        map_dpp_enabled=1
    else
        map_dpp_enabled=0

        config_load $MAP
        uci_set $MAP MAPConfigSettings 'EnableConfigService' 0
        uci_set $MAP MAPConfigSettings 'EnableDPPReconfig' 0
        uci_set $MAP MultiAP 'Enable1905Security' 1
        uci_set $MAP MAPConfigSettings 'MapOnboardingType' 'none'
        uci_set $MAP MAPConfigSettings 'MapConfigDumpToFile' 0
        uci_set $MAP MAPConfigSettings 'DebugLevel' 0
        uci_commit $MAP
    fi
    if [ "$map_6g_enable" -eq 1 ]; then
        uci_set $MAP MultiAP 'Map6GEnabled' 1
    else
        uci_set $MAP MultiAP 'Map6GEnabled' 0
    fi
    uci_commit $MAP

    if ! __repacd_is_son_mode; then
        __repacd_echo "Multi-AP SIG algorithms must use 'son' RE mode"
        return 1
    fi

    if [ "$traffic_separation_enabled" -gt 0 ]; then
        __repacd_echo "Traffic separation not supported with Multi-AP SIG" \
                      "Topology Optimization"
        return 1
    fi

    if [ "$ethernet_monitoring_enabled" -gt 0 ]; then
        __repacd_echo "Ethernet monitoring not supported with Multi-AP SIG" \
                      "Topology Optimization"
        return 1
    fi

    if [ "$enable_steering" -eq 0 ]; then
        __repacd_echo "Steering must be enabled with Multi-AP SIG" \
                      "Topology Optimization"
        return 1
    fi

    __repacd_map_reset_5g_attempts



    __repacd_echo "Starting Multi-AP SIG auto-configuration"
    __repacd_echo "Ethernet connection to GW=$ether_gwcon"
    __repacd_echo "GW Connected Mode=$gwcon_mode"
    __repacd_echo "MAP Version: $map_version"
    __repacd_echo "Num VLAN Supported: $num_vlan_supported"
    __repacd_echo "Combined R1 and R2 backhaul: $map_single_r1r2_bh"
    __repacd_echo "Onboarding Type : $onboarding_type"
    __repacd_echo "Dump Hostapd , supplicant and config service debug logs : $dump_security_logs"
    __repacd_echo "DPP Gas Comeback Delay : $dpp_gas_comeback_delay"
    __repacd_echo "Enable Interworking ANQP : $enable_anqp_interworking"
    __repacd_echo "MLO Support: $enable_mlo"
    __repacd_echo "Single Netdev Support: $enable_single_netdev"
    __repacd_echo "Fast Onboarding Support: $map_fast_onboarding"

    if [ "$onboarding_type" == "dpp" -a "$dump_security_logs" -eq 1 \
                            -a "$first_config_required" -gt 0 ]; then
        rm /tmp/hostapd.log
        rm /tmp/supplicant.log

        uci_set $MAP hy 'breakPadEnabled' '1'
        hostapdPID=$(ps | grep hostapd/global | grep -v grep | awk '{print$1}')
        suppPID=$(ps | grep wpa_supplicantglobal | grep -v grep | awk '{print$1}')
        kill $hostapdPID
        kill $suppPID
        if [ -e "/var/run/wpa_supplicantglobal" ]
        then
            rm /var/run/wpa_supplicantglobal &> /dev/null
        fi
        hostapd -g /var/run/hostapd/global -P /var/run/hostapd-global.pid \
                -ddddK -f /tmp/hostapd.log &
        wpa_supplicant -g /var/run/wpa_supplicantglobal -ddddK -f /tmp/supplicant.log &
    fi

    # For now, we can only manage a single network.
    config_get managed_network repacd ManagedNetwork '$primarybridgename'
    __repacd_echo "Managed network: $managed_network"

    local is_controller=0
    if __repacd_gw_mode || [ "$gwcon_mode" = 'Controller' ]; then
        # WAN group not empty; this device will act as controller regardless of
        # the GatewayConnectedMode setting
        is_controller=1
    fi

    if [ "$gwcon_mode" = 'CAP' ]; then
        # Bridged CAP and it needs to be controller
        # if the GatewayConnectedMode is CAP
        is_controller=1
    fi

    if [ "$MBsta_onboarding" -eq 0 ]; then
        config_load wireless
        config_foreach __repacd_number_of_non_11be_radio wifi-device
    fi

    if [ "$is_controller" -eq 0 ] && [ "$enable_single_netdev" -eq 1 ]; then
        config_load wireless
        config_foreach __repacd_wifimon_get_mld_matching_role wifi-mld "Non-AP" sta_mld

        configured_mld_count=0
        config_foreach __repacd_wifimon_get_configured_mld_count wifi-mld
        __repacd_echo "Available MLD section: $configured_mld_count"
        # find existing configured MLD count
        if [ "$configured_mld_count" -ge 0 ]; then
            mldIndex=$configured_mld_count
        fi

        if [ "$MBsta_onboarding" -eq 1 ]; then
            if [ -z "$sta_mld" ]; then
                config_foreach __repacd_MBsta_find_BHmlo_group wifi-device
                #Fresh wireless load required since we are updating it in above function
                config_load wireless
                config_foreach __repacd_MBsta_mldgroups_to_create wifi-device
                if [ "$number_of_bsta" -gt 0 ] && [ "$number_of_mlo_radio" -gt 0 ]; then
                    mld_bsta_group="mld$mldIndex"
                fi
                if [ "$number_of_mlo_radio" -gt 0 ]; then
                    #If there are mlo enbled radios then we calculate the
                    #total mld sections to be created based on number
                    #of BBSS and non11beradio
                    total_bsta_mld_count=$((number_of_bsta-number_of_mlo_radio))
                    total_bsta_mld_count=$((total_bsta_mld_count+1))
                    if [ "$number_of_non_11be_radio" -gt 0 ]; then
                        total_bsta_mld_count=$((total_bsta_mld_count-number_of_non_11be_radio))
                    fi
                elif [ "$number_of_mlo_radio" -eq 0 ]; then
                    total_bsta_mld_count=$((number_of_bsta-number_of_non_11be_radio))
                fi
                __repacd_echo "Number of Non-AP MLD: $total_bsta_mld_count Number of Non 11be radio: $number_of_non_11be_radio"
                __repacd_echo " MLO Non-AP mld group: $mld_bsta_group Number of BH BSTA: $number_of_bsta number of radio with MLO enabled: $number_of_mlo_radio"
                # Create New STA MLD section next to existing MLD
                __repacd_echo "Create New STA MLD section"
                while [ "$mldIndex" -lt "$total_bsta_mld_count" ]; do
                    if __repacd_create_mld_section "sta_mld_default" "$base_mld_mac_addr" "Non-AP"; then
                        __repacd_echo "STA MLD create mldIndex $mldIndex"
                    else
                        __repacd_echo "Error: Unable to create STA MLD"
                    fi
                done
            fi
        else
            if [ -z "$sta_mld" ]; then
                # Create New STA MLD section next to existing MLD
                __repacd_echo "Create New STA MLD section"
                if __repacd_create_mld_section "sta_mld_default" "$base_mld_mac_addr" "Non-AP"; then
                    sta_mld="mld$((mldIndex-1))"
                else
                    __repacd_echo "Error: Unable to create STA MLD"
                fi
            fi
        fi

        if [ "$MBsta_onboarding" -eq 0 ]; then
            __repacd_echo "Resolved STA MLD: [$sta_mld]"
        fi
        __repacd_echo "Next available MLD Index: $mldIndex"

        # Reset mldIndex to start searching from first MLD
        mldIndex=0
    fi

    # Grab a lock to prevent any updates from being made by the daemon.
    whc_wifi_config_lock

    # Since the controller could tear down all AP interfaces, we need to
    # allow hyd to run with no interfaces.
    uci_set $MAPLBD config_Adv 'AllowZeroAPInterfaces' 1

    # Also disable IAS on both bands, as we do not yet have the messaging
    # support for it & set the Forwading mode by default to SINGLE for MAP.
    uci_set $MAPLBD IAS 'Enable_W2' 0
    uci_set $MAPLBD IAS 'Enable_W5' 0
    uci_set $MAP hy 'ForwardingMode' 'SINGLE'
    uci_commit $MAPLBD
    uci_commit $MAP

    uci_set $MAP MultiAP 'MapVersion' $map_version
    uci_commit $MAP
    # Disable the DBDC repeater feature on all devices, as there will only
    # ever be a single backhaul STA interface.
    config_load wireless
    config_foreach __repacd_config_dbdc_device wifi-device \
                   0 config_changed

    if [ "$map_version" -ge 2 ]; then
        # Stop instance of repacd that is already running
        __stop_repacd_run
        __repacd_map_reset_default_bridge_config $is_controller
    fi

    # Set country to global on all devices
    config_load wireless
    config_foreach __repacd_config_set_device_country wifi-device $map_country

    # Enable SAWF
    config_load 'ezmesh'
    config_get enable_sawf MAPSawfSettings 'EnableSawf' '0'
    if [ "$enable_sawf" -eq 1 -o "$map_version" -ge 6 ]; then
        uci set wireless.qcawifi.sawf='1'
        uci commit
        __repacd_echo "**Enabling SAWF**"
    elif [ "$enable_sawf" -eq 0 -a "$map_version" -lt 6 ]; then
        uci set wireless.qcawifi.sawf='0'
        uci commit
        __repacd_echo "**Disabling SAWF**"
    fi

    if [ "$map_version" -ge 6 ]; then
        uci_set wsplcd config 'maxNumAPLinks' 2
        uci_set repacd MAPConfig 'bstaLinkToRemove' 'default'
        uci_set repacd MAPConfig 'bstaLinkToAdd' 'default'
        uci_set repacd MAPConfig 'bstaLinkRemoveActive' '0'
        uci_set ezmesh MAPQoSMSettings 'EnableSCS' '1'
        uci_set wsplcd config 'MapQosmEnabled' '1'

        uci_commit wsplcd
        uci_commit ezmesh
        uci commit repacd
    fi

    # Enable spec based definitions
    if [ "$useSpecBasedDefinitions" -eq 1 ]; then
        uci set wsplcd.config.useSpecBasedDefinitions='1'
        uci_commit wsplcd
    fi

    # Skip auto config for standalone controller, which must be manually configured
    if [ "$first_config_required" -gt 0 ] && [ "$standalone_controller" -eq 0 ]; then
        __repacd_reset_map_default_config $is_controller
        config_changed=1
        if [ "$onboarding_type" == "dpp" ]; then
            uci set repacd.MAPConfig.fallBackToNonDPP='0'
            #enableDPPOverWPS is used by PB hotplug script to enable/disable dpp_wps in hostapd/supplicant
            [ "$use_pb_for_dpp" == "1" ] && uci set repacd.MAPConfig.enableDPPOverWPS='1'
            uci_commit repacd
        fi
        uci_set repacd MAPConfig 'FirstConfigRequired' 0
        if [ "$map_fast_onboarding" -eq 1 ]; then
            uci_set repacd MAPConfig 'OnboardingDone' "0"
	    uci_set repacd MAPConfig 'SkipStaRestart' "0"
            uci_set wsplcd config 'MapFastOnboarding' "1"
            uci_set $MAP MAPConfigSettings 'MapFastOnboarding' "1"
            uci_set repacd MAPConfig 'MLObSTAconfigured' "0"
            uci_commit wsplcd
            uci_commit repacd
        fi

        __repacd_echo "Performed initial config on $managed_network VAPs"
    fi

    if __repacd_gw_mode || [ "$ether_gwcon" -gt 0 ]; then
        if !  __repacd_config_gwcon_map_ap $is_controller $standalone_controller; then
            return 1
        fi
    else
        # WAN group empty or non-existent
        # Switch to agent mode
        __repacd_config_map_re
    fi

    local enable_son=1
    __repacd_config_mcsd "$manage_mcsd" "$enable_son"

    whc_wifi_config_unlock

    config_load 'repacd'
    config_get_bool skip_repacd_dependent_restarts MAPConfig 'SkipRepacdDependentRestarts' '0'
    if [ "$skip_repacd_dependent_restarts" -eq 0 ]; then
        hyd_stop=1
        hyd_start=1
        wsplcd_stop=1
        wsplcd_start=1

        ## bSTA switch handling
        if [ "$map_fast_onboarding" -eq 1 -a "$is_controller" -eq 0 ]; then
            __repacd_echo "Start Role $start_role"
            __repacd_echo "Config Changed $config_changed"
            __repacd_echo "STA Config Changed $sta_config_changed"
            if [ "$MBsta_onboarding" -eq 1 ]; then
                config_load wireless
                config_foreach __repacd_set_reset_ignore_bstavap_in_wireless wifi-iface
            fi
            if [ "$sta_config_changed" -ge 1 ] && [ -n "$current_sta" ] && [ "$MBsta_onboarding" -eq 0 ]; then
                if [ "$is_current_sta_mlo" -eq 1 ]; then
                    current_sta=""
                    is_current_sta_mlo=0
                fi
                if [ "$map_dpp_enabled" -eq 0 ]; then
                    hyd_start=0
                    wsplcd_start=0
                    wsplcd_restart=0
                fi
                __repacd_echo "For fast onboarding stop MAP daemons"
                __repacd_echo "Switching between STAs"
                # Do not get new sta interface if mld bSTA switch is triggerred
                if [ "$preferred_mld_sta" -eq 0 ]; then
                    __repacd_map_get_new_sta
                fi
                __repacd_echo "Current STA $current_sta"
                __repacd_echo "Preferred STA $preferred_sta"
                __repacd_echo "Doing wifi only for the STAs"
                # Use single line multi_up command
                if [ "$use_single_line_multi_up" -eq 1 ]; then
                    __repacd_echo "Cmd: wifi multi_up $current_sta $preferred_sta"
                    wifi multi_up $current_sta $preferred_sta
                else
                    __repacd_echo "Cmd: wifi multi_up $current_sta"
                    __repacd_echo "Cmd: wifi multi_up $preferred_sta"
                    wifi multi_up $current_sta
                    wifi multi_up $preferred_sta
                fi

            elif [ "$sta_config_changed" -ge 1 ] && [ "$start_role" = "CAP" ] && [ "$MBsta_onboarding" -eq 0 ]; then
                __repacd_echo "Switching from wifi to eth backhaul"

                config_load wireless
                config_foreach __repacd_get_map_sta_iface wifi-iface
                if [ -n "$current_sta" ]; then
                    __repacd_echo "CAP mode disable STA (wifi multi_down $current_sta $sta_mld)"
                    wifi multi_down $current_sta $sta_mld
                fi

            elif [ "$start_role" = "CAP" ] && [ "$MBsta_onboarding" -eq 1 ]; then
                __repacd_echo "Switching to Ethernet. Disabling $MBsta_mlo_sta_iface_list $MBsta_non_mlo_sta_iface_list"
                config_foreach __repacd_set_reset_ignore_bstavap_in_wireless wifi-iface
                if [ -z "$MBsta_mlo_sta_iface_list" ] || [ -z "$MBsta_non_mlo_sta_iface_list" ]; then
                    MBsta_mlo_sta_iface_list=''
                    MBsta_non_mlo_sta_iface_list=''
                    config_load wireless
                    config_foreach __repacd_initialize_sta_ifaces_list wifi-iface
                fi
                if [ -n "$MBsta_mlo_sta_iface_list" ]; then
                    __repacd_disconnect_link $MBsta_mlo_sta_iface_list
                fi
                if [ -n "$MBsta_non_mlo_sta_iface_list" ]; then
                    __repacd_disconnect_link $MBsta_non_mlo_sta_iface_list
                fi

            elif [ -z "$current_sta" ] && [ "$sta_config_changed" -ge 1 ] \
                    && ([ "$start_role" = "NonCAP" ] || [ "$start_role" = "init" ]) && [ "$MBsta_onboarding" -eq 0 ]; then
                __repacd_echo "Start Role: $start_role , Switching from eth to wifi backhaul"

                config_load wireless
                config_foreach __repacd_get_map_sta_iface wifi-iface
                __repacd_echo "Current STA $current_sta"
                __repacd_echo "Preferred STA $preferred_sta"
                __repacd_echo "Doing wifi only for the STAs"

                if [ -n "$preferred_sta" ]; then
                    __repacd_echo "NonCAP mode enable STA (wifi multi_up $preferred_sta)"
                    wifi multi_up $preferred_sta
                else
                    __repacd_echo "NonCAP mode enable STA (wifi multi_up $current_sta)"
                    wifi multi_up $current_sta
                fi
            elif [ "$MBsta_onboarding" -eq 1 ] && ([ "$start_role" = "NonCAP" ] || [ "$start_role" = "init" ]); then
                #When we are onboarding out of box in ethernet & BHSSID is not configured
                #We copy  the BH credentials from the BH AP vap entry into the bsta
                #vap entries and issue multi_up of mld
                __repacd_echo "Doing wpa_cli reconnect for the STAs, moving to WIFI"
                config_foreach __repacd_set_reset_ignore_bstavap_in_wireless wifi-iface
                local onboarded=0
                config_get onboarded MAPConfig 'OnboardingDone' 0
                config_get MBsta_mlo_mode MAPConfig 'MBsta_mlo_mode' 0
                config_get MBsta_non_mlo_mode MAPConfig 'MBsta_non_mlo_mode' 0
                config_get backhaulssid MAPConfig BackhaulSSID
                config_get MBsta_connected_bhssid MAPConfig MultibSTAConnectedBackhaulSSID
                config_get MBsta_connected_bhkey MAPConfig MultibSTAConnectedBackhaulKey

                __repacd_echo "ConnectedBackhaulSSID: $MBsta_connected_bhssid ConnectedBackhaulKey: $MBsta_connected_bhkey MLO sta intf:$MBsta_mlo_sta_iface_list Non MLO sta intf:$MBsta_non_mlo_sta_iface_list  onboarded:$onboarded backhaulssid: $backhaulssid"
                __repacd_echo "MLO bSTA MLD group: $MBsta_mld_for_eth"
                #Below condition will be true when we onboard in ethernet out of box
                #MBsta_connected_bhssid is updated based on wpa_state of sta vap.
                #wpa_state=COMPLETED, means Agent successfully connected to a node
                if [ "$onboarded" -eq 1 ] && [ -z "$MBsta_connected_bhssid" ]; then
                    uci set repacd.MAPConfig.BackhaulSSID="$MBsta_BHssid"
                    uci set repacd.MAPConfig.MultibSTAConnectedBackhaulSSID="$MBsta_BHssid"
                    uci set repacd.MAPConfig.MultibSTAConnectedBackhaulKey="$MBsta_BHkey"
                    uci set repacd.MAPConfig.MultibSTAConnectedBackhaulEncryption="$MBsta_BHencryption"

                    uci commit repacd
                    MBsta_mld_for_eth="$MBsta_mlo_bstagroup"
                    local non_11be_radio_count=0
                    config_get non_11be_radio_count MAPConfig 'number_of_non_11be_radio' 0
                    if [ -n "$MBsta_nonmlo_bstagroup" ]; then
                        MBsta_mld_for_eth="$MBsta_mld_for_eth $MBsta_nonmlo_bstagroup"
                    fi
                    __repacd_echo "Invoking: wifi multi_up $MBsta_mld_for_eth"
                    wifi multi_up $MBsta_mld_for_eth
                    sleep 5
                fi
                #Ethernet to Wifi we reset the short_timer_used
                config_get MBsta_short_timer_used MAPConfig 'MBsta_shorttimerused' 0
                if [ "$MBsta_short_timer_used" -eq 1 ]; then
                    MBsta_short_timer_used=0
                    uci set repacd.MAPConfig.MBsta_shorttimerused='0'
                    uci commit repacd
                fi
                if [ -n "$MBsta_connected_bhssid" ]; then
                    if [ "$MBsta_mlo_mode" -eq 1 ]; then
                        if [ -n "$MBsta_mlo_sta_iface_list" ]; then
                            __repacd_MBsta_reconnect_link $MBsta_mlo_sta_iface_list
                            __repacd_MBsta_disconnect_link $MBsta_non_mlo_sta_iface_list
                            __repacd_echo "Reconnected MLO STA $MBsta_mlo_sta_iface_list & disconnected NON MLO since MBsta_mlo_mode=1"
                        fi
                    elif [ "$MBsta_non_mlo_mode" -eq 1 ]; then
                        if [ -n "$MBsta_non_mlo_sta_iface_list" ]; then
                            __repacd_MBsta_reconnect_link $MBsta_non_mlo_sta_iface_list
                            __repacd_MBsta_disconnect_link $MBsta_mlo_sta_iface_list
                            __repacd_echo "Reconnected NONMLO STA $MBsta_non_mlo_sta_iface_list & disconnected MLO since MBsta_non_mlo_mode=1"
                        fi
                    else
                        #In cases where we never connected in Wifi and moved to Ethernet
                        #both modes will be 0 so we enable all sta vaps to connect
                        config_get MBsta_upfront_mlo MAPConfig 'MultibSTAConfigureMLObSTAUpfront'
                        if [ "$MBsta_upfront_mlo" -eq 0 ]; then
                            if [ -n "$MBsta_mlo_sta_iface_list" ]; then
                                __repacd_MBsta_reconnect_link $MBsta_mlo_sta_iface_list
                                __repacd_echo "Reconnecting MLO STA $MBsta_mlo_sta_iface_list"
                            fi
                            if [ -n "$MBsta_non_mlo_sta_iface_list" ]; then
                                __repacd_MBsta_reconnect_link $MBsta_non_mlo_sta_iface_list
                                __repacd_echo "Reconnecting NONMLO STA $MBsta_non_mlo_sta_iface_list"
                            fi
                        else
                            __repacd_echo "Upfront MLO connection expected"
                            if [ -n "$MBsta_mlo_sta_iface_list" ]; then
                                __repacd_MBsta_reconnect_link $MBsta_mlo_sta_iface_list
                                if [ -n "$MBsta_non_mlo_sta_iface_list" ]; then
                                    __repacd_MBsta_disconnect_link $MBsta_non_mlo_sta_iface_list
                                    __repacd_echo "Reconnected MLO STA $MBsta_mlo_sta_iface_list & disconnected NON MLO since upfront mlo=1"
                                fi
                                __repacd_echo "Reconnected MLO STA $MBsta_mlo_sta_iface_list since upfront mlo=1"
                            fi
                        fi
                    fi
                fi
            fi
        fi

        if [ "$map_fast_onboarding" -eq 1 ]; then
            [ -f "$MAP_TEMP_INTF_LIST" ] && rm $MAP_TEMP_INTF_LIST
            [ -f "$MAP_TEMP_INTF_LIST_WSPLCD" ] && rm $MAP_TEMP_INTF_LIST_WSPLCD
        fi
        __repacd_restart_dependencies
    else
        __repacd_echo "Skipping Repacd dependent daemon restart on repacd start/restart"
    fi

    # We are trying to apply the Vlan before VAPs comes up, so ezmesh & wsplcd config file has
    # some invalid interfaces.
    # Workaround is added to loop until interfaces are up and then vlan is initiated.
    local status=0
    retry=1
    while [ "$retry" -gt 0 ]; do
        config_load wireless
        status=0
        config_foreach __repacd_check_one_iface_ready_multi wifi-iface status
        if [ "$status" -eq 1 ]; then
            __repacd_echo "some interfaces not found/up. Looping untill all the interfaces are up..."
            sleep 1
        else
            __repacd_echo "All interfaces found"
            retry=0
        fi
    done

    if [ "$map_version" -ge 2 ]; then
        if [ "$map_ts_enabled" -gt 0 ]; then
            if [ "$enable_slo" -eq 1 -a -z "$base_mld_mac_addr" ]; then
                ubus call network reload
            fi
            if [ "$is_controller" -gt 0 ]; then
                __repacd_map_apply_vlan_config
            fi

            # Set bridge MAC
            __repacd_map_set_bridge_mac
            ubus call network reload

            # Set egress/ingress priorty maps for Controller
            if [ "$is_controller" -gt 0 -a "$map_version" -ge 3 ]; then
                sleep 2
                __repacd_map_set_egress_ingress_ports
            fi

            if [ "$is_controller" -gt 0 ]; then
                config_load wireless
                config_foreach __repacd_map_check_backhaul_vaps wifi-iface
            fi
        fi

        config_get is_fallback_nonDPP MAPConfig 'fallBackToNonDPP' '0'
        if [ "$map_version" -ge 3 -a "$map_dpp_enabled" -eq 1 -a "$is_fallback_nonDPP" -eq 0 ]; then
            # Delete temp files
            rm /tmp/map_key_info.tmp
            rm /tmp/mapConfig.log

            # Get Configurator key if already present
            dpp_config_key=$(cat /etc/map_dpp_key)
            dpp_key_len=${#dpp_config_key}

            if [ "$is_controller" -eq 1 ]; then
                __repacd_set_map_dpp_config $is_controller $start_role
            fi

            if [ "$is_controller" -eq 0 ]; then
                config_load wireless
                config_foreach __repacd_get_map_sta_iface wifi-iface

                __repacd_set_map_dpp_config $is_controller $start_role
            fi
        fi

        # stop/start hyd only if there is any change in network config
        # due to addition of VLAN interfaces. If required VLAN iface already
        # present then we can avoid hyd stop/start. This will save some time
        # and avoid any delays after repacd restart.
        if [ "$net_config_changed" -gt 0 ] || [ "$config_changed" -gt 0 ] || [ "$is_controller" -eq 1 ]; then
            config_changed=0
            if [ "$skip_repacd_dependent_restarts" -eq 0 ]; then
                hyd_stop=1
                hyd_start=1
                wsplcd_stop=1
                wsplcd_start=1
                if [ "$map_fast_onboarding" -eq 1 ]; then
                    [ -f "$MAP_TEMP_INTF_LIST" ] && rm $MAP_TEMP_INTF_LIST
                    [ -f "$MAP_TEMP_INTF_LIST_WSPLCD" ] && rm $MAP_TEMP_INTF_LIST_WSPLCD
                fi
                __repacd_restart_dependencies
            else
                __repacd_echo "Skipping Repacd dependent daemon restart on repacd start/restart"
            fi
        fi
    fi

    if ! __repacd_gw_mode; then
        __stop_repacd_run

        # Transform the boolean value into what the daemon expects
        if [ "$autoconf" -gt 0 ]; then
            autoconf='autoconf'
        else
            autoconf=''
        fi

        # Start the script that monitors the link state.
        #
        # In this NonCAP mode, it will keep checking whether there is a link
        # to the gateway over ethernet.
        __repacd_echo "Starting  RE Placement and Auto-config Daemon"
        start-stop-daemon -S -x /usr/sbin/repacd-run.sh -b -- \
            "map" "$start_role" $config_re_mode $resolved_re_mode \
            $resolved_re_submode $autoconf
    fi
    ENDTIME=$(date +%s)
    echo "repacd map started in $(($ENDTIME - $STARTTIME)) seconds"
}

# Get STA iface
__repacd_get_map_sta_iface() {
    local config="$1"
    local iface disabled mode my_bootstrap sta_iface
    local device

    config_get iface "$config" ifname
    config_get disabled "$config" disabled '0'
    config_get mode "$config" mode
    config_get device "$config" device
    config_get mld "$config" mld ''

    if [ "$mode" != "sta" ]; then
        return
    fi

    if [ -n "$iface" -a "$disabled" -eq 0 -a "$mode" = "sta" ]; then
        dpp_sta_iface=$iface
        dpp_sta_found=1

        if [ -z "$dpp_sta_iface" ]; then
            __repacd_echo "STA iface invalid"
        fi

        __repacd_echo "STA iface $dpp_sta_iface"
    fi

    if [ "$mode" = "sta" -a "$map_fast_onboarding" -eq 1 ]; then
        if [ -n "$iface" ]; then
            __repacd_echo "STA found iface $iface"
            # Aggregate the current sta with mld partner link interface name
            # instead of mldX name. multi_up mld2 is not disabling the MLO
            # partner link interfaces, so invoking multi_up with each link name
            if [ -n "$mld" -a "$disabled" -eq 1 ]; then
                current_sta="$current_sta $device $iface"
            else
                current_sta="$device $iface"
            fi
            __repacd_echo "STA device iface $current_sta"
        fi

        if [ -z "$iface" ]; then
            __repacd_echo "STA iface not found"
            preferred_sta="$device"
            if [ -z "$mld" ]; then
                __repacd_map_get_new_sta
            else
               # Update the preferred sta entry with mldX ifname while
               # switching from eth to wifi backhaul
               preferred_sta="$mld"
               __repacd_echo "MLO Preferred STA device iface $preferred_sta"
            fi
            __repacd_echo "STA device iface $current_sta"
        fi
    fi
}

# Set dpp_key on iface
__repacd_set_dpp_key_iface() {
    local config="$1"
    local key="$2"
    local iface disabled mode

    config_get iface "$config" ifname
    config_get disabled "$config" disabled '0'
    config_get mode "$config" mode

    if [ -n "$iface" -a "$disabled" -eq 0 -a "$mode" = "ap" ]; then
        uci_set wireless "$config" dpp_key "$key"
        uci_commit wireless
    fi

    if [ -n "$iface" -a "$disabled" -eq 0 -a "$mode" = "sta" ]; then
        uci_set wireless "$config" dpp_key "$key"
        uci_commit wireless
    fi
}

# Setup dpp on controller and agent
# input: $1 - is_controller: whether this device is acting as the controller
# input: $2 - start_role: start role for device
__repacd_set_map_dpp_config() {
    local is_controller=$1
    local start_role=$2
    local dump_security_logs enable_cs_logs
    local enable_dpp_reconfig

    config_load repacd
    config_get enable_cs_logs MAPConfig 'EnableConfigServiceLogs' '0'
    config_get dump_security_logs MAPConfig 'DPPDumpSecurityLog' '0'
    config_get enable_dpp_reconfig MAPConfig 'EnableDPPReconfig' '0'

    __repacd_echo "DPP Key : $dpp_config_key"
    __repacd_echo "DPP Key Len: $dpp_key_len"
    __repacd_echo "DPP Reconfig Enabled: $enable_dpp_reconfig"

    uci_set $MAP MAPConfigSettings 'EnableConfigService' 1
    if [ "$enable_dpp_reconfig" -eq 1 ]; then
        uci_set $MAP MAPConfigSettings 'EnableDPPReconfig' 1
    fi

    if [ "$start_role" == "NonCAP" -o "$start_role" == "RE" ]; then
        uci_set $MAP MAPConfigSettings 'EnableConfigService' 0
    fi
    uci_set $MAP MultiAP 'Enable1905Security' 1
    uci_set $MAP MAPConfigSettings 'MapEProfile' 'generic'
    uci_set $MAP MAPConfigSettings 'MapOnboardingType' 'dpp'
    uci_set $MAP MAPConfigSettings 'MapConfigDumpToFile' $enable_cs_logs
    uci_set $MAP MAPConfigSettings 'DebugLevel' 0

    if [ "$enable_cs_logs" -eq 1 -a "$dump_security_logs" -eq 0 ]; then
        uci_set $MAP MAPConfigSettings 'DebugLevel' 1
    fi
    if [ "$enable_cs_logs" -eq 1 -a "$dump_security_logs" -eq 1 ]; then
        uci_set $MAP MAPConfigSettings 'DebugLevel' 2
    fi

    # Start controller socket on hostapd
    hostapd_cli -i ath0 -p /var/run/hostapd-wifi0 dpp_controller_start tcp_port=7999

    # Controller Config
    if [ "$is_controller" -eq 1 ]; then
        if [ "$dpp_key_len" -eq 0 ]; then
            # Get Key
            hostapd_cli -i ath0 -p /var/run/hostapd-wifi0 dpp_configurator_remove '*'
            hostapd_cli -i ath0 -p /var/run/hostapd-wifi0 dpp_configurator_add curve=P-256
            dpp_config_key=$(hostapd_cli -i ath0 -p /var/run/hostapd-wifi0 \
                                         dpp_configurator_get_key 1)
            echo $dpp_config_key > /etc/map_dpp_key
        fi

        # Get CSIGN JWK
        hostapd_cli -i ath0 -p /var/run/hostapd-wifi0 dpp_map_get_jwk_csign curve=P-256 \
                    key=$dpp_config_key

        uci_set $MAP MAPConfigSettings 'RoleController' 1
        uci_set $MAP MAPConfigSettings 'RoleAgent' 0
        uci_set $MAP MAPConfigSettings 'DPPConfiguratorKey' $dpp_config_key
        uci_commit $MAP
    fi

    # Agent Config
    if [ "$is_controller" -eq 0 ]; then
        __repacd_echo "Configure DPP Agent"
        if [ "$dpp_sta_found" -eq 0 ]; then
            __repacd_echo "Configure Ethernet Agent"

            if [ "$dpp_key_len" -eq 0 ]; then
                # Get Key
                hostapd_cli -i ath0 -p /var/run/hostapd-wifi0 dpp_configurator_remove '*'
                hostapd_cli -i ath0 -p /var/run/hostapd-wifi0 dpp_configurator_add curve=P-256
                dpp_config_key=$(hostapd_cli -i ath0 -p /var/run/hostapd-wifi0 \
                                             dpp_configurator_get_key 1)
                echo $dpp_config_key > /etc/map_dpp_key
            fi

            # Bootstrap Gen
            hostapd_cli -i ath0 -p /var/run/hostapd-wifi0 dpp_bootstrap_gen type=qrcode \
                        key=$dpp_config_key
            hostapd_cli -i ath0 -p /var/run/hostapd-wifi0 dpp_bootstrap_info 1
            my_bootstrap=$(hostapd_cli -i ath0 -p /var/run/hostapd-wifi0 dpp_bootstrap_get_uri 1)
        else
            __repacd_echo "Configure bSTA $dpp_sta_iface"
            # Get Key
            if [ "$dpp_key_len" -eq 0 ]; then
                wpa_cli -p /var/run/wpa_supplicant-$dpp_sta_iface dpp_configurator_remove '*'
                wpa_cli -p /var/run/wpa_supplicant-$dpp_sta_iface dpp_configurator_add curve=P-256
                dpp_config_key=$(wpa_cli -p /var/run/wpa_supplicant-$dpp_sta_iface \
                                         dpp_configurator_get_key 1 | grep 30770)
                echo $dpp_config_key > /etc/map_dpp_key

                __repacd_echo "DPP Key : $dpp_config_key"

                config_load wireless
                config_foreach __repacd_set_dpp_key_iface wifi-iface $dpp_config_key

                __repacd_echo "DPP Key Len 0. First time generating Key. Restart nw"
                wifi load
            fi

            # Generate BootStrap
            wpa_cli -p /var/run/wpa_supplicant-$dpp_sta_iface dpp_bootstrap_remove '*'
            wpa_cli -p /var/run/wpa_supplicant-$dpp_sta_iface dpp_bootstrap_gen type=qrcode \
                    key=$dpp_config_key
            wpa_cli -p /var/run/wpa_supplicant-$dpp_sta_iface dpp_bootstrap_info 1

            my_bootstrap=$(wpa_cli -p /var/run/wpa_supplicant-$dpp_sta_iface dpp_bootstrap_get_uri 1 \
                               | grep DPP)
        fi

        config_load wireless
        config_foreach __repacd_set_dpp_key_iface wifi-iface $dpp_config_key

        rm /tmp/map_bootstrap_info.tmp
        echo $my_bootstrap > /tmp/map_bootstrap_info.tmp
        uci_set $MAP MAPConfigSettings 'DPPConfiguratorKey' $dpp_config_key
        uci_set $MAP MAPConfigSettings 'RoleController' 0
        uci_set $MAP MAPConfigSettings 'RoleAgent' 1
        uci_commit $MAP
    fi
}

__repacd_config_set_default_maplite_vap_param() {
    local vapIdx=$1
    config_load repacd
    config_get map_version MAPConfig 'MapVersionEnabled'

    uci set wireless.@wifi-iface[$vapIdx].encryption='psk2+ccmp'
    uci set wireless.@wifi-iface[$vapIdx].key=maplite-123
    if [ "$map_version" -lt 6 ]; then
        uci set wireless.@wifi-iface[$vapIdx].wps_pbc=1
        uci set wireless.@wifi-iface[$vapIdx].wps_pbc_enable=1
        uci set wireless.@wifi-iface[$vapIdx].wps_pbs_start_time=0
        uci set wireless.@wifi-iface[$vapIdx].wps_pbs_duration=120
    fi
    uci set wireless.@wifi-iface[$vapIdx].dpp_configurator_connectivity=1
    uci set wireless.@wifi-iface[$vapIdx].dpp_map=1
    uci set wireless.@wifi-iface[$vapIdx].map=$map_version
    uci commit wireless
}

__repacd_config_set_default_maplite_radio_param() {
    local rIdx=$1

    uci set wireless.wifi$rIdx.htmode=HT20
    uci set wireless.wifi$rIdx.txchainmask=15
    uci set wireless.wifi$rIdx.rxchainmask=15
    uci commit wireless
}

# Delete all of the VAPs for the given network that are marked as unmanaged.
#
# input: $1 config: section being considered
# input: $2 mode: ap or sta
__repacd_delete_managed_maplite_vaps() {
    local config="$1"
    local network repacd_security_unmanaged mode

    config_get network "$config" network
    config_get mode "$config" mode
    config_get device "$config" device

    if [ $mode = "ap_smart_monitor" -o $mode = "$2" ]; then
        uci delete "wireless.$config"
    fi

    if [ "$3" = "$network" -a "$4" != "$device" ]; then
        uci delete "wireless.$config"
    fi
}

__repacd_MBsta_disconnect_link() {
    local link="$@"
    local link_item
    local network_id

    if [ -n "$link" ]; then
        for link_item in $link; do
            wpa_cli -p "/var/run/wpa_supplicant-$link_item" -i "$link_item" disconnect
            __repacd_echo "DISCONNECT & DISABLE $link_item"
            network_id=`wpa_cli -p /var/run/wpa_supplicant-$link_item list_network | grep $MBsta_connected_bhssid | awk '{print $1}'`
            if [ -z  $network_id ]; then
                network_id=0
            fi
            __repacd_echo "Brought down $link_item on networkid $network_id"
            wpa_cli -p /var/run/wpa_supplicant-$link_item disable_network $network_id
        done
    else
        __repacd_echo "Empty link, cannot disconnect"
    fi
}

__repacd_MBsta_reconnect_link() {
    local link="$@"
    local link_item
    local network_id

    if [ -n "$link" ]; then
        __repacd_echo "MultibSTAConnectedBackhaulSSID: $MBsta_connected_bhssid"
        for link_item in $link; do
            usleep 500000
            __repacd_echo " Reconnecting on interface $link_item from list: $link"
            network_id=`wpa_cli -p /var/run/wpa_supplicant-$link_item list_network | grep $MBsta_connected_bhssid | awk '{print $1}'`
            if [ -z  $network_id ]; then
                network_id=0
            fi
            wpa_cli -p /var/run/wpa_supplicant-$link_item enable_network $network_id
            __repacd_echo "Brought up $link_item on networkid $network_id"
            wpa_cli -p "/var/run/wpa_supplicant-$link_item" -i "$link_item" reconnect
            __repacd_echo "ENABLING $link_item TO SCAN & CONNECT"

        done
    else
        __repacd_echo "Empty link, cannot reconnect"
    fi
}

# Perform the startup actions when operating MAP Lite Mode
# (Plug Fest Mode).
__start_map_lite() {
    kill $repacdPID
    local unicast_flood_disable
    local hostapdPID suppPID
    local bh_type sta_device
    local onboarding_type
    local ezmeshFlag

    wifimeshconfig map

    __repacd_echo "Starting Multi-AP Lite Mode"
    config_get dev_reset_default MAPConfig 'MapDevResetDefault' '0'
    config_get deviceMode MAPConfig 'MapDeviceMode'
    config_get num_vlan_supported MAPConfig 'NumberOfVLANSupported' '0'
    config_get map_version MAPConfig 'MapVersionEnabled'
    config_get num_vlan_supported MAPConfig 'NumberOfVLANSupported' '0'
    config_get map_primary_nw MAPConfig 'VlanNetworkPrimary' ''
    config_get map_backhaul_nw MAPConfig 'VlanNetworkBackHaul' ''
    config_get map_single_r1r2_bh MAPConfig 'CombinedR1R2Backhaul'
    config_get map_country MAPConfig 'MapCountry'
    config_get bh_type MAPConfig 'MapBackaulType'
    config_get sta_device MAPConfig 'MapSTARadio'
    config_get my_key MAPConfig 'DPPKey'
    config_get my_bootstrap MAPConfig 'DPPMyBootstrap'
    config_get onboarding_type MAPConfig 'OnboardingType'
    config_get_bool ezmeshFlag repacd 'Ezmesh' '0'
    config_get deviceMode MAPConfig 'MapDeviceMode'

    map_dpp_enabled=0
    map_ts_enabled=1
    unicast_flood_disable=1

    __repacd_echo "dev_reset_default: $dev_reset_default"
    __repacd_echo "MAP device Mode: $deviceMode"
    __repacd_echo "MAP Version: $map_version"
    __repacd_echo "Num VLAN Supported: $num_vlan_supported"
    __repacd_echo "DPP Enabled: $map_dpp_enabled"
    __repacd_echo "Backhaul Type: $bh_type"
    __repacd_echo "sta_device: $sta_device"
    __repacd_echo "My Bootstrap : $my_bootstrap"
    __repacd_echo "Onboarding Type : $onboarding_type"
    __repacd_echo "Ezmesh enabled : $ezmeshFlag"
    __repacd_echo "Board name : $board_name"

    # In SDX boards, as there is no network file to fetch and
    # add the guest bridge, adding this globally defined guest_bridge
    # Note : This is the guest bridge hardcoded in sigma-dut as well
    case "$board_name" in
        ap-sdx*)
            if [ "$map_version" -ge 2 ]; then
                brctl addbr $guest_bridge
                ifconfig $guest_bridge up
            fi
        ;;
    esac

    if [ "$onboarding_type" == "dpp" ]; then
        map_dpp_enabled=1
    fi

    if [ "$ezmeshFlag" -eq 1 ]; then
        if  [ "$deviceMode" = "CAP" ]; then
            uci_set $MAP config 'Enable' 1
        fi
        /etc/init.d/hyd stop
        /etc/init.d/hyd disable
    fi

    # Set country to global on all devices
    config_load wireless
    config_foreach __repacd_config_set_device_country wifi-device $map_country

    # Enable Radio
    config_load wireless
    config_foreach __repacd_config_set_radio_enable wifi-device
    __repacd_echo "Num radio $map_num_radio"

    # Disable vaps of Radio that is marked to be skipped and update mld for 11be iface
    config_load wireless
    config_foreach __repacd_config_update_radio_vaps wifi-iface

    if [ "$dev_reset_default" -eq 1 ]; then
        __repacd_map_reset_default_bridge_config

        if [ "$map_version" -ge 3 ]; then
            kill $repacdPID

            rm /tmp/hostapd.log
            rm /tmp/supplicant.log
            rm /tmp/map_bootstrap_info.tmp
            rm /tmp/map_peer_bootstrap_info.tmp
            rm /tmp/map_key_info.tmp
            rm /tmp/map_bh_info.tmp
            rm /tmp/map_fh_info.tmp
            rm /tmp/ptk.txt
            rm /tmp/pmk.txt
            rm /tmp/map_dpp_bsta_agent_info.tmp
            rm /tmp/hyd.log
            rm /tmp/mapConfig.log
            rm /tmp/ezmesh.log
            rm /tmp/map_sta_info.tmp
            rm /etc/map_peer_bootstrap_info.tmp
            rm /tmp/wsplcd-lan.log

            # Create /tmp/ptk.txt and /tmp/pmk.txt only for WFA mode
            # when mapVersion > 3 and DPP is enabled on dev reset.
            # Hostapd will write to these files only if this file
            # exists.
            if [ "$map_dpp_enabled" -eq 1 -o "$map_version" -ge 6 ]; then
                touch /tmp/ptk.txt
                touch /tmp/pmk.txt
            fi

           for i in 0 1 2 3 4 5; do
               if [ "$i" -lt "$map_num_radio" ]; then
                   __repacd_config_set_default_maplite_vap_param $i
                   __repacd_config_set_default_maplite_radio_param $i
               fi
           done

           # Reset Configs
           uci_set $MAP MAPConfigSettings 'EnableConfigService' 0
           uci_set $MAP MAPConfigSettings 'RoleController' 0
           uci_set $MAP MAPConfigSettings 'RoleAgent' 0
           uci_set $MAP MAPConfigSettings 'MapOnboardingType' 'legacy'
        fi

        config_load wsplcd
        config_load $MAP
        config_load mcsd

        # Common config to CAP and Agent
        uci_set $MAP MultiAP 'EnableSigmaDUT' 1
        uci_set $MAP MultiAP 'MapPFCompliant' 1
        uci_set $MAP MultiAP 'MapVersion' $map_version
        # Set Forwarding Mode to SINGLE to disable HA, HD
        uci_set $MAP hy 'ForwardingMode' 'SINGLE'

        # Logs
        uci_set $MAP config ezmeshlog '1'
        uci_set $MAP MAPConfigSettings 'MapConfigDumpToFile' '1'

        # WSPLCD
        uci_set wsplcd config 'ConfigSta' 0
        uci_set wsplcd config 'DeepClone' 0
        uci_set wsplcd config 'DebugLevel' "DUMP"
        uci_set wsplcd config 'WriteDebugLogToFile' "APPEND"
        uci_set wsplcd config 'LogFileSize' 10000
        uci_set wsplcd config 'MapMaxBss' 4
        uci_set wsplcd config 'ManageVAPInd' 0
        uci_set wsplcd config 'MapPFCompliant' 1
        uci_set wsplcd config 'MapEnable' $map_version
        uci_set wsplcd config 'ConfigApplyTimeout' 1

        uci_set mcsd config 'Enable' 0

        if [ "$map_version" -ge 2 ]; then
            config_load 'repacd'
            uci_set repacd MAPConfig 'EnableMapTSLogs' '1'
            uci_set repacd MAPConfig 'MapTrafficSeparationEnable' '1'
            uci_set repacd repacd 'EnableMixedBackhaul' '1'
            uci_set repacd MAPConfig 'TSUseSameBridgeMAC' '0'

            uci_set $MAP MultiAP 'NumberOfVLANSupported' $num_vlan_supported
            uci_set $MAP MultiAP 'Map2TrafficSepEnabled' 1
            uci_set $MAP MultiAP 'CombinedR1R2Backhaul' $map_single_r1r2_bh
            uci_set $MAP MultiAP 'VlanNetworkPrimary' $map_primary_nw
            uci_set wsplcd config 'NumberOfVLANSupported' $num_vlan_supported
            uci_set wsplcd config 'Map2TrafficSepEnabled' 1
            uci_set wsplcd config 'CombinedR1R2Backhaul' $map_single_r1r2_bh
        fi

        if [ "$map_version" -ge 3 ]; then
            uci_set $MAP MAPSPSettings 'EnableSP' '1'
            uci_set $MAP MAPSPSettings 'EnableEnhancedSP' '1'
            uci_set $MAP MAPSPSettings 'MaxSPRules' '10'
            uci_set $MAP hy 'breakPadEnabled' '1'

            #ecm is disabled for now for lite mode of operation
            /etc/init.d/qca-nss-ecm stop
            sysctl -w net.bridge.bridge-nf-call-iptables=1
            ssdk_sh fdb learnctrl set disable
            ssdk_sh fdb entry flush 1
            sleep 1

            if [ "$map_version" -ge 5 ]; then
                /etc/init.d/qca-nss-ecm restart
                sleep 2
                echo 1 > /sys/kernel/debug/ecm/ecm_classifier_mscs/multi_ap_enabled
                echo 0 > /sys/kernel/debug/ecm/ecm_classifier_dscp/enabled
                sleep 2
            fi

            #Service prioritzation enabled
            echo 1 > /proc/sys/net/emesh-sp/enable
        fi

        if [ "$map_version" -ge 4 ]; then
            uci_set $MAP MultiAP 'EnableSpatialReuse' '1'
        fi

        if [ "$map_version" -ge 6 ]; then
            uci_set repacd repacd 'Ezmesh6GEnable' '1'
            uci_set $MAP MultiAP 'Map6GEnabled' 1
            if  [ "$deviceMode" != "CAP" ]; then
                uci_set repacd MAPConfig 'EnableMLO' '1'
                uci_set wsplcd config 'EnableMLO' '1'
                uci_set repacd MAPConfig 'bstaLinkToRemove' 'default'
                uci_set repacd MAPConfig 'bstaLinkToAdd' 'default'
                uci_set wsplcd config 'MapR1R2MixNoSupport' 1
                uci set repacd.MAPWiFiLink.RSSINumMeasurements=1
                uci_set repacd MAPConfig 'useSpecBasedDefinitions' 1
                uci_set wsplcd config 'useSpecBasedDefinitions' 1
                uci_set wsplcd config 'earlyAPCapabilitySupported' 1
                uci_set wsplcd config 'maxNumAPLinks' 2
                if [ "$deviceMode" = "agt1" ]; then
                    uci set repacd.MAPConfig.mld_mac_addr="10:2A:F5:22:64:31"
                elif [ "$deviceMode" = "DUT" ]; then
                    uci set repacd.MAPConfig.mld_mac_addr="10:2A:F5:22:64:41"
                elif [ "$deviceMode" = "agt2" ]; then
                    uci set repacd.MAPConfig.mld_mac_addr="10:2A:F5:22:64:51"
                fi

                # create a bsta mld by default
                config_load wireless
                config_foreach __repacd_wifimon_get_mld_matching_role wifi-mld "Non-AP" sta_mld

                configured_mld_count=0
                config_foreach __repacd_wifimon_get_configured_mld_count wifi-mld
                __repacd_echo "Available MLD section: $configured_mld_count"
                # find existing configured MLD count
                if [ "$configured_mld_count" -ge 0 ]; then
                    mldIndex=$configured_mld_count
                fi

                if [ -z "$sta_mld" ]; then
                    # Create New STA MLD section next to existing MLD
                    __repacd_echo "Create New STA MLD section"
                    if __repacd_create_mld_section "sta_mld_default" "$base_mld_mac_addr" "Non-AP"; then
                        sta_mld="mld$((mldIndex-1))"
                    else
                        __repacd_echo "Error: Unable to create STA MLD"
                    fi
                fi
                __repacd_echo "Resolved STA MLD: [$sta_mld]"
                __repacd_echo "Next available MLD Index: $mldIndex"

                config_foreach __repacd_config_update_mld_section_maplite wifi-iface

                # Reset mldIndex to start searching from first MLD
                mldIndex=0

                # Generate mld section based on MLDConfig parameter
                __repacd_generate_mld_section
            else
                uci_set repacd MAPConfig 'useSpecBasedDefinitions' 1
                uci_set wsplcd config 'useSpecBasedDefinitions' 1
                uci_set repacd MAPConfig 'EnableMLO' '1'
                uci_set wsplcd config 'EnableMLO' '1'
                uci_set wsplcd config 'earlyAPCapabilitySupported' 1
                config_load wireless
                config_foreach __repacd_config_update_mld_section_maplite wifi-iface
                # Reset mldIndex to start searching from first MLD
                mldIndex=0

                # Generate mld section based on MLDConfig parameter
                __repacd_generate_mld_section
            fi

            if [ "$deviceMode" = "agt2" ]; then
                if ! grep -q re_ul_resp=0x3 /ini/internal/QCN9224_i.ini ; then
                    sed -i '5i\re_ul_resp=0x3' /ini/internal/QCN9224_i.ini
                    sed -i '6d' /ini/internal/QCN9224_i.ini
                    wifi unload
                    sleep 2
                fi
            else
                if grep -q re_ul_resp=0x3 /ini/internal/QCN9224_i.ini ; then
                    sed -i '/re_ul_resp=0x3/d' /ini/internal/QCN9224_i.ini
                    wifi unload
                    sleep 2
                fi
            fi
        fi

        if  [ "$deviceMode" = "CAP" ]; then
            # Set HYD Parameters
            uci_set $MAP config 'Mode' "HYROUTER"
            uci_set $MAP MultiAP 'EnableController' 1
            # Set WSPLCD Parameters
            uci_set wsplcd config 'RunMode' "REGISTRAR"
        else
            # Set HYD Parameters
            uci_set $MAP config 'Mode' "HYCLIENT"
            uci_set $MAP MultiAP 'EnableAgent' 1
            # Set WSPLCD Parameters
            uci_set wsplcd config 'RunMode' "ENROLLEE"
            uci_set wsplcd config 'Map2EnableMboOcePmf' '1'
            # keeping channel scan interval to R2 only as we do not need
            # it in R3
            if [ "$map_version" -eq 2 ]; then
                uci_set $MAP MultiAP 'ChanScanIntervalMin' 1
                uci_set $MAP MultiAP 'EnableBootOnlyScan' 0
            fi
        fi

        uci_commit $MAP
        uci_commit wsplcd
        uci_commit mcsd

        /etc/init.d/mcsd stop
        /etc/init.d/mcsd disable

        __repacd_map_set_bridge_mac
        /etc/init.d/hyfi-bridging start

        # Enable this code only when want to enable hostapd and supplicant logs
        # for MAP certification
        #hostapdPID=$(ps | grep hostapd/global | grep -v grep | awk '{print$1}')
        #suppPID=$(ps | grep wpa_supplicantglobal | grep -v grep | awk '{print$1}')
        #kill $hostapdPID
        #kill $suppPID
        #if [ -e "/var/run/wpa_supplicantglobal" ]
        #then
        #    rm /var/run/wpa_supplicantglobal &> /dev/null
        #fi
        #sleep 2
        #hostapd -g /var/run/hostapd/global -P /var/run/hostapd-global.pid \
        #        -ddddK -f /tmp/hostapd.log &
        #wpa_supplicant -g /var/run/wpa_supplicantglobal -ddddK -f /tmp/supplicant.log &

        config_load mesh_app
        config_get_bool enableDE config 'EnableDataElements' '0'
        if [ "$enableDE" -eq 1 ]; then
            whc_network_restart
        else
            /etc/init.d/network restart
            sleep 5
            # When setting vdev ID check using wifi unload and load
            # Do not remove the wifi unload and load
            if [ "$map_version" -ge 2 -a "$map_dpp_enabled" -eq 0 ]; then
                uci set wireless.qcawifi=qcawifi
                uci set wireless.qcawifi.dp_tx_allow_per_pkt_vdev_id_check=1
                if [ "$map_version" -ge 6 ]; then
                    uci set wireless.qcawifi.ema_ap_support_wps_in_6ghz=1
                    uci set wireless.qcawifi.ema_ap_vendor_ie_size_low=0x55556666
                    uci set wireless.qcawifi.ema_ap_vendor_ie_size_high=0x55555555
                    uci set wireless.qcawifi.ema_ap_beacon_common_part_size=600
                fi
                uci commit
                wifi unload
                wifi load
            fi
        fi
        sleep 2

        if  [ "$map_version" -ge 3 -a "$deviceMode" != "CAP" ]; then
            return
        fi
    fi

    if [ "$ezmeshFlag" -eq 1 ]; then
        if  [ "$deviceMode" != "CAP" ]; then
            uci_set $MAP config 'Enable' 1
        fi
    fi

    # Restart EZMESH Modules
    if [ "$map_version" -ge 1 -a "$dev_reset_default" -eq 0 ]; then
        if [ "$bh_type" = "eth" ]; then
            /etc/init.d/$MAP restart
            if [ "$map_dpp_enabled" -eq 0 ]; then
                /etc/init.d/wsplcd restart
            fi
        else
            # STA created . Do Wifi
            wifi unload
            wifi load
        fi
    fi

    if [ "$map_version" -ge 3 -a "$map_dpp_enabled" -eq 1 ]; then
        uci_set $MAP MultiAP 'Enable1905Security' 1
        uci_set $MAP MAPConfigSettings 'EnableConfigService' 1
        uci_set $MAP MAPConfigSettings 'EnableDPPReconfig' 1
        uci_set $MAP MAPConfigSettings 'MapConfigDumpToFile' 1
        uci_set $MAP MAPConfigSettings 'DebugLevel' 1
        uci_set $MAP MAPConfigSettings 'MapEProfile' 'ALSpecific'
        uci_set $MAP MAPConfigSettings 'MapOnboardingType' 'dpp'
        uci_commit $MAP

        # Disable wsplcd for DPP
        /etc/init.d/wsplcd stop
        /etc/init.d/wsplcd disable

        # DPP Setting for CAP . dev reset = 2 means Setting Peer Bootstrap
        if  [ "$dev_reset_default" -eq 2 -a "$deviceMode" = "CAP" ]; then
            # Get Key
            hostapd_cli -i ath0 -p /var/run/hostapd-wifi0 dpp_configurator_remove 1
            hostapd_cli -i ath0 -p /var/run/hostapd-wifi0 dpp_configurator_add curve=P-256
            key=$(hostapd_cli -i ath0 -p /var/run/hostapd-wifi0 dpp_configurator_get_key 1)

            # Get CSIGN JWK
            hostapd_cli -i ath0 -p /var/run/hostapd-wifi0 dpp_map_get_jwk_csign curve=P-256 key=$key

            # Start controller socket on hostapd
            hostapd_cli -i ath0 -p /var/run/hostapd-wifi0 dpp_controller_start tcp_port=7999

            uci_set $MAP MAPConfigSettings 'RoleController' 1
            uci_set $MAP MAPConfigSettings 'RoleAgent' 0
            uci_set $MAP MAPConfigSettings 'DPPConfiguratorKey' $key
            uci_set $MAP MAPConfigSettings 'DPPMyBootstrap' $my_bootstrap
            uci_commit $MAP
            /etc/init.d/$MAP restart
        fi

        # DPP setting for RE
        if [ "$dev_reset_default" -eq 0 -a "$deviceMode" != "CAP" ]; then
            if [ "$bh_type" = "wifi" ]; then
                config_load wireless
                config_foreach __repacd_delete_managed_maplite_vaps wifi-iface sta

                uci add wireless wifi-iface
                uci set wireless.@wifi-iface[$map_num_radio].device=$sta_device
                uci set wireless.@wifi-iface[$map_num_radio].network=lan
                uci set wireless.@wifi-iface[$map_num_radio].mode=sta
                uci set wireless.@wifi-iface[$map_num_radio].ssid=maplite
                uci set wireless.@wifi-iface[$map_num_radio].encryption='none'
                uci set wireless.@wifi-iface[$map_num_radio].dpp=1
                uci set wireless.@wifi-iface[$map_num_radio].wds=1
                uci set wireless.@wifi-iface[$map_num_radio].map=3
                uci set wireless.@wifi-iface[$map_num_radio].MapBSSType=128
                uci set wireless.@wifi-iface[$map_num_radio].dpp_key=$my_key
                uci set wireless.@wifi-iface[$map_num_radio].dpp_map=1
                uci commit wireless
                wifi load
            fi

            if [ "$bh_type" = "eth" ]; then
                /etc/init.d/hyfi-bridging start
                /etc/init.d/hyfi-bridging restart
                /etc/init.d/$MAP restart

                # Start controller socket on hostapd
                hostapd_cli -i ath0 -p /var/run/hostapd-wifi0 dpp_controller_start tcp_port=7999
                hostapd_cli -i ath0 -p /var/run/hostapd-wifi0 dpp_bootstrap_gen type=qrcode \
                            key=$my_key
                hostapd_cli -i ath0 -p /var/run/hostapd-wifi0 dpp_bootstrap_info 1
                my_bootstrap=$(hostapd_cli -i ath0 -p /var/run/hostapd-wifi0 dpp_bootstrap_get_uri 1)
            fi

            rm /tmp/map_bootstrap_info.tmp
            echo $my_bootstrap > /tmp/map_bootstrap_info.tmp
            uci_set $MAP MAPConfigSettings 'RoleController' 0
            uci_set $MAP MAPConfigSettings 'RoleAgent' 1
            uci_set $MAP MAPConfigSettings 'DPPConfiguratorKey' $my_key
            uci_set $MAP MAPConfigSettings 'DPPMyBootstrap' $my_bootstrap
            uci_commit $MAP
            /etc/init.d/$MAP restart
        fi
    fi

    if  [ "$deviceMode" != "CAP" ]; then
        kill $repacdPID

        # Start the script that monitors the link state.
        #
        # When in NonCAP mode, it will keep checking whether there is a link
        # to the gateway over ethernet. When in CAP mode, it will keep
        # checking the WAN/LAN ifaces.
        __repacd_echo "Starting MAP Lite RE Daemon"
        start-stop-daemon -S -x /usr/sbin/repacd-run.sh -b -- \
                          "maplite" "init" "maplite" "maplite" "maplite"

    fi
}

# Reset the count of the number of times the 5 GHz bSTA was attempted.
# This should only be invoked on boot and when switching between roles
# (eg. to CAP).
__repacd_map_reset_5g_attempts() {
    uci_set repacd 'MAPWiFiLink' '5gAttemptsCount' 0
    uci_set repacd 'MAPWiFiLink' 'MLOAttemptsCount' 0
    uci_commit repacd
}

# Force a restart into CAP mode using the Multi-AP SIG Topoology Optimization
# algorithm.
#
# @see restart_in_cap_mode
__restart_in_cap_mode_map() {
    __repacd_echo "__restart_in_cap_mode_map"
    __stop_repacd_run

    # Reset the counter here in case we switch back into NonCAP (aka. RE)
    # mode. It is easier to do it here than on the NonCAP restart since
    # the latter is also used to force a bSTA change.
    __repacd_map_reset_5g_attempts

    local ether_gwcon=1 start_role='CAP' autoconf=1
    __start_map $ether_gwcon $start_role $autoconf
}

# Force a restart in NonCAP mode using the Multi-AP SIG Topology Optimization
# algorithm.
#
# @see restart_in_noncap_mode
__restart_in_noncap_mode_map() {
    __repacd_echo "__restart_in_noncap_mode_map"
    __stop_repacd_run

    local ether_gwcon=0 start_role='NonCAP' autoconf=1
    __start_map $ether_gwcon $start_role $autoconf
}

# Force a restart into Range Extender (RE) mode with the Multi-AP SIG Topology
# Optimization algorithm.
#
# @see restart_in_re_mode
__restart_in_re_mode_map() {
    __repacd_echo "__restart_in_re_mode_map"
    __stop_repacd_run

    local ether_gwcon=0 start_role='RE' autoconf=1
    __start_map $ether_gwcon $start_role $autoconf
}

__repacd_update_vap_parameters() {
    local fronthaul_ssid fronthaul_key backhaul_ssid backhaul_key backhaul_authtype
    local backhaul_suffix smartmonitor_ssid
    local fh_vap_count vlan_id nw_name num_vlan vlan_nw_postfix
    local map_r1_sta_assoc_disallow map_r2_sta_assoc_disallow
    local vap_bh_bss_type
    local ad_vap_index
    local enable_mlo fronthaul_mld backhaul_mld dot1x_support
    # Default values
    vlan_id=0 map_r1_sta_assoc_disallow=0 map_r2_sta_assoc_disallow=0
    vap_bh_bss_type=$((map_r1_sta_assoc_disallow|map_r2_sta_assoc_disallow))
    fh_vap_count=1 nw_name="$primarybridgename" is_controller=1

    config_load repacd
    config_get fronthaul_ssid MAPConfig 'FronthaulSSID' ''
    config_get fronthaul_key MAPConfig 'FronthaulKey' ''
    config_get fronthaul_authtype MAPConfig 'FronthaulAuthType' 0
    config_get backhaul_ssid MAPConfig 'BackhaulSSID' ''
    config_get backhaul_key MAPConfig 'BackhaulKey' ''
    config_get backhaul_suffix MAPConfig 'BackhaulSuffix' ''
    config_get backhaul_authtype MAPConfig 'BackhaulAuthType' 0

    config_get smartmonitor_ssid MAPConfig 'SmartMonitorSSID' 'ap_smart_monitor'
    config_get map_ts_enabled MAPConfig 'MapTrafficSeparationEnable' '0'
    config_get num_vlan_supported MAPConfig 'NumberOfVLANSupported' '0'

    # Config required for EasyMesh Rev2 Traffic Separation
    config_get map_version MAPConfig 'MapVersionEnabled' '1'
    config_get num_vlan_supported MAPConfig 'NumberOfVLANSupported' '0'
    config_get map_ts_enabled MAPConfig 'MapTrafficSeparationEnable' '0'
    config_get map_primary_nw MAPConfig 'VlanNetworkPrimary' '0'
    config_get map_backhaul_nw MAPConfig 'VlanNetworkBackHaul' '0'
    config_get enable_anqp_interworking MAPConfig 'EnableANQPInterworking' '0'

    # Config required to handle MLO params
    config_get enable_mlo MAPConfig 'EnableMLO' 0
    config_get fronthaul_mld MAPConfig 'FronthaulMld' ''
    config_get backhaul_mld MAPConfig 'BackhaulMld' ''

    # Config Required for DPP
    config_get onboarding_type MAPConfig 'OnboardingType' ''
    config_get dpp_gas_comeback_delay MAPConfig 'DPPGasComebackDelay' ''

    # Config Required for 8021x support
    config_get dot1x_support MAPConfig  'Enable8021x' '0'

    if [ "$onboarding_type" == "dpp" ]; then
        map_dpp_enabled=1
    fi
    if [ "$map_version" -gt 1 ]; then
        config_get map_r1_sta_assoc_disallow MAPConfig 'R1AgentAssocDisAllowed' '0'
        config_get map_r2_sta_assoc_disallow MAPConfig 'R2AgentAssocDisAllowed' '0'
        vap_bh_bss_type=$(($((map_r1_sta_assoc_disallow<<3))|$((map_r2_sta_assoc_disallow<<2))))
        if [ "$map_ts_enabled" -gt 0 ]; then
            config_get map_primary_vlan MAPConfig 'VlanIDNwPrimary' '0'
            nw_name=$map_primary_nw
        fi
    fi

    local DEVICES=
    __repacd_get_devices DEVICES

    for device in $DEVICES; do
        local no_vlan_val
        if [ "$map_version" -gt 1 -a "$map_ts_enabled" -gt 0 ]; then
            config_get no_vlan_val $device no_vlan '0'
            if [ "$no_vlan_val" -eq 0 ]; then
                uci_set wireless $device no_vlan '1'
            fi
        fi
    done

    config_load wireless
    config_foreach __repacd_update_vap_info wifi-iface "$fronthaul_ssid" "$fronthaul_key" \
                             "$backhaul_ssid" "$backhaul_key" "$smartmonitor_ssid" \
                             "$fh_vap_count" "$vap_bh_bss_type" "$map_primary_vlan" "$nw_name" \
                             "$fronthaul_authtype" "$backhaul_authtype" "$vlan_nw_postfix" \
                             "$is_controller" "$map_version" "$map_ts_enabled" \
                             "$num_vlan_supported" "$map_primary_vlan" "$map_primary_nw" \
                             "$map_backhaul_nw" "$enable_anqp_interworking" "$enable_mlo" "$fronthaul_mld" \
                             "$backhaul_mld" "$map_dpp_enabled" "$dpp_gas_comeback_delay" "$dot1x_support"
    uci_commit wireless


}

__repacd_update_vap_info() {
    local config="$1"
    local name=$1
    local iface network disabled device
    local fronthaul_ssid=$2
    local fronthaul_key=$3
    local backhaul_ssid=$4
    local backhaul_key=$5
    local smartmonitor_ssid=$6
    local fh_vap_count=$7
    local vap_bh_bss_type=$8
    local vlan_id=$9
    local nw_name=${10}
    local fronthaul_authtype=${11}
    local backhaul_authtype=${12}
    local vlan_nw_postfix=${13}
    local is_controller=${14}
    local map_version=${15}
    local map_ts_enabled=${16}
    local num_vlan_supported=${17}
    local map_primary_vlan=${18}
    local map_primary_nw=${19}
    local map_backhaul_nw=${20}
    local enable_anqp_interworking=${21}
    local enable_mlo=${22}
    local fronthaul_mld=${23}
    local backhaul_mld=${24}
    local map_dpp_enabled=${25}
    local dpp_gas_comeback_delay=${26}
    local dot1x_support=${27}

    local fronthaul_encryption backhaul_encryption
    local smartmonitor_encryption='none'
    local enable_smart_monitor_mode
    local bss_attribute device_6GL
    local override_enabled override_value device_5GL band overrideSSID=''
    local currentband useOnlyOverride mld_set

    local skip_vap fh_skip_vap fhNwOne_skip_vap fhNwTwo_skip_vap fhNwThree_skip_vap
    local additional1_skip_vap additional2_skip_vap additional3_skip_vap
    local vap_skip vap_hidden
    local hidden_vap fh_hidden fhNwOne_hidden fhNwTwo_hidden fhNwThree_hidden
    local additional1_hidden additional2_hidden additional3_hidden

    local auth_primary_server auth_primary_port auth_primary_secret rsn_preauth_val
    local auth_secondary_server auth_secondary_port auth_secondary_secret rsn_preauth_interfaces_val

    # Whether the same VAP is used for fronthaul and backhaul or not.
    local shared_vaps=0
    local is_controller=0
    if __repacd_gw_mode || [ "$gwcon_mode" = 'Controller' ]; then
        # WAN group not empty; this device will act as controller regardless of
        # the GatewayConnectedMode setting
        is_controller=1
    fi

    config_load repacd
    config_get override_enabled MAPConfig OverrideVapConfigEnable '0'
    config_get device_5GL MAPConfig Radio5GL '0'
    config_get device_6GL MAPConfig Radio6GL '0'
    config_get fh_skip_vap MAPConfig 'Fronthaul_skipvap' '0'
    config_get fhNwOne_skip_vap MAPConfig 'FronthaulNwOne_skipvap' '0'
    config_get fhNwTwo_skip_vap MAPConfig 'FronthaulNwTwo_skipvap' '0'
    config_get fhNwThree_skip_vap MAPConfig 'FronthaulNwThree_skipvap' '0'
    config_get additional1_skip_vap MAPConfig AdditionalFH1_skipvap '0'
    config_get additional2_skip_vap MAPConfig AdditionalFH2_skipvap '0'
    config_get additional3_skip_vap MAPConfig AdditionalFH3_skipvap '0'
    config_get fh_hidden MAPConfig 'Fronthaul_hidden' '0'
    config_get fhNwOne_hidden MAPConfig 'FronthaulNwOne_hidden' '0'
    config_get fhNwTwo_hidden MAPConfig 'FronthaulNwTwo_hidden' '0'
    config_get fhNwThree_hidden MAPConfig 'FronthaulNwThree_hidden' '0'
    config_get additional1_hidden MAPConfig AdditionalFH1_hidden '0'
    config_get additional2_hidden MAPConfig AdditionalFH2_hidden '0'
    config_get additional3_hidden MAPConfig AdditionalFH3_hidden '0'
    config_get managed_network repacd ManagedNetwork '$primarybridgename'
    config_get dummy_vaps_enable MAPConfig "EnableCloningOptimization" 0
    skip_vap=""
    hidden_vap=""

    if [ "$fronthaul_ssid" = "$backhaul_ssid" ]; then
        shared_vaps=1
    fi

    if [ -n "$fronthaul_key" ]; then
        fronthaul_encryption='psk2+ccmp'
        [ "$fronthaul_authtype" -eq 12 ] && fronthaul_encryption='ccmp'
        [ "$fronthaul_authtype" -eq 13 ] && fronthaul_encryption='gcmp-256'
        [ "$fronthaul_authtype" -eq 14 ] && fronthaul_encryption='ccmp'
        [ "$fronthaul_authtype" -eq 15 ] && fronthaul_encryption='psk2+gcmp-256'
        [ "$fronthaul_authtype" -eq 16 ] && fronthaul_encryption='ccmp+gcmp-256'
        [ "$fronthaul_authtype" -eq 17 ] && fronthaul_encryption='psk2+ccmp+gcmp-256'
    else
        fronthaul_encryption='none'
    fi
    if [ "$dot1x_support" -eq 1 ]; then
        local fh_dot1x_encryption fh_dot1x_encryption fh_dot1x_encryption
        __repacd_get_dot1x_details $fronthaul_authtype fh_dot1x_encryption \
            bss_auth_type bss_encr_type
        if [ -n "$fh_dot1x_encryption" ]; then
            fronthaul_encryption="$fh_dot1x_encryption"
            fronthaul_key=''
        fi
    fi

    if [ -n "$backhaul_key" ]; then
        backhaul_encryption='psk2+ccmp'
        #Enable SAE only mode for pure bakchaul BSS
        if [ "$map_version" -gt 1 ] && [ "$is_controller" -gt 0 -o "$dummy_vaps_enable" -eq 1 ] && \
              [ "$shared_vaps" -eq 0 ]; then
            if [ "$backhaul_authtype" -eq 12 ]; then
                backhaul_encryption='ccmp'
            elif [ "$backhaul_authtype" -eq 17 ]; then
                backhaul_encryption='psk2+ccmp+gcmp-256'
            elif [ "$backhaul_authtype" -eq 15 ]; then
                backhaul_encryption='psk2+gcmp-256'
            fi
        fi
    else
        backhaul_encryption='none'
    fi

    config_get iface "$config" ifname
    config_get device "$config" device
    config_get disabled "$config" disabled '0'
    config_get mode "$config" mode
    config_get MapBSSType "$config" MapBSSType '0'
    config_get network "$config" network
    config_get vap_skip "$config" map_SkipVapRestart '0'
    config_get vap_hidden "$config" hidden '0'
    config_load 'repacd'
    config_get_bool enable_smart_monitor_mode MAPConfig 'EnableSmartMonitorMode' '0'
    config_load $MAP
    config_get disable_steering "$config" 'SteeringDisabled' '0'
    config_get enable_wps_on_backhaul MAPConfig 'EnableBackhaulWPS' '0'
    config_get mld_set "$config" 'mld' ''
    config_get_bool create_ctrl_fbss "$device" repacd_create_ctrl_fbss '1'

    local fronthaul_ssid fronthaul_key fronthaul_authtype fronthaul_encryption additional_fh_vaps
    local ad_vap_index ad_vap device_5GL override_enabled device_6GL

    config_get override_enabled MAPConfig OverrideVapConfigEnable
    config_get device_5GL MAPConfig Radio5GL 0
    config_get device_6GL MAPConfig Radio6GL 0
    config_get currentband $device band '0'

    if [ "$currentband" -eq "3" -a "$kernel_version" -eq "6" ]; then
        backhaul_encryption="ccmp"
        fronthaul_encryption="ccmp"
        if [ "$backhaul_authtype" -eq 17 ]; then
            backhaul_encryption='psk2+ccmp+gcmp-256'
        elif [ "$backhaul_authtype" -eq 15 ]; then
            backhaul_encryption='psk2+gcmp-256'
        fi
        if [ "$fronthaul_authtype" -eq 13 ]; then
            fronthaul_encryption='gcmp-256'
        elif [ "$fronthaul_authtype" -eq 14 ]; then
            fronthaul_encryption='ccmp'
        elif [ "$fronthaul_authtype" -eq 15 ]; then
            fronthaul_encryption='psk2+gcmp-256'
        elif [ "$fronthaul_authtype" -eq 16 ]; then
            fronthaul_encryption='ccmp+gcmp-256'
        elif [ "$fronthaul_authtype" -eq 17 ]; then
            fronthaul_encryption='psk2+ccmp+gcmp-256'
        fi
    fi

    if whc_is_5g_radio "$device"; then
        if [ "$currentband" -eq 3 ]; then
            if [ $device != $device_6GL ]; then
                band="6G"
            else
                band="6GL"
            fi
        elif [ $device != $device_5GL ]; then
            band="5G"
        else
            band="5GL"
        fi
    elif ! whc_is_5g_radio "$device"; then
        band="2G"
    fi

    config_get overrideSSID MAPConfig FHOverrideSSID"$vlan_nw_postfix""$band"
    config_get useOnlyOverride MAPConfig ForceOverrideCredential 0

    if [ -n "$iface" -a "$disabled" -eq 0 -a "$mode" = "ap" ]; then
        if [ $((MapBSSType & 0x20)) -eq 32 ]; then
            if [ "$disable_steering" -eq 0 ]; then
                if [ "$network" != "$primarybridgename" ]; then
                    for nwVar in One Two Three; do
                        config_get temp_network MAPConfig "VlanNetwork"$nwVar '0'
                        if [ $temp_network == $network  ]; then
                            i=$nwVar
                            break
                        fi
                    done

                    vlan_nw_postfix="Nw"$i
                    if [ "$i" == "One" ]; then
                        skip_vap=$fhNwOne_skip_vap
                        hidden_vap=$fhNwOne_hidden
                    elif [ "$i" == "Two" ];then
                        skip_vap=$fhNwTwo_skip_vap
                        hidden_vap=$fhNwTwo_hidden
                    elif [ "$i" == "Three" ];then
                        skip_vap=$fhNwThree_skip_vap
                        hidden_vap=$fhNwThree_hidden
                    fi
                    config_get fronthaul_ssid MAPConfig "FronthaulSSID"$vlan_nw_postfix ''
                    config_get fronthaul_key MAPConfig "FronthaulKey"$vlan_nw_postfix ''
                    config_get fronthaul_authtype MAPConfig "FronthaulAuthType"$vlan_nw_postfix 0
                    config_get vlan_id MAPConfig "VlanID"$vlan_nw_postfix '0'
                    config_get nw_name MAPConfig "VlanNetwork"$i ''


                    if [ "$dot1x_support" -eq 1 ]; then
                        config_get auth_primary_server MAPConfig "AuthPrimaryServer"$vlan_nw_postfix ''
                        config_get auth_primary_port MAPConfig "AuthPrimaryPort"$vlan_nw_postfix ''
                        config_get auth_primary_secret MAPConfig "AuthPrimarySecret"$vlan_nw_postfix ''
                        config_get auth_secondary_server MAPConfig "AuthSecondaryServer"$vlan_nw_postfix ''
                        config_get auth_secondary_port MAPConfig "AuthSecondaryPort"$vlan_nw_postfix ''
                        config_get auth_secondary_secret MAPConfig "AuthSecondarySecret"$vlan_nw_postfix ''
                        config_get rsn_preauth_val MAPConfig "RSN_PreAuth"$vlan_nw_postfix ''
                        config_get rsn_preauth_interfaces_val MAPConfig "RSN_PreAuth_Interfaces"$vlan_nw_postfix ''

                        if [ -z "$auth_primary_server" -o -z "$auth_primary_port" -o -z "$auth_primary_secret" -o \
                                -z "$auth_secondary_server" -o -z "$auth_secondary_port" -o -z "$auth_secondary_secret" \
                                -o -z "$rsn_preauth_val" -o -z "$rsn_preauth_interfaces_val" ]; then
                            uci delete wireless."$name".auth_server
                            uci delete wireless."$name".auth_port
                            uci delete wireless."$name".auth_secret
                            uci delete wireless."$name".auth_server2
                            uci delete wireless."$name".auth_port2
                            uci delete wireless."$name".auth_secret2
                            uci delete wireless."$name".rsn_preauth
                            uci delete wireless."$name".rsn_preauth_interfaces
                        else
                            __repacd_update_vap_param "$name" 'auth_server' "$auth_primary_server"
                            __repacd_update_vap_param "$name" 'auth_port' "$auth_primary_port"
                            __repacd_update_vap_param "$name" 'auth_secret' "$auth_primary_secret"
                            __repacd_update_vap_param "$name" 'auth_server2' "$auth_secondary_server"
                            __repacd_update_vap_param "$name" 'auth_port2' "$auth_secondary_port"
                            __repacd_update_vap_param "$name" 'auth_secret2' "$auth_secondary_secret"
                            __repacd_update_vap_param "$name" 'rsn_preauth' "$rsn_preauth_val"
                            __repacd_update_vap_param "$name" 'rsn_preauth_interfaces' "$rsn_preauth_interfaces_val"
                        fi
                    fi

                    if [ -z "$fronthaul_ssid" ]; then
                       local ssid_suffix
                       __repacd_generate_ssid_suffix ssid_suffix
                       fronthaul_ssid="mapsig-${ssid_suffix}-$i"
                       __repacd_generate_psk fronthaul_key
                    fi
                    if [ -n "$fronthaul_key" ]; then
                        fronthaul_encryption='psk2+ccmp'
                        [ "$fronthaul_authtype" -eq 12 ] && fronthaul_encryption='ccmp'
                        [ "$fronthaul_authtype" -eq 13 ] && fronthaul_encryption='gcmp-256'
                        [ "$fronthaul_authtype" -eq 14 ] && fronthaul_encryption='ccmp'
                        [ "$fronthaul_authtype" -eq 15 ] && fronthaul_encryption='psk2+gcmp-256'
                        [ "$fronthaul_authtype" -eq 16 ] && fronthaul_encryption='ccmp+gcmp-256'
                        [ "$fronthaul_authtype" -eq 17 ] && fronthaul_encryption='psk2+ccmp+gcmp-256'
                    else
                        fronthaul_encryption='none'
                    fi

                    if [ "$dot1x_support" -eq 1 ]; then
                        local fh_dot1x_encryption fh_dot1x_encryption fh_dot1x_encryption
                        __repacd_get_dot1x_details $fronthaul_authtype fh_dot1x_encryption \
                            bss_auth_type bss_encr_type
                        if [ -n "$fh_dot1x_encryption" ]; then
                            fronthaul_encryption="$fh_dot1x_encryption"
                            fronthaul_key=''
                        fi
                    fi

                    __repacd_update_vap_param "$name" 'wps_pbc_enable' '0'
                    __repacd_update_vap_param "$name" 'wps_pbc_start_time' '0'
                    __repacd_update_vap_param "$name" 'wps_pbc_duration' '120'
                fi

                if [ "$override_enabled" -eq 1 ]; then
                    config_get fronthaul_ssid MAPConfig 'FHOverrideSSID'$vlan_nw_postfix$band "$fronthaul_ssid"
                    config_get fronthaul_key MAPConfig 'FHOverrideKey'$vlan_nw_postfix$band "$fronthaul_key"
                    config_get fronthaul_authtype MAPConfig 'FHOverrideAuthType'$vlan_nw_postfix$band $fronthaul_authtype
                    if [ -n "$fronthaul_key" ]; then
                        fronthaul_encryption='psk2+ccmp'
                        [ "$fronthaul_authtype" -eq 12 ] && fronthaul_encryption='ccmp'
                        [ "$fronthaul_authtype" -eq 13 ] && fronthaul_encryption='gcmp-256'
                        [ "$fronthaul_authtype" -eq 14 ] && fronthaul_encryption='ccmp'
                        [ "$fronthaul_authtype" -eq 15 ] && fronthaul_encryption='psk2+gcmp-256'
                        [ "$fronthaul_authtype" -eq 16 ] && fronthaul_encryption='ccmp+gcmp-256'
                        [ "$fronthaul_authtype" -eq 17 ] && fronthaul_encryption='psk2+ccmp+gcmp-256'
                    else
                        fronthaul_encryption='none'
                    fi

                    if [ "$dot1x_support" -eq 1 ]; then
                        local fh_dot1x_encryption fh_dot1x_encryption fh_dot1x_encryption
                        __repacd_get_dot1x_details $fronthaul_authtype fh_dot1x_encryption \
                            bss_auth_type bss_encr_type
                        if [ -n "$fh_dot1x_encryption" ]; then
                            fronthaul_encryption="$fh_dot1x_encryption"
                            fronthaul_key=''
                        fi
                    fi
                fi
                if [ -z skip_vap ];then
                    skip_vap=$fh_skip_vap
                fi
                if [ -z hidden_vap ];then
                    hidden_vap=$fh_hidden
                fi
                __repacd_map_set_vap_param_from_config "$name" "$band" "$skip_vap" \
                                                        "map_SkipVapRestart" "$vap_skip"
                __repacd_map_set_vap_param_from_config "$name" "$band" "$hidden_vap" \
                                                        "hidden" "$vap_hidden"

                if [ -n "$fronthaul_ssid" ] && [ -n $mld_set ]; then
                    uci set wireless.$mld_set.mld_ssid="$fronthaul_ssid"
                fi

                __repacd_update_vap_param "$name" 'ssid' "$fronthaul_ssid"
                __repacd_update_vap_param "$name" 'key' "$fronthaul_key"
                __repacd_update_vap_param "$name" 'encryption' $fronthaul_encryption
                __repacd_update_vap_param "$name" 'map' $map_version
                if [ "$network" != "$primarybridgename" ]; then
                        __repacd_update_vap_param "$name" 'wps_pbc' '0'
                else
                        __repacd_update_vap_param "$name" 'wps_pbc' '1'
                fi
                if [ "$map_version" -gt 1 ]; then
                    if [ $fronthaul_authtype -eq 1 ]; then
                        __repacd_update_vap_param "$name" 'sae' 1
                        [ "$currentband" -eq 3  ] && __repacd_update_vap_param "$name" 'en_6g_sec_comp' 0
#                        uci delete wireless."$name".sae_password
#                        uci delete wireless."$name".sae_groups
                        uci delete wireless."$name".sae_pwe
                            uci delete wireless.$name.wpa_key_mgmt
                            uci delete wireless.$name.add_sha256
                            uci delete wireless.$name.add_sha384
                            uci delete wireless.$name.group_mgmt_cipher
                            uci del_list wireless."$name".sae_groups="19"
                    elif [ "$fronthaul_authtype" -eq 12 -o "$fronthaul_authtype" -eq 13 -o \
                           "$fronthaul_authtype" -eq 14 -o "$fronthaul_authtype" -eq 15 -o \
                            "$fronthaul_authtype" -eq 16 -o "$fronthaul_authtype" -eq 17 ]; then
                        __repacd_update_vap_param "$name" 'sae' 1
#                        uci delete wireless."$name".sae_password
#                        uci delete wireless."$name".sae_groups
#                        uci add_list wireless."$name".sae_password="$fronthaul_key"
#                        uci add_list wireless."$name".sae_groups="19"
                        if [ "$fronthaul_authtype" -eq 12 ];then
                            uci delete wireless.$name.wpa_key_mgmt
                            uci delete wireless.$name.add_sha256
                            uci delete wireless.$name.add_sha384
                            uci delete wireless.$name.group_mgmt_cipher
                            uci del_list wireless."$name".sae_groups="19"
                        else
                           if [ "$fronthaul_authtype" -eq 15 ] || [ "$fronthaul_authtype" -eq 17 ]; then
                               __repacd_update_vap_param "$name" 'wpa_key_mgmt' "WPA-PSK SAE SAE-EXT-KEY"
                           else
                               __repacd_update_vap_param "$name" 'wpa_key_mgmt' "SAE SAE-EXT-KEY"
                           fi
                            __repacd_update_vap_param "$name" 'add_sha256' "0"
                            __repacd_update_vap_param "$name" 'add_sha384' "0"
                            __repacd_update_vap_param "$name" 'group_mgmt_cipher' "BIP-GMAC-256"
                            __repacd_update_vap_param "$name" 'sae_password' "$key"
                            uci add_list wireless."$name".sae_groups="19"
                        fi
                        if [ "$currentband" -eq 3 ]; then
                            __repacd_update_vap_param "$name" 'sae_pwe' 1
                            if [ "$fronthaul_authtype" -eq 12 -o "$fronthaul_authtype" -eq 14 ]; then
                                __repacd_update_vap_param "$name" 'en_6g_sec_comp' 1
                            else
                                __repacd_update_vap_param "$name" 'sae_pwe' 2
                                __repacd_update_vap_param "$name" 'en_6g_sec_comp' 0
                            fi
                        fi
                    else
                        __repacd_update_vap_param "$name" 'sae' 0
#                        uci delete wireless."$name".sae_password
#                        uci delete wireless."$name".sae_groups
                        uci delete wireless."$name".sae_pwe
                        [ "$currentband" -eq 3  ] && __repacd_update_vap_param "$name" 'en_6g_sec_comp' 0
                    fi
                fi
            else
                config_load $MAP
                config_get ad_vap_index "$config" 'additional_fh_vap_index' '0'
                local bandstr
                if [ "$override_enabled" -eq 1 ]; then
                    if whc_is_5g_radio "$device"; then
                        bandstr='5G'
                        if [ $device_5GL == $device ]; then
                            bandstr='5GL'
                        fi
                    elif ! whc_is_5g_radio "$device"; then
                        bandstr='2G'
                    fi

                    if [ "$currentband" -eq 3 ]; then
                        bandstr='6G'
                        if [ $device_6GL == $device ]; then
                            bandstr='6GL'
                        fi
                    fi
                    config_get fronthaul_ssid MAPConfig "AdditionalFHOverride"${bandstr}"ssid"${ad_vap_index} ''
                    config_get fronthaul_key MAPConfig "AdditionalFHOverride"${bandstr}"key"${ad_vap_index} ''
                    config_get fronthaul_authtype MAPConfig "AdditionalFHOverride"${bandstr}"auth"${ad_vap_index} ''
                else
                    config_get fronthaul_ssid MAPConfig "AdditionalFHssid"${ad_vap_index} ''
                    config_get fronthaul_key MAPConfig "AdditionalFHkey"${ad_vap_index} ''
                    config_get fronthaul_authtype MAPConfig "AdditionalFHauth"${ad_vap_index} ''
                fi

                if [ $ad_vap_index -gt 0 ]; then

                    if [ -z "$fronthaul_ssid" ]; then
                        break;
                    fi
                    if [ "$ad_vap_index" -eq 1 ]; then
                        skip_vap=$additional1_skip_vap
                        hidden_vap=$additional1_hidden
                    elif [ "$ad_vap_index" -eq 2 ]; then
                        skip_vap=$additional2_skip_vap
                        hidden_vap=$additional2_hidden
                    elif [ "$ad_vap_index" -eq 3 ]; then
                        skip_vap=$additional3_skip_vap
                        hidden_vap=$additional3_hidden
                    fi
                    __repacd_map_set_vap_param_from_config "$name" "$band" "$skip_vap" \
                                                        "map_SkipVapRestart" "$vap_skip"
                    __repacd_map_set_vap_param_from_config "$name" "$band" "$hidden_vap" \
                                                        "hidden" "$vap_hidden"

                    if [ -n "$fronthaul_ssid" ] && [ -n $mld_set ]; then
                        uci set wireless.$mld_set.mld_ssid="$fronthaul_ssid"
                    fi

                    if [ -n "$fronthaul_key" ]; then
                        fronthaul_encryption='psk2+ccmp'
                        [ "$fronthaul_authtype" -eq 12 ] && fronthaul_encryption='ccmp'
                        [ "$fronthaul_authtype" -eq 13 ] && fronthaul_encryption='gcmp-256'
                        [ "$fronthaul_authtype" -eq 14 ] && fronthaul_encryption='ccmp'
                        [ "$fronthaul_authtype" -eq 15 ] && fronthaul_encryption='psk2+gcmp-256'
                        [ "$fronthaul_authtype" -eq 16 ] && fronthaul_encryption='ccmp+gcmp-256'
                        [ "$fronthaul_authtype" -eq 17 ] && fronthaul_encryption='psk2+ccmp+gcmp-256'
                    else
                        fronthaul_encryption='none'
                    fi

                    if [ "$dot1x_support" -eq 1 ]; then
                        local fh_dot1x_encryption fh_dot1x_encryption fh_dot1x_encryption
                        __repacd_get_dot1x_details $fronthaul_authtype fh_dot1x_encryption \
                            bss_auth_type bss_encr_type
                        if [ -n "$fh_dot1x_encryption" ]; then
                            fronthaul_encryption="$fh_dot1x_encryption"
                            fronthaul_key=''
                        fi
                    fi
                    __repacd_update_vap_param "$name" 'ssid' "$fronthaul_ssid"
                    __repacd_update_vap_param "$name" 'key' "$fronthaul_key"
                    __repacd_update_vap_param "$name" 'encryption' $fronthaul_encryption
                    __repacd_update_vap_param "$name" 'wps_pbc' '0'
                    if [ "$map_version" -ge 3 -a "$map_dpp_enabled" -eq 1 ]; then
                        __repacd_update_vap_param "$name" 'dpp_map' 1
                        __repacd_update_vap_param "$name" 'dpp_configurator_connectivity' 1
                        __repacd_update_vap_param "$name" 'gas_comeback_delay' $dpp_gas_comeback_delay
                    fi
                    if [ "$map_version" -gt 1 ]; then
                        if [ $fronthaul_authtype -eq 1 ]; then
                            __repacd_update_vap_param "$name" 'sae' 1
                            [ "$currentband" -eq 3  ] && __repacd_update_vap_param "$name" 'en_6g_sec_comp' 0
#                            uci delete wireless."$name".sae_password
#                            uci delete wireless."$name".sae_groups
                            uci delete wireless."$name".sae_pwe
                            uci delete wireless.$name.wpa_key_mgmt
                            uci delete wireless.$name.add_sha256
                            uci delete wireless.$name.add_sha384
                            uci delete wireless.$name.group_mgmt_cipher
                            uci del_list wireless."$name".sae_groups="19"
                        elif [ "$fronthaul_authtype" -eq 12 -o "$fronthaul_authtype" -eq 13 -o \
                               "$fronthaul_authtype" -eq 14 -o "$fronthaul_authtype" -eq 15 -o \
                               "$fronthaul_authtype" -eq 16 -o "$fronthaul_authtype" -eq 17 ]; then
                            __repacd_update_vap_param "$name" 'sae' 1
#                            uci delete wireless."$name".sae_password
#                            uci delete wireless."$name".sae_groups
#                            uci add_list wireless."$name".sae_password="$fronthaul_key"
#                            uci add_list wireless."$name".sae_groups="19"
                            if [ "$fronthaul_authtype" -eq 12 ];then
                                uci delete wireless.$name.wpa_key_mgmt
                                uci delete wireless.$name.add_sha256
                                uci delete wireless.$name.add_sha384
                                uci delete wireless.$name.group_mgmt_cipher
                                uci del_list wireless."$name".sae_groups="19"
                            else
                                if [ "$fronthaul_authtype" -eq 15 ] || [ "$fronthaul_authtype" -eq 17 ]; then
                                    __repacd_update_vap_param "$name" 'wpa_key_mgmt' "WPA-PSK SAE SAE-EXT-KEY"
                                else
                                    __repacd_update_vap_param "$name" 'wpa_key_mgmt' "SAE SAE-EXT-KEY"
                                fi
                                __repacd_update_vap_param "$name" 'add_sha256' "0"
                                __repacd_update_vap_param "$name" 'add_sha384' "0"
                                __repacd_update_vap_param "$name" 'group_mgmt_cipher' "BIP-GMAC-256"
                                __repacd_update_vap_param "$name" 'sae_password' "$key"
                                uci add_list wireless."$name".sae_groups="19"
                            fi
                            if [ "$currentband" -eq 3 ]; then
                                __repacd_update_vap_param "$name" 'sae_pwe' 1
                                if [ "$fronthaul_authtype" -eq 12 -o "$fronthaul_authtype" -eq 14 ]; then
                                    __repacd_update_vap_param "$name" 'en_6g_sec_comp' 1
                                fi
                            else
                                __repacd_update_vap_param "$name" 'sae_pwe' 2
                                __repacd_update_vap_param "$name" 'en_6g_sec_comp' 0
                            fi
                        else
                            __repacd_update_vap_param "$name" 'sae' 0
#                            uci delete wireless."$name".sae_password
#                            uci delete wireless."$name".sae_groups
                            uci delete wireless."$name".sae_pwe
                           [ "$currentband" -eq 3  ] && __repacd_update_vap_param "$name" 'en_6g_sec_comp' 0
                        fi
                    fi
                fi
            fi
            if [ "$enable_mlo" -eq 1 -a -n "$fronthaul_mld" -a -n "$mld_set" ]; then
                if __repacd_is_mld_vap "$mld_set" "$band"; then
                    __repacd_echo "Attaching fronthaul vap to mld $mld_set"
                    __repacd_update_vap_param "$name" 'mld' $mld_set
                fi
            fi
        elif [ $((MapBSSType & 0x40)) -eq 64 ] || [ $((MapBSSType & 0x44)) -eq 68 ] || [ $((MapBSSType & 0x48)) -eq 72 ]; then

             if [ -n "$backhaul_ssid" ] && [ -n $mld_set ]; then
                uci set wireless.$mld_set.mld_ssid="$backhaul_ssid"
             fi
             __repacd_update_vap_param "$name" 'ssid' "$backhaul_ssid"
             __repacd_update_vap_param "$name" 'key' $backhaul_key
             __repacd_update_vap_param "$name" 'encryption' $backhaul_encryption
            if [ "$enable_wps_on_backhaul" -eq 1 -a "$create_ctrl_fbss" -eq 0 ]; then
                __repacd_update_vap_param "$name" 'wps_pbc' '1'
            else
                __repacd_update_vap_param "$name" 'wps_pbc' '0'
            fi
            __repacd_update_vap_param "$name" 'map' $map_version
            if [ "$map_version" -gt 1 ]; then
        if [ $backhaul_authtype -eq 1 ]; then
            __repacd_update_vap_param "$name" 'sae' 1
            [ "$currentband" -eq 3  ] && __repacd_update_vap_param "$name" 'en_6g_sec_comp' 0
            #uci delete wireless."$name".sae_password
            #uci delete wireless."$name".sae_groups
            uci delete wireless."$name".sae_pwe
            #uci add_list wireless."$name".sae_password="$backhaul_key"
            #uci add_list wireless."$name".sae_groups="19"
            uci delete wireless.$name.wpa_key_mgmt
            uci delete wireless.$name.add_sha256
            uci delete wireless.$name.add_sha384
            uci delete wireless.$name.group_mgmt_cipher
            uci del_list wireless."$name".sae_groups="19"
        elif [ "$backhaul_authtype" -eq 12 ]; then
            __repacd_update_vap_param "$name" 'sae' 1
            #uci delete wireless."$name".sae_password
            #uci delete wireless."$name".sae_groups
            #uci add_list wireless."$name".sae_password="$backhaul_key"
            #uci add_list wireless."$name".sae_groups="19"
            if [ "$currentband" -eq 3 ]; then
                __repacd_update_vap_param "$name" 'sae_pwe' 1
            else
                __repacd_update_vap_param "$name" 'sae_pwe' 2
            fi
            uci delete wireless.$name.wpa_key_mgmt
            uci delete wireless.$name.add_sha256
            uci delete wireless.$name.add_sha384
            uci delete wireless.$name.group_mgmt_cipher
            uci del_list wireless."$name".sae_groups="19"
            [ "$currentband" -eq 3  ] && __repacd_update_vap_param "$name" 'en_6g_sec_comp' 1
        elif [ "$backhaul_authtype" -eq 15 -o "$backhaul_authtype" -eq 17 ]; then
            __repacd_update_vap_param "$name" 'sae' 1
            __repacd_update_vap_param "$name" 'wpa_key_mgmt' "SAE SAE-EXT-KEY"
            __repacd_update_vap_param "$name" 'add_sha256' "0"
            __repacd_update_vap_param "$name" 'add_sha384' "0"
            __repacd_update_vap_param "$name" 'group_mgmt_cipher' "BIP-GMAC-256"
            __repacd_update_vap_param "$name" 'sae_password' "$backhaul_key"
            uci add_list wireless."$name".sae_groups="19"
        else
            __repacd_update_vap_param "$name" 'sae' 0
            uci delete wireless."$name".sae_password
            uci delete wireless."$name".sae_groups
            uci delete wireless."$name".sae_pwe
            [ "$currentband" -eq 3  ] && __repacd_update_vap_param "$name" 'en_6g_sec_comp' 0
                fi
            fi
            if [ "$enable_mlo" -eq 1 -a -n "$backhaul_mld" -a -n "$mld_set" ]; then
                if __repacd_is_mld_vap "$backhaul_mld" "$band"; then
                    __repacd_echo "Attaching backhaul vap to mld $backhaul_mld"
                    __repacd_update_vap_param "$name" 'mld' $backhaul_mld
                fi
            fi
        fi
        if [ "$is_controller" -gt 0 ]; then
            __repacd_update_vap_param "$name" 'root_distance' '0'
        fi
        if [ "$map_version" -ge 3 -a "$map_dpp_enabled" -eq 1 ]; then
            __repacd_update_vap_param "$name" 'dpp_map' 1
            __repacd_update_vap_param "$name" 'dpp_configurator_connectivity' 1
            __repacd_update_vap_param "$name" 'gas_comeback_delay' $dpp_gas_comeback_delay
        fi
        if [ "$enable_anqp_interworking" -eq 1 ]; then
            # Set Interworking and ANQP
            uci_set wireless "$config" interworking 1
            uci del_list wireless."$name".anqp_elem="$anqp_ele"
            uci add_list wireless."$name".anqp_elem="$anqp_ele"
        fi

        # In case of SDX, fetch mapVlanID from bridge name as per convention i.e., bridgeX will have vlan id X
        if [ "$map_version" -gt 1 -a "$map_ts_enabled" -gt 0 -a "$is_controller" -gt 0 -a "$disable_steering" -eq 0 ]; then
            case "$board_name" in
                ap-sdx*)
                    vlanIdfromBridge="$(echo "$network" | awk '{split($0,a,"bridge"); print a[2]}')"
                    if [ "$vlanIdfromBridge" -eq "0"  ] || [ "$network" -eq "backhaul"  ]; then
                        __repacd_update_vap_param "$name" 'mapVlanID' $map_primary_vlan
                    else
                        __repacd_update_vap_param "$name" 'mapVlanID' $vlanIdfromBridge
                    fi
                    ;;
            esac
            __repacd_update_vap_param "$name" 'map8021qvlan' $map_primary_vlan
        fi
        __repacd_update_vap_param "$name" 'network' $network

    elif [ -n "$iface" -a "$disabled" -eq 0 -a "$mode" = "ap_smart_monitor" ]; then
        if [ "$enable_smart_monitor_mode" -eq 1 ] && [ "$fh_vap_count" -eq 1 ]; then
            __repacd_update_vap_param "$name" 'ssid' $smartmonitor_ssid
            __repacd_update_vap_param "$name" 'encryption' $smartmonitor_encryption
            __repacd_update_vap_param "$name" 'neighbourfilter' 1
            __repacd_update_vap_param "$name" 'set_monrxfilter' 1
            __repacd_update_vap_param "$name" 'disable' 0
            __repacd_update_vap_param "$name" 'wsplcd_unmanaged' 1
            __repacd_update_vap_param "$name" 'repacd_security_unmanaged' 1
            __repacd_update_vap_param "$name" 'wps_pbc' 0
        fi
    fi
}

# Create default vaps
__repacd_coloc_create_vap() {
    local is_controller=$1
    local fronthaul_ssid fronthaul_key backhaul_ssid backhaul_key
    local backhaul_suffix smartmonitor_ssid fh_skip_vap fh_hidden
    local nw_name num_vlan vlan_nw_postfix vlan_id=0 fh_vap_count=1
    local map_r1_sta_assoc_disallow=0 map_r2_sta_assoc_disallow=0
    local ad_vap_index fronthaul_mld backhaul_mld vap_bh_bss_type=0

    # Default configurations
    brprefix='br-'
    primarybridgename='lan'
    nw_name="$primarybridgename"

    config_load repacd
    config_get fronthaul_ssid MAPConfig 'FronthaulSSID' ''
    config_get fronthaul_key MAPConfig 'FronthaulKey' ''
    config_get fronthaul_authtype MAPConfig 'FronthaulAuthType' 0
    config_get backhaul_ssid MAPConfig 'BackhaulSSID' ''
    config_get backhaul_key MAPConfig 'BackhaulKey' ''
    config_get backhaul_suffix MAPConfig 'BackhaulSuffix' ''
    config_get backhaul_authtype MAPConfig 'BackhaulAuthType' 0
    config_get fh_skip_vap MAPConfig 'Fronthaul_skipvap' '0'
    config_get fh_hidden MAPConfig 'Fronthaul_hidden' '0'
    config_get smartmonitor_ssid MAPConfig 'SmartMonitorSSID' 'ap_smart_monitor'
    config_get managed_network repacd ManagedNetwork '$primarybridgename'

    if [ "$map_version" -gt 1 ]; then
        config_get map_r1_sta_assoc_disallow MAPConfig 'R1AgentAssocDisAllowed' '0'
        config_get map_r2_sta_assoc_disallow MAPConfig 'R2AgentAssocDisAllowed' '0'
        vap_bh_bss_type=$(($((map_r1_sta_assoc_disallow<<3))|$((map_r2_sta_assoc_disallow<<2))))
        if [ "$map_ts_enabled" -gt 0 ]; then
            config_get map_primary_vlan MAPConfig 'VlanIDNwPrimary' '0'
            nw_name=$map_primary_nw
        fi
    fi

    config_load wireless
    config_foreach __repacd_delete_managed_vaps wifi-iface $managed_network
    uci_commit wireless

    # Clear out the environment to avoid tha vaps enable and disable unnecessarily
    unset $(set | grep '^CONFIG_' | awk -F'=' '{print $1}')

    # Reload to reset our variable state after the deletion
    config_load wireless

    if [ -z "$fronthaul_ssid" ]; then
        local ssid_suffix
        __repacd_generate_ssid_suffix ssid_suffix
        fronthaul_ssid="mapsig-${ssid_suffix}"
        __repacd_generate_psk fronthaul_key
    fi

    if [ -z "$backhaul_ssid" ]; then
        backhaul_ssid="${fronthaul_ssid}${backhaul_suffix}"

        if [ "$fronthaul_ssid" = "$backhaul_ssid" ]; then
            backhaul_key=$fronthaul_key
        else
            __repacd_generate_psk backhaul_key
        fi
    fi
    __repacd_echo "create initail vap"
    __repacd_echo "fronthaul_ssid: $fronthaul_ssid backhaul_ssid: $backhaul_ssid"
    __repacd_echo "smartmonitor_ssid: $smartmonitor_ssid"
    __repacd_echo "map_primary_vlan: $map_primary_vlan nw_name: $nw_name"

    # Create Primary/Private VAP in each radio - with regular or overried credential
    __repacd_create_vaps_map "$is_controller" "$fronthaul_ssid" "$fronthaul_key" \
                             "$backhaul_ssid" "$backhaul_key" "$smartmonitor_ssid" \
                             "$fh_vap_count" "$vap_bh_bss_type" "$map_primary_vlan" "$nw_name" \
                             "$fronthaul_authtype" "$backhaul_authtype" "$vlan_nw_postfix" \
                             "$fronthaul_mld" "$backhaul_mld" "$fh_skip_vap" "$fh_hidden"
}

# Create required bridges for traffic seperation in R2
create_default_bridges() {

    local is_controller=$1

    # Config required for EasyMesh R2 Traffic Separation
    config_load 'repacd'
    config_get map_version MAPConfig 'MapVersionEnabled' '1'
    config_get num_vlan_supported MAPConfig 'NumberOfVLANSupported' '0'
    config_get map_ts_enabled MAPConfig 'MapTrafficSeparationEnable' '0'
    config_get map_enable_backhaul_WPS MAPConfig 'EnableBackhaulWPS' '0'
    config_get map_primary_nw MAPConfig 'VlanNetworkPrimary' ''
    config_get map_backhaul_nw MAPConfig 'VlanNetworkBackHaul' ''

    __repacd_echo "Creating $num_vlan_supported secondary bridges for R2 Traffic Separation"
    __repacd_echo "map_version: $map_version map_ts_enabled: $map_ts_enabled"
    __repacd_echo "num_vlan_supported: $num_vlan_supported"

    if [ "$map_version" -ge 2 ]; then
        # Stop instance of repacd that is already running
        # To Remove and add all bridges except primary bridge
        __stop_repacd_run
        __repacd_map_reset_default_bridge_config $is_controller
    fi

}

# Update virtual interface in ezmesh config
# And add it in loopback interface
__repacd_coloc_update_wlan_virtual_ifaces(){

    local config="$1"
    local iface network mode disabled

    config_get iface "$config" ifname
    config_get network "$config" network
    config_get mode "$config" mode
    config_get ssid "$config" ssid
    config_get disabled "$config" disabled '0'
    config_get mld "$config" mld

    [ -n "$mld" ] && iface="$mld"

    if [ "$ssid" = "$virtualSSID" ]; then
        uci set ezmesh.config.DualAlidIntf=$iface
        uci commit ezmesh
        hyctl loopbackif br-$network $iface
    fi
}

# Create virtual vap for ext_ctrl_qca_ agent
__repacd_coloc_add_virtual_vap() {

    config_load 'ezmesh'
    config_get_bool dualalid config 'DualAlid' '0'
    config_load 'repacd'
    config_get_bool enable_virtual_ap  MAPConfig 'EnableVirtualAP' '0'
    config_get virtualSSID MAPConfig 'VirtualSSID' 'VirtualAP'
    config_get virtual  MAPConfig 'VirtualDevice' 'wifi0'
    config_get enable_single_netdev MAPConfig 'EnableSingleNetdev' 0

    __repacd_echo "Create virtual vap on device $virtual"
    local currentband;
    config_get currentband $virtual band '0'

    if [ "$enable_virtual_ap" -eq 1 ] && [ "$dualalid" -eq 1 ]; then
        name=$(uci add wireless wifi-iface)
        config_changed=1
        __repacd_init_vap "$name" $virtual 'ap' "$virtualSSID" \
                           "$smartmonitor_encryption" 'none' '0' "$enable_single_netdev"
        __repacd_update_vap_param "$name" 'disable' 0
        __repacd_update_vap_param "$name" 'hidden' 1
        __repacd_update_vap_param "$name" 'map' "$map_version"
        __repacd_update_vap_param "$name" 'wsplcd_unmanaged' 1
        __repacd_update_vap_param "$name" 'repacd_security_unmanaged' 1
        __repacd_update_vap_param "$name" 'wps_pbc' 0
        __repacd_update_vap_param "$name" 'virtualAP' 1
        __repacd_update_vap_param "$name" 'network' "lan"
        if [ "$currentband" -eq 3  ]; then
            __repacd_update_vap_param "$name" 'sae' 1
            __repacd_update_vap_param "$name" 'en_6g_sec_comp' 0
        fi
    fi
    __repacd_echo "Added virtual vap device in wireless config"
    uci_commit wireless
    wifi

    /etc/init.d/hyfi-bridging start
    # Update Virtual vap in ezmesh config
    if [ "$dualalid" -eq 1 ] && [ "$enable_virtual_ap" -eq 1 ];then
        config_load wireless
        config_foreach __repacd_coloc_update_wlan_virtual_ifaces wifi-iface
    fi

    __repacd_echo "restarting hyfi bridge and ezmesh"
    /etc/init.d/ezmesh restart

}

# Wait until vaps are up.
# By using bit rate it wil check
__repacd_coloc_check_bitrate() {

    local config="$1"
    config_get iface "$config" ifname
    config_get MapBSSType "$config" MapBSSType '0'

    if [ $((MapBSSType & 0x10)) -ne 16 ]; then
        while true
        do
            local bitRate=$(repacdcli $iface get_bitrate)
            local roundOffBitRate=${bitRate%.*}

            if [ "$roundOffBitRate" -ne 0 ] || [ -n "$apBitRate" ]; then
                break
            fi
            sleep 2
        done
    fi
}

# Add vlan ID for all ethernet port
__repacd_coloc_add_all_vlan_ethernet() {

    local num_vlan=$1

    __repacd_echo "add virtual lan for ethernet"
    # Create new bridge based on num Vlan configured by user
    config_load network
    for i in Primary One Two Three; do
        config_get nw_name MAPConfig "VlanNetwork"$i '0'
        config_get vlan_id MAPConfig "VlanIDNw"$i '0'

        if [ "$num_vlan" -eq 0 ]; then
            break
        fi

        if [ "$vlan_id" -eq 0 ]; then
            return
        fi

        # Configure ethernet interface with VLAN for secondary networks
        if [ "$i" != "Primary" ]; then
            __repacd_map_add_vlan_ethernet $nw_name $vlan_id
        fi

        num_vlan=$((num_vlan-1))
    done
}

# Create required defualt vaps for mesh
create_default_vap() {

   __repacd_coloc_create_vap $1
   __repacd_coloc_add_virtual_vap
   wifi
}

# Create virtual lan and add it to respective bridge
create_vlan() {
. /lib/functions/repacd-vlanmon-map.sh
. /lib/functions/repacd-wifimon-map.sh

    config_load network
    config_load 'ezmesh'
    config_get_bool dualalid config 'DualAlid' '0'

    config_load 'repacd'
    config_get_bool wifi_restart MAPConfig 'restartWifi' '0'
    config_get map_version MAPConfig 'MapVersionEnabled' '1'
    config_get map_ts_enabled MAPConfig 'MapTrafficSeparationEnable' '0'
    config_get map_primary_nw MAPConfig 'VlanNetworkPrimary' ''
    config_get_bool virtual MAPConfig 'EnableVirtualAP' '0'
    config_get virtualssid MAPConfig 'VirtualSSID' 'VirtualAP'
    config_get virtualdevice MAPConfig 'VirtualDevice' 'wifi0'
    config_get num_vlan_supported MAPConfig 'NumberOfVLANSupported' '0'
    config_get enable_fastcloning MAPConfig 'EnableCloningOptimization' '0'

    uci set wsplcd.config.Map2TrafficSepEnabled='1'
    uci set repacd.MAPConfig.MapTrafficSeparationActive='1'
    uci set repacd.MAPConfig.OnboardingDone='1'
    uci set wsplcd.config.NumberOfVLANSupported=$num_vlan_supported
    uci commit repacd
    uci commit wsplcd

    if [ "$wifi_restart" -ne 0 ]; then
        __repacd_echo "restarting wifi restartWifi: $wifi_restart"
        uci set repacd.MAPConfig.MapFastOnboarding='1'
        uci commit repacd
        [ "$enable_fastcloning" -eq 0 ] && repacd_wifimon_check_onboarding
        [ "$enable_fastcloning" -eq 1 ] && repacd_wifimon_check_onboarding_OPT
        uci set repacd.MAPConfig.MapFastOnboarding='0'
        uci commit repacd
    fi

    # Need to wait until vaps to be up
    __repacd_echo "Waiting for vaps to get ready"
    config_load wireless
    config_foreach __repacd_coloc_check_bitrate wifi-iface

    # Apply vlan ID and add it to respective bridges
    __repacd_echo "Apply vlan ID for wireless"
    repacd_map_vlanmon_init
    repacd_map_vlanmon_check

    # Add vlan ID for all eth ports
    if [ "$map_version" -ge 2 ]; then
        uci set repacd.MAPConfig.MapTrafficSeparationActive='1'
        uci set repacd.MAPConfig.MapTrafficSeparationEnable='1'
        config_get num_vlan_supported MAPConfig 'NumberOfVLANSupported' '0'
        uci set ezmesh.MultiAP.NumberOfVLANSupported=$num_vlan_supported
        uci set ezmesh.MultiAP.Map2TrafficSepEnabled='1'
        uci commit repacd
        uci commit ezmesh
        __repacd_echo "Adding vlan ID for all eth ports"
        __repacd_coloc_add_all_vlan_ethernet $num_vlan_supported
    fi

    # Need to restart wsplcd, hyfi-bridging and ezmesh
    __repacd_echo "Restarting wsplcd, hyfi-bridging and ezmesh"
    /etc/init.d/wsplcd restart
    /etc/init.d/hyfi-bridging restart
    /etc/init.d/ezmesh restart
}
