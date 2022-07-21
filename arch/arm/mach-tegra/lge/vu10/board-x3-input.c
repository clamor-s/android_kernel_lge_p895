/*
 * arch/arm/mach-tegra/lge/board-x3-input.c
 *
 * Copyright (C) 2011 LG Electronics, Inc.
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
#include <asm/mach-types.h>
#include <asm/mach/arch.h>

#include <mach-tegra/board.h>
#include <mach-tegra/clock.h>
#include <lge/board-x3.h>
#include <lge/board-x3-input.h>
#include <mach-tegra/devices.h>
#include <mach-tegra/gpio-names.h>

#if defined (CONFIG_SENSORS_APDS990X)
#include <linux/apds990x.h> 
#endif

#if defined (CONFIG_TOUCHSCREEN_SYNAPTICS_COMMON)
int touch_power_control(int on)
{
    return 1;
}

int touch_power_init(void)
{
	int ret = 0;

	printk("[TOUCH] %s\n", __func__);

	ret = gpio_request(TOUCH_3V0_EN, "TOUCH_3V0_EN");
	if (ret < 0){		
		goto failed_second;
	}
	ret = gpio_direction_output(TOUCH_3V0_EN, 0);
	if (ret < 0){
		goto failed_second;
	}
	else {
		tegra_gpio_enable(TOUCH_3V0_EN);
	}
	
	ret = gpio_request(TOUCH_1V8_EN, "TOUCH_1V8_EN");
	if (ret < 0) {		
		goto failed_first;
	}

	ret = gpio_direction_output(TOUCH_1V8_EN, 0);
	if (ret < 0) {
		goto failed_first;
	} else {
		tegra_gpio_enable(TOUCH_1V8_EN);
	}
	
	printk("[TOUCH] %s : successful\n", __func__);

	return 0;

failed_second:
	printk(KERN_ERR "%s(%d) failed\n", __func__, __LINE__);
	gpio_free(TOUCH_3V0_EN);
failed_first:
	printk(KERN_ERR "%s(%d) failed\n", __func__, __LINE__);
	gpio_free(TOUCH_1V8_EN);
	return ret;
}

struct touch_device_caps touch_caps = {
	.button_support 	= 1,
	.number_of_button	= 4,
	.button_name 		= {KEY_MENU, KEY_HOME, KEY_BACK, KEY_SEARCH},	
	.y_button_boundary	= 0,
	.button_margin 		= 10,
	.is_width_supported 	= 1,
	.is_pressure_supported 	= 1,
	.is_id_supported	= 1,
	.max_width 		= 15,
	.max_pressure 		= 0xFF,
	.max_id			= 10,
	.lcd_x			= 768,
	.lcd_y			= 1024,
	.x_max			= 1535, //1520,
	.y_max			= 2047, //2027,	// 2244,
};

struct touch_operation_role touch_role = {
	.operation_mode 	= INTERRUPT_MODE,
	.key_type		= TOUCH_HARD_KEY, /* rev.a : hard_key, rev.b : soft_key */
	.report_mode		= CONTINUOUS_REPORT_MODE,
	.delta_pos_threshold 	= 0,
	.orientation 		= 0,
	.booting_delay 			= 200,
	.reset_delay		= 20,
	.report_period		= 10000000, 	/* 12.5 msec -> 10.0 msec(X3) */
	.suspend_pwr		= POWER_OFF,
	.resume_pwr		= POWER_ON,
	.jitter_filter_enable	= 1,
	.jitter_curr_ratio		= 26,	
	.accuracy_filter_enable	= 1,
	.ghost_finger_solution_enable = 1,
	.irqflags 		= IRQF_TRIGGER_FALLING,
};

struct touch_power_module touch_pwr = {
	.use_regulator	= 0,
	.power		= touch_power_control,
	.gpio_init	= touch_power_init,
};

struct touch_platform_data  synaptics_pdata = {
	.int_pin		= TEGRA_GPIO_PQ3,
	.reset_pin		= TEGRA_GPIO_PO1,
	.maker			= "Synaptics",
	.caps	= &touch_caps,
	.role	= &touch_role,
	.pwr	= &touch_pwr,
};
#endif

#if defined(CONFIG_SENSORS_APDS990X)
#define PROX_LDO_EN		TEGRA_GPIO_PX1
uint32_t prox_pwr_mask = 0; //don't need but

static s32 x3_apds990x_power_init(void)
{
	s32 ret = 0;

	printk(KERN_INFO"%s start!![%d][%d] \n",__func__,__LINE__, PROX_LDO_EN);
	//PROX  LDO Enable : 2.6V & 1.8V
	tegra_gpio_enable(PROX_LDO_EN);
	ret = gpio_request(PROX_LDO_EN, "PROX_LDO_EN");
	if (ret < 0) {
		printk("PROX_Sensor[1] : Fail to request Sensor LDO enabling\n");
		return ret;
	}

	ret=gpio_direction_output(PROX_LDO_EN, 0);
	if (ret < 0) {
		printk("PROX_Sensor[2] : Fail to direct Sensor LDO enabling\n");
		gpio_free(PROX_LDO_EN);
		return ret;
	}       
	
	/* PORXI_LDO_EN */
	//tegra_pinmux_set_pullupdown(TEGRA_PINGROUP_SDMMC3_DAT7, TEGRA_PUPD_NORMAL);
	
	printk(KERN_DEBUG "[PROX_Sensor]: Proximity LDO Enable before toggle "
		"at BOARD: %d, Line[%d]\n", 
	gpio_get_value(PROX_LDO_EN), __LINE__);
}

static int prox_common_power_set(unsigned char onoff, int sensor)
{
	gpio_set_value(PROX_LDO_EN, onoff);
	
	printk(" prox_common_power_set : %d, Line[%d]\n", gpio_get_value(PROX_LDO_EN), __LINE__);

	if (onoff)
		prox_pwr_mask |= sensor;
	else
		prox_pwr_mask &= ~sensor;	

	return 0;	
}

int prox_power_on(int sensor)
{
    int ret = 0;
	ret = prox_common_power_set(1, sensor);
	return ret;
}

int prox_power_off(int sensor)
{
    int ret = 0;
	ret = prox_common_power_set(0, sensor);
	return ret;	
}

static s32 x3_apds990x_irq_set(void)
{
	s32 ret = 0;
	printk(KERN_INFO"[SENSOR-APDS] %s start(%d, %d)!!!!![%d] \n",
			__func__, TEGRA_GPIO_PK2, TEGRA_GPIO_TO_IRQ(TEGRA_GPIO_PK2), __LINE__);

	//PROX INT
	tegra_gpio_enable(TEGRA_GPIO_PK2);
	
	ret = gpio_request(TEGRA_GPIO_PK2, "PROX_INT");
	if (ret < 0) {
		printk("Sensor[1] : Fail to request Proximity INT enabling\n");
		return ret;
	}

	ret = gpio_direction_input(TEGRA_GPIO_PK2);
	if (ret < 0) {
		printk("Sensor[2] : Fail to request Proximity INT enabling\n");
		gpio_free(TEGRA_GPIO_PK2);
		return ret;
	}
}
#endif  

#if defined(MPU_SENSORS_MPU6050B1)

static int sensors_ldo_en = TEGRA_GPIO_PD0;

static s32 x3_mpuirq_init(void)
{
	s32 ret = 0;

//SENSOR INT
	tegra_gpio_enable(TEGRA_GPIO_PH4);

	ret = gpio_request(TEGRA_GPIO_PH4, "SENSOR_INT");
	if (ret < 0)
	{
		printk("Sensor : Fail to request MOTION INT enabling\n");
		return ret;
	}

	ret=gpio_direction_input(TEGRA_GPIO_PH4);
	if (ret < 0)
	{
		printk("Sensor : Fail to request MOTION INT enabling\n");
		gpio_free(TEGRA_GPIO_PH4);
		return ret;
	}
}
#endif  //MPU_SENSORS_MPU6050B1

#if defined (CONFIG_SENSORS_APDS990X)

struct apds990x_proximity_platform_data x3_prox_data = {
        .gpio		= TEGRA_GPIO_PK2,
      	.irq		= TEGRA_GPIO_TO_IRQ(TEGRA_GPIO_PK2),
        .irqflags	= IRQF_TRIGGER_FALLING,
        .power_on 	= prox_power_on,
	.power_off 	= prox_power_off,
};
#endif

#if defined(MPU_SENSORS_MPU6050B1)
#include <linux/mpu.h> 

struct mpu_platform_data mpu6050_data = {
	.int_config      = 0x10,
	.level_shifter 	 = 0,
	.orientation     = { -1,  0,  0,
			0, -1,  0,
			0,  0,  1},
};

struct ext_slave_platform_data mpu_compass_data = {
	.address		  = 0x0E,
	.adapt_num		  = 0,
	.bus              = EXT_SLAVE_BUS_PRIMARY,
	.orientation      = {-1,  0,  0,
				0,  1,  0,
				0,  0, -1},
};
#endif

int __init x3_sensor_input_init(void)
{
#ifdef MPU_SENSORS_MPU6050B1
	x3_mpuirq_init();
#endif

#ifdef CONFIG_SENSORS_APDS990X
	x3_apds990x_power_init();
	x3_apds990x_irq_set();
#endif
}
