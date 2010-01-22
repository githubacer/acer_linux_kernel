/*
 * arch/arm/mach-tegra/include/mach/iomap.h
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

#ifndef __MACH_TEGRA_IOMAP_H
#define __MACH_TEGRA_IOMAP_H

#include <mach/io.h>
#include <asm/sizes.h>

#define TEGRA_ARM_PERIF_BASE    	0x50040000
#define TEGRA_ARM_PERIF_SIZE    	SZ_8K

#define TEGRA_ARM_INT_DIST_BASE      	0x50041000
#define TEGRA_ARM_INT_DIST_SIZE      	SZ_4K

#define TEGRA_PRIMARY_ICTLR_BASE     	0x60004000
#define TEGRA_PRIMARY_ICTLR_SIZE     	SZ_64

#define TEGRA_SECONDARY_ICTLR_BASE   	0x60004100
#define TEGRA_SECONDARY_ICTLR_SIZE   	SZ_64

#define TEGRA_TERTIARY_ICTLR_BASE   	0x60004200
#define TEGRA_TERTIARY_ICTLR_SIZE   	SZ_64

#define TEGRA_QUATERNARY_ICTLR_BASE	0x60004300
#define TEGRA_QUATERNARY_ICTLR_SIZE	SZ_64

#define TEGRA_TMR1_BASE			0x60005000
#define TEGRA_TMR1_SIZE			SZ_8

#define TEGRA_TMR2_BASE			0x60005008
#define TEGRA_TMR2_SIZE			SZ_8

#define TEGRA_TMRUS_BASE		0x60005010
#define TEGRA_TMRUS_SIZE		SZ_64

#define TEGRA_TMR3_BASE			0x60005050
#define TEGRA_TMR3_SIZE			SZ_8

#define TEGRA_TMR4_BASE			0x60005058
#define TEGRA_TMR4_SIZE			SZ_8

#define TEGRA_CLK_RESET_BASE		0x60006000
#define TEGRA_CLK_RESET_SIZE		SZ_4K

#define TEGRA_FLOW_CTRL_BASE		0x60007000
#define TEGRA_FLOW_CTRL_SIZE		20

#define TEGRA_GPIO_BASE			0x6000D000
#define TEGRA_GPIO_SIZE			SZ_1K

#define TEGRA_EXCEPTION_VECTORS_BASE    0x6000F000
#define TEGRA_EXCEPTION_VECTORS_SIZE    SZ_4K

#define TEGRA_UARTA_BASE		0x70006000
#define TEGRA_UARTA_SIZE		SZ_64

#define TEGRA_UARTB_BASE		0x70006040
#define TEGRA_UARTB_SIZE		SZ_64

#define TEGRA_UARTC_BASE		0x70006200
#define TEGRA_UARTC_SIZE		SZ_256

#define TEGRA_UARTD_BASE		0x70006300
#define TEGRA_UARTD_SIZE		SZ_256

#define TEGRA_UARTE_BASE		0x70006400
#define TEGRA_UARTE_SIZE		SZ_256

#define TEGRA_NAND_BASE			0x70008000
#define TEGRA_NAND_SIZE			SZ_256

#define TEGRA_PMC_BASE  		0x7000E400
#define TEGRA_PMC_SIZE  		SZ_256

#define TEGRA_USB_PHYS		0xC5000000
#define TEGRA_USB_BASE		(IO_TO_VIRT(TEGRA_USB_PHYS))
#define TEGRA_USB_SIZE		SZ_16K

#define TEGRA_USB1_PHYS		0xC5004000
#define TEGRA_USB1_BASE		(IO_TO_VIRT(TEGRA_USB1_PHYS))
#define TEGRA_USB1_SIZE		SZ_16K

#define TEGRA_USB2_PHYS		0xC5008000
#define TEGRA_USB2_BASE		(IO_TO_VIRT(TEGRA_USB2_PHYS))
#define TEGRA_USB2_SIZE		SZ_16K


#endif
