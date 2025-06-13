/*
 * Copyright (C) 2023 Avnet Embedded, MSC Technologies GmbH
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

#include <linux/ctype.h>
#include "string.h"

char *str_tolower(char *str)
{
	int idx;
	for (idx = 0; str[idx]; idx++)
		str[idx] = tolower(str[idx]);
	return str;
}

char *str_toupper(char *str)
{
	int idx;
	for (idx = 0; str[idx]; idx++)
		str[idx] = toupper(str[idx]);
	return str;
}
