/*
 * arch/arm/mach-tegra/include/mach/debug-macro.S
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

#include <mach/io.h>

	.macro  addruart,rx
        mrc     p15, 0, \rx, c1, c0
        tst     \rx, #1                 @ MMU enabled?
        ldreq   \rx, =IO_APB_PHYS       @ physical
        ldrne   \rx, =IO_APB_VIRT        @ virtual
        orr     \rx, \rx, #0x6300
	.endm

	.macro senduart,rd,rx
	.endm

	.macro waituart,rd,rx
	.endm

	.macro busyuart,rd,rx
	.endm

