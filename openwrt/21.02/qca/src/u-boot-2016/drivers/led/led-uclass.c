/*
 * Copyright (c) 2015 Google, Inc
 * Written by Simon Glass <sjg@chromium.org>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <dm.h>
#include <errno.h>
#include <led.h>
#include <dm/root.h>
#include <dm/uclass-internal.h>

int led_get_by_label(const char *label, struct udevice **devp)
{
	struct udevice *dev;
	struct uclass *uc;
	int ret;

	ret = uclass_get(UCLASS_LED, &uc);
	if (ret)
		return ret;
	uclass_foreach_dev(dev, uc) {
		struct led_uclass_plat *uc_plat = dev_get_uclass_platdata(dev);

		/* Ignore the top-level LED node */
		if (uc_plat->label && !strcmp(label, uc_plat->label))
		{
			*devp = dev;
			return 0;
		}
	}


	return -ENODEV;
}

int led_set_state(struct udevice *dev, enum led_state_t state)
{
	struct led_ops *ops = led_get_ops(dev);

	if (!ops->set_state)
		return -ENOSYS;

	return ops->set_state(dev, state);
}

enum led_state_t led_get_state(struct udevice *dev)
{
	struct led_ops *ops = led_get_ops(dev);

	if (!ops->get_state)
		return -ENOSYS;

	return ops->get_state(dev);
}

#ifdef CONFIG_LED_BLINK
int led_set_period(struct udevice *dev, int period_ms)
{
	struct led_ops *ops = led_get_ops(dev);

	if (!ops->set_period)
		return -ENOSYS;

	return ops->set_period(dev, period_ms);
}
#endif

int led_set_on(struct udevice *dev, int on)
{
	struct led_ops *ops = led_get_ops(dev);

	if (!ops->set_on)
		return -ENOSYS;

	return ops->set_on(dev, on);
}

UCLASS_DRIVER(led) = {
	.id		= UCLASS_LED,
	.name		= "led",
	.per_device_platdata_auto_alloc_size = sizeof(struct led_uclass_plat),
};
