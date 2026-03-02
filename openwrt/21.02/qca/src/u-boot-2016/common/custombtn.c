#include <common.h>
#include <command.h>
#include <dm.h>
#include <button.h>
#include <poller.h>
#include <dm/uclass-internal.h>
#include <dm/device.h>
#include <led.h>

DECLARE_GLOBAL_DATA_PTR;

static int resetbtn_pressed = 0;

int recovery_btn_check(void)
{
	return resetbtn_pressed;
}

void led_config_control(const char *cmd, const char *name, const char *arg)
{
	const char *led = NULL;
	char buf[128];
	int config_node = -1;

	config_node = fdt_path_offset(gd->fdt_blob, "/config");
	
	if (config_node < 0)
	{
		return;
	}

	led = fdt_getprop(gd->fdt_blob, config_node, name, NULL);

	if (!led)
		return;

	sprintf(buf, "%s %s %s", cmd, led, arg);

	run_command(buf, 0);	
}

static void gpio_power_clr(void)
{
	int config_node = -1;
	int power_gpio;

	config_node = fdt_path_offset(gd->fdt_blob, "/config");
	
	if (config_node < 0) {
		return;
	}

	power_gpio = fdtdec_get_uint(gd->fdt_blob,config_node, "gpio_power_clr", 0);

	if (power_gpio == 0)
		return;

	gpio_set_value(power_gpio, 0);

}

void led_control(const char *cmd, const char *name, const char *arg)
{
	char buf[128];

	sprintf(buf, "%s %s %s", cmd, name, arg);

	run_command(buf, 0);
}


static int do_custombtn(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	struct udevice *dev;
	int ret, counter = 0;
	const char *button_label = NULL;
	ulong ts;

	gpio_power_clr();
	
	for (uclass_find_first_device(UCLASS_LED, &dev);dev;uclass_find_next_device(&dev)) 
	{
		struct led_uclass_plat *plat = dev_get_uclass_platdata(dev);

		if (!plat->label)
			continue;
			
		led_control("led", plat->label, "on");
	}

	mdelay(500);

	for (uclass_find_first_device(UCLASS_LED, &dev);dev;uclass_find_next_device(&dev))
	{
		struct led_uclass_plat *plat = dev_get_uclass_platdata(dev);

		if (!plat->label)
			continue;
		led_control("led", plat->label, "off");
	}

	mdelay(500);

	led_config_control("led", "system_led", "on");

	button_label = "reset";

	ret = button_get_by_label(button_label, &dev);
	if (ret) 
	{
		printf("Button '%s' not found\n", button_label);
		return CMD_RET_SUCCESS;
	}

	if (!button_get_state(dev))
	{
		return CMD_RET_SUCCESS;
	}

	printf("RESET button is pressed for: %2d second(s)", counter++);

	ts = get_timer(0);

	while (button_get_state(dev) && counter < 3)
	{
		if (get_timer(ts) < 1000)
			continue;

		ts = get_timer(0);

		printf("\b\b\b\b\b\b\b\b\b\b\b\b%2d second(s)", counter++);
	}

	printf("\n");

	if (counter == 3) 
	{
		led_config_control("ledblink", "reset_led", "500");
		led_config_control("ledblink", "hwdog_trigger", "5000");
		resetbtn_pressed = 1;
	}

	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(
	custombtn, 1, 0, do_custombtn,
	"custom button check",
	""
);
