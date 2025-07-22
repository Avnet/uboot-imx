#ifndef __MSC_MX9_FUSE_H__
#define __MSC_MX9_FUSE_H__

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
#include "mac_tools.h"

typedef enum {
	FUSE_MAC1,
	FUSE_MAC2,
} t_fuse_mac_id;

int mx9_fuse_read_mac(t_fuse_mac_id id, t_eth_addr mac);
int mx9_fuse_write_mac(t_fuse_mac_id id, t_eth_addr mac, bool dry);

#endif /* __MSC_MX9_FUSE_H__ */
