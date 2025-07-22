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

#include <linux/errno.h>
#include "mac_tools.h"

int str_to_mac(char *str, t_eth_addr mac)
{
	char buf[18];

	if (strlen(str) == (2 * ARP_HLEN)) {
		int idx;
		for (idx=0; idx<ARP_HLEN; idx++) {
			buf[3 * idx] = str[2 * idx];
			buf[3 * idx + 1] = str[2 * idx + 1];
			buf[3 * idx + 2] = ':';
		}
		buf[17] = '\0';
	}
	else if (strlen(str) == (2 * ARP_HLEN + 5))
	{
		strcpy(buf, str);
	}
	else {
		printf("Failed, invalid MAC address format!\n");
		return -EINVAL;
	}

	string_to_enetaddr(buf, mac);

	return 0;
}

int mac_cmp(const t_eth_addr mac1, const t_eth_addr mac2)
{
	int idx;
	for (idx=0; idx<ARP_HLEN; idx++) {
		if(mac1[idx] != mac2[idx])
			return (-1);
	}
	return 0;
}