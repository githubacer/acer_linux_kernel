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

#include <linux/init.h>
#include <linux/time.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/clockchips.h>
#include <linux/clocksource.h>
#include <linux/clk.h>
#include <asm/mach/time.h>
#include <asm/io.h>
#include <asm/mach/time.h>
#include <mach/iomap.h>

#define TIMERUS_CNTR_1US 0x10
#define TIMERUS_USEC_CFG 0x14
#define TIMERUS_CNTR_FREEZE 0x4c

#define TIMER1_BASE 0x0
#define TIMER2_BASE 0x8
#define TIMER3_BASE 0x50
#define TIMER4_BASE 0x58

#define TIMER_PTV 0x0
#define TIMER_PCR 0x4

#define TIMER1_IRQ 32
#define TIMER2_IRQ 33
#define TIMER3_IRQ 41
#define TIMER4_IRQ 42

struct tegra_timer;

static void __iomem *timer_reg_base = IO_ADDRESS(TEGRA_TMR1_BASE);

#define timer_writel(value, reg) \
	__raw_writel(value, (u32)timer_reg_base + (reg))
#define timer_readl(reg) \
	__raw_readl((u32)timer_reg_base + (reg))

static int tegra_timer_set_next_event(unsigned long cycles,
					 struct clock_event_device *evt)
{
	u32 reg;
	//printk("%s: %lu\n", __FUNCTION__, cycles);

	reg = 0x80000000 | ((1000000/HZ)*(cycles+1)-1);
	timer_writel(reg, TIMER1_BASE + TIMER_PTV);

	return 0;
}

static void tegra_timer_set_mode(enum clock_event_mode mode,
				    struct clock_event_device *evt)
{
	u32 reg;
	//printk("%s: %d\n", __FUNCTION__, mode);

	timer_writel(0, TIMER1_BASE + TIMER_PTV);

	switch (mode) {
	case CLOCK_EVT_MODE_PERIODIC:
		reg = 0xC0000000 | ((1000000/HZ)-1);
		timer_writel(reg, TIMER1_BASE + TIMER_PTV);
		break;
	case CLOCK_EVT_MODE_ONESHOT:
		break;
	case CLOCK_EVT_MODE_UNUSED:
	case CLOCK_EVT_MODE_SHUTDOWN:
	case CLOCK_EVT_MODE_RESUME:
		break;
	}
}

static cycle_t tegra_clocksource_read(struct clocksource *cs)
{
	return timer_readl(TIMERUS_CNTR_1US);
}

static struct clock_event_device tegra_clockevent = {
	.name		= "timer0",
	.rating         = 300,
	.features       = CLOCK_EVT_FEAT_ONESHOT, CLOCK_EVT_FEAT_PERIODIC,
	.mult           = 16777,
	.shift		= 24,
	.set_next_event	= tegra_timer_set_next_event,
	.set_mode	= tegra_timer_set_mode,
};

static struct clocksource tegra_clocksource =
{
	.name	= "timer_us",
	.rating	= 300,
	.read	= tegra_clocksource_read,
	.mask	= 0xFFFFFFFFUL,
	.mult	= 1000,
	.shift	= 0,
	.flags	= CLOCK_SOURCE_IS_CONTINUOUS,
};

static irqreturn_t tegra_timer_interrupt(int irq, void *dev_id)
{
	struct clock_event_device *evt = (struct clock_event_device*)dev_id;
	//printk("%s\n", __FUNCTION__);
	timer_writel(1<<30, TIMER1_BASE + TIMER_PCR);
	evt->event_handler(evt);
	return IRQ_HANDLED;
}

static struct irqaction tegra_timer_irq = {
	.name		= "timer0",
	.flags		= IRQF_DISABLED | IRQF_TIMER | IRQF_TRIGGER_HIGH,
	.handler	= tegra_timer_interrupt,
	.dev_id		= &tegra_clockevent,
	.irq            = TIMER1_IRQ,
};

void __init tegra_init_timer(void) {
	struct clk *c;
	int ret;

	c = clk_get_sys("timer_us", "clk");
	printk("Timer clock rate %lu\n", clk_get_rate(c));

	switch (clk_get_rate(c)) {
	case 12000000:
		timer_writel(0x000b, TIMERUS_USEC_CFG);
		break;
	case 13000000:
		timer_writel(0x000c, TIMERUS_USEC_CFG);
		break;
	case 19200000:
		timer_writel(0x045f, TIMERUS_USEC_CFG);
		break;
	case 26000000:
		timer_writel(0x0019, TIMERUS_USEC_CFG);
		break;
	default:
		WARN(1, "Unknown clock rate");
	}

	if (clocksource_register(&tegra_clocksource)) {
		printk(KERN_ERR "Failed to register clocksource\n");
		BUG();
	}

	if ((ret = setup_irq(tegra_timer_irq.irq, &tegra_timer_irq))) {
		printk(KERN_ERR "Failed to register timer IRQ: %d\n", ret);
		BUG();
	}

	tegra_clockevent.max_delta_ns =
		clockevent_delta2ns(0x1fffffff, &tegra_clockevent);
	tegra_clockevent.max_delta_ns =
		clockevent_delta2ns(0x1, &tegra_clockevent);
	tegra_clockevent.cpumask = cpu_all_mask;
	tegra_clockevent.irq = TIMER1_IRQ;
	clockevents_register_device(&tegra_clockevent);

	return;
}
