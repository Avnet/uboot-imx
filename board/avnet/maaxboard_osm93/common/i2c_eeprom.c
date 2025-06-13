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

#include <common.h>
#include <config.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <i2c.h>
#include <command.h>
#include <linux/delay.h>
#include "i2c_eeprom.h"

//#define __DEBUG__

#ifndef CONFIG_SYS_EEPROM_PAGE_WRITE_BITS
	#define CONFIG_SYS_EEPROM_PAGE_WRITE_BITS	8
#endif
#define	EEPROM_PAGE_SIZE		(1 << CONFIG_SYS_EEPROM_PAGE_WRITE_BITS)
#define	EEPROM_PAGE_OFFSET(x)	((x) & (EEPROM_PAGE_SIZE - 1))

#if defined(__DEBUG__)
static void __dump(const char *label, uchar *buffer, unsigned len)
{
	int idx;
	uchar *ptr = buffer;

	printf ("%s:\n", label);

	for (idx=0; idx<len; idx++, ptr++) {
		if (idx && (idx%16 == 0))
			printf ("\n");
		printf ("%02x ", *ptr);
	}
	printf ("\n");
}
#endif

__weak int i2c_eeprom_write_enable(const i2c_eeprom_t *eeprom, int state)
{
	return 0;
}

static int i2c_eeprom_addr(const i2c_eeprom_t *eeprom, unsigned offset, uchar *addr)
{
	unsigned blk_off;

	blk_off = offset & 0xff;	/* block offset */

	if (eeprom->alen == 1) {
		addr[0] = offset >> 8;		/* block number */
		addr[1] = blk_off;			/* block offset */
	}
	else {
		addr[0] = offset >> 16;		/* block number */
		addr[1] = offset >>  8;		/* upper address octet */
		addr[2] = blk_off;			/* lower address octet */
	}

	addr[0] |= eeprom->dev_addr;	/* insert device address */

	return 0;
}

static int i2c_eeprom_len(const i2c_eeprom_t *eeprom, unsigned offset, unsigned end)
{
	unsigned len = end - offset;
	unsigned blk_off = offset & 0xff;
	unsigned maxlen = EEPROM_PAGE_SIZE - EEPROM_PAGE_OFFSET(blk_off);

	if (maxlen > eeprom->rw_blk_size)
		maxlen = eeprom->rw_blk_size;

	if (len > maxlen)
		len = maxlen;

	return len;
}

static int i2c_eeprom_rw_block(const i2c_eeprom_t *eeprom, unsigned offset, uchar *addr,
		uchar *buffer, unsigned len, bool read)
{
	int ret = 0;


#if defined(__DEBUG__)
	if (!read)
		__dump("Write", buffer, len);

#endif

#if CONFIG_IS_ENABLED(DM_I2C)
	struct udevice *dev;

	ret = i2c_get_chip_for_busnum(eeprom->bus_id, addr[0], eeprom->alen, &dev);
	if (ret) {
		printf("No EEPROM @ bus %d address 0x%02x\n", eeprom->bus_id, eeprom->dev_addr);
		return -ENODEV;
	}

	if (read)
		ret = dm_i2c_read(dev, offset, buffer, len);
	else
		ret = dm_i2c_write(dev, offset, buffer, len);

#else

	if (read)
		ret = i2c_read(addr[0], offset, eeprom->alen, buffer, len);
	else
		ret = i2c_write(addr[0], offset, eeprom->alen, buffer, len);
#endif

	if (ret)
		ret = 1;

#if defined(__DEBUG__)
	if (read)
		__dump("Read", buffer, len);

#endif

	return ret;
}

static int i2c_eeprom_rw(const i2c_eeprom_t *eeprom, unsigned offset, uchar *buffer,
	unsigned cnt, bool read)
{
	unsigned end = offset + cnt;
	unsigned len;
	int rcode = 0;
	uchar addr[3];

	while (offset < end) {
		i2c_eeprom_addr(eeprom, offset, addr);
		len = i2c_eeprom_len(eeprom, offset, end);

		rcode = i2c_eeprom_rw_block(eeprom, offset, addr, buffer, len, read);

		buffer += len;
		offset += len;

		if (!read)
			mdelay(eeprom->write_delay_ms);
	}

	return rcode;
}

int i2c_eeprom_read(const i2c_eeprom_t *eeprom, unsigned offset, uchar *buffer, unsigned cnt)
{
	int ret;
#if CONFIG_IS_ENABLED(SYS_I2C_LEGACY)
	if (eeprom->bus_id >= 0)
		i2c_set_bus_num(eeprom->bus_id);
#endif

	/*
	 * Read data until done or would cross a page boundary.
	 * We must write the address again when changing pages
	 * because the next page may be in a different device.
	 */
	ret = i2c_eeprom_rw(eeprom, offset, buffer, cnt, 1);

#if defined(__DEBUG__)
	__dump("Read (common)", buffer, cnt);
#endif

	return ret;
}

int i2c_eeprom_write(const i2c_eeprom_t *eeprom, unsigned offset, uchar *buffer, unsigned cnt)
{
	int ret;

#if defined(__DEBUG__)
	__dump("Write (common)", buffer, cnt);
#endif

#if CONFIG_IS_ENABLED(SYS_I2C_LEGACY)
	if (eeprom->bus_id >= 0)
		i2c_set_bus_num(eeprom->bus_id);
#endif

	i2c_eeprom_write_enable(eeprom, 1);

	/*
	 * Write data until done or would cross a write page boundary.
	 * We must write the address again when changing pages
	 * because the address counter only increments within a page.
	 */
	ret = i2c_eeprom_rw(eeprom, offset, buffer, cnt, 0);

	i2c_eeprom_write_enable(eeprom, 0);

	return ret;
}
