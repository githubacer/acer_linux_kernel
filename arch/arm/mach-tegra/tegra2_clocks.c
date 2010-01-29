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
#include <linux/module.h>
#include <linux/clk.h>
#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/delay.h>

#include <asm/io.h>
#include <asm/clkdev.h>

#include <mach/io.h>
#include <mach/clock.h>
#include <mach/iomap.h>

#define CLK_RST_CONTROLLER_RST_DEVICES         0x004
#define CLK_RST_CONTROLLER_RST_DEVICES_SET_CLR 0x300
#define CLK_RST_CONTROLLER_CLK_OUT_ENB         0x010
#define CLK_RST_CONTROLLER_CLK_OUT_ENB_SET_CLR 0x320
#define CLK_RST_CONTROLLER_CLK_SOURCE          0x100

static void __iomem *reg_clk_base;

#define clk_writel(value, reg) \
	__raw_writel(value, (u32)reg_clk_base + (reg))
#define clk_readl(reg) \
	__raw_readl((u32)reg_clk_base + (reg))

void tegra2_arch_clk_init(void) {
	reg_clk_base = IO_ADDRESS(TEGRA_CLK_RESET_BASE);
}
EXPORT_SYMBOL(tegra2_arch_clk_init);

static int clk_div71_possible_rate(struct clk *c, unsigned long rate) {
	unsigned long input_rate = clk_get_rate(c);
	int divider_u71;

	divider_u71 = (input_rate*2)/rate;
	if (rate * divider_u71 == input_rate*2)
		return divider_u71 - 2;
	else
		return -1;
}

/* Default clk ops */
static int tegra2_clk_enable(struct clk *c) {
	printk("%s on clock %s\n", __FUNCTION__, c->name);
	return 0;
}

static void tegra2_clk_disable(struct clk *c) {
	printk("%s on clock %s\n", __FUNCTION__, c->name);
}

static unsigned long tegra2_clk_get_rate(struct clk *c) {
	printk("%s on clock %s\n", __FUNCTION__, c->name);
	return c->rate;
}

/* clk_m functions */
static unsigned long tegra2_clk_m_autodetect_rate(struct clk *c) {
	u32 clock_autodetect;
	u32 auto_clock_control = clk_readl(0x50) & ~(3<<30);
	clk_writel(0x80000001, 0x58);
	do {} while ((clock_autodetect = clk_readl(0x5c)) & 0x80000000);
	switch (clock_autodetect) {
	case 732:
	case 733:
		c->rate = 12000000;
		auto_clock_control |= 2<<30;
		break;
	case 794:
		c->rate = 13000000;
		auto_clock_control |= 0<<30;
		break;
	case 1172:
		c->rate = 19200000;
		auto_clock_control |= 1<<30;
		break;
	case 1587:
		c->rate = 26000000;
		auto_clock_control |= 3<<30;
		break;
	default:
		WARN(1, "Unexpected clock autodetect value %d", clock_autodetect);
	}
	clk_writel(auto_clock_control, 0x50);
	return c->rate;
}

static void tegra2_clk_m_init(struct clk *c) {
	printk("%s on clock %s\n", __FUNCTION__, c->name);
	tegra2_clk_m_autodetect_rate(c);
}

static int tegra2_clk_m_enable(struct clk *c) {
	int ret = 0;
	printk("%s on clock %s\n", __FUNCTION__, c->name);
	return ret;
}

static void tegra2_clk_m_disable(struct clk *c) {
	printk("%s on clock %s\n", __FUNCTION__, c->name);
	BUG();
}

static struct clk_ops tegra_clk_m_ops = {
	.init     = tegra2_clk_m_init,
	.enable   = tegra2_clk_m_enable,
	.disable  = tegra2_clk_m_disable,
	.get_rate = tegra2_clk_get_rate,
};

/* PLL Functions */
static int tegra2_pll_clk_set_rate(struct clk *c, unsigned long rate) {
	u32 reg;
	unsigned long input_rate;
	const struct clk_pll_table *sel;

	printk("%s on clock %s\n", __FUNCTION__, c->name);
	//BUG_ON(c->refcnt != 0);

	input_rate = clk_get_rate(c->parent);
	for (sel = c->pll_table; sel->input_rate != 0; sel++) {
		if (sel->input_rate == input_rate && sel->output_rate == rate) {
			c->n = sel->n;
			c->m = sel->m;
			c->p = sel->p;
			c->cpcon = sel->cpcon;
			reg = clk_readl(c->reg);
			if (c->flags & PLL_FIXED)
				reg |= 1<<28;
			reg &= ~0x7FFF;
			reg |= (c->m << 0) | (c->n << 8);
			BUG_ON(c->p > 2);
			if (c->p == 2)
				reg |= 1<<20;
			printk(" %s: %08x\n", c->name, reg);
			clk_writel(reg, c->reg);
			c->rate = rate;

			reg = c->cpcon << 8;
			clk_writel(reg, c->reg+0xC);
			return 0;
		}
	}
	return 1;
}

static int tegra2_pll_clk_enable(struct clk *c) {
	int ret = 0;
	u32 reg;
	printk("%s on clock %s\n", __FUNCTION__, c->name);

	reg = clk_readl(c->reg);
	reg &= ~(1 << 31);
	reg |= 1 << 30;
	clk_writel(reg, c->reg);
	printk(" reg: %08x\n", clk_readl(c->reg));
	return ret;
}

static void tegra2_pll_clk_disable(struct clk *c) {
	u32 reg;
	printk("%s on clock %s\n", __FUNCTION__, c->name);

	reg = clk_readl(c->reg);
	reg &= ~(1 << 31);
	reg &= ~(1 << 30);
	clk_writel(reg, c->reg);
	printk(" reg: %08x\n", clk_readl(c->reg));
}

static struct clk_ops tegra_pll_ops = {
	.enable   = tegra2_pll_clk_enable,
	.disable  = tegra2_pll_clk_disable,
	.set_rate = tegra2_pll_clk_set_rate,
	.get_rate = tegra2_clk_get_rate,
};


/* Mux functions */
static int tegra2_mux_clk_enable(struct clk *c) {
	int ret = 0;
	printk("%s on clock %s\n", __FUNCTION__, c->name);
	return ret;
}

static void tegra2_mux_clk_disable(struct clk *c) {
	printk("%s on clock %s\n", __FUNCTION__, c->name);
}

static int tegra2_mux_clk_set_parent(struct clk *c, struct clk *parent) {
	int ret = 0;
	printk("%s on clock %s\n", __FUNCTION__, c->name);
	return ret;
}

static struct clk_ops tegra2_mux_clk_ops = {
	.enable		= tegra2_mux_clk_enable,
	.disable	= tegra2_mux_clk_disable,
	.set_parent	= tegra2_mux_clk_set_parent,
};

/* Clock divider ops */
static struct clk_ops tegra_div_ops = {
	.enable = tegra2_clk_enable,
	.disable = tegra2_clk_disable,
};

/* Periph clk ops */
static void tegra2_periph_clk_init(struct clk *c) {
	clk_set_parent(c, c->inputs[0].input);
}

static int tegra2_periph_clk_enable(struct clk *c) {
	u32 reg;
	reg = (1<<(c->clk_num%32));
	clk_writel(reg, CLK_RST_CONTROLLER_CLK_OUT_ENB_SET_CLR + 8*(c->clk_num/32));
	clk_writel(reg, CLK_RST_CONTROLLER_RST_DEVICES_SET_CLR + 4 + 8*(c->clk_num/32));
	return 0;
}

static void tegra2_periph_clk_disable(struct clk *c) {
	u32 reg;
	reg = (1<<(c->clk_num%32));
	clk_writel(reg, CLK_RST_CONTROLLER_CLK_OUT_ENB_SET_CLR + 4 + 8*(c->clk_num/32));
	clk_writel(reg, CLK_RST_CONTROLLER_RST_DEVICES_SET_CLR + 8*(c->clk_num/32));
}

static int tegra2_periph_clk_set_parent(struct clk *c, struct clk *p) {
	int ret = 0;
	u32 reg;
	const struct clk_mux_sel *sel;
	for (sel = c->inputs; sel->input != NULL; sel++) {
		if (sel->input == p) {
			c->parent = p;
			reg = clk_readl(c->reg);
			reg &= ~(0x3<<30);
			reg |= (sel->value)<<30;
			clk_writel(reg, c->reg);
		}
	}

	return ret;
}

static int tegra2_periph_clk_set_rate(struct clk *c, unsigned long rate) {
	int ret = 0;
	u32 reg;
	int divider_u71;
	const struct clk_mux_sel *sel;
	printk("%s: %lu\n", __FUNCTION__, rate);
	for (sel = c->inputs; sel->input != NULL; sel++) {
		printk("   %s: %lu\n", sel->input->name, clk_get_rate(sel->input));
		if (c->flags & DIV_U71) {
			divider_u71 = clk_div71_possible_rate(sel->input, rate);
			if (divider_u71 >= 0) {
				/* FIXME: ensure we don't go to a too-high rate */
				clk_set_parent(c, sel->input);
				udelay(1);
				reg = clk_readl(c->reg);
				reg &= ~(0xFF);
				reg |= divider_u71;
				clk_writel(reg, c->reg);
				return 1;
			}
		} else {
			if (clk_get_rate(sel->input) == rate) {
				clk_set_parent(c, sel->input);
				return 1;
			}
		}
	}
	BUG();
	return ret;
}

static struct clk_ops tegra_periph_clk_ops = {
	.init       = &tegra2_periph_clk_init,
	.enable     = &tegra2_periph_clk_enable,
	.disable    = &tegra2_periph_clk_disable,
	.set_parent = &tegra2_periph_clk_set_parent,
	.set_rate   = &tegra2_periph_clk_set_rate,
};

static struct clk tegra_32k_clk = {
	.name = "tegra_32k_clk",
	.rate = 32678,
	.ops  = NULL,
};

static struct clk tegra_input_clk = {
	.name = "tegra_input_clk",
	.flags = ENABLE_ON_INIT,
	.rate = 12000000,
	.ops = NULL,
};

static struct clk_pll_table tegra_pll_s_table[] = {
	{32768, 12000000, 366, 1, 1, 1},
	{32768, 13000000, 397, 1, 1, 1},
	{32768, 19200000, 586, 1, 1, 1},
	{32768, 26000000, 793, 1, 1, 1},
	{0, 0, 0, 0, 0, 0},
};

static struct clk tegra_pll_s = {
	.name = "tegra_pll_s",
	.ops = &tegra_pll_ops,
	.reg       = 0xf0,
	.input_min = 32768,
	.input_max = 32768,
	.parent     = &tegra_32k_clk,
	.cf_min    = 0, /* FIXME */
	.cf_max    = 0, /* FIXME */
	.vco_min   = 12000000,
	.vco_max   = 26000000,
	.pll_table = tegra_pll_s_table,
};

static struct clk_mux_sel tegra_clk_m_sel[] = {
	{ .input = &tegra_32k_clk, .value = 0},
	{ .input = &tegra_pll_s,  .value = 1},
	{ 0, 0},
};
static struct clk tegra_clk_m = {
	.name      = "tegra_clk_m",
	.flags     = ENABLE_ON_INIT,
	.ops       = &tegra_clk_m_ops,
	.inputs    = tegra_clk_m_sel,
	.reg       = 0x1fc,
	.reg_mask  = (1<<28),
	.reg_shift = 28,
};

/*static struct clk_div tegra_clk_pll_in = {
	.clk = {
		.name = "tegra_clk_pll_in",
		.ops  = &tegra_clk_pll_in_ops,
	},
	.reg = */

static struct clk tegra_pll_c = {
	.name      = "tegra_pll_c",
	.ops       = &tegra_pll_ops,
	.reg       = 0x80,
	.input_min = 2000000,
	.input_max = 31000000,
	.parent    = &tegra_clk_m,
	.cf_min    = 1000000,
	.cf_max    = 6000000,
	.vco_min   = 20000000,
	.vco_max   = 140000000,
	/*.pll_table = tegra_pll_c_table,*/
};

static struct clk tegra_pll_c_out1 = {
	.name      = "tegra_pll_c_out1",
	.ops       = &tegra_div_ops,
	.flags     = DIV_U71,
	.parent    = &tegra_pll_c,
	.reg       = 0x84,
	.reg_shift = 0,
};

static struct clk tegra_pll_m = {
	.name      = "tegra_pll_m",
	.ops       = &tegra_pll_ops,
	.reg       = 0x90,
	.input_min = 2000000,
	.input_max = 31000000,
	.parent    = &tegra_clk_m,
	.cf_min    = 1000000,
	.cf_max    = 6000000,
	.vco_min   = 20000000,
	.vco_max   = 120000000,
	/*.pll_table = tegra_pll_m_table,*/
};

static struct clk tegra_pll_m_out1 = {
	.name      = "tegra_pll_m_out1",
	.ops       = &tegra_div_ops,
	.flags     = DIV_U71,
	.parent    = &tegra_pll_m,
	.reg       = 0x94,
	.reg_shift = 0,
};

static struct clk_pll_table tegra_pll_p_table[] = {
	{ 12000000, 216000000, 432, 12, 2, 8},
	{ 13000000, 216000000, 432, 13, 2, 8},
	{ 19200000, 216000000, 90,   4, 2, 1},
	{ 26000000, 216000000, 432, 26, 2, 8},
	{ 12000000, 432000000, 432, 12, 1, 8},
	{ 13000000, 432000000, 432, 13, 1, 8},
	{ 19200000, 432000000, 90,   4, 1, 1},
	{ 26000000, 432000000, 432, 26, 1, 8},
	{ 0, 0, 0, 0, 0, 0 },
};

static struct clk tegra_pll_p = {
	.name      = "tegra_pll_p",
	.flags     = ENABLE_ON_INIT | PLL_FIXED,
	.ops       = &tegra_pll_ops,
	.reg       = 0xa0,
	.input_min = 2000000,
	.input_max = 31000000,
	.parent    = &tegra_clk_m,
	.cf_min    = 1000000,
	.cf_max    = 6000000,
	.vco_min   = 20000000,
	.vco_max   = 140000000,
	.pll_table = tegra_pll_p_table,
};

static struct clk tegra_pll_p_out1 = {
	.name      = "tegra_pll_p_out1",
	.ops       = &tegra_div_ops,
	.flags     = ENABLE_ON_INIT | DIV_U71 | DIV_U71_FIXED,
	.parent    = &tegra_pll_p,
	.reg       = 0xa4,
	.reg_shift = 0,
};

static struct clk tegra_pll_p_out2 = {
	.name      = "tegra_pll_p_out2",
	.ops       = &tegra_div_ops,
	.flags     = ENABLE_ON_INIT | DIV_U71 | DIV_U71_FIXED,
	.parent    = &tegra_pll_p,
	.reg       = 0xa4,
	.reg_shift = 16,
};

static struct clk tegra_pll_p_out3 = {
	.name      = "tegra_pll_p_out3",
	.ops       = &tegra_div_ops,
	.flags     = ENABLE_ON_INIT | DIV_U71 | DIV_U71_FIXED,
	.parent    = &tegra_pll_p,
	.reg       = 0xa8,
	.reg_shift = 0,
};

static struct clk tegra_pll_p_out4 = {
	.name      = "tegra_pll_p_out4",
	.ops       = &tegra_div_ops,
	.flags     = ENABLE_ON_INIT | DIV_U71 | DIV_U71_FIXED,
	.parent    = &tegra_pll_p,
	.reg       = 0xa8,
	.reg_shift = 16,
};

static struct clk_mux_sel tegra_periph_sel1[] = {
	{ .input = &tegra_pll_p, .value = 0},
	{ .input = &tegra_pll_c, .value = 1},
	{ .input = &tegra_pll_m, .value = 2},
	{ .input = &tegra_clk_m, .value = 3},
	{ 0, 0},
};

static struct clk_mux_sel tegra_periph_sel_clk_m[] = {
	{ .input = &tegra_clk_m, .value = 0},
	{ 0, 0},
};

#define PERIPH_CLK(t, n, c, r, s, f) \
	static struct clk t = {	\
		.name      = n,				\
		.ops       = &tegra_periph_clk_ops,	\
		.clk_num   = c,				\
		.reg       = r,				\
		.inputs    = s,				\
		.flags     = f,				\
	}

PERIPH_CLK(tegra_clk_source_uarta, "uarta", 6,  0x178, tegra_periph_sel1, MUX | DIV_U71);
PERIPH_CLK(tegra_clk_source_uartb, "uartb", 7,  0x17c, tegra_periph_sel1, MUX | DIV_U71);
PERIPH_CLK(tegra_clk_source_uartc, "uartc", 45, 0x1a0, tegra_periph_sel1, MUX | DIV_U71);
PERIPH_CLK(tegra_clk_source_uartd, "uartd", 65, 0x1c0, tegra_periph_sel1, MUX | DIV_U71);
PERIPH_CLK(tegra_clk_source_uarte, "uarte", 66, 0x1c4, tegra_periph_sel1, MUX | DIV_U71);
PERIPH_CLK(tegra_clk_source_nand,  "nand",  13, 0x160, tegra_periph_sel1, MUX | DIV_U71);
PERIPH_CLK(tegra_clk_source_timer, "timer", 5, 0, tegra_periph_sel_clk_m, 0);

#define CLK(dev, con, ck)     \
	{       \
		.dev_id = dev,  \
		.con_id = con,  \
		.clk = ck,  \
	}

struct clk_lookup tegra_clks[] = {
	/* external root sources */
	CLK(NULL, "input_clk",  &tegra_input_clk),
	CLK(NULL, "32k_clk", &tegra_32k_clk),
	CLK(NULL, "pll_s", &tegra_pll_s),
	CLK(NULL, "clk_m", &tegra_clk_m),
	CLK(NULL, "pll_c", &tegra_pll_c),
	CLK(NULL, "pll_c_out1", &tegra_pll_c_out1),
	CLK(NULL, "pll_m", &tegra_pll_m),
	CLK(NULL, "pll_m_out1", &tegra_pll_m_out1),
	CLK(NULL, "pll_p", &tegra_pll_p),
	CLK(NULL, "pll_p_out1", &tegra_pll_p_out1),
	CLK(NULL, "pll_p_out2", &tegra_pll_p_out2),
	CLK(NULL, "pll_p_out3", &tegra_pll_p_out3),
	CLK(NULL, "pll_p_out4", &tegra_pll_p_out4),
	CLK("uart.0", "clk", &tegra_clk_source_uarta),
	CLK("uart.1", "clk", &tegra_clk_source_uartb),
	CLK("uart.2", "clk", &tegra_clk_source_uartc),
	CLK("uart.3", "clk", &tegra_clk_source_uartd),
	CLK("uart.4", "clk", &tegra_clk_source_uarte),
	CLK("timer_us", "clk", &tegra_clk_source_timer),
	CLK("tegra_nand", "clk", &tegra_clk_source_nand),
	CLK(NULL, NULL, NULL),
};
