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
#include <linux/clk.h>
#include <linux/list.h>
#include <linux/init.h>
#include <linux/module.h>
#include <asm/clkdev.h>
#include <mach/clock.h>

static LIST_HEAD(clocks);
static DEFINE_MUTEX(clocks_mutex);
static DEFINE_SPINLOCK(clk_spinlock);

extern struct clk_lookup tegra_clks[];

/**
 * clk_preinit - initialize any fields in the struct clk before clk init
 * @clk: struct clk * to initialize
 *
 * Initialize any struct clk fields needed before normal clk initialization
 * can run.  No return value.
 */
void clk_preinit(struct clk *clk) {
}

int clk_register(struct clk *clk) {
	mutex_lock(&clocks_mutex);
	list_add(&clk->node, &clocks);
	mutex_unlock(&clocks_mutex);
	return 0;
}

int clk_enable(struct clk *c) {
	int ret = 0;
	unsigned long flags;
	if (c->refcnt == 0) {
		if (c->parent) {
			ret = clk_enable(c->parent);
			if (ret)
				goto err;
		}

		if (c->ops && c->ops->enable) {
			spin_lock_irqsave(&clk_spinlock, flags);
			ret = c->ops->enable(c);
			spin_unlock_irqrestore(&clk_spinlock, flags);
			if (ret) {
				if (c->parent)
					clk_disable(c->parent);
				goto err;
			}
		}
	}
	c->refcnt++;
err:
	return ret;
}
EXPORT_SYMBOL(clk_enable);

void clk_disable(struct clk *c) {
	unsigned long flags;
	if (c->refcnt == 0) {
		WARN(1, "Attempting to disable clock %s with refcnt 0", c->name);
		return;
	}
	if (c->refcnt == 1) {
		if (c->parent)
			clk_disable(c->parent);

		if (c->ops && c->ops->disable) {
			spin_lock_irqsave(&clk_spinlock, flags);
			c->ops->disable(c);
			spin_unlock_irqrestore(&clk_spinlock, flags);
		}
	}
	c->refcnt --;
}
EXPORT_SYMBOL(clk_disable);

int clk_set_parent(struct clk *c, struct clk *parent) {
	if (c->ops && c->ops->set_parent)
		return c->ops->set_parent(c, parent);
	else
		BUG();
}
EXPORT_SYMBOL(clk_set_parent);

int clk_set_rate(struct clk *c, unsigned long rate) {
	if (c->ops && c->ops->set_rate)
		return c->ops->set_rate(c, rate);
	else
		BUG();
}
EXPORT_SYMBOL(clk_set_rate);

unsigned long clk_get_rate(struct clk *c) {
	if (c->ops && c->ops->get_rate)
		return c->ops->get_rate(c);
	else if (c->parent)
		return clk_get_rate(c->parent);
	else
		BUG();
}
EXPORT_SYMBOL(clk_get_rate);

void clk_enable_init_clocks(void) {
	struct clk *clkp;

	list_for_each_entry(clkp, &clocks, node) {
		if (clkp->flags & ENABLE_ON_INIT)
			clk_enable(clkp);
	}
}
EXPORT_SYMBOL(clk_enable_init_clocks);

void tegra2_arch_clk_init(void); /* FIXME */

int __init tegra_init_clock(void) {
	struct clk_lookup *cl;

	tegra2_arch_clk_init();

	printk("%s\n", __FUNCTION__);
	for (cl = tegra_clks; cl->clk != NULL; cl++) {
		if (cl->clk->ops && cl->clk->ops->init)
			cl->clk->ops->init(cl->clk);
		clk_preinit(cl->clk);
		clkdev_add(cl);
		clk_register(cl->clk);
	}

	clk_enable_init_clocks();
	return 0;
}
