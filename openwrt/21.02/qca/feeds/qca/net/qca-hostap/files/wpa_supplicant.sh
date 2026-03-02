#
# Copyright (c) 2017,2023 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.
#

#
# Copyright (c) 2014, The Linux Foundation. All rights reserved.
#
 
. /lib/wifi/wifi-utils.sh

# Check OpenWrt version and parse bridge value which is in json format
owrt_versn=`cat /etc/openwrt_release | grep 'RELEASE' | grep -Eo '[+-]?[0-9]+([.][0-9]+)?'`
owrt_versn_ret=0
WPAD_VARRUN="$(get_wpad_var_run)"
ker_ver=`uname -r |cut -d. -f1`

wpa_supplicant_setup_vif() {
	local vif="$1"
	local driver="$2"
	local key="$key"
	local options="$3"
	local freq="" crypto=""
	local sae owe suite_b dpp sae_groups owe_group sae_password sae_password_id pasn
	local sae_str owe_str dpp_str dpp_connector dpp_csign dpp_netaccesskey dpp_map dpp_pfs dpp_1905_connector dpp_pp_key pasn_str
	local ieee80211r_str
	local device beacon_prot
	local board_name
	local pwe=""
	local sta_mld_addr=""
	local force_hostapd_attach
	local mlo_links=""
	local pref_assoc_lk=""
	local preferred_ap_mld_addr=""
	local mld_ifname=""
	local mlo_skip_link=""
	local ml_probing_cfg=""
	local ignore_old_scan=""

	[ -f /tmp/sysinfo/sdk_board_name ] && {
		board_name=ap$(cat /tmp/sysinfo/sdk_board_name | awk -F 'ap' '{print$2}')
	}

	[ -n "$4" ] && freq="frequency=$4"
	config_get_bool sae "$vif" sae 0
	config_get_bool owe "$vif" owe 0
	config_get sae_password "$vif" sae_password
	config_get sae_password_id "$vif" sae_password_id
	config_get ieee80211w "$vif" ieee80211w 0
	config_get suite_b "$vif" suite_b 0
	config_get ieee80211r "$vif" ieee80211r 0
	config_get dpp "$vif" dpp 0
	config_get device "$vif" device
	config_get sae_pwe "$vif" sae_pwe 2
	config_get pasn "$vif" pasn 0
	config_get ssid_protection "$vif" ssid_protection 0

	if [ -n "$ssid_protection" ]
	then
		ssid_protection_config="ssid_protection=$ssid_protection"
	fi

	config_get mld "$vif" mld
	config_get mld_macaddr "$mld" mld_macaddr
	[ -n "$mld_macaddr" ] && sta_mld_addr="sta_mld_addr=$mld_macaddr"

	[ -n "$mld_macaddr" ] && mld_ifname="mld_ifname=$mld"

        if [ $ker_ver -lt 6 ]; then
	    config_get allow_non_ml "$mld" allow_non_ml 0
	    [ -n "$allow_non_ml" ] && allow_non_ml_assoc="allow_non_ml_assoc=$allow_non_ml"
	    config_get legacy_downgrade "$mld" legacy_downgrade 0
	    [ -n "$legacy_downgrade" ] && legacy_downgrade="legacy_downgrade=$legacy_downgrade"
        fi

	config_get preferred_assoc_link "$vif" preferred_assoc_link 0
	[ -n "$mld_macaddr" ] && pref_assoc_lk="preferred_assoc_link=$preferred_assoc_link"
	config_get preferred_ap_mld_addr "$mld" preferred_ap_mld_addr ""
	[ -n "$mld_macaddr" -a -n "$preferred_ap_mld_addr" ] && \
	preferred_ap_mld_addr="preferred_ap_mld_addr=$preferred_ap_mld_addr"
	config_get mlo_skip_link_time "$mld" mlo_skip_link_time 0
	[ -n "$mld_macaddr" ] && mlo_skip_link="mlo_skip_link_time=$mlo_skip_link_time"
	config_get_bool ml_probing "$mld" ml_probing 0
	[ -n "$mld_macaddr" ] && ml_probing_cfg="ml_probing=$ml_probing"

	config_get link_mac "$mld" link_mac
	uniq_link_macs=$(for mac in $link_mac; do echo $mac; done | sort -u)
        len=0
        for i in $uniq_link_macs;do len=`expr $len + 1`; done
        [ -n "$mld_macaddr" ] && mlo_links="num_mlo_links=$len"

	config_get qwrap_enable "$device" qwrap_enable 0
	if [ $qwrap_enable -eq 0 ]; then
		config_get ignore_old_scan_res "$vif" ignore_old_scan_res 1
		[ -n "$mld_macaddr" ] && ignore_old_scan="ignore_old_scan_res=$ignore_old_scan_res"
	fi

	echo "Num Links = $len"
	if [ $len -gt 3 ]; then
		echo "MLO Clients are supported only with up to 3 links: wpa supplicant setup failed!"
		return 1
	fi

	if [ $ker_ver -ge 6 ] && [ -n "$mld_macaddr" ]
	then
		local cur_num_links=0
		config_get cur_num_links "$mld" cur_num_links 0
		cur_num_links=`expr $cur_num_links + 1`;

		config_set "$mld" cur_num_links "$cur_num_links"

		if [ -f /sys/class/net/$device/supported_freq_list ]
		then
			local freq_list=""
			config_get freq_list "$mld" freq_list ""
			freq_list="${freq_list}$(cat /sys/class/net/$device/supported_freq_list)"
			config_set "$mld" freq_list "$freq_list"
		fi

		if [ $cur_num_links -lt $len ]
		then
			echo "MLO Client , postponing supplicant attach"
			return
		fi
		echo "MLO CLient, Going forward to program conf"
	fi

	if [ $suite_b -eq 192 ]
	then
		key_mgmt=WPA-EAP-SUITE-B-192
		pairwise="pairwise=GCMP-256"
		group="group=GCMP-256"
		group_mgmt="group_mgmt=BIP-GMAC-256"
		ieee80211w="ieee80211w=2"
		proto="proto=RSN"

		config_get eap_type "$vif" eap_type
		[ -n "$eap_type" ] && eap_type="eap=$eap_type"

		config_get identity "$vif" identity
		[ -n "$identity" ] && identity="identity=\"$identity\""

		config_get ca_cert "$vif" ca_cert
		[ -n "$ca_cert" ] && ca_cert="ca_cert=\"$ca_cert\""

		config_get client_cert "$vif" client_cert
		[ -n "$client_cert" ] && client_cert="client_cert=\"$client_cert\""

		config_get priv_key "$vif" priv_key
		[ -n "$priv_key" ] && priv_key="private_key=\"$priv_key\""

		config_get priv_key_pwd "$vif" priv_key_pwd
		[ -n "$priv_key_pwd" ] && priv_key_pwd="private_key_passwd=\"$priv_key_pwd\""

		config_get phase1 "$vif" phase1
		[ -n "$phase1" ] && phase1="phase1=\"$phase1\""

	else
		# make sure we have the encryption type and the psk
		[ -n "$enc" ] || {
			config_get enc "$vif" encryption
		}

		enc_list=`echo "$enc" | sed "s/+/ /g"`

		for enc_var in $enc_list; do
			case "$enc_var" in
				*tkip)
					crypto="TKIP $crypto"
					;;
				*aes)
					crypto="CCMP $crypto"
					;;
				*ccmp)
					crypto="CCMP $crypto"
					;;
				*ccmp-256)
					crypto="CCMP-256 $crypto"
					;;
				*gcmp)
					crypto="GCMP $crypto"
					;;
				*gcmp-256)
					crypto="GCMP-256 $crypto"
				;;
			esac
		done

		case "$enc_list" in
				dpp|sae*|psk2)
					if [ -f /sys/class/net/$device/ciphercaps ]
					then
						cat /sys/class/net/$device/ciphercaps | grep -i "gcmp"
						if [ $? -eq 0 ]
						then
							crypto="CCMP CCMP-256 GCMP GCMP-256"
						else
							crypto="CCMP"
						fi
					fi
				;;
		esac

		[ -n "$key" ] || {
			config_get key "$vif" key
		}

		local net_cfg bridge
		config_get bridge "$vif" bridge
		[ -z "$bridge" ] && {
			net_cfg="$(find_net_config "$vif")"
			[ -z "$net_cfg" ] || { \
				case "$board_name" in
				ap-sdxpinn*)
					bridge="$(get_ap_sdxpinn_bridge_interface "$net_cfg")"
				;;
				*)
                    #owrt_versn_ret=$(awk -v a="$owrt_versn" -v b="19.07" 'BEGIN{print(a>b)}')
                    #if [ $owrt_versn_ret -eq 1 ]; then
					#    local bridge_iface="$(bridge_interface "$net_cfg")"
					#    bridge_json="$(echo $bridge_iface | sed -n 's/.*{\(.*\)}.*$/\{\1\}/p')"
					#    json_init
					#    json_load "$bridge_json"
                    #    json_get_var bridge bridge
                    #else
                       bridge="$(bridge_interface "$net_cfg")"
                    #fi
				;;
				esac
				config_set "$vif" bridge "$bridge"
			}
		}

		local mode ifname addr4 modestr=""
		config_get mode "$vif" mode
		config_get ifname "$vif" ifname
		config_get_bool addr4 "$vif" addr4 0
		config_get_bool extap "$vif" extap 0

		if [ $ker_ver -ge 6 ] && [ -n "$mld_macaddr" ]
		then
			ifname="$mld"
		fi

		config_get device "$vif" device
		config_get_bool qwrap_enable "$device" qwrap_enable 0

		[ -z "$bridge" ] || [ "$mode" = ap ] || [ "$mode" = sta -a $addr4 -eq 1 ] || \
		[ "$mode" = sta -a $extap -eq 1 ] || [ $qwrap_enable -ne 0 ] || {
			echo "wpa_supplicant_setup_vif($ifname): Refusing to bridge $mode mode interface"
			return 1
		}
		[ "$mode" = "adhoc" ] && modestr="mode=1"

		key_mgmt='NONE'
		case "$enc" in
			*none*) ;;
			*wep*)
				config_get key "$vif" key
				key="${key:-1}"
				case "$key" in
					[1234])
						for idx in 1 2 3 4; do
							local zidx
							zidx=$(($idx - 1))
							config_get ckey "$vif" "key${idx}"
							[ -n "$ckey" ] && \
								append "wep_key${zidx}" "wep_key${zidx}=$(prepare_key_wep "$ckey")"
						done
						wep_tx_keyidx="wep_tx_keyidx=$((key - 1))"
					;;
					*)
						wep_key0="wep_key0=$(prepare_key_wep "$key")"
						wep_tx_keyidx="wep_tx_keyidx=0"
					;;
				esac
				case "$enc" in
					*mixed*)
						wep_auth_alg='auth_alg=OPEN SHARED'
					;;
					*shared*)
						wep_auth_alg='auth_alg=SHARED'
					;;
					*open*)
						wep_auth_alg='auth_alg=OPEN'
					;;
				esac
			;;
			*psk*)
				key_mgmt='WPA-PSK'
				# if you want to use PSK with a non-nl80211 driver you
				# have to use WPA-NONE and wext driver for wpa_s
				[ "$mode" = "adhoc" -a "$driver" != "nl80211" ] && {
					key_mgmt='WPA-NONE'
					driver='wext'
				}

				if [ "${ieee80211r}" -eq 1 ]
				then
					ieee80211r_str=FT-PSK
				fi

				if [ ${sae} -eq 1 ]
				then
					if [ ${sae_password} ]
					then
						sae_pwd="sae_password=\"${sae_password}\""
					else
						sae_pwd="sae_password=\"${key}\""
					fi
				fi
				if [ ${#key} -eq 64 ]; then
					passphrase="psk=${key}"
				else
					passphrase="psk=\"${key}\""
				fi

				[ -n "$crypto" ] || crypto="CCMP"
				pairwise="pairwise=$crypto"

				case "$enc" in
					*mixed*)
						proto='proto=RSN WPA'
					;;
					*psk2*)
						proto='proto=RSN'
						config_get ieee80211w "$vif" ieee80211w 0
					;;
					*psk*)
						proto='proto=WPA'
					;;
				esac
			;;
			*wpa*|*8021x*)
				proto='proto=WPA2'
				key_mgmt='WPA-EAP'
				config_get ieee80211w "$vif" ieee80211w 0
				config_get ca_cert "$vif" ca_cert
				config_get eap_type "$vif" eap_type
				ca_cert=${ca_cert:+"ca_cert=\"$ca_cert\""}

				[ -n "$crypto" ] || crypto="CCMP"
				pairwise="pairwise=$crypto"

				case "$eap_type" in
					tls)
						config_get identity "$vif" identity
						config_get client_cert "$vif" client_cert
						config_get priv_key "$vif" priv_key
						config_get priv_key_pwd "$vif" priv_key_pwd
						identity="identity=\"$identity\""
						client_cert="client_cert=\"$client_cert\""
						priv_key="private_key=\"$priv_key\""
						priv_key_pwd="private_key_passwd=\"$priv_key_pwd\""
					;;
					peap|ttls)
						config_get auth "$vif" auth
						config_get identity "$vif" identity
						config_get password "$vif" password
						phase2="phase2=\"auth=${auth:-MSCHAPV2}\""
							identity="identity=\"$identity\""
						password="password=\"$password\""
						;;
				esac
				eap_type="eap=$(echo $eap_type | tr 'a-z' 'A-Z')"
			;;
			ccmp*|gcmp*|sae*)
				proto='proto=RSN'
				if [ ${sae} -eq 1 ]
				then
					if [ ${sae_password} ]
					then
						sae_pwd="sae_password=\"${sae_password}\""
					else
						sae_pwd="sae_password=\"${key}\""
					fi
				fi
				case $enc in
					ccmp-256)
						[ -n "$crypto" ] || crypto="CCMP-256"
						pairwise="pairwise=$crypto"
					;;
					gcmp-256)
						[ -n "$crypto" ] || crypto="GCMP-256"
						pairwise="pairwise=$crypto"
					;;
					ccmp*)
						[ -n "$crypto" ] || crypto="CCMP"
						pairwise="pairwise=$crypto"
					;;
					gcmp*)
						[ -n "$crypto" ] || crypto="GCMP"
						pairwise="pairwise=$crypto"
					;;
					sae*)
						pairwise="pairwise=$crypto"
					;;
				esac
			;;
		esac

		keymgmt='NONE'

		# Allow SHA256
		case "$enc" in
			*wpa*|*8021x*) keymgmt=WPA-EAP;;
			ccmp*|gcmp*|sae*) keymgmt=;;
			*psk*) keymgmt=WPA-PSK;;
		esac

		if [ "${sae}" -eq 1 -a "${ieee80211r}" -gt 0 ]
		then
			ieee80211w=2
			sae_str=FT-SAE
		elif [ "${sae}" -eq 1 ]
		then
			if [ "${keymgmt}" == "WPA-PSK" ];then
				ieee80211w=1
			else
				ieee80211w=2
			fi
			sae_str=SAE
		fi

		if [ "${owe}" -eq 1 ]
		then
			proto='proto=RSN'
			ieee80211w=2
			owe_only='owe_only=1'
			owe_str=OWE
			passphrase=
			config_get  owe_group "$vif" owe_group
			if [ "$owe_group" ];then
				owe_group="owe_group=$owe_group"
			else
				owe_group=
			fi
		fi

		if [ "${pasn}" -eq 1 ]
		then
			pasn_str=PASN
			proto="proto=RSN"
		fi

		if [ "${dpp}" -eq 1 ]
		then
			dpp_str=DPP
			proto="proto=RSN"
		fi
		config_get map "$vif" map 0
		if [ "${dpp}" -eq 1 ] || [ "$map" -ge 3 ]
		then
			config_get dpp_connector "$vif" dpp_connector
			config_get dpp_1905_connector "$vif" dpp_1905_connector
			config_get dpp_csign     "$vif" dpp_csign
			config_get dpp_pp_key     "$vif" dpp_pp_key
			config_get dpp_netaccesskey "$vif" dpp_netaccesskey

			[ -n "$dpp_connector" ] && dpp_connector="dpp_connector=\"${dpp_connector}\""
			[ -n "$dpp_1905_connector" ] && dpp_1905_connector="dpp_1905_connector=${dpp_1905_connector}"
			[ -n "$dpp_csign" ] && dpp_csign="dpp_csign=$dpp_csign"
			[ -n "$dpp_pp_key" ] && dpp_pp_key="dpp_pp_key=$dpp_pp_key"
			[ -n "$dpp_netaccesskey" ] && dpp_netaccesskey="dpp_netaccesskey=$dpp_netaccesskey"
			update_config="update_config=1"
		fi

		case "$ieee80211w" in
			0)
				key_mgmt="${keymgmt}"
			;;
			1)
				if [ "$key_mgmt" != "NONE" ]
				then
					key_mgmt="${keymgmt} ${keymgmt}-SHA256"
				fi
			;;
			2)
				if [ "$owe" -eq 1 ] || [ "$sae" -eq 1 ] || [ "$dpp" -eq 1 ]
				then
					key_mgmt="${keymgmt}"
				else
					key_mgmt="${keymgmt}-SHA256"
				fi
			;;
		esac

		if [ "${sae}" -eq 1 ]
		then
			add_sae_groups() {
				sae_groups=$(echo $1 | tr "," " ")
			}
			config_list_foreach  "$vif" sae_groups add_sae_groups
			if [ "$sae_groups" ];then
				sae_groups="sae_groups=$sae_groups"
			else
				sae_groups=
			fi
			key_mgmt="${sae_str} ${key_mgmt}"
			pwe="sae_pwe=$sae_pwe"
			[ -n "${sae_password_id}" ] && sae_password_id="sae_password_id=\"$sae_password_id\""
			config_get local_key_mgmt "$vif" key_mgmt
			[ -n "$local_key_mgmt" ] && key_mgmt="$local_key_mgmt"
		fi
		if [ "${owe}" -eq 1 ]
		then
			key_mgmt="${key_mgmt} ${owe_str}"
		fi
		if [ "${pasn}" -eq 1 ]
		then
			key_mgmt="${key_mgmt} ${pasn_str}"
		fi
		if [ "${ieee80211r}" -eq 1 ]
		then
			key_mgmt="${key_mgmt} ${ieee80211r_str}"
		fi

		[ "$ieee80211w" -gt "0" ] && {
			config_get beacon_prot "$vif" beacon_prot 0
			[ -n "$beacon_prot" ] && beacon_prot="beacon_prot=$beacon_prot"
		}

		[ -n "$ieee80211w" ] && ieee80211w="ieee80211w=$ieee80211w"

		if [ $sae -ne 1 ] && [ $owe -ne 1 ] && [ $dpp -ne 1 ]
		then
			case "$pairwise" in
				*CCMP-256*) group="group=CCMP-256 GCMP-256 GCMP CCMP TKIP";;
				*GCMP-256*) group="group=GCMP-256 GCMP CCMP TKIP";;
				*GCMP*) group="group=GCMP CCMP TKIP";;
				*CCMP*) group="group=CCMP TKIP";;
				*TKIP*) group="group=TKIP";;
			esac
		else
			case "$pairwise" in
				*CCMP-256*) group="group=CCMP-256 GCMP-256 GCMP CCMP";;
				*GCMP-256*) group="group=GCMP-256 GCMP CCMP";;
				*GCMP*) group="group=GCMP CCMP";;
				*CCMP*) group="group=CCMP";;
			esac
		fi
		if [ "${dpp}" -eq 1 ]
		then
			if [ "$key_mgmt" != "NONE" ]
			then
				key_mgmt="${dpp_str} ${key_mgmt}"
			else
				key_mgmt="${dpp_str}"
			fi
			[ -n "$crypto" ] || crypto="CCMP"
			pairwise="pairwise=$crypto"
			ieee80211w="ieee80211w=1"
		fi

	#End of suite_b is 192 check
	fi

	config_get MapBSSType "$vif" MapBSSType 0
	config_get map "$vif" map 0

	if [ "$map" -ge 3 ]; then
		config_get dpp_map "$vif" dpp_map
		[ -n "$dpp_map" ] && dpp_map="dpp_map=$dpp_map"
	fi
	config_get dpp_pfs "$vif" dpp_pfs
	[ -n "$dpp_pfs" ] && dpp_pfs="dpp_pfs=$dpp_pfs"

	config_get multi_ap_backhaul_sta "$vif" multi_ap_backhaul_sta
	[ -n "$multi_ap_backhaul_sta" ] && dpp_pfs="multi_ap_backhaul_sta=$multi_ap_backhaul_sta"

	multi_ap_backhaul_sta=0
	# MapBSSType 128, vap is backhaul STA
	if [ $MapBSSType -eq 128 ]; then
		multi_ap_backhaul_sta=1
		if [ "$map" -ge 2 ]; then
			wps_cred_add_sae=1
			config_get vlan_bridge "$vif" vlan_bridge
			if [ -n "$vlan_bridge" ]; then
				config_set "$vif" bridge "$vlan_bridge"
			fi
		fi
	fi
	wps_cred_add_sae=1

	config_get ifname "$vif" ifname
	config_get bridge "$vif" bridge
	config_get ssid "$vif" ssid
	config_get bssid "$vif" bssid
	bssid=${bssid:+"bssid=$bssid"}

	if [ $ker_ver -ge 6 ] && [ -n "$mld_macaddr" ]
	then
		ifname="$mld"
	fi

	config_get pmf "$vif" ieee80211w 0
	config_get_bool wps_pbc "$vif" wps_pbc 0
        if [ $ker_ver -lt 6 ]; then
	    config_get_bool wps_cross_band_overlap "$vif" wps_cross_band_overlap 0
        fi
	config_get_bool urnm_mfpr_x20 "$vif" urnm_mfpr_x20 0
	config_get_bool pasn_corrupt_mic "$vif" pasn_corrupt_mic 0
	config_get_bool skip_connect "$vif" skip_connect 0

	config_get config_methods "$vif" wps_config
	[ "$wps_pbc" -gt 0 ] && append config_methods "push_button display"

	[ -n "$config_methods" ] && {
		wps_cred="wps_cred_processing=2"
		wps_config_methods="config_methods=$config_methods"
		update_config="update_config=1"

		# fix the overlap session of WPS PBC for two STA vifs
		macaddr=$(cat /sys/class/net/${bridge}/address)
		uuid=$(echo "$macaddr" | sed 's/://g')
		[ -n "$uuid" ] && {
			uuid_config="uuid=87654321-9abc-def0-1234-$uuid"
		}
	}

	local ctrl_interface wait_for_wrap=""

	if [ $qwrap_enable -ne 0 ]; then
		ctrl_interface="$WPAD_VARRUN/wpa_supplicant"
		if [ -f "/tmp/qwrap_conf_filename-$ifname.conf" ]; then
			rm -rf /tmp/qwrap_conf_filename-$ifname.conf
		fi
		echo -e "/var/run/wpa_supplicant-$ifname.conf \c\h" > /tmp/qwrap_conf_filename-$ifname.conf
		wait_for_wrap="-W"
	fi

	if [ -n "$freq_list" ]
	then
		save_freq_list="freq_list=$freq_list"
	fi

	ctrl_interface="$WPAD_VARRUN/wpa_supplicant-$ifname"
	case "$board_name" in
	ap-sdxpinn*)
		# If cert is not enabled, bring-up iface as a deprivileged user (wifi)
		cert_enabled="$(is_cert_enabled)"
		[ "$cert_enabled" -eq 0 ] && ctrl_interface_group="ctrl_interface_group=1010"
	;;
	esac

	rm -rf $ctrl_interface
	rm -f /var/run/wpa_supplicant-$ifname.conf

        if [ $ker_ver -ge 6 ]; then
	    cat > /var/run/wpa_supplicant-$ifname.conf <<EOF
ctrl_interface=$ctrl_interface
$ctrl_interface_group
$wps_config_methods
pmf=$pmf
$wps_cred
$update_config
$uuid_config
$sae_groups
$dot11RSNAConfigPMKLifetime
$pwe
wps_cred_add_sae=$wps_cred_add_sae
$dpp_map
$dpp_1905_connector
$ignore_old_scan
$save_freq_list
network={
	$modestr
	scan_ssid=1
	ssid="$ssid"
	$bssid
	key_mgmt=$key_mgmt
	$proto
	$freq
	$ieee80211w
	$beacon_prot
	$passphrase
	$sae_pwd
	$sae_password_id
	$pairwise
	$group
	$eap_type
	$ca_cert
	$client_cert
	$priv_key
	$priv_key_pwd
	$phase2
	$identity
	$password
	$wep_key0
	$wep_key1
	$wep_key2
	$wep_key3
	$wep_tx_keyidx
	$wep_auth_alg
	$owe_only
	$group_mgmt
	$phase1
	$owe_group
	$dpp_connector
	$dpp_csign
	$dpp_pp_key
	$dpp_netaccesskey
	multi_ap_backhaul_sta=$multi_ap_backhaul_sta
	multi_ap_profile=$map
	$dpp_pfs
	$ssid_protection_config
}
EOF
        else
	    cat > /var/run/wpa_supplicant-$ifname.conf <<EOF
ctrl_interface=$ctrl_interface
$ctrl_interface_group
$wps_config_methods
pmf=$pmf
$wps_cred
$update_config
$uuid_config
wps_cross_band_overlap=$wps_cross_band_overlap
urnm_mfpr_x20=$urnm_mfpr_x20
pasn_corrupt_mic=$pasn_corrupt_mic
skip_connect=$skip_connect
$sae_groups
$dot11RSNAConfigPMKLifetime
$pwe
wps_cred_add_sae=$wps_cred_add_sae
$dpp_map
$dpp_1905_connector
$sta_mld_addr
$mlo_links
$pref_assoc_lk
$allow_non_ml_assoc
$legacy_downgrade
$mld_ifname
$mlo_skip_link
$ml_probing_cfg
$ignore_old_scan
network={
	$modestr
	scan_ssid=1
	ssid="$ssid"
	$bssid
	$preferred_ap_mld_addr
	key_mgmt=$key_mgmt
	$proto
	$freq
	$ieee80211w
	$beacon_prot
	$passphrase
	$sae_pwd
	$sae_password_id
	$pairwise
	$group
	$eap_type
	$ca_cert
	$client_cert
	$priv_key
	$priv_key_pwd
	$phase2
	$identity
	$password
	$wep_key0
	$wep_key1
	$wep_key2
	$wep_key3
	$wep_tx_keyidx
	$wep_auth_alg
	$owe_only
	$group_mgmt
	$phase1
	$owe_group
	$dpp_connector
	$dpp_csign
	$dpp_pp_key
	$dpp_netaccesskey
	multi_ap_backhaul_sta=$multi_ap_backhaul_sta
	multi_ap_profile=$map
	$dpp_pfs
	$ssid_protection_config
}
EOF
        fi

	config_get force_hostapd_attach "$device" force_hostapd_attach

	# Attach athX ifaces to wpa_supplicant if "force_hostapd_attach" is set
	# to 1. IPQ/SDX75 boards will have it set to 1 always; SDX65 boards can
	# configure this option in the wireless file
	if [ "$force_hostapd_attach" -eq 1 ]; then
		[ -z "$proto" -a "$key_mgmt" != "NONE" ] || {\
			# If there is a change in path of wpa_supplicant-$ifname.lock file, please make the path
			# change also in wrapd_api.c file.
			[ -f "/var/run/wpa_supplicant-$ifname.lock" ] &&
				rm /var/run/wpa_supplicant-$ifname.lock
			wpa_cli -g $WPAD_VARRUN/wpa_supplicantglobal interface_add  $ifname /var/run/wpa_supplicant-$ifname.conf $driver $WPAD_VARRUN/wpa_supplicant-$ifname "" $bridge
			touch /var/run/wpa_supplicant-$ifname.lock
		}

		if [ "${dpp}" -eq 1 ] || [ "$map" -ge 3 ]
		then
			type=
			config_get dpp_type "$vif" dpp_type "qrcode"
			config_get dpp_curve "$vif" dpp_curve
			config_get dpp_key "$vif" dpp_key
			config_get dpp_info "$vif" dpp_info
			config_get channel "$device" channel auto
			config_get htmode  "$device" htmode auto
			config_get hwmode "$device" hwmode auto
			config_get pkex_code "$vif" pkex_code
			config_get pkex_identifier "$vif" pkex_identifier
			type=$dpp_type
			dpp_type="type=$dpp_type"

			if [ -z $dpp_curve ]; then
				dpp_curve=
			else
				dpp_curve="curve=$dpp_curve"
			fi

			if [ -z $dpp_key ]; then
				dpp_key=
			else
				dpp_key="key=$dpp_key"
			fi

			if [ -z $dpp_info ]; then
				dpp_info=
			else
				dpp_info="info=SN=$dpp_info"
			fi

			if [ "$pkex_identifier" ]; then
				pkex_identifier="pkex_identifier=$pkex_identifier"
			fi

			if [ "$pkex_code" ]; then
				pkex_code="pkex_code=$pkex_code"
			fi

			if [ "${htmode}" == "auto" ]
			then
				case "$hwmode:$htmode" in
					*ng:*) htmode=HT20;;
					*na:*) htmode=HT40;;
					*ac:*) htmode=HT80;;
					*axg:*) htmode=HT20;;
					*beg:*) htmode=HT20;;
					*axa:*) htmode=HT80;;
					*bea:*) htmode=HT80;;
					*:*) htmode=HT20;
				esac
			fi

			if [ "$channel" == "auto" ]; then
				channel=
			else
				dpp_operating_class_setup "$htmode" "$channel" "$band"
				dpp_operating_class=$?
				echo "dpp_operating_class=$dpp_operating_class" > /dev/console
				channel="chan=$dpp_operating_class/$channel"
			fi

			run_wpa_cli() {
				ifname=$1
				ctrl_path=$2
				arguments=$3
				wpa_cli -i $ifname -p $ctrl_path $arguments
			}

			run_wpa_cli $ifname "$WPAD_VARRUN/wpa_supplicant-$ifname" "-a /lib/wifi/dpp-supplicant-update-uci -B"
			wpa_cli -i $ifname -p $WPAD_VARRUN/wpa_supplicant-$ifname dpp_bootstrap_remove \*
			wpa_cli -i $ifname -p $WPAD_VARRUN/wpa_supplicant-$ifname dpp_pkex_remove \*
			wpa_cli -i $ifname -p $WPAD_VARRUN/wpa_supplicant-$ifname dpp_configurator_remove \*
			if [ "$map" -ge 3 -a $band -ne 3 ]; then
				run_wpa_cli $ifname "$WPAD_VARRUN/wpa_supplicant-$ifname" "DPP_BOOTSTRAP_GEN $dpp_type $dpp_curve $dpp_info $dpp_key"
			else
				run_wpa_cli $ifname  "$WPAD_VARRUN/wpa_supplicant-$ifname" "DPP_BOOTSTRAP_GEN $dpp_type $dpp_curve  $channel mac=$(cat /sys/class/net/$ifname/address | sed 's/://g') $dpp_info $dpp_key"
			fi

			if [ -z "$map" ] || [ "$map" -eq 0 ]; then
				if [ "${type}" == "qrcode" ]
				then
					run_wpa_cli $ifname "$WPAD_VARRUN/wpa_supplicant-$ifname" "dpp_auth_init peer=1  role=enrollee"
				else
					run_wpa_cli $ifname "$WPAD_VARRUN/wpa_supplicant-$ifname" "dpp_pkex_add own=1 $pkex_identifier role=enrollee $pkex_code"
				fi
			fi
		fi
	fi
}
