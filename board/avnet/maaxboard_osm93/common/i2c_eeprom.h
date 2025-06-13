#ifndef __MSC_I2C_EEPROM_H__
#define __MSC_I2C_EEPROM_H__
/*
 * Copyright (C) 2021 AVNET Embedded, MSC Technologies GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

typedef struct i2c_eeprom {
	unsigned	bus_id;
	unsigned	dev_addr;
	unsigned	alen;
	unsigned	rw_blk_size;
	unsigned	write_delay_ms;
} i2c_eeprom_t;

int i2c_eeprom_read(const i2c_eeprom_t *eeprom, unsigned offset, uchar *buffer, unsigned cnt);
int i2c_eeprom_write(const i2c_eeprom_t *eeprom, unsigned offset, uchar *buffer, unsigned cnt);

#endif /* __MSC_I2C_EEPROM_H__ */
