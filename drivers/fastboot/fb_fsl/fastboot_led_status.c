// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright 2024 Avnet
 */

#include "fastboot_led_status.h"



static struct gpio_desc fastboot_led_desc;
static int fastboot_led_init_flag = 0;

int fastboot_led_init(void)
{
	int ret = 0;
	fastboot_led_init_flag = 0;

	ret = dm_gpio_lookup_name("GPIO2_12", &fastboot_led_desc);
	if (ret)
		return ret;

	ret = dm_gpio_request(&fastboot_led_desc, "LED_RED");
	if (ret)
		return ret;

	ret = dm_gpio_set_dir_flags(&fastboot_led_desc, GPIOD_IS_OUT);
	if (ret)
		return ret;

	fastboot_led_init_flag = 1;

	ret = fastboot_led_on();
	if (ret)
		return ret;

	return ret;
}


int fastboot_led_toggle(void)
{
	int ret = 0;
	static int led_toggle_value = 0;

	if(fastboot_led_init_flag)
	{
		led_toggle_value = !led_toggle_value;
		ret = dm_gpio_set_value(&fastboot_led_desc, led_toggle_value);
		if (ret)
		{
			printf("%s,dm_gpio_set_value return error=%d\n", __func__, ret);
			return ret;
		}
	}

	return ret;
}


int fastboot_led_on(void)
{
	int ret = 0;

	if(fastboot_led_init_flag)
	{
		ret = dm_gpio_set_value(&fastboot_led_desc, 1);
		if (ret)
		{
			printf("%s,dm_gpio_set_value return error=%d\n", __func__, ret);
			return ret;
		}
	}

	return ret;
}

int fastboot_led_off(void)
{
	int ret = 0;

	if(fastboot_led_init_flag)
	{
		ret = dm_gpio_set_value(&fastboot_led_desc, 0);
		if (ret)
		{
			printf("%s,dm_gpio_set_value return error=%d\n", __func__, ret);
			return ret;
		}
	}

	return ret;
}
