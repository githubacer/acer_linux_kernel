/*
 * arch/arm/mach-tegra/include/mach/io.h
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

#ifndef __MACH_TEGRA_IO_H
#define __MACH_TEGRA_IO_H

#define IO_SPACE_LIMIT 0xffffffff

/* On TEGRA, many peripherals are very closely packed in
 * two 256MB io windows (that actually only use about 64KB
 * at the start of each).
 *
 * We will just map the first 1MB of each window (to minimize
 * pt entries needed) and provide a macro to transform physical
 * io addresses to an appropriate void __iomem *.
 *
 */

#define IO_EXCEP_PHYS   0x00000000
#define IO_EXCEP_VIRT   0xFE000000

#define IO_CPU_PHYS     0x50000000
#define IO_CPU_VIRT     0xFC000000

#define IO_PPSB_PHYS	0x60000000
#define IO_PPSB_VIRT	0xFE200000

#define IO_APB_PHYS	0x70000000
#define IO_APB_VIRT	0xFE300000

#define IO_TO_VIRT(n) ( \
	(((n) & 0xF0000000) == IO_PPSB_PHYS ? IO_PPSB_VIRT :	\
         ((n) & 0xF0000000) == IO_APB_PHYS ? IO_APB_VIRT :	\
         ((n) & 0xF0000000) == IO_CPU_PHYS ? IO_CPU_VIRT :	\
         IO_EXCEP_VIRT)						\
	| ((n) &  0x000FFFFF))

#ifndef __ASSEMBLER__

#define IO_ADDRESS(n) ((void __iomem *) IO_TO_VIRT(n))

static inline void __iomem *__io(unsigned long addr)
{
	return (void __iomem *)addr;
}
#define __io(a)         __io(a)
#define __mem_pci(a)    (a)

#endif

#endif
