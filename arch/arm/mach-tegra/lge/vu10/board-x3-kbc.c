/*
 * arch/arm/mach-tegra/board-x3-kbc.c
 * Keys configuration for Nvidia tegra3 x3 platform.
 *
 * Copyright (C) 2011 NVIDIA, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307, USA
 */

#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <mach/io.h>
#include <mach/iomap.h>
#include <mach/kbc.h>
#include <linux/gpio_keys.h>
#include <linux/dma-mapping.h>

#include <mach-tegra/gpio-names.h>
#include <mach-tegra/board.h>
#include <lge/board-x3.h>
#include <mach-tegra/devices.h>
#include <mach-tegra/wakeups-t3.h>

#define GPIO_KEY(_id, _gpio, _iswake)		\
	{					\
		.code = _id,			\
		.gpio = TEGRA_GPIO_##_gpio,	\
		.active_low = 1,		\
		.desc = #_id,			\
		.type = EV_KEY,			\
		.wakeup = _iswake,		\
		.debounce_interval = 1,	\
	}

#define PMC_WAKE_STATUS		0x14
#define PMC_WAKE2_STATUS	0x168

static int x3_wakeup_key(void)
{
	u64 status;
	
	status = __raw_readl(IO_ADDRESS(TEGRA_PMC_BASE) + PMC_WAKE_STATUS);
	status |= ((u64)readl(IO_ADDRESS(TEGRA_PMC_BASE) + PMC_WAKE2_STATUS)) << 32;

	printk("x3_wakeup_key : status %lu\n", status);

#if defined(CONFIG_MACH_VU10)
	if (x3_get_hw_rev_pcb_version() == hw_rev_pcb_type_A) {
		return (status & TEGRA_WAKE_GPIO_PC7) ? KEY_POWER : (status & TEGRA_WAKE_GPIO_PO4) ? KEY_VOLUMEDOWN : KEY_RESERVED;
	}
	else {
		if (status & TEGRA_WAKE_GPIO_PC7)
			return KEY_POWER;
		else if (status & TEGRA_WAKE_GPIO_PO4)
			return KEY_VOLUMEDOWN;
		else if (status & TEGRA_WAKE_GPIO_PB6)
			return KEY_HP;
		else
			return KEY_RESERVED;
	}
#else
	return (status & TEGRA_WAKE_GPIO_PC7) ? KEY_POWER : (status & TEGRA_WAKE_GPIO_PO4) ? KEY_VOLUMEDOWN : KEY_RESERVED;
#endif

}

static struct gpio_keys_button x3_keys[] = {
	[0] = GPIO_KEY(KEY_VOLUMEUP, PI6, 0),
	[1] = GPIO_KEY(KEY_VOLUMEDOWN, PO4, 1),
	[2] = GPIO_KEY(KEY_POWER, PC7, 1),
	[3] = GPIO_KEY(KEY_HP, PB6 , 1),
};

static struct gpio_keys_platform_data x3_keys_platform_data = {
	.buttons	= x3_keys,
	.nbuttons	= ARRAY_SIZE(x3_keys),	
	.rep		= 0,
	.wakeup_key  = x3_wakeup_key,
};

static struct platform_device x3_keys_device = {
	.name   = "gpio-keys",
	.id     = 0,
	.dev    = {
		.platform_data  = &x3_keys_platform_data,
	},
};

int __init x3_kbc_init(void)
{
	int i;

	pr_info("Registering tegra-kbc-gpio\n");

#if defined(CONFIG_MACH_VU10)
	if (x3_get_hw_rev_pcb_version() == hw_rev_pcb_type_A) {
		x3_keys[0].gpio = TEGRA_GPIO_PO7; // VOL UP
		x3_keys[3].gpio = TEGRA_GPIO_PS1; // HP
	}
#endif

	for (i = 0; i < ARRAY_SIZE(x3_keys); i++)
		tegra_gpio_enable(x3_keys[i].gpio);	

	platform_device_register(&x3_keys_device);

	pr_info("Registering successful tegra-kbc\n");
	return 0;
}

