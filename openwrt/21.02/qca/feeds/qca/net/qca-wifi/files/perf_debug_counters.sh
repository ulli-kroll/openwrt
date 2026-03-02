#
# Copyright (c) 2023 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.
#
#!/bin/sh

#constant Zero
readonly ZERO=0

#Run stats command between start_loop and end_loop times
start_loop=1
end_loop=2

#Dumps the output in output.txt
exec > "/output.txt"

#Enable sysctl for edma ring stats
echo 1 > proc/edma_ring_util_stats/enable_ring_stats_utilization

# print_bold ()
# prints the value
#
# ARGUMENTS:
#       $1: string
# OUTPUTS:
#       Writes String to STDOUT
print_bold () {
		echo "$1"
		echo "====================================================="
}

# set_console_intr_core_3 ()
# executes the command and sets console core 3
#
# OUTPUTS:
#       Writes String to STDOUT
set_console_intr_core_3 () {
	var=$(echo $(cat proc/interrupts | grep "msm_serial0") | head -c 2)
	eval "echo 8 > proc/irq/$var/smp_affinity"
}

# clear_stats ()
# executes command to clear the stats
#
# OUTPUTS:
#       Writes String to STDOUT
clear_stats () {
	#clear host stats
	cfg80211tool ath0 txrx_stats 257
	cfg80211tool ath1 txrx_stats 257
	cfg80211tool ath2 txrx_stats 257

	#clear ssdk stats
	echo 0 > /sys/ssdk/packet_counter

	#clear ring stats
	cat /sys/kernel/debug/qca-nss-dp/stats/clear_ring_stats
	echo ""
}

# sleep_between_commands ()
#
# updates sleep_time value
#
# GLOBALS:
# 	sleep_time
# ARGUMENTS:
#	$1: script mode(instant/avg)
#	$2: (optional) sleep_time
sleep_between_commands () {
	if [ -n "$2" ]; then
		sleep_time=$2
	else
		case $1 in
		"instant")
			sleep_time=3
			;;
		"avg")
			sleep_time=60
			;;
		esac
	fi
}

# exec_commands ()
#
# executes the stats commands
#
# GLOBALS:
#       sleep_time
# ARGUMENTS:
#       $1: string
#       $2: sleep_time
# OUTPUTS:
#       Writes String to STDOUT
exec_commands () {
	print_bold "$1"
	for i in $(seq $start_loop $end_loop)
		do
		echo " Iterartion:$i"
		echo " -------------"
		dmesg -c >> bootlog.txt
		eval "$1 && dmesg"
		sleep $2
		done
}

# collect_stats ()
#
# set of commands
#
# GLOBALS:
#       sleep_time
# ARGUMENTS:
#       $1: sleep_time
# OUTPUTS:
#       Writes String to STDOUT
collect_stats () {
	for x in $(seq 0 2)
		do
		athX=ath$x
		for y in 260 261 263
			do
			#txrx_stats
			exec_commands "cfg80211tool $athX txrx_stats $y" $1
			done
		#qdisc stats
		exec_commands "tc -s qdisc show dev $athX" $1
		done
	#sar
	if [[ $1 -eq $ZERO ]]; then
		print_bold "sar -n DEV 3 5"
		sar -n DEV 3 5
	else
		print_bold "sar -n DEV $1 5"
		sar -n DEV $1 5
	fi

	#Reo stats
	reo=$(echo "0xA3852C 0xA385A4 0xA3861C 0x94203C")
	for X in $(seq 0 2)
		do
		for Y in $reo
			do
			exec_commands "athdiag --get --wifi $X --quiet --address $Y" $1
			done
		done

	#packet counter stats
	exec_commands "cat /sys/ssdk/packet_counter" $1

	#ring stats
	for s in rx_ring_stats tx_ring_stats
		do
		exec_commands "cat /sys/kernel/debug/qca-nss-dp/stats/$s" $1
		done

	#Miami and Alder specific ports and eths
	board=$(cat /tmp/sysinfo/sdk_board_name)
	print_bold $board
	echo""

	if [[ $board == *"ipq9574"* ]]; then
		eth_max=5
		ports=$(echo $(seq 10 12) $(seq 15 15))
	elif [[ $board == *"ipq5332"* ]]; then
		eth_max=1
		ports=$(seq 8 9)
	fi

	for i in $(seq 0 $eth_max)
		do
		ethX="eth$i"
		#ethtool stats
		exec_commands "ethtool -S $ethX | grep pause" $1
		#qdisc stats
		exec_commands "tc -s qdisc show dev $ethX" $1
		done

	downlink_queue=$(seq 0 8 48)
	uplink_queue=$(seq 204 8 252)
	queue=$(echo "$downlink_queue $uplink_queue")
	for n in queue
		do
		#queue stats
		exec_commands "ssdk_sh qm cnt get $n" $1
		done

	for i in $ports
		do
		#ports stats
		exec_commands "ssdk_sh bm portcounter get $i" $1
		done

	#vp stats
	exec_commands "cat /sys/kernel/debug/qca-nss-ppe/ppe_vp/vp_stats" $1
}

# instantaneous_stats_output ()
#
# dumps stats
#
# GLOBALS:
#       sleep_time
#       date
#       ZERO
instantaneous_stats_output () {
	print_bold "Instantaneous stats output"

	#set console core3
	set_console_intr_core_3

	# Start logging
	echo "Script started at $(date)"

	clear_stats
	collect_stats $sleep_time

	# End logging
	echo "Script ending at $(date)"
}

# average_stats_output ()
#
# dumps stats
#
# GLOBALS:
#       sleep_time
#       date
#       ZERO
average_stats_output () {
	print_bold "Average stats output"

	#set console core3
	set_console_intr_core_3

	# Start logging
	print_bold "Script started at $(date)"

	clear_stats

	#loop 1
	start_loop=1
	end_loop=1
	collect_stats $ZERO

	#sleep time
	print_bold "sleep $sleep_time"
	sleep $sleep_time

	#loop 2
	start_loop=2
	end_loop=2
	collect_stats $ZERO

	# End logging
	print_bold "Script ended at $(date)"
}

# The script will pass $1 args denoting: instant, avg or inst_n_avg modes
choice=$1
case $choice in

# Pattern instant
"instant")
	sleep_between_commands "instant" $2
	instantaneous_stats_output
	;;

# Pattern avg
"avg")
	sleep_between_commands "avg" $2
	average_stats_output
	;;

# Pattern inst_n_avg
"inst_n_avg")
	sleep_between_commands "instant" $2
	instantaneous_stats_output
	sleep_between_commands "avg" $3
	average_stats_output
	;;

# Default Pattern
*)
	echo "command usage: perf_debug_counters.sh <instant|avg|inst_n_avg> [sleep_time]"
	;;
esac
