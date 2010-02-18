/*
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
#include <linux/platform_device.h>
#include <linux/serial_8250.h>
#include <linux/clk.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/dma-mapping.h>
#include <linux/fsl_devices.h>

#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/time.h>
#include <asm/setup.h>

#include <mach/io.h>
#include <mach/iomap.h>
#include <mach/board.h>
#include <mach/irqs.h>
#include <mach/iomap.h>
#include <mach/nand.h>

#include <linux/usb/android_composite.h>

/* NVidia bootloader tags */
#define ATAG_NVIDIA		0x41000801

#define ATAG_NVIDIA_RM			0x1
#define ATAG_NVIDIA_DISPLAY		0x2
#define ATAG_NVIDIA_FRAMEBUFFER		0x3
#define ATAG_NVIDIA_CHIPSHMOO		0x4
#define ATAG_NVIDIA_CHIPSHMOOPHYS	0x5
#define ATAG_NVIDIA_PRESERVED_MEM_0	0x10000
#define ATAG_NVIDIA_PRESERVED_MEM_N	2
#define ATAG_NVIDIA_FORCE_32		0x7fffffff

struct tag_tegra {
        __u32 bootarg_key;
        __u32 bootarg_len;
        char bootarg[1];
};

static int __init parse_tag_nvidia(const struct tag *tag)
{

	return 0;
}
__tagtable(ATAG_NVIDIA, parse_tag_nvidia);

static struct sys_timer tegra_timer = {
	.init = tegra_init_timer,
};

static struct tegra_nand_chip_parms nand_chip_parms[] = {
	// Samsung K5E2G1GACM
	[0] = {
		.vendor_id   = 0xEC,
		.device_id   = 0xAA,
		.capacity    = 256,
		.timing      = {
			.trp		= 21,
			.trh		= 15,
			.twp		= 21,
			.twh		= 15,
			.tcs		= 31,
			.twhr		= 60,
			.tcr_tar_trr	= 20,
			.twb		= 100,
			.trp_resp	= 30,
			.tadl		= 100,
		},
	},
	// Hynix H5PS1GB3EFR
	[1] = {
		.vendor_id   = 0xAD,
		.device_id   = 0xDC,
		.capacity    = 512,
		.timing      = {
			.trp		= 12,
			.trh		= 10,
			.twp		= 12,
			.twh		= 10,
			.tcs		= 20,
			.twhr		= 80,
			.tcr_tar_trr	= 20,
			.twb		= 100,
			.trp_resp	= 20,
			.tadl		= 70,
		},
	},
};

/* Current layout is:
 *
 * BCT @ 0 (0x300000)        -- boot config table
 * PT  @ 0x300000 (0x1000)   -- partition table
 * EBT @ 0x301000 (0x100000) -- bootloader
 * BMP @ 0x401000 (0x148c)   -- rgb565 bitmap
 * WAV @ 0x40248c (0x2a000)  -- wav audio clip
 * ARG @ 0x42c48c (0x800)    -- ??
 * DRM @ 0x42cc8c (0x19000)  -- bleh?
 * UIP @ 0x445c8c (0x800)    -- update information partition
 * USP @ 0x44648c (0x600000) -- update staging partition
 * USR @ 0xa4648c (THE REST) -- <available>
 *
 * What we will do is we will actually just skip the first 16MB, and just
 * mark it as vendor, and then layout our partitions.
 *
 * so:
 *
 *
 */
static struct mtd_partition harmony_nand_partitions[] = {
	[0] = {
		.name		= "recovery",
		.offset		= 0x1b80*0x800,
		.size		= 0xa00*0x800,
		.mask_flags	= MTD_WRITEABLE, /* r/o */
	},
	[1] = {
		.name		= "boot",
		.offset		= 0x2680*0x800,
		.size		= 0x1000*0x800,
	},
	[2] = {
		.name		= "system",
		.offset		= 0x3780*0x800,
		.size		= 0xef40*0x800,
	},
	[3] = {
		.name		= "cache",
		.offset		= 0x127c0*0x800,
		.size		= 0x4000*0x800,
	},
	[4] = {
		.name		= "userdata",
		.offset		= 0x168c0*0x800,
		.size		= 0x29640*0x800,
	},
};

struct tegra_nand_platform harmony_nand_data = {
	.max_chips	= 8,
	.chip_parms	= nand_chip_parms,
	.nr_chip_parms  = ARRAY_SIZE(nand_chip_parms),
	.parts		= harmony_nand_partitions,
	.nr_parts	= ARRAY_SIZE(harmony_nand_partitions),
};

static struct resource resources_nand[] = {
	[0] = {
		.start  = INT_NANDFLASH,
		.end    = INT_NANDFLASH,
		.flags  = IORESOURCE_IRQ,
	},
};

struct platform_device tegra_nand_device = {
	.name           = "tegra_nand",
	.id             = -1,
	.num_resources  = ARRAY_SIZE(resources_nand),
	.resource       = resources_nand,
	.dev            = {
		.platform_data = NULL,
	},
};

static struct plat_serial8250_port debug_uart_platform_data[] = {
	{
		.membase	= IO_ADDRESS(TEGRA_UARTD_BASE),
		.mapbase	= TEGRA_UARTD_BASE,
		.irq		= INT_UARTD,
		.flags		= UPF_BOOT_AUTOCONF,
		.iotype		= UPIO_MEM,
		.regshift	= 2,
		.uartclk	= 216000000/16 * 16,
	}, {
		.flags		= 0
	}
};

static struct platform_device debug_uart = {
	.name = "serial8250",
	.id = 0,
	.dev = {
		.platform_data = debug_uart_platform_data,
	},
};

/* OTG gadget device */
static u64 tegra_otg_dmamask = DMA_BIT_MASK(32);


static struct resource tegra_otg_resources[] = {
        [0] = {
                .start  = TEGRA_USB_PHYS,
                .end    = TEGRA_USB_PHYS + TEGRA_USB_SIZE - 1,
                .flags  = IORESOURCE_MEM,
        },
        [1] = {
                .start  = INT_USB,
                .end    = INT_USB,
                .flags  = IORESOURCE_IRQ,
        },
};

static struct fsl_usb2_platform_data tegra_otg_pdata = {
	.operating_mode	= FSL_USB2_DR_DEVICE,
	.phy_mode	= FSL_USB2_PHY_UTMI,
};

struct platform_device tegra_otg = {
	.name = "fsl-tegra-udc",
	.id   = -1,
	.dev  = {
		.dma_mask		= &tegra_otg_dmamask,
		.coherent_dma_mask	= 0xffffffff,
		.platform_data = &tegra_otg_pdata,
	},
	.resource = tegra_otg_resources,
	.num_resources = ARRAY_SIZE(tegra_otg_resources),
};

static char *usb_functions[] = { "usb_mass_storage" };
static char *usb_functions_adb[] = { "usb_mass_storage", "adb" };

static struct android_usb_product usb_products[] = {
        {
                .product_id     = 0xDEAD,
                .num_functions  = ARRAY_SIZE(usb_functions),
                .functions      = usb_functions,
        },
        {
                .product_id     = 0xBEEF,
                .num_functions  = ARRAY_SIZE(usb_functions_adb),
                .functions      = usb_functions_adb,
        },
};

/* standard android USB platform data */
static struct android_usb_platform_data andusb_plat = {
        .vendor_id                      = 0x18d1,
        .product_id                     = 0x0002,
        .manufacturer_name      = "Google",
        .product_name           = "Harmony!",
        .serial_number          = "0000",
        .num_products = ARRAY_SIZE(usb_products),
        .products = usb_products,
        .num_functions = ARRAY_SIZE(usb_functions_adb),
        .functions = usb_functions_adb,
};


static struct platform_device androidusb_device = {
        .name   = "android_usb",
        .id     = -1,
        .dev    = {
                .platform_data  = &andusb_plat,
        },
};

static struct platform_device *harmony_devices[] __initdata = {
	&debug_uart,
	&tegra_nand_device,
	&tegra_otg,
	&androidusb_device
};

static void __init tegra_harmony_init(void) {
	struct clk *clk;

	clk = clk_get_sys(NULL, "pll_p");
	clk_enable(clk);
	clk_set_rate(clk, 216000000);

	clk = clk_get_sys("uart.3", "clk");
	clk_set_rate(clk, 216000000);
	clk_enable(clk);

	tegra_nand_device.dev.platform_data = &harmony_nand_data;

	platform_add_devices(harmony_devices, ARRAY_SIZE(harmony_devices));
}

MACHINE_START(HARMONY, "harmony")
	.boot_params  = 0x00000100,
	.phys_io        = IO_APB_PHYS,
	.io_pg_offst    = ((IO_APB_VIRT) >> 18) & 0xfffc,
	.init_irq       = tegra_init_irq,
	.init_machine   = tegra_harmony_init,
	.map_io         = tegra_map_common_io,
	.timer          = &tegra_timer,
MACHINE_END
