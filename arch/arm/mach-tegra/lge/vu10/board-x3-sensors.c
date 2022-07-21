/*
 * arch/arm/mach-tegra/board-x3-sensors.c
 *
 * Copyright (c) 2011, NVIDIA CORPORATION, All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * Neither the name of NVIDIA CORPORATION nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/mpu.h>
#include <linux/nct1008.h>
#include <linux/regulator/consumer.h>
#ifdef CONFIG_SENSORS_INA230
#include <linux/platform_data/ina230.h>
#endif
#include <mach/gpio.h>
#include <media/ar0832_main.h>
#include <media/imx111.h>
#include <media/mt9m114.h>
#include <media/tps61050.h>
#include <media/ov9726.h>
#include <mach/edp.h>
#include <linux/slab.h>
#include <mach/thermal.h>


#include <mach-tegra/cpu-tegra.h>
#include <mach-tegra/gpio-names.h>
#include <lge/board-x3.h>
#if defined(CONFIG_VIDEO_IMX119)
#include <media/imx119.h>
#endif

#include <media/lm3559.h>
#include <media/dw9714.h>

static struct lm3559_platform_data flash_led_data = {
	.gpio_act = TEGRA_GPIO_PBB3,
};

static struct dw9714_info focuser_data = {
};
#define X3_NCT1008_IRQ	TEGRA_GPIO_PI5

static int nct_get_temp(void *_data, long *temp)
{
	struct nct1008_data *data = _data;
	return nct1008_thermal_get_temp(data, temp);
}

static int nct_get_temp_low(void *_data, long *temp)
{
	struct nct1008_data *data = _data;
	return nct1008_thermal_get_temp_low(data, temp);
}

static int nct_set_limits(void *_data,
			long lo_limit_milli,
			long hi_limit_milli)
{
	struct nct1008_data *data = _data;

	return nct1008_thermal_set_limits(data,
					lo_limit_milli,
					hi_limit_milli);
}

static int nct_set_alert(void *_data,
			 void (*alert_func)(void *),
			 void *alert_data)
{
	struct nct1008_data *data = _data;
	return nct1008_thermal_set_alert(data, alert_func, alert_data);
}

static int nct_set_shutdown_temp(void *_data, long shutdown_temp)
{
	struct nct1008_data *data = _data;
	return nct1008_thermal_set_shutdown_temp(data,
						shutdown_temp);
}

static void nct1008_probe_callback(struct nct1008_data *data)
{
	struct tegra_thermal_device *thermal_device;

	thermal_device = kzalloc(sizeof(struct tegra_thermal_device),
			GFP_KERNEL);
	if (!thermal_device) {
		pr_err("unable to allocate thermal device\n");
		return;
	}

	thermal_device->name = "nct1008";
	thermal_device->data = data;
	thermal_device->id = THERMAL_DEVICE_ID_NCT_EXT;
	thermal_device->offset = TDIODE_OFFSET;
	thermal_device->get_temp = nct_get_temp;
	thermal_device->get_temp_low = nct_get_temp_low;
	thermal_device->set_limits = nct_set_limits;
	thermal_device->set_alert = nct_set_alert;
	thermal_device->set_shutdown_temp = nct_set_shutdown_temp;

	tegra_thermal_device_register(thermal_device);
}

static struct nct1008_platform_data x3_nct1008_pdata = {
	.supported_hwrev	= true,
	.ext_range		= true,
	.conv_rate		= 0x08,
	/* 4 * 2C. Bug 844025 - 1C for device accuracies */
	.offset			= 8, 
	.probe_callback		= nct1008_probe_callback,
};

static struct i2c_board_info x3_i2c4_nct1008_board_info[] = {
	{
		I2C_BOARD_INFO("nct1008", 0x4C),
		.irq = TEGRA_GPIO_TO_IRQ(X3_NCT1008_IRQ),
		.platform_data = &x3_nct1008_pdata,
	}
};

static void x3_nct1008_init(void)
{
	int ret;

	tegra_gpio_enable(X3_NCT1008_IRQ);
	ret = gpio_request(X3_NCT1008_IRQ, "temp_alert");
	if (ret < 0) {
		pr_err("%s: gpio_request failed %d\n", __func__, ret);
		return;
	}

	ret = gpio_direction_input(X3_NCT1008_IRQ);
	if (ret < 0) {
		pr_err("%s: gpio_direction_input failed %d\n", __func__, ret);
		gpio_free(X3_NCT1008_IRQ);
		return;
	}
	
	i2c_register_board_info(4, x3_i2c4_nct1008_board_info,
				ARRAY_SIZE(x3_i2c4_nct1008_board_info));
}

static inline void x3_msleep(u32 t)
{
	/*
	If timer value is between ( 10us - 20ms),
	usleep_range() is recommended.
	Please read Documentation/timers/timers-howto.txt.
	*/
	usleep_range(t*1000, t*1000 + 500);
}

#if defined(CONFIG_VIDEO_IMX111)
struct imx111_platform_data x3_imx111_data = { };

static struct i2c_board_info imx111_i2c2_boardinfo[] = {
	{
		//chen.yingchun 20111221 slave address change for conflict with LM3533 on rev.b board
		//I2C_BOARD_INFO("imx111", 0x36),
		I2C_BOARD_INFO("imx111", 0x10),
		.platform_data = &x3_imx111_data,
	},
	{
		//I2C_BOARD_INFO("imx111_focuser", 0x18),
		I2C_BOARD_INFO("dw9714", 0x0c),
		.platform_data  = &focuser_data,
	},  
	{
		I2C_BOARD_INFO("lm3559",  0x53),
		.platform_data  = &flash_led_data,
	},
};
#endif

#if defined(CONFIG_VIDEO_IMX119)
struct imx119_platform_data x3_imx119_data = { };

static struct i2c_board_info imx119_i2c2_boardinfo[] = {
	{
		I2C_BOARD_INFO("imx119", 0x37),
		.platform_data = &x3_imx119_data,
	},
};
#endif

static void x3_cam_init(void)
{
	int ret;
	int i;

	pr_info("%s:++\n", __func__);

#if defined(CONFIG_VIDEO_IMX111)
	i2c_register_board_info(2, imx111_i2c2_boardinfo,
			ARRAY_SIZE(imx111_i2c2_boardinfo));
#endif

#if defined(CONFIG_VIDEO_IMX119)
	i2c_register_board_info(2, imx119_i2c2_boardinfo,
			ARRAY_SIZE(imx119_i2c2_boardinfo));
#endif
}

#ifdef CONFIG_SENSORS_INA230
static struct ina230_platform_data ina230_platform = {
	.rail_name = "VDD_AC_BAT",
	.current_threshold = TEGRA_CUR_MON_THRESHOLD,
	.resistor = TEGRA_CUR_MON_RESISTOR,
	.min_cores_online = TEGRA_CUR_MON_MIN_CORES,
};

static struct i2c_board_info x3_i2c4_ina230_info[] = {
	{
		I2C_BOARD_INFO("ina230", 0x40),
		.platform_data = &ina230_platform,
		.irq = -1,
	},
};

static int __init x3_ina230_init(void)
{
	return i2c_register_board_info(4, x3_i2c4_ina230_info,
				       ARRAY_SIZE(x3_i2c4_ina230_info));
}
#endif

int __init x3_sensors_init(void)
{
#ifdef CONFIG_SENSORS_ISL29028	
	x3_isl_init();
#endif
	x3_nct1008_init();
#ifdef CONFIG_SENSORS_INA230
	x3_ina230_init();
#endif
	x3_cam_init();

	return 0;
}
