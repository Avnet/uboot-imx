// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright 2024 Avnet
 */
#include <asm/gpio.h>

#ifndef FASTBOOT_LED_STATUS_H
#define FASTBOOT_LED_STATUS_H

int fastboot_led_init(void);
int fastboot_led_toggle(void);
int fastboot_led_on(void);
int fastboot_led_off(void);



#endif	// FASTBOOT_LED_STATUS_H
