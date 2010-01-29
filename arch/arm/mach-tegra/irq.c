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
#include <linux/interrupt.h>
#include <linux/irq.h>

#include <asm/hardware/gic.h>
#include <asm/io.h>

#include <mach/io.h>
#include <mach/board.h>
#include <mach/iomap.h>

#if 0
#define ICTLR_CPU_IER		0x20
#define ICTLR_CPU_IER_SET	0x24
#define ICTLR_CPU_IER_CLR	0x28
#define ICTLR_CPU_IEP_CLASS	0x2C

static void __iomem *ictlr_reg_base[] = {
	IO_ADDRESS(TEGRA_PRIMARY_ICTLR_BASE),
	IO_ADDRESS(TEGRA_SECONDARY_ICTLR_BASE),
	IO_ADDRESS(TEGRA_TERTIARY_ICTLR_BASE),
	IO_ADDRESS(TEGRA_QUATERNARY_ICTLR_BASE),
};

extern void gic_mask_irq(unsigned int);
extern void gic_unmask_irq(unsigned int);
extern void gic_ack_irq(unsigned int);
extern void gic_set_cpu(unsigned int, const struct cpumask*);

static void tegra2_mask_irq(unsigned int irq) {
	//void __iomem *base;
	//printk("%s: %d\n", __FUNCTION__, irq);
	gic_mask_irq(irq);

	//irq -= 32;
	//base = ictlr_reg_base[irq>>5];
	//__raw_writel(1 << (irq & 31), base + ICTLR_CPU_IER_CLR);
}

static void tegra2_unmask_irq(unsigned int irq) {
	//void __iomem *base;
	//printk("%s: %d\n", __FUNCTION__, irq);
	gic_unmask_irq(irq);

	//irq -= 32;
	//base = ictlr_reg_base[irq>>5];
	//__raw_writel(1 << (irq & 31), base + ICTLR_CPU_IER_SET);
}

static void tegra2_ack_irq(unsigned int irq) {
	//printk("%s: %d\n", __FUNCTION__, irq);
	gic_ack_irq(irq);
}

#ifdef CONFIG_SMP
static void tegra2_set_cpu(unsigned int irq, const struct cpumask *mask_val) {
	gic_set_cpu(irq, mask_val);
}
#endif

static struct irq_chip tegra_irq_chip = {
	.name		= "tegra2",
	.mask   	= tegra2_mask_irq,
	.unmask 	= tegra2_unmask_irq,
	.ack    	= tegra2_ack_irq,
#ifdef CONFIG_SMP
	.set_affinity 	= tegra2_set_cpu,
#endif
};
#endif

void __init tegra_init_irq(void) {
	/*int i;*/

	printk("%s\n", __FUNCTION__);
	gic_dist_init(0, IO_ADDRESS(TEGRA_ARM_INT_DIST_BASE), 32);
	gic_cpu_init(0, IO_ADDRESS(TEGRA_ARM_PERIF_BASE + 0x100));

	/*for (i=0; i<4; i++) {
		__raw_writel(~0UL, ictlr_reg_base[i] + ICTLR_CPU_IER_CLR);
		__raw_writel(0, ictlr_reg_base[i] + ICTLR_CPU_IEP_CLASS);
	}

	for (i=32; i<NR_IRQS; i++) {
		if (set_irq_chip(i, &tegra_irq_chip))
			BUG();
		set_irq_handler(i, handle_level_irq);
	}*/

	tegra_init_clock();
}
