#!/bin/sh
# Copyright (c) 2023 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# RDK helper script to configure the VAP's with credentials received
# from M2 message.

#set -x

DNS_CONF="/var/dnsmasq.conf"
reset_dnsmasq=0
reset_firewall=0
bridge_count=0
MapVersion=`syscfg get ezmesh.MultiAP.MapVersion`
TSEnabled=`syscfg get ezmesh.MultiAP.Map2TrafficSepEnabled`
if [ "$MapVersion" == "2" ] && [ "$TSEnabled" == "1" ]; then
     backhaulssid=`syscfg show | grep -i MapBSSType=72 | cut -d '.' -f 2`
     vap_primary_vlan=`syscfg get wireless.$backhaulssid.map8021qvlan`
else
     backhaulssid=`syscfg show | grep -i MapBSSType=64 | cut -d '.' -f 2`
fi


##########################
# MAPR2 helper functions #
##########################

query_firewall() {
        is_rule_exist=`iptables -L INPUT -n --line-number | grep -i ez | awk '{print $1}'`
        if [ -z "$is_rule_exist" ]; then
                reset_firewall=1
        fi
}

#Flush Firewall Rules for Secondary Bridges if Exist - $1=SecondaryBridgeName $2=SecondaryBridgeIP
_flush_firewall() {
        INPUT_rule_num=`iptables -L INPUT -n --line-number | grep -i ez | awk '{print $1}' | sed -n '1!G;h;$p'`
        rule_num=`echo ${INPUT_rule_num/// }`
        for i in $rule_num; do
                `iptables -D INPUT $i`
        done

        FORWARD_rule_num=`iptables -L FORWARD -n --line-number | grep -i ez | awk '{print $1}' | sed -n '1!G;h;$p'`
        rule_num=`echo ${FORWARD_rule_num/// }`
        for i in $rule_num; do
                `iptables -D FORWARD $i`
        done
}

_flush_firewall_default_rules () {
        if [[ -n "$1" ]] && [[ -n "$2" ]]; then
                #Flushing INPUT firewall rules
                _flush_firewall
        fi
}

_query_ip() {
        for brlan_ifno in `seq 1 2`; do
                bridge_ip=`syscfg get network.lan.brlan$brlan_ifno.ipaddr`
                echo "Flushing existing firewall rules: __flush_firewall_default_rules brlan$brlan_ifno $bridge_ip" >> /tmp/rdk_helper_logs
                if [ -n $bridge_ip ]; then
                        _flush_firewall_default_rules brlan$brlan_ifno $bridge_ip
                fi
        done
}


query_dnsmasq_conf() {
        grep -i $1 $DNS_CONF
        if [ $? -eq 0 ]; then
                reset_dnsmasq=0
        else
                reset_dnsmasq=1
        fi
}

dnsmasq_dhcp_set=1
#Applying the Firewall rules for secondary bridges - $1=brlan interface
_config_firewall_apply_rules_to_bridge () {
        bridge=$1
        bridge_ip=`syscfg get network.lan.$bridge.ipaddr`
        iptables -A INPUT -d $bridge_ip/32 -i $bridge -m comment --comment "EZMESH TRAFFIC" -j ACCEPT
        iptables -A INPUT -i $bridge -m pkttype ! --pkt-type unicast -m comment --comment "EZMESH TRAFFIC" -j ACCEPT

        iptables -D FORWARD -m comment --comment "EZMESH TRAFFIC" -j xlog_drop_wan2lan
        iptables -A FORWARD -i $bridge -o erouter0 -m comment --comment "EZMESH TRAFFIC" -j ACCEPT
        iptables -A FORWARD -i erouter0 -o $bridge -m comment --comment "EZMESH TRAFFIC" -j ACCEPT
        iptables -A FORWARD -i brlan0 -o $bridge -m comment --comment "EZMESH TRAFFIC" -j DROP
        iptables -A FORWARD -i $bridge -o brlan0 -m comment --comment "EZMESH TRAFFIC" -j DROP
        iptables -A FORWARD -i $bridge -o $bridge -m comment --comment "EZMESH TRAFFIC" -j ACCEPT
        iptables -A FORWARD -m comment --comment "EZMESH TRAFFIC" -j xlog_drop_wan2lan
}


_set_bridge() {
        if [ "$TSEnabled" == "1" ] && [ "ath$3" == "$backhaulssid" ]; then
                syscfg set wireless.ath$3.network backhaul
                if [ $vap_primary_vlan == "0" ]; then
                        grep -i "bridge=" $2
                        if [ $? -eq 0 ]; then
                                sed -i s/^bridge=.*/bridge=$1/ $2
                        else
                                echo "bridge=$1" >> $2
                        fi
                else
                        sed -i '/bridge=/d' $2
                fi
        else
		if [ "ath$3" == "$backhaulssid" ]; then
			syscfg set wireless.ath$3.network backhaul
		else
			syscfg set wireless.ath$3.network $1
		fi
                grep -i "bridge=" $2
                if [ $? -eq 0 ]; then
                    sed -i s/^bridge=.*/bridge=$1/ $2
                else
                    echo "bridge=$1" >> $2
                fi
        fi
}

_rdk_config_bridge() {
                cur_bridge=$1
                query_dnsmasq_conf $cur_bridge
                query_firewall
                #Retrieving Pool Index
                PoolID=""
                if [ $cur_bridge != $primary_bridge ]; then
                        if [ $cur_bridge == brlan1 ]; then
                                PoolID=2
                        elif [ $cur_bridge == brlan2 ]; then
                                PoolID=3
                        elif [ $cur_bridge == brlan3 ]; then
                                PoolID=4
                        fi
                fi
                #Exporting syscfg confirm
                if [ -n $PoolID ]; then
                        syscfg set ezmesh.DHCP.Enable.PoolID$PoolID TRUE
                else
                        syscfg set ezmesh.DHCP.Enable.PoolID$PoolID FALSE
                fi

                #Assigning IP to Secondary bridges
                if [ $cur_bridge != $primary_bridge ]; then
                        ip_addr=`syscfg get network.lan.$1.ipaddr`
                        echo "ifconfig $cur_bridge $ip_addr up" >> /tmp/rdk_helper_logs
                        ifconfig $cur_bridge $ip_addr up
                fi

                dhcp_startip=`sysevent get dhcp_server_"$PoolID"_startaddr`
                dhcp_endip=`sysevent get dhcp_server_"$PoolID"_endaddr`
                dhcp_subnet=`sysevent get dhcp_server_"$PoolID"_subnet`
                dhcp_lease=`sysevent get dhcp_server_"$PoolID"_leasetime`
                if [ "$reset_dnsmasq" != "1" ]; then
                       if [ "$reset_firewall" != "1" ]; then
                                return
                       else
                               _config_firewall_apply_rules_to_bridge $cur_bridge
                       fi

                       return
                fi

                #Add DNS Masq configuration for secondary Bridges, Not handling the sequence like if the entry is already present
                if [ $cur_bridge != $primary_bridge ]; then
                        if [ [ -z $dhcp_startip ] && [ -z $dhcp_endip ] ]; then
                                echo "dhcp_startip=$dhcp_startip or dhcp_endip=$dhcp_endip is not configured" >> /tmp/rdk_helper_logs
                        else
                                export dnsmasq_dhcp_set=`expr $dnsmasq_dhcp_set + 1`
                                echo "interface=$cur_bridge" >> /var/dnsmasq.conf
                                echo "interface=$cur_bridge" >> /tmp/rdk_helper_logs
                                echo "dhcp-range=set:$dnsmasq_dhcp_set,$dhcp_startip,$dhcp_endip,$dhcp_subnet,$dhcp_lease" >> /var/dnsmasq.conf
                                echo "dhcp-range=set:$dnsmasq_dhcp_set,$dhcp_startip,$dhcp_endip,$dhcp_subnet,$dhcp_lease" >> /tmp/rdk_helper_logs
                        fi
                fi

                #Applying Firewall Rules for additional bridge
                if [ $cur_bridge != $primary_bridge ]; then
                        echo "Applying Firewall Rules: _config_firewall_apply_rules_to_bridge $cur_bridge" >> /tmp/rdk_helper_logs
                        _config_firewall_apply_rules_to_bridge $cur_bridge
                fi

                #Restart dnsmasq service
                if [ $cur_bridge != $primary_bridge ]; then
                        echo "Restarting DNS MASQ for $cur_bridge" >> /tmp/rdk_helper_logs
                        killall dnsmasq
                        dnsmasq -P 4096 -C /var/dnsmasq.conf
                fi
}

#Creating VLAN ID interface for Secondary bridges - #1 = bridge #2 = vlanid
_rdk_add_eth_vlan() {
	local vlan_id
	local Idx
	local numVlan=`syscfg get ezmesh.MultiAP.NumberOfVLANSupported`

	[ -n $numVlan ] && {
	    Idx=1
	    while [ $Idx -lt $numVlan ]; do
		vlan_id=`syscfg get network.lan$Idx.vlanid`
		ethernet=`syscfg get lan_ethernet_physical_ifnames`
		for eth in $ethernet; do
			vlan_api vlan_hal_addInterface brlan$Idx $eth $vlan_id
			hyctl setiftype brlan$Idx $eth.$vlan_id eth
			hyctl setifbcast brlan$Idx $eth.$vlan_id enable
			hyctl setifgrp brlan$Idx $eth.$vlan_id 1 r
		done
		Idx=$(($Idx+1))
	    done
	}
}

_network_config() {
        VLANSupported=`syscfg get ezmesh.MultiAP.NumberOfVLANSupported`
        VLANSupported=`expr $VLANSupported - 1`
        for i in `seq 0 $VLANSupported`; do
                if [ "$i" != "0" ]; then
                        syscfg set network.lan$i.alias brlan$i
                        syscfg set network.lan$i.type bridge
                else
                        syscfg set network.lan.alias brlan0
                        syscfg set network.lan.type bridge
                fi
        done
        primary_bridge=brlan0
        syscfg set network.alias "lan lan1 lan2"
        syscfg commit
}

_is_bridge_exist() {
        for i in `seq 1 3`; do
               if [[ "`syscfg get network.lan$i.ifname`" == *".$vap_profile_vlan"* ]]; then
                     bridge_exist=1
                     bridge_no="$i"
                     return
               else
                     bridge_exist=0
               fi
        done
        return
}

_hyfi_ath_config () {
        local Idx=0

        if [ "ath$interface" == "$backhaulssid" ] && [ $vap_primary_vlan != "0" ]; then
                hyctl setiftype brlan0 $backhaulssid.$vap_primary_vlan $radio
                hyctl setifbcast brlan0 $backhaulssid.$vap_primary_vlan enable
                hyctl setifgrp brlan0 $backhaulssid.$vap_primary_vlan 1 r
                cfg80211tool $backhaulssid vlan_tag 1
        fi

        if [ "$MapVersion" == "2" ] && [ "$TSEnabled" == "1" ]; then
                local numVlan
                numVlan=`syscfg get ezmesh.MultiAP.NumberOfVLANSupported`
                numVlan=`expr $numVlan - 1`
                vlanID=`syscfg get Easy-Mesh-ProfileVlan-$(($interface+1))-Id`

                for i in `seq 0 $numVlan`; do
                     ret=`vlan_api _is_this_interface_available_in_given_linux_bridge $backhaulssid brlan$i $vlanID`
                     ret=`echo $ret | awk -F '' '{print $NF}'`

                     if [ $ret -eq 0 ]; then
                          Idx=$i
                          break;
                     fi
                done
        fi

        hyctl setiftype brlan$Idx ath$interface $radio
        hyctl setifbcast brlan$Idx ath$interface enable
        hyctl setifgrp brlan$Idx ath$interface 1 r
}

_hyfi_eth_config () {
        for i in `syscfg get lan_ethernet_physical_ifnames`; do
            hyctl setiftype brlan0 $i eth
            hyctl setifbcast brlan0 $i enable
            hyctl setifgrp brlan0 $i 1 r
        done
}

network_sync() {
        for i in primary one two three; do
                if [ "$i" == "primary" ]; then
                        bridge=`ls /sys/class/net/brlan0/brif/`
                        if_name=`echo ${bridge/// }`
                        syscfg set network.lan.ifname "$if_name"
                elif [ "$i" == "one" ]; then
                        bridge=`ls /sys/class/net/brlan1/brif/`
                        if_name=`echo ${bridge/// }`
                        syscfg set network.lan1.ifname "$if_name"
                elif [ "$i" == "two" ]; then
                        bridge=`ls /sys/class/net/brlan2/brif/`
                        if_name=`echo ${bridge/// }`
                        syscfg set network.lan2.ifname "$if_name"
                elif [ "$i" == "three" ]; then
                        bridge=`ls /sys/class/net/brlan3/brif/`
                        if_name=`echo ${bridge/// }`
                        syscfg set network.lan3.ifname "$if_name"
                fi
        done
        syscfg commit
}

repacd_get_vap_config() {

        FHSSID=`syscfg get repacd.MAPConfig.FronthaulSSID`
        FHSSID_key=`syscfg get repacd.MAPConfig.FronthaulKey`
        FHSSID_NwOne=`syscfg get repacd.MAPConfig.FronthaulSSIDNwOne`
        FHSSID_NwOne_key=`syscfg get repacd.MAPConfig.FronthaulKeyNwOne`
        FHSSID_NwTwo=`syscfg get repacd.MAPConfig.FronthaulSSIDNwTwo`
        FHSSID_NwTwo_key=`syscfg get repacd.MAPConfig.FronthaulKeyNwTwo`
        BHSSID=`syscfg get repacd.MAPConfig.BackhaulSSID`
        BHSSID_key=`syscfg get repacd.MAPConfig.BackhaulKey`
}

repacd_set_config() {

        for i in Primary One Two ; do
                if [ "$i" == "Primary" ]; then
                        syscfg set repacd.MAPConfig.VlanIDNwPrimary `syscfg get network.lan.vlanid`
                        syscfg set ezmesh.MultiAP.VlanIDNwPrimary `syscfg get network.lan.vlanid`
                elif [ "$i" == "One" ]; then
                        syscfg set repacd.MAPConfig.VlanIDNwOne `syscfg get network.lan1.vlanid`
                        syscfg set ezmesh.MultiAP.VlanIDNwOne `syscfg get network.lan1.vlanid`
                elif [ "$i" == "Two" ]; then
                        syscfg set repacd.MAPConfig.VlanIDNwTwo `syscfg get network.lan2.vlanid`
                        syscfg set ezmesh.MultiAP.VlanIDNwTwo `syscfg get network.lan2.vlanid`
                fi
        done
        syscfg commit
}

############################
# MAPR2 helper functions end
############################

update_wifi_params() {
if [ "$MapBSSType" != "16" ]; then

        wifi_api wifi_setSSIDEnable $interface 1
        wifi_api wifi_setSSIDName $interface $ssid_value

        hidden=`syscfg get wireless.ath$interface.hidden`
        isolate=`syscfg get wireless.ath$interface.isolate`

        if [ "$hidden" == "1" ]; then
                wifi_api wifi_setApSsidAdvertisementEnable $interface 0
        else
                wifi_api wifi_setApSsidAdvertisementEnable $interface 1
        fi

        if [ "$isolate" == "1" ]; then
                wifi_api wifi_setApIsolationEnable $interface 1
        else
                wifi_api wifi_setApIsolationEnable $interface 0
        fi

        #wifi_api wifi_setApSsidAdvertisementEnable $interface 1

        #if [ "$interface" == "4" ] || [ "$interface" == "5" ]; then
        #    wifi_api wifi_setApIsolationEnable $interface 1
        #fi

        wifi_api wifi_setApSecurityModeEnabled $interface $Mode
        wifi_api wifi_setApWpaEncryptionMode $interface $EncryptMethod
        wifi_api wifi_setApSecurityKeyPassphrase $interface $key

        if [ "$MapBSSType" == "64" ] || [ "$pbc_state" == "0" ]; then
                echo "bsstype:$MapBSSType ssid:$SSID false" >> /tmp/rdk_helper_logs
                wifi_api wifi_setApWpsEnable $interface 0
        elif [ "$pbc_state" == "1" ]; then
                echo "bsstype:$MapBSSType ssid:$SSID true" >> /tmp/rdk_helper_logs
                wifi_api wifi_setApWpsEnable $interface 1
                #wifi_api wifi_setApWpsButtonPush $interface
        fi
fi

}



#flush the existing multi-ap configurations to ensure no over-lap or additional configurations are getting appended
_lan_unset() {
        VLANSupported=`syscfg get ezmesh.MultiAP.NumberOfVLANSupported`
        VLANSupported=`expr $VLANSupported - 1`
        for i in `seq 0 $VLANSupported`; do
                if [ "$i" == "0" ]; then
                        syscfg unset network.lan.ifname
                else
                        syscfg unset network.lan$i.ifname
                        syscfg unset network.lan$i.alias
                        syscfg unset network.lan$i.vlanid
                        syscfg unset network.lan$i.type
                fi
        done
}

_flush_hostap_map_stale_configs() {
        sed -i '/multi_ap*/d' /nvram/hostapd*

        for i in `seq 1 3`; do
                vlan_api vlan_hal_delGroup brlan$i
        done
        if [ $vap_primary_vlan != "0" ]; then
                vlan_api vlan_hal_delInterface $primary_bridge $backhaulssid $vap_primary_vlan
        fi
        for iface in /sys/class/net/ath*; do
                iface=`basename $iface`
                echo "$iface" | grep '\.' >/dev/null 2>&1
                if [ "$?" -eq "0" ]; then
                        vconfig rem $iface
                fi
        done

        for iface in /sys/class/net/eth*; do
                iface=`basename $iface`
                echo "$iface" | grep '\.' >/dev/null 2>&1
                if [ "$?" -eq "0" ]; then
                        vconfig rem $iface
                fi
        done

        _lan_unset
        _query_ip
}

script_start () {
        _flush_hostap_map_stale_configs


        #need generic handling
        TOTAL_SSID="1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34"
        echo "LINENO: ${LINENO} - Backhaul SSID = `syscfg get wireless.$backhaulssid.ssid`" >> /tmp/rdk_helper_logs
        echo "LINENO: ${LINENO} - Backhaul Key = `syscfg get wireless.$backhaulssid.key` ">> /tmp/rdk_helper_logs

        _network_config
        # Configuring the VAP's
        for SSID in $TOTAL_SSID
        do
                interface=`expr $SSID - 1`
                ssid_value=`syscfg get wireless.ath$interface.ssid`
                enable_state=`syscfg get wireless.ath$interface.disabled`
                key=`syscfg get wireless.ath$interface.key`
                pbc_state=`syscfg get wireless.ath$interface.wps_pbc`
                MapBSSType=`syscfg get wireless.ath$interface.MapBSSType`
                map_profile=`syscfg get wireless.ath$interface.map`
                encryption=`syscfg get wireless.ath$interface.encryption`
                ifname=`syscfg get wireless.ath$interface.ifname`
                multi_ap_backhaul_ssid=`syscfg get wireless.$backhaulssid.ssid`
                multi_ap_backhaul_wpa_passphrase=`syscfg get wireless.$backhaulssid.key`
                dpp=`syscfg get wireless.ath$interface.dpp`

                if [ "$dpp" == "1" ]; then
                    dpp_map=`syscfg get wireless.ath$interface.dpp_map`
                    dpp_controller=`syscfg get wireless.ath$interface.dpp_controller`
                    gas_comeback_delay=`syscfg get wireless.ath$interface.gas_comeback_delay`
                    if [ "$gas_comeback_delay" == "0" ]; then
                        gas_comeback_delay="3000"
                    fi
                    dpp_configurator_connectivity=`syscfg get wireless.ath$interface.dpp_configurator_connectivity`
                fi
                if [ "$MapVersion" == "2" ]; then
                    vap_primary_vlan=`syscfg get wireless.ath$interface.map8021qvlan`
                    vap_profile_vlan=`syscfg get wireless.ath$interface.mapVlanID`
                    if [ -z "$vap_primary_vlan" ] && [ ! -z "$MapBSSType" ]; then
                        syscfg set wireless.ath$interface.MapBSSType 16
                    fi
                fi

                if [ -z "$MapBSSType" ] || [ "$MapBSSType" == "16" ]; then
                        wifi_api wifi_setSSIDEnable $interface 0
                        continue # since VAP is in teardown state we are not updating the rest of the cred
                else
                        ### adding all interfaces to the primary bridge
                        #_rdk_config_bridge "$primary_bridge" "/nvram/hostapd$interface.conf" "$interface"
                        if [ "$MapVersion" == "2" ]; then
                                _set_bridge "$primary_bridge" "/nvram/hostapd$interface.conf" "$interface"
                        fi
                fi
                ##delete bridge for the interface
                if [ "$MapVersion" == "2" ] && [ "$TSEnabled" == "1" ]; then
                        ##check if primaryvlan and profilevlan are same or not
                        if [ "$vap_primary_vlan" != "$vap_profile_vlan" -a "$vap_profile_vlan" != "0" ]; then

                                if [ "$vap_profile_vlan" == "0" ] || [ "$vap_profile_vlan" == "1" ]; then
                                        unset vap_profile_vlan
                                fi

                                _is_bridge_exist

                                ##to do remove hostpad bridge deletion and create using a funcition
                                ##use grep to check bridge is there inhostapd or not
                                if [ "$bridge_exist" == "1" ]; then
                                        _set_bridge "brlan$bridge_no" "/nvram/hostapd$interface.conf" "$interface"
                                        #_rdk_config_bridge "brlan$bridge_no" "/nvram/hostapd$interface.conf" "$interface"
                                        if [ $vap_primary_vlan != "0" ]; then
                                                vlan_api vlan_hal_addInterface brlan$bridge_no $backhaulssid $vap_profile_vlan
                                        fi
                                else
                                        if [ "$bridge_count" != "3" ]; then
                                                bridge_count=`expr $bridge_count + 1`
                                        fi
                                        vlan_api vlan_hal_addGroup "brlan$bridge_count" 1
                                        _set_bridge "brlan$bridge_count" "/nvram/hostapd$interface.conf" "$interface"
                                        _rdk_config_bridge "brlan$bridge_count" "/nvram/hostapd$interface.conf" "$interface"

                                        if [ $vap_primary_vlan != "0" ]; then
                                                vlan_api vlan_hal_addInterface brlan$bridge_count $backhaulssid $vap_profile_vlan
                                        fi
                                        _rdk_add_eth_vlan "brlan$bridge_count" $vap_profile_vlan

                                        syscfg set network.lan$bridge_count.vlanid $vap_profile_vlan

                                        validGroup=`vlan_api _is_this_group_available_in_linux_bridge brlan$bridge_count`
                                        ret=$(echo $validGroup | awk -F '' '{print $NF}')
                                        [ $ret -eq 0 ] && hyctl attach brlan$bridge_count
                                fi
                        else
                                _set_bridge "$primary_bridge" "/nvram/hostapd$interface.conf" "$interface"
                                #_rdk_config_bridge "$primary_bridge" "/nvram/hostapd$interface.conf" "$interface"
                        fi

                        if  [ ! -e "/sys/class/net/$backhaulssid.$vap_primary_vlan/" ]; then
                                if [ "$vap_primary_vlan" != "0" -a "$vap_primary_vlan" != "1" ]; then
                                        vlan_api vlan_hal_addInterface $primary_bridge $backhaulssid $vap_primary_vlan
                                fi
                                syscfg set network.lan.vlanid $vap_primary_vlan

                        fi
                        network_sync
                        syscfg commit
                fi
                sec=$(echo $encryption | cut -d '+' -f 1)
                # Security Mode
                if [ "$sec" == "none" ]
                then
                        Mode="None"
                elif [ "$sec" == "psk" ]
                then
                        Mode="WPA-Personal"
                elif [ "$sec" == "psk2" ]
                then
                        Mode="WPA2-Personal"
                elif [ "$sec" == "sae" ]
                then
                        Mode="WPA3-Personal"
                elif [ "$sec" == "psk-mixed" ]
                then
                        Mode="WPA-WPA2-Personal"
                elif [ "$sec" == "sae-mixed" ]
                then
                        Mode="WPA3-Personal-Transition"
                elif [ "$sec" == "wpa" ]
                then
                        Mode="WPA-Enterprise"
                elif [ "$sec" == "wpa2" ]
                then
                        Mode="WPA2-Enterprise"
                elif [ "$sec" == "wpa3" ]
                then
                        Mode="WPA3-Enterprise"
                else
                        Mode="WPA-WPA2-Enterprise"
                fi

                encrypt=$(echo $encryption | cut -d '+' -f 2,3)
                # Encrytion
                if [ "$encrypt" == "tkip+aes" ]
                then
                        EncryptMethod="TKIPandAESEncryption"
                elif [ "$encrypt" == "tkip" ]
                then
                        EncryptMethod="TKIPEncryption"
                else
                        EncryptMethod="AESEncryption"
                fi

                if [ -z "$MapBSSType" ] || [ "$MapBSSType" == "16" ]; then
                        wifi_api wifi_setSSIDEnable $interface 0
                        continue # since VAP is in teardown state we are not updating the rest of the cred
                fi

                # Hostapd File Updation
                # hostapd conf file update
                # map 1, multi_ap_profile-1 enabled
                # map 2, multi_ap_profile-2 enabled
                hostapd_file="/nvram/hostapd$interface.conf"
                if [ -e "$hostapd_file" ]
                then
                        grep -i "multi_ap_profile=" $hostapd_file
                        if [ $? -eq 0 ]; then
                                sed -i s/^multi_ap_profile=.*/multi_ap_profile=$map_profile/ $hostapd_file
                        else
                                echo "multi_ap_profile=$map_profile" >> $hostapd_file
                        fi

                        if [ "$TSEnabled" == "1" ] && [ "$MapVersion" == "2" ]; then
                                grep -i "multi_ap_vlanid=" $hostapd_file
                                if [ $? -eq 0 ]; then
                                        sed -i s/^multi_ap_vlanid.*/multi_ap_vlanid=$vap_primary_vlan/ $hostapd_file
                                else
                                        echo "multi_ap_vlanid=$vap_primary_vlan" >> $hostapd_file
                                fi
                        fi
                        # MapBSSType 96, vap is both fronthaul and backhaul BSS
                        # MapBSSType 32, vap is fronthaul BSS
                        # MapBSSType 64, vap is backhaul BSS
                        if [ $(($((MapBSSType&64)) >> 6)) -eq 1 ] && [ $(($((MapBSSType&32)) >> 5)) -eq 1 ]; then
                                multi_ap_type=3
                        elif [ $(($((MapBSSType&32)) >> 5)) -eq 1 ]; then
                                multi_ap_type=2
                        elif [ $(($((MapBSSType&64)) >> 6)) -eq 1 ]; then
                                multi_ap_type=1
                        fi

			if [ $MapVersion -eq 2 -a $TSEnabled -eq 0 -a $(($((MapBSSType&64)) >> 6)) -eq 1 ]; then
				multi_ap_type=1
			fi
                        grep -i "multi_ap=" $hostapd_file
                        if [ $? -eq 0 ]; then
                                sed -i s/^multi_ap=.*/multi_ap=$multi_ap_type/ $hostapd_file
                        else
                                echo "multi_ap=$multi_ap_type" >> $hostapd_file
                        fi

                        # Backhaul SSID / Keypassphrase updation in FH hostapd file
                        if [ $MapBSSType != 64 ] && [ $MapBSSType != 72 ]; then
                                grep -i "multi_ap_backhaul_ssid=" $hostapd_file
                                if [ $? -eq 0 ]; then
                                        sed -i s/^multi_ap_backhaul_ssid.*/multi_ap_backhaul_ssid=\""$multi_ap_backhaul_ssid"\"/ $hostapd_file
                                else
                                        echo "multi_ap_backhaul_ssid="\"$multi_ap_backhaul_ssid\" >> $hostapd_file
                                fi

                                if [ -n "$multi_ap_backhaul_wpa_passphrase" ]; then
                                        grep -i "multi_ap_backhaul_wpa_passphrase=" $hostapd_file
                                        if [ $? -eq 0 ]; then
                                                sed -i s/^multi_ap_backhaul_wpa_passphrase.*/multi_ap_backhaul_wpa_passphrase=$multi_ap_backhaul_wpa_passphrase/ $hostapd_file
                                        else
                                                echo "multi_ap_backhaul_wpa_passphrase=$multi_ap_backhaul_wpa_passphrase" >> $hostapd_file
                                        fi
                                fi
                        fi
                        if [ "$dpp" == "1" ]; then
                                if [ ! -z $dpp_map ]; then
                                        grep -i "dpp_map=" $hostapd_file
                                        if [ $? -eq 0 ]; then
                                                sed -i s/^dpp_map=.*/dpp_map=$dpp_map/ $hostapd_file
                                        else
                                                echo "dpp_map=$dpp_map" >> $hostapd_file
                                        fi
                                fi
                                if [ $MapBSSType != 64 ] && [ $MapBSSType != 72 ]; then
                                        grep -i "dpp_controller=" $hostapd_file
                                        if [ $? -eq 0 ]; then
                                                sed -i s/^dpp_configurator_connectivity=.*/dpp_configurator_connectivity=$dpp_configurator_connectivity/ $hostapd_file
                                        else
                                                echo "dpp_controller=$dpp_controller" >> $hostapd_file
                                        fi
                                fi
                                if [ ! -z $dpp_configurator_connectivity ]; then
                                        grep -i "dpp_configurator_connectivity=" $hostapd_file
                                        if [ $? -eq 0 ]; then
                                                sed -i s/^dpp_configurator_connectivity=.*/dpp_configurator_connectivity=$dpp_configurator_connectivity/ $hostapd_file
                                        else
                                                echo "dpp_configurator_connectivity=$dpp_configurator_connectivity" >> $hostapd_file
                                        fi
                                fi
                                if [ ! -z $gas_comeback_delay ]; then
                                        grep -i "gas_comeback_delay=" $hostapd_file
                                        if [ $? -eq 0 ]; then
                                                sed -i s/^gas_comeback_delay=.*/gas_comeback_delay=$gas_comeback_delay/ $hostapd_file
                                        else
                                                echo "gas_comeback_delay=$gas_comeback_delay" >> $hostapd_file
                                        fi
                                fi
                        fi
                fi

                # cfg80211tool_mesh updation
                [ $map_profile -gt 0 ] && cfg80211tool_mesh "$ifname" map "$map_profile"

                if [ ! -z $MapBSSType ]; then
                        [ $MapBSSType -gt 0 ] && cfg80211tool_mesh "$ifname" MapBSSType "$MapBSSType"
                fi

                #need generic handling
                if [ `expr $interface % 2` == 0 ]; then
                        radioifname="wifi1"
                        vifs_iface="ath0 ath2 ath4 ath12 ath14"
                else
                        radioifname="wifi0"
                        vifs_iface="ath1 ath3 ath5 ath13 ath15"
                fi

                syscfg get wireless.$radioifname.vifs
                syscfg set wireless.$radioifname.vifs $vifs_iface
                syscfg set wireless.ath$interface wifi-iface
                syscfg set wireless.ath$interface.TYPE wifi-iface
                syscfg set wireless.ath$interface.device $radioifname
                syscfg set wireless.ath$interface.ifname ath$interface
                syscfg set wireless.ath$interface.ssid $ssid_value
                #syscfg set wireless.ath$interface.network brlan0
                #syscfg set wireless.ath$interface.wps_pbc 1
                syscfg set wireless.ath$interface.mode ap
                syscfg commit

                update_wifi_params

                #need generic handling
                if [ `expr $interface % 2` == 0 ]; then
                        radio="w2"
                else
                        radio="w5"
                fi

                # Whenever there is any configuration changes for a vap and hostapd reload is invoked,
                # VAP interface is going into non relay state in hyfi bridge.
                # Using below commands we are enabling interface to proper state in hyfi-bridge.
                wifi_api qca_hostapdRaw_Rem_Add $interface
                _hyfi_ath_config

        done

        _hyfi_eth_config
        repacd_set_config
	_rdk_add_eth_vlan
	# Delete backhaul vap in primary bridge
	if [ "$MapVersion" -ge "2" ]; then
		if [ "$TSEnabled" == "1" ]; then
			vlan_api vlan_hal_delInterface brlan0 $backhaulssid 1
		fi
	fi
	rm -rf /nvram/hyfi_enable

        echo "Cloning [Done] rdk_helper script " >> /tmp/rdk_helper_logs
}

#------------------------------------------------------------------
# ENTRY
#------------------------------------------------------------------
check_dhcp_server_pool() {
        for i in `seq 2 4`; do
                pool=`syscfg get ezmesh.DHCP.Enable.PoolID$i`
                if [ -n "$pool" ]; then
                        firewall_restart=1
                fi
        done
}

firewall_restart() {
        _query_ip
        query_firewall

        for brlan_ifno in `seq 1 2`; do
                query_dnsmasq_conf brlan$brlan_ifno
                _rdk_config_bridge brlan$brlan_ifno
        done
}

reinit() {
        _lan_unset

        for i in `ls /nvram/hostapd*.conf`; do
            grep -i "bridge=" $i
            if [ $? -eq 0 ]; then
                 sed -i s/^bridge=.*/bridge=$primary_bridge/ $i
            else
                 echo "bridge=$primary_bridge" >> $i
            fi
        done

        # reset BH vap network type
        syscfg set wireless.$backhaulssid.network $primary_bridge
        syscfg commit
}

primary_bridge=brlan0
#service_init
case "$1" in
    "firewall-restart")
    if [ "$MapVersion" == "2" ]; then
        check_dhcp_server_pool
        if [ "$firewall_restart" != "0" ]; then
            firewall_restart
        fi
    fi
    ;;
    "eth-hyfi-enable")
    if [ "$MapVersion" == "2" ] && [ "$TSEnabled" == "1" ]; then
       if [ -n $vap_primary_vlan ]; then
            reinit
       fi
    fi

    _hyfi_eth_config
    ;;
    *)
    script_start
    #Updating the Radio Mac static on reboot
    if ! [ -f /nvram/radiomac.txt ]; then
        echo "[2G Radio Mac]" >> /nvram/radiomac.txt
        mac_output=`ifconfig ath0 | grep HWaddr | cut -d " " -f 11`
        echo $mac_output >> /nvram/radiomac.txt
        echo "[5G Radio Mac]" >> /nvram/radiomac.txt
        mac_output=`ifconfig ath1 | grep HWaddr | cut -d " " -f 11`
        echo $mac_output >> /nvram/radiomac.txt
    fi
    #Restarting Ezmesh and Wsplcd after helper script
    if [ "$MapVersion" == "2" ]; then
       systemctl restart qca-ezmesh.service
       systemctl restart qca-wsplcd.service
    fi
    ;;
esac


