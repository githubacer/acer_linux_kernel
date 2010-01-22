/*
 * arch/arm/mach-tegra/io.c
 *
 * Copyright (C) 2010 Google, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/kernel.h>
#include <linux/init.h>

#include <mach/hardware.h>
#include <asm/io.h>
#include <asm/page.h>
#include <asm/mach/map.h>

#include <mach/board.h>

static struct map_desc tegra_io_desc[] __initdata = {
	{
		.virtual = (unsigned long) IO_ADDRESS(IO_PPSB_PHYS),
		.pfn = __phys_to_pfn(IO_PPSB_PHYS),
		.length = SZ_1M,
		.type = MT_DEVICE,
	},
	{
		.virtual = (unsigned long) IO_ADDRESS(IO_APB_PHYS),
		.pfn = __phys_to_pfn(IO_APB_PHYS),
		.length = SZ_1M,
		.type = MT_DEVICE,
	},
	{
		.virtual = (unsigned long) IO_ADDRESS(IO_CPU_PHYS),
		.pfn = __phys_to_pfn(IO_CPU_PHYS),
		.length = SZ_1M,
		.type = MT_DEVICE,
	},
	{
		.virtual = (unsigned long) IO_ADDRESS(IO_EXCEP_PHYS),
		.pfn = __phys_to_pfn(IO_EXCEP_VIRT),
		.length = SZ_4K,
		.type = MT_DEVICE,
	}
};


void __init tegra_map_common_io(void)
{
	iotable_init(tegra_io_desc, ARRAY_SIZE(tegra_io_desc));
}
