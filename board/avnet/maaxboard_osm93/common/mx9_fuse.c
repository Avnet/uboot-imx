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

#include <linux/types.h>
#include <linux/errno.h>
#include <fuse.h>
#include "mx9_fuse.h"

int mx9_fuse_read_mac(t_fuse_mac_id id, t_eth_addr mac)
{
	u32 fuse_39_3, fuse_39_4, fuse_39_5;
	int ret;

	switch(id)
	{
		case FUSE_MAC1:
			ret = fuse_read(39, 3, &fuse_39_3);
			if (ret)
				return -EINVAL;

			ret = fuse_read(39, 4, &fuse_39_4);
			if (ret)
				return -EINVAL;

			mac[0] = (fuse_39_4 >>  8) & 0xff;
			mac[1] = (fuse_39_4 >>  0) & 0xff;
			mac[2] = (fuse_39_3 >> 24) & 0xff;
			mac[3] = (fuse_39_3 >> 16) & 0xff;
			mac[4] = (fuse_39_3 >>  8) & 0xff;
			mac[5] = (fuse_39_3 >>  0) & 0xff;

			break;

		case FUSE_MAC2:
			ret = fuse_read(39, 4, &fuse_39_4);
			if (ret)
				return -EINVAL;

			ret = fuse_read(39, 5, &fuse_39_5);
			if (ret)
				return -EINVAL;

			mac[0] = (fuse_39_5 >> 24) & 0xff;
			mac[1] = (fuse_39_5 >> 16) & 0xff;
			mac[2] = (fuse_39_5 >>  8) & 0xff;
			mac[3] = (fuse_39_5 >>  0) & 0xff;
			mac[4] = (fuse_39_4 >> 24) & 0xff;
			mac[5] = (fuse_39_4 >> 16) & 0xff;

			break;

		default:
			return -EINVAL;
	}

	return 0;
}

int mx9_fuse_write_mac(t_fuse_mac_id id, t_eth_addr mac, bool dry)
{
	int ret;
	u32 fuse_39_3, fuse_39_4, fuse_39_5;

	switch(id)
	{
		case FUSE_MAC1: {
			fuse_39_4 = (mac[0] << 8) | (mac[1] << 0);
			fuse_39_3 = (mac[2] << 24) | (mac[3] << 16) | (mac[4] << 8) | (mac[5] << 0);

			if (!dry) {
				ret = fuse_prog(39, 4, fuse_39_4);
				if (ret)
					return -EINVAL;

				ret = fuse_prog(39, 3, fuse_39_3);
				if (ret)
					return -EINVAL;
			}
			else {
				printf("OTP: fuse[39:4] 0x%08x\n", fuse_39_4);
				printf("OTP: fuse[39:3] 0x%08x\n", fuse_39_3);
			}

			break;
		}

		case FUSE_MAC2: {
			fuse_39_5 = (mac[0] << 24) | (mac[1] << 16) | (mac[2] << 8) | (mac[3] << 0);
			fuse_39_4 = (mac[4] << 24) | (mac[5] << 16);

			if (!dry) {
				ret = fuse_prog(39, 5, fuse_39_5);
				if (ret)
					return -EINVAL;

				ret = fuse_prog(39, 4, fuse_39_4);
				if (ret)
					return -EINVAL;
			}
			else {
				printf("OTP: fuse[39:5] 0x%08x\n", fuse_39_5);
				printf("OTP: fuse[39:4] 0x%08x\n", fuse_39_4);
			}

			break;
		}

		default:
			return -EINVAL;
	}

	return 0;
}
