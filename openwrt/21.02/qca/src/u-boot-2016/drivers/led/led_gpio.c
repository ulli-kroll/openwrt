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
#include <asm/gpio.h>
#include <dm/lists.h>

DECLARE_GLOBAL_DATA_PTR;

struct led_gpio_priv {
	struct gpio_desc gpio;
};

static int gpio_led_set_state(struct udevice *dev, enum led_state_t state)
{
	struct led_uclass_plat *uc_plat = dev_get_uclass_platdata(dev);
	int ret;

	if (!uc_plat)
		return 0;

	switch (state) {
	case LEDST_OFF:
	case LEDST_ON:
		break;
	case LEDST_TOGGLE:
		ret = gpio_get_value(uc_plat->gpio);
		if (ret < 0)
			return ret;
		state = !ret;
		break;
	default:
		return -ENOSYS;
	}

	gpio_set_value(uc_plat->gpio, state);
	return 0;
}

static enum led_state_t gpio_led_get_state(struct udevice *dev)
{
	struct led_uclass_plat *uc_plat = dev_get_uclass_platdata(dev);
	int ret;

	ret = gpio_get_value(uc_plat->gpio);
	if (ret < 0)
		return ret;

	return ret ? LEDST_ON : LEDST_OFF;
}


static int gpio_led_set_on(struct udevice *dev, int on)
{
	struct led_gpio_priv *priv = dev_get_priv(dev);
	struct led_uclass_plat *uc_plat = dev_get_uclass_platdata(dev);

	if(priv)
	{
		if (!dm_gpio_is_valid(&priv->gpio))
			return -EREMOTEIO;
	}

	gpio_set_value(uc_plat->gpio, on);

	return 0;
}

static int led_gpio_probe(struct udevice *dev)
{
	struct led_uclass_plat *uc_plat = dev_get_uclass_platdata(dev);
	struct led_gpio_priv *priv = dev_get_priv(dev);

	/* Ignore the top-level LED node */
	if (!uc_plat->label)
		return 0;

	priv->gpio.dev = dev;
	return 0;

/*	return gpio_request_by_name(dev, "gpios", 0, &priv->gpio, GPIOD_IS_OUT);*/
}

static int led_gpio_remove(struct udevice *dev)
{
	/*
	 * The GPIO driver may have already been removed. We will need to
	 * address this more generally.
	 */
#ifdef CONFIG_DM_GPIO
	struct led_gpio_priv *priv = dev_get_priv(dev);

	if (dm_gpio_is_valid(&priv->gpio))
		dm_gpio_free(dev, &priv->gpio);
#endif

	return 0;
}

extern int qca_gpio_config(int);

static int led_gpio_bind(struct udevice *parent)
{
	const void *blob = gd->fdt_blob;
	struct udevice *dev;
	int node;
	int ret;

	for (node = fdt_first_subnode(blob, parent->of_offset);
	     node > 0;
	     node = fdt_next_subnode(blob, node)) {
		struct led_uclass_plat *uc_plat;
		const char *label;

		label = fdt_getprop(blob, node, "label", NULL);
		if (!label) {
			debug("%s: node %s has no label\n", __func__,
			      fdt_get_name(blob, node, NULL));
			return -EINVAL;
		}
		ret = device_bind_driver_to_node(parent, "gpio_led",
						 fdt_get_name(blob, node, NULL),
						 node, &dev);
		if (ret)
			return ret;
		uc_plat = dev_get_uclass_platdata(dev);
		uc_plat->label = label;
		uc_plat->gpio = fdtdec_get_uint(blob,node, "gpio", 0);

		qca_gpio_config(node);
	}


	return 0;
}

static const struct led_ops gpio_led_ops = {
	.set_on		= gpio_led_set_on,
	.set_state	= gpio_led_set_state,
	.get_state	= gpio_led_get_state,
};

static const struct udevice_id led_gpio_ids[] = {
	{ .compatible = "gpio-leds" },
	{ }
};

U_BOOT_DRIVER(led_gpio) = {
	.name	= "gpio_led",
	.id	= UCLASS_LED,
	.of_match = led_gpio_ids,
	.ops	= &gpio_led_ops,
	.priv_auto_alloc_size = sizeof(struct led_gpio_priv),
	.bind	= led_gpio_bind,
	.probe	= led_gpio_probe,
	.remove	= led_gpio_remove,
};
