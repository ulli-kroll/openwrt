# Copyright (c) 2014, 2019, 2021, 2024 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# 2014 Qualcomm Atheros, Inc.
#
# All Rights Reserved.
# Qualcomm Atheros Confidential and Proprietary.
#

. /lib/functions.sh
. /lib/functions/service.sh

SERVICE_NAME=btconfig

btconfig_setup() {
	BTCONFIG_TIMEOUT_SECONDS=60
	BT_CONFIG_ONE_TIME_SETUP_FILE="/tmp/btconfig_onetime_setup_done"
	BT_CONFIG_ARGS=
	BT_PORT="/dev/ttyMSM1"
	BT_BAUDRATE="115200"
	PATCH_FILE="/bluetooth/msbtfw11.tlv"
	NVM_FILE="/bluetooth/msnv11.bin"
	BTCONFIG_BIN_FILE="/usr/sbin/btconfig"

	if [ -f ${BT_CONFIG_ONE_TIME_SETUP_FILE} ]; then
		logger -t btconfig "File ${BT_CONFIG_ONE_TIME_SETUP_FILE} exist"
		logger -t btconfig "btconfig setup already done"

		return 0
	fi

	append BT_CONFIG_ARGS "BT-PORT=${BT_PORT}"

	append BT_CONFIG_ARGS "BT-BAUDRATE=${BT_BAUDRATE}"

	if ! [ -f ${PATCH_FILE} ]; then
		logger -t btconfig "File ${PATCH_FILE} does not exist"
	fi
	append BT_CONFIG_ARGS "PATCH=${PATCH_FILE}"

	if ! [ -f ${NVM_FILE} ]; then
		logger -t btconfig "File ${NVM_FILE} does not exist"
	fi
	append BT_CONFIG_ARGS "NVM=${NVM_FILE}"

	append BT_CONFIG_ARGS "1 1"

	if ! [ -f ${BTCONFIG_BIN_FILE} ]; then
		logger -t btconfig "File ${BTCONFIG_BIN_FILE} does not exist"
	fi

	logger -t btconfig "Invoking ${BTCONFIG_BIN_FILE} using options ${BT_CONFIG_ARGS}"
	echo "Invoking ${BTCONFIG_BIN_FILE} using options ${BT_CONFIG_ARGS}"
	set -o pipefail
	timeout ${BTCONFIG_TIMEOUT_SECONDS} ${BTCONFIG_BIN_FILE} ${BT_CONFIG_ARGS} | logger -t btconfig

	EXIT_STATUS=$?
	if [ ${EXIT_STATUS} -eq "0" ]; then
		logger -t btconfig "btconfig completed successfully"
		echo "btconfig completed successfully"
	elif [ ${EXIT_STATUS} -eq "143" ]; then
		logger -t btconfig "btconfig timedout!"
		echo "btconfig timedout!"
	else
		echo "btconfig did not timeout. Something else(ret=${EXIT_STATUS}) went wrong."
	fi

	touch ${BT_CONFIG_ONE_TIME_SETUP_FILE}
	logger -t btconfig "btconfig setup done"
	echo "btconfig setup done"

	return ${EXIT_STATUS}
}

btconfig_teardown() {
	logger -t btconfig "teardown btconfig"
}
