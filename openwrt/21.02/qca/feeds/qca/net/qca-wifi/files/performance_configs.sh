#
# Copyright (c) 2022-2024 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.
#
#!/bin/sh
#


#Enable Skb Recycler explicitly for 512M profile.
[ -e /proc/device-tree/MP_512 ] && echo 1 >  proc/net/skb_recycler/skb_recycler_enable

#Increase Max skb recycler buffer count per CPU pool
echo "16384" > /proc/net/skb_recycler/max_skbs

#Reduce Max skb recycler buffer count per CPU pool for 256M or 512M profile to 2048.
[ -e /proc/device-tree/MP_256 ] || [ -e /proc/device-tree/MP_512 ] && echo "2048" > /proc/net/skb_recycler/max_skbs

#Enable PPE RFS feature for fair flow round robin across all CPUs
echo 1 > /sys/sfe/ppe_rfs_feature

#Configure System CPU to Max clock speed
echo "performance" > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
echo "performance" > /sys/devices/system/cpu/cpu1/cpufreq/scaling_governor
echo "performance" > /sys/devices/system/cpu/cpu2/cpufreq/scaling_governor
echo "performance" > /sys/devices/system/cpu/cpu3/cpufreq/scaling_governor

#Disable WLAN extended Statistics collection
cfg80211tool wifi0 enable_ol_stats 0
cfg80211tool wifi1 enable_ol_stats 0
cfg80211tool wifi2 enable_ol_stats 0
cfg80211tool wifi3 enable_ol_stats 0

#Disable Generic receive offload(GRO) on interfaces
ethtool -K eth0 gro off
ethtool -K eth1 gro off
ethtool -K eth4 gro off
ethtool -K eth5 gro off
ethtool -K ath0 gro off
ethtool -K ath1 gro off
ethtool -K ath2 gro off
ethtool -K ath3 gro off
ethtool -K mld0 gro off
ethtool -K mld1 gro off
ethtool -K mld2 gro off
ethtool -K mld3 gro off

#Disable Wdiag log
cfg80211tool wifi0 dl_loglevel 0xffff0006
cfg80211tool wifi1 dl_loglevel 0xffff0006
cfg80211tool wifi2 dl_loglevel 0xffff0006
cfg80211tool wifi3 dl_loglevel 0xffff0006
cfg80211tool wifi0 dcs_enable 0
cfg80211tool wifi1 dcs_enable 0
cfg80211tool wifi2 dcs_enable 0
cfg80211tool wifi3 dcs_enable 0

#Offload checksum calculation
ethtool -K ath0 tx on
ethtool -K ath1 tx on
ethtool -K ath2 tx on
ethtool -K ath3 tx on
ethtool -K ath0.sta1 tx on
ethtool -K ath1.sta1 tx on
ethtool -K ath2.sta1 tx on
ethtool -K ath3.sta1 tx on
ethtool -K mld0 tx on
ethtool -K mld1 tx on
ethtool -K mld2 tx on
ethtool -K mld3 tx on
