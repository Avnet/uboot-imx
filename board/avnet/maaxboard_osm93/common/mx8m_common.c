/*
 * Copyright (C) 2023 Avnet Embedded
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


#include <asm/mach-imx/sys_proto.h>
#include "mx8m_common.h"

const char* mx8m_get_plat_str(void)
{
	switch(get_cpu_type()) {
		case MXC_CPU_IMX8MQ:
		case MXC_CPU_IMX8MD:
		case MXC_CPU_IMX8MQL:
			return "imx8m";
		case MXC_CPU_IMX8MM:
		case MXC_CPU_IMX8MMD:
		case MXC_CPU_IMX8MML:
		case MXC_CPU_IMX8MMDL:
		case MXC_CPU_IMX8MMS:
		case MXC_CPU_IMX8MMSL:
			return "imx8mm";
		case MXC_CPU_IMX8MN:
		case MXC_CPU_IMX8MNDL:
		case MXC_CPU_IMX8MND:
		case MXC_CPU_IMX8MNSL:
		case MXC_CPU_IMX8MNS:
		case MXC_CPU_IMX8MNL:
			return "imx8mn";
		case MXC_CPU_IMX8MP:
		case MXC_CPU_IMX8MP6:
		case MXC_CPU_IMX8MPL:
		case MXC_CPU_IMX8MPD:
			return "imx8mp";
	}
	return "N/A";
}

const char* mx8m_get_proc_str(void)
{
	switch(get_cpu_type()) {
		case MXC_CPU_IMX8MQ:
			return "qc";
		case MXC_CPU_IMX8MD:
			return "dc";
		case MXC_CPU_IMX8MQL:
			return "qcl";

		case MXC_CPU_IMX8MN:
			return "imx8mn";
		case MXC_CPU_IMX8MND:
			return "imx8mnd";
		case MXC_CPU_IMX8MNL:
			return "imx8mnl";
		case MXC_CPU_IMX8MNDL:
			return "imx8mndl";
		case MXC_CPU_IMX8MNS:
			return "imx8mns";
		case MXC_CPU_IMX8MNSL:
			return "imx8mnsl";

		case MXC_CPU_IMX8MM:
			return "imx8mm";
		case MXC_CPU_IMX8MML:
			return "imx8mml";
		case MXC_CPU_IMX8MMD:
			return "imx8mmd";
		case MXC_CPU_IMX8MMDL:
			return "imx8mmdl";
		case MXC_CPU_IMX8MMSL:
			return "imx8mmsl";

		case MXC_CPU_IMX8MP:
			return "imx8mp";
		case MXC_CPU_IMX8MP6:
			return "imx8mp6";
		case MXC_CPU_IMX8MPL:
			return "imx8mpl";
		case MXC_CPU_IMX8MPD:
			return "imx8mpd";
	}
	return "N/A";
}
