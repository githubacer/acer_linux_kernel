/*
 * arch/arm/mach-tegra/include/mach/uncompress.h
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

#ifndef __MACH_TEGRA_UNCOMPRESS_H
#define __MACH_TEGRA_UNCOMPRESS_H

#include <linux/types.h>
#include <linux/serial_reg.h>

#include "hardware.h"

static void putc(int c)
{
	volatile u8 *uart = (volatile u8 *)(0x70006300);
	int shift = 2;

        while (!(uart[UART_LSR << shift] & UART_LSR_THRE))
                barrier();
        uart[UART_TX << shift] = c;

}

static inline void flush(void)
{
}

static inline void konk_delay(int delay)
{
	int i;

	for(i = 0; i < (1000 * delay); i++) {
		barrier();
	}
}


static inline void arch_decomp_setup(void)
{
	volatile u8 *uart = (volatile u8 *)(0x70006300);
	volatile u32 *addr;
	int shift = 2;

/*
	addr = (volatile u32 *)0x70000014;
	*addr &= ~(1<<29);

	addr = (volatile u32 *)0x70000084;
	*addr &= ~(3<<2);

	addr = (volatile u32 *)0x700000b0;
	*addr &= ~(3<<24);

	konk_delay(5);

*/
	addr = (volatile u32 *)0x600061c0;
	*addr = 0;

	addr = (volatile u32 *)0x60006018;
	*addr |= (1<<1);

	konk_delay(5);

	addr = (volatile u32 *)0x6000600c;
	*addr &= ~(1<<1);

	konk_delay(5);


	uart[UART_LCR << shift] |= UART_LCR_DLAB;
	uart[UART_DLL << shift] = 0x7C;
	uart[UART_DLM << shift] = 0x0;
	uart[UART_LCR << shift] = 3;
}

static inline void arch_decomp_wdog(void)
{
}

#endif
