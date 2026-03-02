# Copyright (c) 2023 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.

# code works for following boards*
# HAWKEYE :-
#  HK-AP-01 (3R)
# ALDER + WAIKIKI :-
# RDP433 - AP-AL02-C4 (3R)
# RDP454 - AP-AL02-C9 (4R)
#  RDP456 - AP-AL02-C16 (5R)

wireless_configurations()
{
	mode=$1
	board=$2
	radios=$3
	band=$4
	features=$5

	cat >> "$board"_"$mode".sh << EOF

# ********************* WIRELESS CONFIGURATIONS ********************************
EOF

	# configs for boards supporting 6G
	if [ "$band" != "9" ];then
		IFS=,
		for i in "$band"; do
			cat >> "$board"_"$mode".sh << EOF
uci set wireless.wifi$i.band='3'
EOF
		done

		cat >> "$board"_"$mode".sh << EOF

uci set wireless.qcawifi.ema_ap_support_wps_in_6ghz=1
uci set wireless.qcawifi.ema_ap_vendor_ie_size_low=0x55556666
uci set wireless.qcawifi.ema_ap_vendor_ie_size_high=0x55555555
#uci set wireless.qcawifi.enable_6ghz_sp_pwrmode_supp=1
#uci set wireless.qcawifi.afc_reg_no_action=1
uci set wireless.qcawifi.ema_ap_beacon_common_part_size=600
EOF
	fi

	cat >> "$board"_"$mode".sh << EOF

/usr/sbin/wifimeshconfig map
uci set wireless.qcawifi=qcawifi
uci set wireless.qcawifi.samessid_disable=1
uci commit wireless
wifi unload
sleep 5
wifi load
sleep 25
EOF

	flag=0
	while [[ "$flag" -ne "$radios" ]]; do
		cat >> "$board"_"$mode".sh << EOF

uci set wireless.wifi$flag.disabled=0
EOF
		let "flag++"
	done

	flag=0
	# CONTROLLER

	if [ "$mode" == "c" ]; then
		while [[ "$flag" -ne "$radios" ]]; do
			cat >> "$board"_c.sh << EOF

uci set wireless.wifi$flag.repacd_create_ctrl_fbss=1
uci set wireless.wifi$flag.repacd_create_ctrl_bbss=1
EOF
			let "flag++"
		done

	# AGENT ( temporary code)
	else
		# HK
		if [[ "$board" == AP-HK01-C1 ]]; then
			cat >> "$board"_a.sh << EOF

#uci set wireless.wifi1.repacd_map_bsta_preference=1
uci set wireless.wifi0.repacd_map_bsta_preference=2
EOF
		# WKK -3R
		elif [[ "$board" == "AP-AL02-C4*" ]]; then
			cat >> "$board"_a.sh << EOF

uci set wireless.wifi0.repacd_map_bsta_preference=1
uci set wireless.wifi1.repacd_map_bsta_preference=2
uci set wireless.wifi2.repacd_map_bsta_preference=3
EOF
		# WKK -4R staragent
		elif [[ "$board" == "AP-AL02-C9" ]];then
			cat >> "$board"_a.sh << EOF

uci set wireless.wifi0.repacd_map_bsta_preference=1
uci set wireless.wifi2.repacd_map_bsta_preference=3
uci set wireless.wifi3.repacd_map_bsta_preference=2
EOF
		# WKK - 5R
		elif [[ "$board" == "AP-AL02-C16" ]];then
			cat >> "$board"_a.sh << EOF

uci set wireless.wifi0.repacd_map_bsta_preference=0
uci set wireless.wifi2.repacd_map_bsta_preference=2

uci set wireless.wifi0.repacd_create_ctrl_fbss=1
uci set wireless.wifi1.repacd_create_ctrl_fbss=1
uci set wireless.wifi2.repacd_create_ctrl_fbss=0
uci set wireless.wifi3.repacd_create_ctrl_fbss=1
uci set wireless.wifi4.repacd_create_ctrl_fbss=1
EOF
		fi
	fi

	cat >> "$board"_"$mode".sh << EOF
uci commit wireless
EOF

	# FEATURES
	# wireless_features=11

	for i in $features; do
		if [ $i == $wireless_features ];then
			cat >> "$board"_"$mode".sh << EOF

#Advanced CAC
uci set wireless.wifi1.setNxtRadarFreq='5745'
uci set wireless.wifi2.setNxtRadarFreq='5745'
EOF
			break
		fi
	done
}

network_configurations()
{
	mode=$1
	board=$2
	radios=$3
	band=$4

	if [ "$mode" == "c" ]; then
		cat >> "$board"_c.sh << EOF

# ************************* NETWORK CONTROLLER CONFIGURATIONS ******************************

uci set network.lan.ipaddr='192.168.1.204'
uci set network.lan.netmask='255.255.255.0'
uci set network.lan.multicast_querier=1
uci set network.lan.igmp_snooping=1
uci set network.lan.ifname='eth1 eth2 eth3 eth4 eth5'
uci set network.wan.proto='dhcp'
uci set network.wan.ifname='eth0'
uci commit network
sleep 5
EOF
	else
		cat >> "$board"_a.sh << EOF

# ************************* NETWORK AGENT CONFIGURATIONS ******************************

uci set network.wan.proto='static'
uci set network.lan.proto=dhcp
uci set network.lan.multicast_querier=1
uci set network.lan.igmp_snooping=1
uci delete network.wan;
uci commit network
EOF
	fi
}

firewall_configurations()
{
	mode=$1
	board=$2
	radios=$3
	band=$4

	cat >> "$board"_"$mode".sh << EOF

# ************************* FIREWALL CONTROLLER CONFIGURATIONS ******************************
# FireWall lan

uci set firewall.lan_zone=zone
uci set firewall.lan_zone.masq=1

# FireWall lan1

uci delete firewall.lan1
uci set firewall.lan1=zone
uci set firewall.lan1.name=lan1
uci set firewall.lan1.network=lan1
uci set firewall.lan1.forward=REJECT
uci set firewall.lan1.output=ACCEPT
uci set firewall.lan1.input=REJECT
uci set firewall.lan1.masq=1

uci delete firewall.lan1_fwd
uci set firewall.lan1_fwd=forwarding
uci set firewall.lan1_fwd.src=lan1
uci setmm firewall.lan1_fwd.dest=wan

uci delete firewall.lan1_dhcp
uci set firewall.lan1_dhcp=rule
uci set firewall.lan1_dhcp.name=lan1_DHCP
uci set firewall.lan1_dhcp.src=lan1
uci set firewall.lan1_dhcp.target=ACCEPT
uci set firewall.lan1_dhcp.proto=udp
uci set firewall.lan1_dhcp.src_port=68
uci set firewall.lan1_dhcp.dest_port=67
uci set firewall.lan1_dhcp.family=ipv4

uci delete firewall.lan1_dns
uci set firewall.lan1_dns=rule
uci set firewall.lan1_dns.name=lan1_DNS
uci set firewall.lan1_dns.src=lan1
uci set firewall.lan1_dns.target=ACCEPT
uci set firewall.lan1_dns.proto=udp
uci set firewall.lan1_dns.dest_port=53

uci delete firewall.lan1_ltogaccess
uci set firewall.lan1_ltogaccess=rule
uci set firewall.lan1_ltogaccess.name='Block-lan-lan1-forwarding'
uci set firewall.lan1_ltogaccess.src='lan'
uci set firewall.lan1_ltogaccess.dest='lan1'
uci set firewall.lan1_ltogaccess.proto='0'
uci set firewall.lan1_ltogaccess.family='any'
uci set firewall.lan1_ltogaccess.target='REJECT'

uci delete firewall.lan1_gtolaccess
uci set firewall.lan1_gtolaccess=rule
uci set firewall.lan1_gtolaccess.name='Block-lan1-lan-forwarding'
uci set firewall.lan1_gtolaccess.src='lan1'
uci set firewall.lan1_gtolaccess.dest='lan'
uci set firewall.lan1_gtolaccess.proto='0'
uci set firewall.lan1_gtolaccess.family='any'
uci set firewall.lan1_gtolaccess.target='REJECT'

# FireWall lan2

uci delete firewall.lan2
uci set firewall.lan2=zone
uci set firewall.lan2.name=lan2
uci set firewall.lan2.network=lan2
uci set firewall.lan2.forward=REJECT
uci set firewall.lan2.output=ACCEPT
uci set firewall.lan2.input=REJECT
uci set firewall.lan2.masq=1

uci delete firewall.lan2_fwd
uci set firewall.lan2_fwd=forwarding
uci set firewall.lan2_fwd.src=lan2
uci set firewall.lan2_fwd.dest=wan

uci delete firewall.lan2_dhcp
uci set firewall.lan2_dhcp=rule
uci set firewall.lan2_dhcp.name=lan2_DHCP
uci set firewall.lan2_dhcp.src=lan2
uci set firewall.lan2_dhcp.target=ACCEPT
uci set firewall.lan2_dhcp.proto=udp
uci set firewall.lan2_dhcp.src_port=68
uci set firewall.lan2_dhcp.dest_port=67
uci set firewall.lan2_dhcp.family=ipv4

uci delete firewall.lan2_dns
uci set firewall.lan2_dns=rule
uci set firewall.lan2_dns.name=lan2_DNS
uci set firewall.lan2_dns.src=lan2
uci set firewall.lan2_dns.target=ACCEPT
uci set firewall.lan2_dns.proto=udp
uci set firewall.lan2_dns.dest_port=53

uci delete firewall.lan2_ltogaccess
uci set firewall.lan2_ltogaccess=rule
uci set firewall.lan2_ltogaccess.name='Block-lan-lan2-forwarding'
uci set firewall.lan2_ltogaccess.src='lan'
uci set firewall.lan2_ltogaccess.dest='lan2'
uci set firewall.lan2_ltogaccess.proto='0'
uci set firewall.lan2_ltogaccess.family='any'
uci set firewall.lan2_ltogaccess.target='REJECT'

uci delete firewall.lan2_gtolaccess
uci set firewall.lan2_gtolaccess=rule
uci set firewall.lan2_gtolaccess.name='Block-lan2-lan-forwarding'
uci set firewall.lan2_gtolaccess.src='lan2'
uci set firewall.lan2_gtolaccess.dest='lan'
uci set firewall.lan2_gtolaccess.proto='0'
uci set firewall.lan2_gtolaccess.family='any'
uci set firewall.lan2_gtolaccess.target='REJECT'
uci commit firewall
EOF
}

dhcp_configurations()
{
	mode=$1
	board=$2
	radios=$3
	band=$4

	if [ "$mode" == "c" ]; then
		cat >> "$board"_c.sh << EOF

# ************************* DHCP CONTROLLER CONFIGURATIONS ******************************

uci delete dhcp.lan1
uci set dhcp.lan1=dhcp
uci set dhcp.lan1.interface=lan1
uci set dhcp.lan1.start=100
uci set dhcp.lan1.limit=150
uci set dhcp.lan1.leasetime='12h'
uci set dhcp.lan1.dhcpv6=server
uci set dhcp.lan1.ra=server
uci commit dhcp

uci delete dhcp.lan2
uci set dhcp.lan2=dhcp
uci set dhcp.lan2.interface=lan2
uci set dhcp.lan2.start=100
uci set dhcp.lan2.limit=150
uci set dhcp.lan2.leasetime='12h'
uci set dhcp.lan2.dhcpv6=server
uci set dhcp.lan2.ra=server
uci commit dhcp
EOF

	else
		cat >> "$board"_a.sh << EOF

# ************************* DHCP AGENT CONFIGURATIONS ******************************

uci set dhcp.lan.ignore=1
uci delete dhcp.lan1
uci set dhcp.lan1=dhcp
uci set dhcp.lan1.ignore=1
uci delete dhcp.lan2
uci set dhcp.lan2=dhcp
uci set dhcp.lan2.ignore=1
uci commit dhcp
EOF
	fi
}

restart()
{
	mode=$1
	board=$2

	cat >> "$board"_"$mode".sh << EOF

/etc/init.d/network restart
/etc/init.d/firewall restart
/etc/init.d/dnsmasq restart
sleep 5
EOF
}

repacd_configurations()
{
	mode=$1
	board=$2
	radios=$3
	band=$4
	features=$5
	version=$6
	bbss=$7

	if [ $bbss == "in-built" ];then
		bbss_value=map-tst-bbss-$board-ssid
	else
		bbss_value=bbss
	fi

	cat >> "$board"_"$mode".sh << EOF

# ************************* REPACD CONFIGURATIONS ******************************

# Fast onboarding 1 - Enable, 0 - Disable
uci set repacd.MAPConfig.MapFastOnboarding=1

#Set Ezmesh='0' if wants to start ezmesh for proprietary functionality
uci set repacd.repacd.Ezmesh='1'

uci set repacd.repacd.Enable=1
uci set repacd.repacd.ConfigREMode='son'
uci set repacd.MAPConfig.Enable=1
uci set repacd.MAPConfig.FirstConfigRequired=1

uci set repacd.MAPConfig.TSUseSameBridgeMAC='0'
uci set repacd.MAPConfig.EnableSmartMonitorMode=1

uci set repacd.MAPWiFiLink.MaxValidBackhaulRSSI='-98'
uci set repacd.MAPWiFiLink.BackhaulRSSIThreshold_2='-98'
uci set repacd.MAPWiFiLink.BackhaulRSSIThreshold_5='-98'

# map-version

uci set repacd.MAPConfig.MapVersionEnabled=$version
uci set repacd.MAPConfig.MapTrafficSeparationEnable=1
uci set repacd.MAPConfig.NumberOfVLANSupported=3
EOF

	# AGENT
	if [ "$mode" == "a" ];then
		cat >> "$board"_a.sh << EOF
uci set repacd.MAPConfig.StartWithHighestPrefSTA=1
uci set repacd.MAPConfig.EnableMapTSLogs=1
EOF

	# CONTROLLER
	else
		if [ "$radios" == 5 ];then
		cat >> "$board"_c.sh << EOF

uci set_list repacd.repacd.BandMapping='2G 5G 5GL 6G 6GL'
EOF
		fi

		cat >> "$board"_c.sh << EOF

uci set repacd.MAPConfig.BSSInstantiationTemplate='scheme-d.conf'

# set 1 if don't want R1 co-exist with R2
uci set repacd.MAPConfig.R1AgentAssocDisAllowed=1
uci set repacd.MAPConfig.R2AgentAssocDisAllowed=0
uci set repacd.MAPConfig.CombinedR1R2Backhaul=0
uci set repacd.MAPConfig.VlanIDNwPrimary=10
uci set repacd.MAPConfig.VlanIDNwOne=20
uci set repacd.MAPConfig.VlanIDNwTwo=30

uci set repacd.MAPConfig.FronthaulKey='map-fbss'
uci set repacd.MAPConfig.BackhaulKey='map-bbss'

uci set repacd.MAPConfig.FronthaulSSID='12-0-map-fbss-$board-ssid'
uci set repacd.MAPConfig.FronthaulSSIDNwOne='12-0-map-fbss-$board-one'
uci set repacd.MAPConfig.FronthaulKeyNwOne='map-fbss-$board-one'
uci set repacd.MAPConfig.FronthaulSSIDNwTwo='12-0-map-fbss-$board-Two'
uci set repacd.MAPConfig.FronthaulKeyNwTwo='map-fbss-$board-Two'
uci set repacd.MAPConfig.BackhaulSSID=$bbss_value

uci set repacd.MAPConfig.BridgeProtoNwOne='static'
uci set repacd.MAPConfig.BridgeIPAddressNwOne='192.168.2.201'
uci set repacd.MAPConfig.BridgeProtoNwTwo='static'
uci set repacd.MAPConfig.BridgeIPAddressNwTwo='192.168.3.201'
EOF
	fi

	if [ $version == 3 ];then
		cat >> "$board"_"$mode".sh << EOF

#DPP Configs

uci set repacd.MAPConfig.OnboardingType='dpp'
uci set repacd.MAPConfig.EnableConfigServiceLogs=0
uci set repacd.MAPConfig.DPPDumpSecurityLog=0
EOF
	fi

	# check for authtype
	set_authtype_WPA2="false"

	# FEATURES
	# repacd_features=3,6,12,13,15,16

	for i in $features; do

		if [ $i == 15 ];then
			set_authtype_WPA2="true"
		fi

		case $i in
			# Start ezmesh- proprietary functionality
			3)	cat >> "$board"_"$mode".sh << EOF

#Set Ezmesh='0' if wants to start ezmesh for proprietary functionality
uci set repacd.repacd.Ezmesh='0'
EOF
			;;

			# Additional FH
			6)	if [[ $mode == c ]];then
					cat >> "$board"_c.sh << EOF

uci set repacd.MAPConfig.AdditionalFH="New_fbss1:11111111:1,New_fbss2:22222222:1,New_fbss3:33333333:0"
EOF
				fi
			;;

			# Enable SLO
			12)	cat >> "$board"_"$mode".sh << EOF

# SLO
uci set repacd.MAPConfig.EnableSLO='1'
# Enable REPACD to manipulate MLD MAC
uci set repacd.MAPConfig.mld_mac_addr="00:2A:F5:41:14:31"
EOF
				;;

			# Set no.of VLANs supported
			13)	read -p "No.of VLANs to be supported : " vlans
				cat >> "$board"_"$mode".sh << EOF
uci set repacd.MAPConfig.NumberOfVLANSupported=$vlans
EOF
				;;

			15)	# CONTROLLER
				# Set fronthaul / backhaul Authtype to WPA2
				if [ "$mode" == c ];then
					cat >> "$board"_"$mode".sh << EOF

#WPA3 you can configure either 0/1 based on your test requirement
#0- WPA2 authentication
#1- SAE Authentication (Front-haul vaps supports WPA3+mixed mode only)

uci set repacd.MAPConfig.FronthaulAuthType=0
uci set repacd.MAPConfig.FronthaulAuthTypeNwOne=0
uci set repacd.MAPConfig.FronthaulAuthTypeNwTwo=0
uci set repacd.MAPConfig.BackhaulAuthType=0
EOF

				# AGENT
				# auto-config STA
				else
					while [ 1 ];do
						read -p "Set backhaul authtype to WPA2 (0) or WPA3 (1) : " type
						if [ $type == 1 ] || [ $type == 0 ];then
							read -p "Enter backhaul SSID given in controller : " bbss_value
							cat >> "$board"_"$mode".sh << EOF

uci set repacd.MAPConfig.BackhaulAuthType=$type
uci set repacd.MAPConfig.BackhaulSSID=bbss_value
uci set repacd.MAPConfig.BackhaulKey='map-bbss'
EOF
						fi
					done
				fi
				;;

			# Enable 6G
			16)	cat >> "$board"_"$mode".sh << EOF

# 6G 1 - Enable, 0 - Disable
uci set repacd.repacd.Ezmesh6GEnable=1
EOF
				;;
		esac
	done


	if [ "$mode" == "c" ] && [ "$set_authtype_WPA2" == "false" ];then
		cat >> "$board"_"$mode".sh << EOF

#WPA3 you can configure either 0/1 based on your test requirement
#0- WPA2 authentication
#1- SAE Authentication (Front-haul vaps supports WPA3+mixed mode only)

uci set repacd.MAPConfig.FronthaulAuthType=1
uci set repacd.MAPConfig.FronthaulAuthTypeNwOne=1
uci set repacd.MAPConfig.FronthaulAuthTypeNwTwo=1
uci set repacd.MAPConfig.BackhaulAuthType=1
EOF
	fi
}

wsplcd_configurations()
{
	mode=$1
	board=$2
	radios=$3
	band=$4
	features=$5

	cat >> "$board"_"$mode".sh << EOF

# ************************* WSPLCD CONFIGURATIONS ******************************
EOF

	if [ "$mode" == "a" ];then
		cat >> "$board"_"$mode".sh << EOF

# Fast onboarding
uci set wsplcd.config.VapSpecificRestart=1
EOF
	fi

	# FEATURES
	# wsplcd_features=5,6,12
	for i in $features; do

		case $i in
			# Enable wsplcd debug
			5)	cat >> "$board"_"$mode".sh << EOF

uci set wsplcd.config.WriteDebugLogToFile=APPEND
uci set wsplcd.config.DebugLevel=DEBUG
uci commit wsplcd
EOF
				;;

			# Additional FH
			6)	cat >> "$board"_"$mode".sh << EOF

#Additional FH
uci set wsplcd.config.MapMaxBss=8
uci commit wsplcd
EOF
				;;

			# Enable SLO
			12)	cat >> "$board"_"$mode".sh << EOF

# SLO
uci set wsplcd.config.EnableSLO=1
EOF
				;;
		esac
	done
}

ezmesh_configurations()
{
	mode=$1
	board=$2
	radios=$3
	band=$4
	features=$5

	cat >> "$board"_"$mode".sh << EOF

# *********************  EZMESH  CONFIGURATIONS ********************************

# Backhaul steering
uci set ezmesh.MultiAP.EnableTopologyOpt=1
EOF

	if [[ $mode == c ]]; then
		cat >> "$board"_"$mode".sh << EOF
uci set ezmesh.MultiAP.TopologyOptMaxIdleWaitSecs='450'
uci set ezmesh.MultiAP.TopologyOptMinIncreasePercentage='5'

#if wants to check On-demand report then set 0 else set 1 to get periodic report
uci set ezmesh.MultiAP.EnableRptIndependentScan='1'
uci set ezmesh.MultiAP.EnableChannelScanRequest='1'

# active BH steering
uci set ezmesh.MultiAP.TopologyOptAllowActiveSteer='1'
EOF
	fi

	cat >> "$board"_"$mode".sh << EOF

# LegacyAPSteering
uci set ezmesh.Monitor.DisableMonitoring='0'
uci set ezmesh.Monitor.MonitorTimer='60'
uci set ezmesh.Monitor.MonitorResponseTimeout='5'
uci set ezmesh.Monitor.DisableSteeringMax11kUnfriendlyClients='0'
EOF

	# CONTROLLER
	if [ "$mode" == "c" ]; then
		cat >> "$board"_c.sh << EOF

uci set ezmesh.MultiAP.EnableChannelScanRequest='1'

# Enable highest bandwidth on each radio for channel selection
uci set ezmesh.MultiAP.ControllerChanSelection320MHz='1'
uci set ezmesh.MultiAP.ControllerChanSelection160MHz='1'
uci set ezmesh.MultiAP.ControllerChanSelection240MHz='1'
uci set ezmesh.MultiAP.ControllerChanSelection40MHzOn2G='1'
EOF
	fi

	# BOTH C/A
	cat >> "$board"_"$mode".sh << EOF

# Enable highest bandwidth on each radio for channel selection
uci set ezmesh.MultiAP.AgentChanSelection320MHz='1'
uci set ezmesh.MultiAP.AgentChanSelection160MHz='1'
uci set ezmesh.MultiAP.AgentChanSelection240MHz='1'
uci set ezmesh.MultiAP.AgentChanSelection40MHzOn2G='1'
EOF


	# FEATURES
	# ezmesh_features=1,2,4,11,14
	for i in $features; do
		case $i in
			# Enable Channel selection
			1)	echo -e  "\n1.Select one mode for Channel Selection
				1.Local operating mode
				2.Local preference mode
				3.Global preference mode
				"
				cat >> "$board"_"$mode".sh << EOF

#Channel Selection, configure one mode at one time, and agent have to be configured same mode as controller.
uci set ezmesh.MultiAP.EnableChannelSelection=1
EOF
				read cmode
				case $cmode in
					# default is local operating mode
					1)  ;;
					2)	cat >> "$board"_"$mode".sh << EOF

#Local preference mode

uci set ezmesh.MultiAP.BkScanIntervalMin=15
uci set ezmesh.MultiAP.EnableChanPrefQuery=1
uci set ezmesh.MultiAP.ChannelSelectionDelaySec=360
uci set ezmesh.MultiAP.ChannelSelectionOnGlobalPref=0
uci set ezmesh.MultiAP.UseCBSRankForHomeChan='1'
EOF
						;;
					3)  cat >> "$board"_"$mode".sh << EOF

# Global preference mode

uci set ezmesh.MultiAP.BkScanIntervalMin=15
uci set ezmesh.MultiAP.EnableChanPrefQuery=1
uci set ezmesh.MultiAP.ChannelSelectionDelaySec=360
uci set ezmesh.MultiAP.ChannelSelectionOnGlobalPref=1
uci set ezmesh.MultiAP.UseCBSRankForHomeChan='1'
EOF
						;;
				esac
				;;

			# Mesh 2.0 mode -Dual/Framework
			2)	if [ "$mode" == "c" ];then
					echo -e  "\n2.select one mode for Mesh 2.0
						1.Dual mode
						2.Framework mode
						3.exit
						"
						read meshmode
						case $meshmode in
							1)	cat >> "$board"_$mode.sh << EOF

uci set ezmesh.config.DualAlid='1'
uci set ezmesh.config.DualAlidIntf='eth2'
uci set ezmesh.config.DualAlidM='Dual'
EOF
								;;

							2)	cat >> "$board"_"$mode".sh << EOF

# Framework mode
uci set ezmesh.config.FrameworkOnlyMode=1
EOF
								;;
							3)	continue ;;
						esac
				else
					cat >> "$board"_"$mode".sh << EOF

# Framework mode
uci set ezmesh.config.FrameworkOnlyMode=1
EOF
				fi
				;;

			# Enable ezmesh Debug
			4)	cat >> "$board"_"$mode".sh << EOF

# Enable logging only for debug purpose
uci set ezmesh.config.ezmeshlog='1'
EOF
				;;

			11)	cat >> "$board"_"$mode".sh << EOF

# Advanced CAC
uci set ezmesh.MultiAP.fallback_freq_5GH='5745'
uci set ezmesh.MultiAP.fallback_freq_5GL='5745'
EOF
				;;


			14)	# CONTROLLER
				if [ "$mode" == c ];then
					cat >> "$board"_c.sh << EOF

# if wants to check On-demand report then set 0 else set 1 to get periodic report
uci set ezmesh.MultiAP.EnableRptIndependentScan='1'
uci set ezmesh.MultiAP.EnableChannelScanRequest='1'
EOF

				# AGENT
				else
					cat >> "$board"_a.sh << EOF

#Set 1 if wants to scan bootup time only
uci set ezmesh.MultiAP.EnableBootOnlyScan='1'
uci set ezmesh.MultiAP.ChanScanIntervalMin='6'
EOF
				fi
				;;
		esac
	done
}

ezlbd_configurations()
{
	mode=$1
	board=$2
	radios=$3
	band=$4
	features=$5

	cat >> "$board"_"$mode".sh << EOF


# ************************* EZLBD CONFIGURATIONS ******************************

#Enabling Multi11k is a choice on homogeneous n/w and mandatory for heterogeneous n/w
#default Multi11k flag is set to ZERO (disabled) in lbd config
#set Delayed11kRequesttimer='2' for congested environment
uci set ezlbd.config.EnableMulti11kRequest=1
uci set ezlbd.Estimator_Adv.Delayed11kRequesttimer='1'

uci set ezlbd.Estimator_Adv.EnableRcpiTypeClassification=0

# legacyAPsteering
uci set ezlbd.APSteer.DisableSteering11kUnfriendlyClients='0'
uci set ezlbd.APSteer.DisableMonitoringLegacyClients='0'
uci set ezlbd.APSteer.DisableSteeringInactiveLegacyClients='0'
uci set ezlbd.APSteer.DisableSteeringActiveLegacyClients='0'

uci set ezlbd.Offload.MUOverloadThreshold_W2='95'
uci set ezlbd.Offload.MUOverloadThreshold_W5='85'
uci set ezlbd.Offload.MUOverloadThreshold_W6='85'
uci set ezlbd.Offload.MUSafetyThreshold_W2='90'
uci set ezlbd.Offload.MUSafetyThreshold_W5='80'
uci set ezlbd.Offload.MUSafetyThreshold_W6='80'
uci set ezlbd.IAS.MaxPollutionTime='150'
uci set ezlbd.SteerExec.SteeringProhibitTime='60'
uci set ezlbd.SteerExec.BTMSteeringProhibitShortTime='30'
uci set ezlbd.Estimator_Adv.11kProhibitTimeShort='30'
uci set ezlbd.Estimator_Adv.11kProhibitTimeLong='60'
uci set ezlbd.SteerExec_Adv.SteeringUnfriendlyTime='90'
uci set ezlbd.SteerExec_Adv.BTMUnfriendlyTime='90'
uci set ezlbd.SteerExec_Adv.BlacklistTime='120'
uci set ezlbd.config.InactDetectionFromTx='1'
uci set ezlbd.StaDB.MarkAdvClientAsDualBand=1
uci set ezlbd.SteerExec_Adv.StartInBTMActiveState=1
uci set ezlbd.Estimator_Adv.EnableRcpiTypeClassification=0
EOF


	# FEATURES
	# ezlbd_features=7,8,9,10,17
	for i in $features; do
		case $i in

			# Configure client classification
			7)	cat >> "$board"_"$mode".sh << EOF

# If configuring Client classification then don't configure regular ezlbd parameters
uci set ezlbd.APSteer.LowRSSIAPSteerThreshold_SIG_W5='20 25'
uci set ezlbd.APSteer.LowRSSIAPSteerThreshold_SIG_W2='45 55'
uci set ezlbd.Estimator_Adv.11kProhibitTimeShort='30 10'
uci set ezlbd.Offload.OffloadingMinRSSI='20 30'
uci set ezlbd.SteerExec.SteeringProhibitTime='300 180'
uci set ezlbd.SteerExec.BTMSteeringProhibitShortTime='30 10'
uci set ezlbd.SteerExec.DisableLegacySteering='0 0'
uci set ezlbd.StaDB.MarkAdvClientAsDualBand='1 1'
uci set ezlbd.SteerExec_Adv.SteeringUnfriendlyTime='600 80'
uci set ezlbd.SteerExec_Adv.MaxSteeringUnfriendly='604800 120'
uci set ezlbd.ActiveSteer.TxRateXingThreshold_UG='50000 70000'
uci set ezlbd.ActiveSteer.TxRateXingThreshold_DG='2000 3000'
uci set ezlbd.ActiveSteer.RateRSSIXingThreshold_UG='30 40'
uci set ezlbd.ActiveSteer.RateRSSIXingThreshold_DG='0 15'
uci set ezlbd.IdleSteer.RSSISteeringPoint_UG='20 30'
uci set ezlbd.IdleSteer.RSSISteeringPoint_DG='5 15'
uci set ezlbd.StaDB_Adv.LegacyUpgradeAllowedCnt='0 1'
uci set ezlbd.StaDB_Adv.LegacyUpgradeMonitorDur='2100 120'
uci set ezlbd.SteerExec_Adv.LegacyUpgradeUnfriendlyTime='300 60'
uci set ezlbd.SteerExec_Adv.TargetLowRSSIThreshold_W5='15 15'
uci set ezlbd.SteerExec_Adv.TargetLowRSSIThreshold_W2='5 5'
uci set ezlbd.SteerExec_Adv.AuthRejMax='3 3'
uci set ezlbd.IdleSteer.NormalInactTimeout='10 10'
uci set ezlbd.Estimator_Adv.BcnrptActiveDuration='50 60'
uci set ezlbd.Estimator_Adv.11kProhibitTimeLong='300 20'
uci set ezlbd.IdleSteer.AuthAllow='0 1'
uci set ezlbd.Estimator_Adv.RSSIDiff_EstW5FromW2='-15 -15'
uci set ezlbd.Estimator_Adv.RSSIDiff_EstW2FromW5='5 5'
EOF
				;;

			8)	cat >> "$board"_"$mode".sh << EOF

# AckRssi
uci set ezlbd.config.EnableAckRSSI=1
uci set ezlbd.config.LogModCounter=10
EOF
				;;

			9)	cat >> "$board"_"$mode".sh << EOF

# Enable RCPI
uci set ezlbd.Estimator_Adv.EnableRcpiTypeClassification=1
EOF
				;;

			10)	cat >> "$board"_"$mode".sh << EOF

# Enable ezlbd persistant feature using the following configuration:
# mkdir /etc/persist
# touch /etc/persist/Persist.datlan
uci set ezlbd.Persist.PersistFile=/etc/persist/Persist.dat
uci set ezlbd.Persist.PersistPeriod=10
uci commit ezlbd
EOF
				;;

			17)	while [ 1 ];do
					read -p $'\nChoose prefered band number\n1. 5G band \n2. 6G band\n' pband
					if [ $pband == 1 ];then
						cat >> "$board"_"$mode".sh << EOF

# 1 = 5G Band, 2 = 6G Band
uci set ezlbd.config.PreferredSteeringBand=1
EOF
						break
					elif [ $pband == 2 ];then
						cat >> "$board"_"$mode".sh << EOF

# 1 = 5G Band, 2 = 6G Band
uci set ezlbd.config.PreferredSteeringBand=2
EOF
						break
					else
						continue
					fi
				done
				;;
		esac

	done
}

# START

# GET BOARD MODEL DETAILS

model=`cat /tmp/sysinfo/model`
echo "

Your board_model is $model"
#Qualcomm Technologies, Inc. IPQ8074/AP-HK01-C1
board=`echo $model | sed 's/.*\///'`
echo "board_name : "$board

# GET RADIO DETAILS

radios=`uci show wireless | grep -c wifi-device`
echo "Number of radios : "$radios

echo -e "\n\n ********** STARTING CONFIGURATIONS ************ \n\n"

# Band variable set to dummy(9) for identifying radios not supporting 6G
band=9

# BAND INFO FOR 6G
sixGradios=AP-AL02-C4,AP-AL02-C16,AP-AL02-C9
IFS=,
for i in $sixGradios;do
	if [ "$board" == $i ];then
		if [ $radios == 5 ];then
			# since pentaband it has 2 radios for 6g L and 6g H
			read -p "Enter band numbers for 6G low and 6G high ( eg: 0,1 for wifi0,wifi1 ) " band
		else
			read -p "Enter band number where 6G is enabled? ( eg: 0 for wifi0 ) " band
		fi
	fi
done

# MAP ENABLED VERSION
read -p "Enter which map version to enable " version

# ASK FOR MODE - C / A
while [ 1 ]; do
	read -p "Choose one controller(c) / agent(a) ? " mode
	if [ $mode =  c ]; then
		break
	elif [ $mode = a ]; then
		break
	else
		continue
	fi
done

# create script file
cd configs/ ; touch "$board"_"$mode".sh ;  chmod +x "$board"_"$mode".sh ;

echo -e "\n\n ********** APPLYING CONFIGURATIONS **************\n\n
choose required features nums separated by comma ( eg: 1,5,10)

1.Enable Channel selection"
if [ $mode == c ];then
	echo "2.Set Mesh 2.0 mode to Dual/Framework (default full)"
else
	echo "2.Set Mesh 2.0 mode to Framework (default full)"
fi
echo "3.Start ezmesh- proprietary functionality
4.Enable ezmesh Debug
5.Enable wsplcd debug
6.Additional FH
7.Configure client classification( wont configure regular ezbld params )
8.Enable AckRssi
9.Enable RCPI
10.Enable ezlbd persistant feature
11.Advanced CAC
12.Enable SLO
13.Set no.of VLANs supported ( default : 3)"

if [ $mode == "a" ];then
	echo "14.Enable Boot-only scan
15.Auto-config STA ( instead of manual WPS push)"
else
	echo "14.Get On-demand scan report
15.Set fronthaul / backhaul Authtype to WPA2 ( default : WPA3)"
fi

if [[ "$band" != "9" ]]; then
	echo "16.Enable 6G
17.Choose prefered steering band"
fi

read -p "Enter feature numbers : " features
IFS=,

# if user choose auto-config STA, he is prompted for backhaul SSID
# for manual WPS push, in-built SSID is taken
bbss="in-built"

# AUTO-CONFIG STA OR MANUAL WPS PUSH
if [ $mode == "c" ];then
	while [ 1 ]; do
		read -p "Do you want to auto-config STA? ( y / n ) : " auto_config
		if [ $auto_config ==  "y" ]; then
			read -p "Enter backhaul SSID for Controller : " bbss
			break
		elif [ $auto_config == "n" ]; then
			break
		else
			continue
		fi
	done
fi

wireless_configurations $mode $board $radios "$band" "$features"
network_configurations $mode $board $radios "$band"
if [ $mode == "c" ];then
	firewall_configurations $mode $board $radios "$band"
fi
dhcp_configurations $mode $board $radios "$band"

restart $mode $board

repacd_configurations $mode $board $radios "$band" "$features" $version $bbss
wsplcd_configurations $mode $board $radios "$band" "$features"
ezmesh_configurations $mode $board $radios "$band" "$features"
ezlbd_configurations $mode $board $radios "$band" "$features"

cat >> "$board"_"$mode".sh << EOF

uci commit wsplcd
uci commit ezlbd
uci commit ezmesh
uci commit repacd
uci commit
/etc/init.d/repacd start

EOF

# RUN REQUIRED CONFIG AND STORE OUTPUT IN A LOG FILE
: '
chmod +x $board_$mode.sh ; ./"$board"_"$mode".sh > ../output/$board_$mode_$( date '+%Y-%m-%d_%H-%M-%S' ).log 2>&1 &
wait
sleep 15
echo -e "\n\n\n*** CONFIGURATION COMPLETED ***\n\n\n"
'

# test
chmod +x test.sh ; ./test.sh > ../output/test_"$mode"_$( date '+%Y-%m-%d_%H-%M-%S' ).log 2>&1 &
echo -e "\n\n\n*** CONFIGURATION COMPLETED ***\n\n\n"


