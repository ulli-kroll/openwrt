/*
 * (C) Copyright 2010
 * Jason Kridner <jkridner@beagleboard.org>
 *
 * Based on cmd_led.c patch from:
 * http://www.mail-archive.com/u-boot@lists.denx.de/msg06873.html
 * (C) Copyright 2008
 * Ulf Samuelsson <ulf.samuelsson@atmel.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <config.h>
#include <command.h>
#include <dm.h>
#include <dm/uclass.h>
#include <led.h>
#include <errno.h>
#include <dm/uclass-internal.h>


static const char *const state_label[] = {
	[LEDST_OFF]	= "off",
	[LEDST_ON]	= "on",
	[LEDST_TOGGLE]	= "toggle",
#ifdef CONFIG_LED_BLINK
	[LEDST_BLINK]	= "blink",
#endif
};

enum led_state_t get_led_cmd(char *var)
{
	int i;

	for (i = 0; i < LEDST_COUNT; i++) {
		if (!strncmp(var, state_label[i], strlen(var)))
			return i;
	}

	return -1;

}

static int show_led_state(struct udevice *dev)
{
	int ret;

	ret = led_get_state(dev);
	if (ret >= LEDST_COUNT)
		ret = -EINVAL;
	if (ret >= 0)
		printf("%s\n", state_label[ret]);

	return ret;
}

static int list_leds(void)
{
	struct udevice *dev;
	int ret;

	for (uclass_find_first_device(UCLASS_LED, &dev);dev;uclass_find_next_device(&dev)) 
	{
		struct led_uclass_plat *plat = dev_get_uclass_platdata(dev);

		if (!plat->label)
			continue;

		printf("%-15s ", plat->label);
		if (device_active(dev))
		{
			ret = show_led_state(dev);
			if (ret < 0)
				printf("Error %d\n", ret);
		} 
		else
		{
			printf("<inactive>\n");
		}
	}

	return 0;
}

#if 0
/*
 * LED drivers providing a blinking LED functionality, like the
 * PCA9551, can override this empty weak function
 */
void __weak __led_blink(led_id_t mask, int freq)
{
}
#endif

int do_led (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
#if 0
	int i, match = 0;
	enum led_cmd cmd;
	int freq;

	/* Validate arguments */
	if ((argc < 3) || (argc > 4))
		return CMD_RET_USAGE;

	cmd = get_led_cmd(argv[2]);
	if (cmd < 0) {
		return CMD_RET_USAGE;
	}

	for (i = 0; led_commands[i].string; i++) {
		if ((strcmp("all", argv[1]) == 0) ||
		    (strcmp(led_commands[i].string, argv[1]) == 0)) {
			match = 1;
			switch (cmd) {
			case LED_ON:
				if (led_commands[i].on)
					led_commands[i].on();
				else
					__led_set(led_commands[i].mask,
							  STATUS_LED_ON);
				break;
			case LED_OFF:
				if (led_commands[i].off)
					led_commands[i].off();
				else
					__led_set(led_commands[i].mask,
							  STATUS_LED_OFF);
				break;
			case LED_TOGGLE:
				if (led_commands[i].toggle)
					led_commands[i].toggle();
				else
					__led_toggle(led_commands[i].mask);
				break;
			case LED_BLINK:
				if (argc != 4)
					return CMD_RET_USAGE;

				freq = simple_strtoul(argv[3], NULL, 10);
				__led_blink(led_commands[i].mask, freq);
			}
			/* Need to set only 1 led if led_name wasn't 'all' */
			if (strcmp("all", argv[1]) != 0)
				break;
		}
	}

	/* If we ran out of matches, print Usage */
	if (!match) {
		return CMD_RET_USAGE;
	}

	return 0;
#endif
	enum led_state_t cmd;
	const char *led_label;
	struct udevice *dev;
#ifdef CONFIG_LED_BLINK
	int freq_ms = 0;
#endif
	int ret;

	/* Validate arguments */
	if (argc < 2)
		return CMD_RET_USAGE;
	led_label = argv[1];
	if (strncmp(led_label, "list", 4) == 0)
		return list_leds();

	cmd = argc > 2 ? get_led_cmd(argv[2]) : LEDST_COUNT;
#ifdef CONFIG_LED_BLINK
	if (cmd == LEDST_BLINK) {
		if (argc < 4)
			return CMD_RET_USAGE;
		freq_ms = dectoul(argv[3], NULL);
	}
#endif
	ret = led_get_by_label(led_label, &dev);
	if (ret)
	{
		printf("LED '%s' not found (err=%d)\n", led_label, ret);
		return CMD_RET_FAILURE;
	}
	switch (cmd) {
	case LEDST_OFF:
	case LEDST_ON:
	case LEDST_TOGGLE:
		ret = led_set_state(dev, cmd);
		break;
#ifdef CONFIG_LED_BLINK
	case LEDST_BLINK:
		ret = led_set_period(dev, freq_ms);
		if (!ret)
			ret = led_set_state(dev, LEDST_BLINK);
		break;
#endif
	case LEDST_COUNT:
		printf("LED '%s': ", led_label);
		ret = show_led_state(dev);
		break;
	}
	if (ret < 0)
	{
		printf("LED '%s' operation failed (err=%d)\n", led_label, ret);
		return CMD_RET_FAILURE;
	}

	return 0;

}

U_BOOT_CMD(
	led, 4, 1, do_led,
	"["
#ifdef CONFIG_BOARD_SPECIFIC_LED
#ifdef STATUS_LED_BIT
	"0|"
#endif
#ifdef STATUS_LED_BIT1
	"1|"
#endif
#ifdef STATUS_LED_BIT2
	"2|"
#endif
#ifdef STATUS_LED_BIT3
	"3|"
#endif
#ifdef STATUS_LED_BIT4
	"4|"
#endif
#ifdef STATUS_LED_BIT5
	"5|"
#endif
#endif
#ifdef STATUS_LED_GREEN
	"green|"
#endif
#ifdef STATUS_LED_YELLOW
	"yellow|"
#endif
#ifdef STATUS_LED_RED
	"red|"
#endif
#ifdef STATUS_LED_BLUE
	"blue|"
#endif
	"all] [on|off|toggle|blink] [blink-freq in ms]",
	"[led_name] [on|off|toggle|blink] sets or clears led(s)"
);
