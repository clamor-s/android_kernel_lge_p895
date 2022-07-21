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


#if defined( LGE_SENSOR_GYROSCOPE)
#include <linux/k3g.h>
#endif

#if defined( LGE_SENSOR_DCOMPASS)
#include <linux/ami306.h>
#endif

#if defined (LGE_SENSOR_ACCELEROMETER)
#include <linux/k3dh.h>
#endif

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

struct lge_sensor_int_gpio{
	s16 num;
	const char *name;
};

static struct lge_sensor_int_gpio lge_sensor_int_data[] = {
#ifdef LGE_SENSOR_ACCELEROMETER
	{
		.num = TEGRA_GPIO_PJ2,
		.name = "MOTION_INT",
		//.config = GPIO_CFG(TEGRA_GPIO_PJ2, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
 	},
#endif //                        
#ifdef LGE_SENSOR_GYROSCOPE
	{
		.num = TEGRA_GPIO_PH4,
		.name = "GYRO_INT",
		//.config = GPIO_CFG(TEGRA_GPIO_PH4, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA),
	},
#endif
#ifdef LGE_SENSOR_DCOMPASS
	{
		.num = TEGRA_GPIO_PH5, 
		.name = "COMPASS_READY",
		//.config = GPIO_CFG(TEGRA_GPIO_PH5, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA),
 	},
#endif	
 	{
		.num = 0,
		//.config = 0,
 	},
};

static int lge_common_sensor_init(void)
{
    int rc;
    int n;

	for(n = 0; n < ARRAY_SIZE(lge_sensor_int_data); n++) {
        if(lge_sensor_int_data[n].num ==0)
			continue;

		rc = gpio_request(lge_sensor_int_data[n].num,lge_sensor_int_data[n].name);
		if (rc)
		{
             printk(KERN_ERR "%s:%d   %s[%d] request failed\n",__func__,__LINE__,lge_sensor_int_data[n].name,lge_sensor_int_data[n].num );
			 gpio_free(lge_sensor_int_data[n].num);
			 continue;
		}
        //gpio_tlmm_config(lge_sensor_int_data[n].config, GPIO_CFG_ENABLE);
		rc = gpio_direction_input(lge_sensor_int_data[n].num);
		if(rc)
		{
             printk(KERN_ERR "%s:%d   %s[%d] direction failed \n",__func__,__LINE__,lge_sensor_int_data[n].name,lge_sensor_int_data[n].num );
			 gpio_free(lge_sensor_int_data[n].num);
			 continue;
		}
	}
    return 0;
}

#if defined (LGE_SENSOR_ACCELEROMETER) || defined (LGE_SENSOR_GYROSCOPE)
uint32_t sensor_pwr_mask = 0;
static int sensor_common_power_init(void)
{
	int ret = -EINVAL;
	printk(KERN_INFO "%s Line: %d\n", __func__, __LINE__); 
	

	//SENSOR_LDO_EN pin Enable for GYRO 3.0V & 1.8V
	tegra_gpio_enable(TEGRA_GPIO_PD0);	

	ret = gpio_request(TEGRA_GPIO_PD0, "SENSOR_LDO_EN");
	if (ret < 0)
	{
		printk("Sensor[1] : Fail to request Sensor LDO enabling\n");
		return ret;
	}

	//ret=gpio_direction_output(TEGRA_GPIO_PD0, 0);
	ret=gpio_direction_output(TEGRA_GPIO_PD0, 1);			//                                                                                  
	if (ret < 0)
	{
		printk("Sensor[2] : Fail to direct Sensor LDO enabling\n");
		gpio_free(TEGRA_GPIO_PD0);
		return ret;
	}

	/* SENSOR_LED_EN */
	//tegra_pinmux_set_pullupdown(TEGRA_PINGROUP_SDMMC3_DAT5, TEGRA_PUPD_NORMAL);
	
	printk(" %s : Sensor LDO Enable before toggle at BOARD: %d, Line[%d]\n",
		__func__,gpio_get_value(TEGRA_GPIO_PD0),__LINE__);
		
	return 0;	

}

static int sensor_common_power_set(unsigned char onoff, int sensor)
{
	int ret = -EINVAL;

	gpio_set_value(TEGRA_GPIO_PD0,onoff);
	
	printk(" sensor_common_power_set : %d, Line[%d]\n",	gpio_get_value(TEGRA_GPIO_PD0),__LINE__);

	if (onoff)
		sensor_pwr_mask |= sensor;
	else
		sensor_pwr_mask &= ~sensor;	

	return 0;	
}

int sensor_power_on(int sensor)
{
    int ret = 0;
	ret = sensor_common_power_set(1, sensor);
	return ret;

}
int sensor_power_off(int sensor)
{
    int ret = 0;
	ret = sensor_common_power_set(0, sensor);
	return ret;	

}

static s32 sensor_common_interrupt_set(void)
{
	s32 ret = 0;

	printk("calling sensor_common_interrupt_set func....\n");

//GYRO INT
	tegra_gpio_enable(TEGRA_GPIO_PH4);

	ret = gpio_request(TEGRA_GPIO_PH4, "GYRO_INT");
	if (ret < 0)
	{
		printk("Sensor : Fail to request GYRO INT enabling\n");
		return ret;
	}

	ret=gpio_direction_input(TEGRA_GPIO_PH4);
	if (ret < 0)
	{
		printk("Sensor : Fail to request GYRO INT enabling\n");
		gpio_free(TEGRA_GPIO_PH4);
		return ret;
	}


//MOTION INT
	tegra_gpio_enable(TEGRA_GPIO_PJ2);

	ret = gpio_request(TEGRA_GPIO_PJ2, "MOTION_INT");
	if (ret < 0)
	{
		printk("Sensor : Fail to request Motion INT enabling\n");
		return ret;
	}

	ret=gpio_direction_input(TEGRA_GPIO_PJ2);
	if (ret < 0)
	{
		printk("Sensor : Fail to request Motion INT enabling\n");
		gpio_free(TEGRA_GPIO_PJ2);
		return ret;
	}

//COMPASS INT
	tegra_gpio_enable(TEGRA_GPIO_PH5);

	ret = gpio_request(TEGRA_GPIO_PH5, "COMPASS_READY");
	if (ret < 0)
	{
		printk("Sensor : Fail to request Compass INT enabling\n");
		return ret;
	}

	ret=gpio_direction_input(TEGRA_GPIO_PH5);
	if (ret < 0)
	{
		printk("Sensor : Fail to request Compass INT enabling\n");
		gpio_free(TEGRA_GPIO_PH5);
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

#if defined (LGE_SENSOR_ACCELEROMETER)
static int k3dh_init(void) { return 0; }
static void k3dh_exit(void) {}

struct k3dh_acc_platform_data accelerometer_pdata = {
	.poll_interval = 100,
	.min_interval = 0,
	.g_range = 0x00,
	.axis_map_x = 1,
	.axis_map_y = 0,
	.axis_map_z = 2,
	.negate_x = 1,
	.negate_y = 1,
	.negate_z = 1,
	.init = k3dh_init,
	.exit = k3dh_exit,
	.power_on = sensor_power_on,
	.power_off = sensor_power_off,	
	.gpio_int1 = -1,
	.gpio_int2 = -1,
};
#endif

#if defined( LGE_SENSOR_GYROSCOPE)
static int k3g_init(void){return 0;}
static void k3g_exit(void){}

struct k3g_platform_data gyroscope_pdata = {
	.fs_range = 0x00 ,
	.axis_map_x = 0,
	.axis_map_y = 1,
	.axis_map_z = 2,
	.negate_x = 0,
	.negate_y = 1,
	.negate_z = 1,
	.init = k3g_init,
	.exit = k3g_exit,
	.power_on = sensor_power_on,
	.power_off = sensor_power_off,
};
#endif

#if defined( LGE_SENSOR_DCOMPASS)
static int ami306_init(void){return 0;}
static void ami306_exit(void){}

struct ami306_platform_data dcompss_pdata = {
	.init = ami306_init,
	.exit = ami306_exit,
	.power_on = sensor_power_on,
	.power_off = sensor_power_off,
	.fdata_mDir   = 18,
	.fdata_sign_x = 1,
	.fdata_sign_y = -1,
	.fdata_sign_z = -1,
	.fdata_order0 = 0,
	.fdata_order1 = 1,
	.fdata_order2 = 2,
};
#endif

int __init x3_sensor_input_init(void)
{
#ifdef MPU_SENSORS_MPU6050B1
	x3_mpuirq_init();
#endif
		
#if defined (LGE_SENSOR)
	sensor_common_power_init();
	sensor_common_interrupt_set();
#endif
#ifdef CONFIG_SENSORS_APDS990X
	x3_apds990x_power_init();
	x3_apds990x_irq_set();
#endif
}
