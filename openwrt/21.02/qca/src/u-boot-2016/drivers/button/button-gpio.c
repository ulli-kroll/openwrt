// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2020 Philippe Reynes <philippe.reynes@softathome.com>
 */

#include <common.h>
#include <button.h>
#include <dm.h>
#include <dm/lists.h>
#include <dm/uclass-internal.h>
#include <asm/gpio.h>

DECLARE_GLOBAL_DATA_PTR;

struct button_gpio_priv {
	struct gpio_desc gpio;
};

static enum button_state_t button_gpio_get_state(struct udevice *dev)
{
	int ret;
	struct button_uc_plat *uc_plat = dev_get_uclass_platdata(dev);

	if (!uc_plat)
		return BUTTON_OFF;

	ret = gpio_get_value(uc_plat->gpio);
	if (ret < 0)
		return ret;

	return ret ? BUTTON_OFF : BUTTON_ON;
}

static int button_gpio_probe(struct udevice *dev)
{
	struct button_uc_plat *uc_plat = dev_get_uclass_platdata(dev);
	struct button_gpio_priv *priv = dev_get_priv(dev);

	/* Ignore the top-level button node */
	if (!uc_plat->label)
		return 0;

	priv->gpio.dev = dev;

/*	ret = gpio_request_by_name(dev, "gpios", 0, &priv->gpio, GPIOD_IS_IN);
	if (ret)
		return ret;
*/
	return 0;
}

static int button_gpio_remove(struct udevice *dev)
{
#ifdef CONFIG_DM_GPIO
	/*
	 * The GPIO driver may have already been removed. We will need to
	 * address this more generally.
	 */
	if (!IS_ENABLED(CONFIG_SANDBOX)) {
		struct button_gpio_priv *priv = dev_get_priv(dev);

		if (dm_gpio_is_valid(&priv->gpio))
			dm_gpio_free(dev, &priv->gpio);
	}
#endif
	return 0;
}

extern int qca_gpio_config(int);

static int button_gpio_bind(struct udevice *parent)
{
	const void *blob = gd->fdt_blob;
	struct udevice *dev;
	int node;
	int ret;

	for (node = fdt_first_subnode(blob, parent->of_offset);
	     node > 0;node = fdt_next_subnode(blob, node)) 
	{
		struct button_uc_plat *uc_plat;
		const char *label;

		label = fdt_getprop(blob, node, "label", NULL);
		if (!label) {
			printf("%s: node %s has no label\n", __func__,
			      fdt_get_name(blob, node, NULL));
			return -EINVAL;
		}
		ret = device_bind_driver_to_node(parent, "button_gpio",
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

static const struct button_ops button_gpio_ops = {
	.get_state	= button_gpio_get_state,
};

static const struct udevice_id button_gpio_ids[] = {
	{ .compatible = "gpio-keys" },
	{ .compatible = "gpio-keys-polled" },
	{ }
};

U_BOOT_DRIVER(button_gpio) = {
	.name		= "button_gpio",
	.id		= UCLASS_BUTTON,
	.of_match	= button_gpio_ids,
	.ops		= &button_gpio_ops,
	.priv_auto_alloc_size	= sizeof(struct button_gpio_priv),
	.bind		= button_gpio_bind,
	.probe		= button_gpio_probe,
	.remove		= button_gpio_remove,
};
