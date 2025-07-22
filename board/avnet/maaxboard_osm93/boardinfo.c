/*
 * Copyright © 2024 AVNET Embedded
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the “Software”), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <common.h>
#include <malloc.h>
#include <errno.h>
#include <dm.h>
#include <i2c.h>
#include <i2c_eeprom.h>
#include "./common/boardinfo.h"
#include "./common/mx9_common.h"

int read_boardinfo(int offset, uint8_t *buffer, int size)
{
	struct udevice *eeprom;
	int ret;

	ret = i2c_get_chip_for_busnum(I2C_A, BI_EEPROM_I2C_ADDR, 1, &eeprom);
	if (ret < 0) {
		puts("ID EEPROM not found\n");
		return ret;
	}

	return i2c_eeprom_read(eeprom, offset, buffer, size);
}

int write_boardinfo(int offset, uint8_t *buffer, int size)
{
	struct udevice *eeprom;
	int ret;

	ret = i2c_get_chip_for_busnum(I2C_A, BI_EEPROM_I2C_ADDR, 1, &eeprom);
	if (ret < 0) {
		puts("ID EEPROM not found\n");
		return ret;
	}

	return i2c_eeprom_write(eeprom, offset, buffer, size);
}

const char* bi_get_form_factor(const board_info_t *bi)
{
	return "osm-sf";
}

const char* bi_get_platform(const board_info_t *bi)
{
	return mx9_get_plat_str();
}

const char* bi_get_processor(const board_info_t *bi)
{
	return mx9_get_proc_str();
}
