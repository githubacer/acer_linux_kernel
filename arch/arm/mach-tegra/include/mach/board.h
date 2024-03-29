/*
 * arch/arm/mach-tegra/include/mach/board.h
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

#ifndef __MACH_TEGRA_BOARD_H
#define __MACH_TEGRA_BOARD_H

#include <linux/types.h>

void __init tegra_add_devices(void);
void __init tegra_map_common_io(void);
void __init tegra_init_irq(void);
void __init tegra_init_gpio(void);
void __init tegra_init_clock(void);
void __init tegra_init_timer(void);
#endif
