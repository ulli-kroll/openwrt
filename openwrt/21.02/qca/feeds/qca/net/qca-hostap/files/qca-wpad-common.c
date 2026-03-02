/*
 *Copyright (c) 2023 Qualcomm Technologies, Inc.
 *All Rights Reserved.
 *Confidential and Proprietary - Qualcomm Technologies, Inc
*/

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

extern int hostapd_unified_main(int argc, char **argv);
extern int wpa_supplicant_unified_main(int argc, char **argv);

struct wpad_cmd {
	const char *cmd;
	int (*handler)(int argc, char *argv[]);
};

static const struct wpad_cmd wpad_commands[] = {
	{ "hostapd", hostapd_unified_main},
	{ "wpa_supplicant", wpa_supplicant_unified_main},
	{ "hostapd-macsec", hostapd_unified_main},
	{ "wpa_supplicant-macsec", wpa_supplicant_unified_main},
	{ NULL, NULL}
};

int main(int argc,char **argv)
{
	const struct wpad_cmd *cmd;
	bool found = false;

	if (!(argc > 1)) {
		fprintf(stderr, "Invalid number of arguments %d\n", argc);
		return -1;
	}

	/* check matching commands */
	cmd = wpad_commands;
	while (cmd->cmd) {
		if (strncasecmp(cmd->cmd, argv[0], strlen(argv[0])) == 0) {
			cmd->handler(argc, argv);
			found = true;
			break;
		}
		cmd++;
	}
	if (!found)
		fprintf(stderr, "Invalid command.\nUsage: %s wpa_supplicant|hostapd [<arguments>]\n", argv[0]);
	return 0;
}
