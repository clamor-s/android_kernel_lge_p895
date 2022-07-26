/*
 * arch/arm/mach-tegra/lge/board-x3-i2c-pin-define.c
 *
 * Copyright (C) 2011 LG Electronics, Inc.
 *
 * Author: dalyong.cha@lge.com
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
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/ctype.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/i2c.h>
#include <linux/i2c/panjit_ts.h>
#include <linux/dma-mapping.h>
#include <linux/delay.h>
#include <linux/i2c-tegra.h>
#include <linux/gpio.h>
#include <linux/input.h>
#include <mach/clk.h>
#include <mach/iomap.h>
#include <mach/irqs.h>
#include <mach/pinmux.h>
#include <mach/iomap.h>
#include <mach/io.h>
#include <mach/spdif.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>

#include <mach-tegra/board.h>
#include <mach-tegra/clock.h>
#include <lge/board-x3.h>
#include <lge/board-x3-misc.h>
#include <lge/board-x3-audio.h>
#include <lge/board-x3-input.h>
#include <mach-tegra/devices.h>
#include <mach-tegra/gpio-names.h>
#include <mach-tegra/fuse.h>

#ifdef CONFIG_I2C_GPIO
#include <linux/i2c-gpio.h>
#endif

#define SII9244_MHL_ADDRESS 0x39 //0x72 MHL tx
#define SII9244A_MHL_ADDRESS 0x3D // 0x7A(TPI) Register
#define SII9244B_MHL_ADDRESS 0x49 //0x92 HDMI rx 
#define SII9244C_MHL_ADDRESS 0x64 //0xC8(cbus)

#if defined(CONFIG_REGULATOR_CAM_SUBPMIC_LP8720)
#include <linux/regulator/lp8720.h>
static struct lp8720_platform_data lp8720_pdata = {
	.en_gpio_num         = TEGRA_GPIO_PBB4 /*220*/,
};

#endif

#ifdef CONFIG_LGE_NFC_PN544
#include <linux/nfc/pn544_lge.h>
static struct pn544_i2c_platform_data nfc_pdata = {
        .irq_gpio = NFC_GPIO_IRQ,
        .ven_gpio = NFC_GPIO_VEN,
        .firm_gpio = NFC_GPIO_FIRM,
};
#endif

static struct i2c_board_info __initdata x3_i2c_bus0_board_info[] = {
#if defined(CONFIG_SND_SOC_TEGRA_MAX98088)
	{
		I2C_BOARD_INFO("max98088", 0x10),
		.platform_data = &max98088_pdata,
	},
#endif
#if defined(MPU_SENSORS_MPU6050B1)   
	{
		I2C_BOARD_INFO("mpu6050" , 0x68),
		.irq = TEGRA_GPIO_TO_IRQ(TEGRA_GPIO_PH4),
		.platform_data = &mpu6050_data,
	},
	{
		I2C_BOARD_INFO("ami306", 0x0E),
		.irq = 0,	//TEGRA_GPIO_TO_IRQ(TEGRA_GPIO_PH5),
		.platform_data = &mpu_compass_data,
	},        
#endif
#if defined(CONFIG_SENSORS_APDS990X)
#define APDS990X_PROXIMITY_ADDR 0x39
	{
		I2C_BOARD_INFO(APDS990x_DRV_NAME, APDS990X_PROXIMITY_ADDR),
		.irq = TEGRA_GPIO_TO_IRQ(TEGRA_GPIO_PK2),
		.platform_data	= &x3_prox_data,
	},
#endif  
#if defined(CONFIG_LGE_NFC_PN544)
	{
		I2C_BOARD_INFO(PN544_DRV_NAME, NFC_I2C_SLAVE_ADDR),
		.type = PN544_DRV_NAME,
		.irq = TEGRA_GPIO_TO_IRQ(NFC_GPIO_IRQ),
		.platform_data = &nfc_pdata,
	},
#endif
};

static struct i2c_board_info __initdata x3_i2c_bus1_board_info[] = {
#if defined(CONFIG_TOUCHSCREEN_SYNAPTICS_COMMON)
	{
		I2C_BOARD_INFO(LGE_TOUCH_NAME, 0x20),
		.irq = TEGRA_GPIO_TO_IRQ(TEGRA_GPIO_PQ3),
		.platform_data = &synaptics_pdata,
	},
#endif
#if defined(CONFIG_LGE_MUIC)
	{
		I2C_BOARD_INFO("muic", 0x44),
		.irq = TEGRA_GPIO_TO_IRQ(MUIC_GPIO),
		.platform_data = 0,
	},
#endif
#if defined(CONFIG_ADC_TSC2007)
	{
		I2C_BOARD_INFO("tsc2007_adc", TSC2007_ADC_SLAVE_ADDR),
	},
#endif
#if defined(CONFIG_BACKLIGHT_LM353X)
#define LM3533_BACKLIGHT_ADDRESS 0x36
	{
		I2C_BOARD_INFO("lm3533_bl", LM3533_BACKLIGHT_ADDRESS),
		.platform_data = &lm3533_pdata,
	},
#endif
};

static struct i2c_board_info __initdata x3_i2c_bus2_board_info[] = {
#if defined(CONFIG_REGULATOR_CAM_SUBPMIC_LP8720)
	{
		I2C_BOARD_INFO(LP8720_I2C_NAME,  LP8720_I2C_ADDR),
		.platform_data = &lp8720_pdata,
	},
#endif
};

static struct i2c_board_info __initdata x3_i2c_bus4_board_info[] = {
#if defined(CONFIG_BATTERY_MAX17043)
	{
		I2C_BOARD_INFO("max17043", MAX17043_SLAVE_ADDR),
		.irq = TEGRA_GPIO_TO_IRQ(MAX17043_GAUGE_INT),
	},

#endif
#if defined(CONFIG_MAX8971_CHARGER)
	{
		I2C_BOARD_INFO("max8971", MAX8971_SLAVE_ADDR),
		.platform_data	= &max8971_data,
		.irq		= TEGRA_GPIO_TO_IRQ(MAX8971_IRQB),		
	},
#endif                          
#if defined(CONFIG_TSPDRV)
	{
		I2C_BOARD_INFO(TSPDRV_I2C_DEVICE_NAME, TSPDRV_I2C_SLAVE_ADDR),
		.type = TSPDRV_I2C_DEVICE_NAME,
		.platform_data = &tspdrv_i2c_pdata,
	},
#endif
};

static struct i2c_board_info __initdata x3_i2c_bus5_board_info[] = {
#if defined(CONFIG_MHL_TX_SII9244)
	{
		I2C_BOARD_INFO("SII9244", SII9244_MHL_ADDRESS),
	},
	{
		I2C_BOARD_INFO("SII9244A", SII9244A_MHL_ADDRESS),
	},
	{
		I2C_BOARD_INFO("SII9244B", SII9244B_MHL_ADDRESS),
	},
	{
		I2C_BOARD_INFO("SII9244C", SII9244C_MHL_ADDRESS),
	},
#endif 
};

static void __init x3_i2c_dev_init(void)
{
	i2c_register_board_info(0, x3_i2c_bus0_board_info,
			ARRAY_SIZE(x3_i2c_bus0_board_info));
	i2c_register_board_info(1, x3_i2c_bus1_board_info,
			ARRAY_SIZE(x3_i2c_bus1_board_info));
	i2c_register_board_info(2, x3_i2c_bus2_board_info, 
			ARRAY_SIZE(x3_i2c_bus2_board_info));
	i2c_register_board_info(4, x3_i2c_bus4_board_info, 
			ARRAY_SIZE(x3_i2c_bus4_board_info));
	i2c_register_board_info(5, x3_i2c_bus5_board_info,
			ARRAY_SIZE(x3_i2c_bus5_board_info));
}

static struct tegra_i2c_platform_data x3_i2c1_platform_data = {
	.adapter_nr	= 0,
	.bus_count	= 1,
	.bus_clk_rate	= { 400000, 0 },		/* fastmode */
	.scl_gpio		= {TEGRA_GPIO_PC4, 0},
	.sda_gpio		= {TEGRA_GPIO_PC5, 0},
	.arb_recovery = arb_lost_recovery,
};

static struct tegra_i2c_platform_data x3_i2c2_platform_data = {
	.adapter_nr	= 1,
	.bus_count	= 1,
	.bus_clk_rate	= { 400000, 0 },		//camera camp 100K -> 400K
	.is_clkon_always = true,
	.scl_gpio		= {TEGRA_GPIO_PT5, 0},
	.sda_gpio		= {TEGRA_GPIO_PT6, 0},
	.arb_recovery = arb_lost_recovery,
};

static struct tegra_i2c_platform_data x3_i2c3_platform_data = {
	.adapter_nr	= 2,
	.bus_count	= 1,
	.bus_clk_rate	= { 400000, 0 },
	.scl_gpio		= {TEGRA_GPIO_PBB1, 0},
	.sda_gpio		= {TEGRA_GPIO_PBB2, 0},
	.arb_recovery = arb_lost_recovery,
};

static struct tegra_i2c_platform_data x3_i2c4_platform_data = {
	.adapter_nr	= 3,
	.bus_count	= 1,
	.bus_clk_rate	= { 100000, 0 },
	.scl_gpio		= {TEGRA_GPIO_PV4, 0},
	.sda_gpio		= {TEGRA_GPIO_PV5, 0},
	.arb_recovery = arb_lost_recovery,
};

static struct tegra_i2c_platform_data x3_i2c5_platform_data = {
	.adapter_nr	= 4,
	.bus_count	= 1,
	.bus_clk_rate	= { 400000, 0 },
	.scl_gpio		= {TEGRA_GPIO_PZ6, 0},
	.sda_gpio		= {TEGRA_GPIO_PZ7, 0},
	.arb_recovery = arb_lost_recovery,
};

#ifdef CONFIG_I2C_GPIO
static struct i2c_gpio_platform_data x3_gpio_i2c_platform_data = {
	.sda_pin		= TEGRA_GPIO_PQ7,
	.sda_is_open_drain	= 1,
	.scl_pin		= TEGRA_GPIO_PQ6,
	.scl_is_open_drain	= 1,	
	.scl_is_output_only = 1,
	.udelay			= 5,	
};
#endif

void __init x3_i2c_init(void)
{
	tegra_i2c_device1.dev.platform_data = &x3_i2c1_platform_data;
	tegra_i2c_device2.dev.platform_data = &x3_i2c2_platform_data;
	tegra_i2c_device3.dev.platform_data = &x3_i2c3_platform_data;
	tegra_i2c_device4.dev.platform_data = &x3_i2c4_platform_data;
	tegra_i2c_device5.dev.platform_data = &x3_i2c5_platform_data;
#ifdef CONFIG_I2C_GPIO
	tegra_gpio_i2c.dev.platform_data = &x3_gpio_i2c_platform_data;
#endif

	x3_i2c_dev_init();

#ifdef CONFIG_I2C_GPIO
	platform_device_register(&tegra_gpio_i2c);
#endif
	platform_device_register(&tegra_i2c_device5);
	platform_device_register(&tegra_i2c_device4);
	platform_device_register(&tegra_i2c_device3);
	platform_device_register(&tegra_i2c_device2);
	platform_device_register(&tegra_i2c_device1);
}
