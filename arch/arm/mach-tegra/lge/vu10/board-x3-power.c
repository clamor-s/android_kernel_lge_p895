/*
 * arch/arm/mach-tegra/board-x3-power.c
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
#include <linux/i2c.h>
#include <linux/pda_power.h>
#include <linux/platform_device.h>
#include <linux/resource.h>
#include <linux/regulator/machine.h>
#include <linux/regulator/gpio-switch-regulator.h>
#include <linux/mfd/tps80031.h>
#include <linux/regulator/tps80031-regulator.h>
#include <linux/mfd/aat2870.h>
#include <linux/mfd/max77663-core.h>
#include <linux/regulator/max77663-regulator.h>
#include <linux/gpio.h>
#include <linux/io.h>
#include <linux/platform_data/tegra_bpc_mgmt.h>
#include <linux/kmsg_dump.h>
#include <mach/edp.h>
#include <mach/iomap.h>
#include <mach/irqs.h>
#include <mach/pinmux.h>
#include <linux/tps80031-charger.h>

#include <mach-tegra/gpio-names.h>
#include <mach-tegra/board.h>
#include <lge/board-x3.h>
#include <mach-tegra/pm.h>
#include <mach-tegra/wakeups-t3.h>

#define PMC_CTRL		0x0
#define PMC_CTRL_INTR_LOW	(1 << 17)

/************************ MAX77663 based regulator ****************/
#if defined(CONFIG_REGULATOR_MAX77663)
static struct regulator_consumer_supply max77663_sd0_supply[] = {
	REGULATOR_SUPPLY("vdd_cpu", NULL),
};

static struct regulator_consumer_supply max77663_sd1_supply[] = {
	REGULATOR_SUPPLY("vdd_core", NULL),
};

static struct regulator_consumer_supply max77663_sd2_supply[] = {
	REGULATOR_SUPPLY("vddio_sdmmc", "sdhci-tegra.3"),
	REGULATOR_SUPPLY("pwrdet_sdmmc4", NULL),
	REGULATOR_SUPPLY("vddio_bb", NULL),
	REGULATOR_SUPPLY("pwrdet_bb", NULL),
	REGULATOR_SUPPLY("vddio_lcd", NULL),
	REGULATOR_SUPPLY("pwrdet_lcd", NULL),
	REGULATOR_SUPPLY("vddio_uart", NULL),
	REGULATOR_SUPPLY("pwrdet_uart", NULL),
	REGULATOR_SUPPLY("vddio_gmi", NULL),
	REGULATOR_SUPPLY("vddio_sdmmc", "sdhci-tegra.0"),
	REGULATOR_SUPPLY("pwrdet_sdmmc1", NULL),
	REGULATOR_SUPPLY("vddio_audio", NULL),
	REGULATOR_SUPPLY("pwrdet_audio", NULL),
	REGULATOR_SUPPLY("vddio_cam", NULL),
	REGULATOR_SUPPLY("pwrdet_cam", NULL),
	REGULATOR_SUPPLY("vddio_sys", NULL),
};

static struct regulator_consumer_supply max77663_sd3_supply[] = {
	REGULATOR_SUPPLY("vddio_ddr", NULL),
	REGULATOR_SUPPLY("vddio_hsic", NULL),
};

static struct regulator_consumer_supply max77663_ldo0_supply[] = {
	REGULATOR_SUPPLY("avdd_plla_p_c", NULL),
	REGULATOR_SUPPLY("avdd_pllm", NULL),
	REGULATOR_SUPPLY("avdd_pllu_d", NULL),
	REGULATOR_SUPPLY("avdd_pllx", NULL),
};

static struct regulator_consumer_supply max77663_ldo1_supply[] = {
	REGULATOR_SUPPLY("vdd_ddr_hs", NULL),
};

static struct regulator_consumer_supply max77663_ldo2_supply[] = {
	REGULATOR_SUPPLY("avdd_usb", NULL),
	REGULATOR_SUPPLY("avdd_hdmi", NULL),
};

static struct regulator_consumer_supply max77663_ldo3_supply[] = {
	REGULATOR_SUPPLY("vddio_sdmmc", "sdhci-tegra.2"),
	REGULATOR_SUPPLY("pwrdet_sdmmc3", NULL),
};

static struct regulator_consumer_supply max77663_ldo4_supply[] = {
	REGULATOR_SUPPLY("vdd_rtc", NULL),
};

static struct regulator_consumer_supply max77663_ldo5_supply[] = {
	REGULATOR_SUPPLY("vdd_ddr_rx", NULL),
};

static struct regulator_consumer_supply max77663_ldo6_supply[] = {
	REGULATOR_SUPPLY("avdd_osc", NULL),
	REGULATOR_SUPPLY("avdd_usb_pll", NULL),
	REGULATOR_SUPPLY("avdd_hdmi_pll", NULL),
};

static struct regulator_consumer_supply max77663_ldo7_supply[] = {
	REGULATOR_SUPPLY("mhl_1v2", NULL),
};

static struct regulator_consumer_supply max77663_ldo8_supply[] = {
	REGULATOR_SUPPLY("avdd_dsi_csi", NULL),
	REGULATOR_SUPPLY("pwrdet_mipi", NULL),
};

static struct max77663_regulator_fps_cfg max77663_fps_cfg[] = {
	{
		.src		= FPS_SRC_0,
		.en_src		= FPS_EN_SRC_EN0,
		.time_period	= FPS_TIME_PERIOD_DEF,
	},
	{
		.src		= FPS_SRC_1,
		.en_src		= FPS_EN_SRC_EN1,
		.time_period	= FPS_TIME_PERIOD_DEF,
	},
	{
		.src		= FPS_SRC_2,
		.en_src		= FPS_EN_SRC_EN0,
		.time_period	= FPS_TIME_PERIOD_DEF,
	},
};

#define MAX77663_PDATA_INIT(_id, _min_uV, _max_uV, _supply_reg,		\
			    _always_on, _boot_on, _apply_uV,		\
			    _init_apply, _init_enable, _init_uV,	\
			    _fps_src, _fps_pu_period, _fps_pd_period, _flags) \
	static struct max77663_regulator_platform_data max77663_regulator_pdata_##_id = \
	{								\
		.init_data = {						\
			.constraints = {				\
				.min_uV = _min_uV,			\
				.max_uV = _max_uV,			\
				.valid_modes_mask = (REGULATOR_MODE_NORMAL |  \
						     REGULATOR_MODE_STANDBY), \
				.valid_ops_mask = (REGULATOR_CHANGE_MODE |    \
						   REGULATOR_CHANGE_STATUS |  \
						   REGULATOR_CHANGE_VOLTAGE), \
				.always_on = _always_on,		\
				.boot_on = _boot_on,			\
				.apply_uV = _apply_uV,			\
			},						\
			.num_consumer_supplies =			\
				ARRAY_SIZE(max77663_##_id##_supply),	\
			.consumer_supplies = max77663_##_id##_supply,	\
			.supply_regulator = _supply_reg,		\
		},							\
		.init_apply = _init_apply,				\
		.init_enable = _init_enable,				\
		.init_uV = _init_uV,					\
		.fps_src = _fps_src,					\
		.fps_pu_period = _fps_pu_period,			\
		.fps_pd_period = _fps_pd_period,			\
		.fps_cfgs = max77663_fps_cfg,				\
		.flags = _flags,					\
	}

/* SD2 must be always turned on because used as pull-up signal for NRST_IO. */
/* LDO4 must be always turned on because connected with vdd_rtc. */

MAX77663_PDATA_INIT(sd0,  600000, 3387500, NULL, 1, 0, 0,
		    0, 0, -1, FPS_SRC_NONE, -1, -1, EN2_CTRL_SD0);
MAX77663_PDATA_INIT(sd1,  800000, 1587500, NULL, 1, 0, 0,
		    1, 1, -1, FPS_SRC_1, -1, -1, 0);
MAX77663_PDATA_INIT(sd2,  600000, 3387500, NULL, 1, 0, 0,
		    1, 1, -1, FPS_SRC_NONE, -1, -1, 0);
MAX77663_PDATA_INIT(sd3,  600000, 3387500, NULL, 0, 0, 0,
		    1, 1, -1, FPS_SRC_NONE, -1, -1, 0);
MAX77663_PDATA_INIT(ldo0, 800000, 2350000, NULL, 0, 0, 0,
		    1, 1, -1, FPS_SRC_1, -1, -1, 0);
MAX77663_PDATA_INIT(ldo1, 800000, 2350000, NULL, 0, 0, 0,
		    1, 1, -1, FPS_SRC_NONE, -1, -1, 0);
MAX77663_PDATA_INIT(ldo2, 800000, 3950000, NULL, 0, 0, 0,
		    0, 0, -1, FPS_SRC_NONE, -1, -1, 0);
MAX77663_PDATA_INIT(ldo3, 1800000, 1800000, NULL, 0, 0, 0,
		    1, 1, 1800000, FPS_SRC_NONE, -1, -1, 0);
MAX77663_PDATA_INIT(ldo4, 800000, 1587500, NULL, 1, 0, 0,
		    1, 1, -1, FPS_SRC_0, -1, -1, 0);
MAX77663_PDATA_INIT(ldo5, 800000, 3950000, NULL, 1, 0, 0,
		    1, 1, -1, FPS_SRC_0, -1, -1, 0);
MAX77663_PDATA_INIT(ldo6, 800000, 3950000, NULL, 1, 0, 0,
		    1, 1, -1, FPS_SRC_NONE, -1, -1, 0);
MAX77663_PDATA_INIT(ldo7, 800000, 3950000, NULL, 0, 0, 0,
			1, 0, 1250000, FPS_SRC_NONE, -1, -1, 0);
MAX77663_PDATA_INIT(ldo8, 800000, 3950000, max77663_rails(sd2), 0, 0, 0,
			0, 0, -1, FPS_SRC_NONE, -1, -1, 0);

#define MAX77663_REG(_id, _data)				\
	{							\
		.name = "max77663-regulator",			\
		.id = MAX77663_REGULATOR_ID_##_id,		\
		.platform_data = &max77663_regulator_pdata_##_data, \
		.pdata_size = sizeof(max77663_regulator_pdata_##_data), \
	}

#define MAX77663_RTC()						\
	{							\
		.name = "max77663-rtc",				\
		.id = 0,					\
	}

static struct mfd_cell max77663_subdevs[] = {
	MAX77663_REG(SD0, sd0),
	MAX77663_REG(SD1, sd1),
	MAX77663_REG(SD2, sd2),
	MAX77663_REG(SD3, sd3),
	MAX77663_REG(LDO0, ldo0),
	MAX77663_REG(LDO1, ldo1),
	MAX77663_REG(LDO2, ldo2),
	MAX77663_REG(LDO3, ldo3),
	MAX77663_REG(LDO4, ldo4),
	MAX77663_REG(LDO5, ldo5),
	MAX77663_REG(LDO6, ldo6),
	MAX77663_REG(LDO7, ldo7),
	MAX77663_REG(LDO8, ldo8),
	MAX77663_RTC(),
};
                                                     
struct max77663_gpio_config max77663_gpio_cfg[] = {
	/*
	 * 4 AP usb detect..
	 */
        {
		.gpio		= MAX77663_GPIO1,
		.dir		= GPIO_DIR_OUT,
		.out_drv	= GPIO_OUT_DRV_OPEN_DRAIN,
		.alternate	= GPIO_ALT_DISABLE,

 	},
	/* 
	 * 4 sleep clock..
	 */
	{
		.gpio		= MAX77663_GPIO4,
		.dir		= GPIO_DIR_OUT,
		.out_drv	= GPIO_OUT_DRV_PUSH_PULL,
		.alternate	= GPIO_ALT_ENABLE,
	},
};

static struct max77663_platform_data max7763_pdata = {
	.irq_base	= MAX77663_IRQ_BASE,
	.gpio_base	= MAX77663_GPIO_BASE,

	.num_gpio_cfgs	= ARRAY_SIZE(max77663_gpio_cfg),
	.gpio_cfgs	= max77663_gpio_cfg,

	.num_subdevs	= ARRAY_SIZE(max77663_subdevs),
	.sub_devices	= max77663_subdevs,
	.flags			= SLP_LPM_ENABLE,
};
#endif  // CONFIG_REGULATOR_MAX77663

static struct i2c_board_info __initdata x3_regulators[] = {
#if defined(CONFIG_REGULATOR_MAX77663)
	{
		/* The I2C address was determined by OTP factory setting */
		I2C_BOARD_INFO("max77663", 0x1C),
		.irq		= INT_EXTERNAL_PMU,
		.platform_data	= &max7763_pdata,
	},
#endif
};

/************************ GPIO based switch regulator ****************/
#if defined(CONFIG_REGULATOR_GPIO_SWITCH)

#define GREG_INIT(_id, _name, _input_supply, _gpio_nr, _active_low, \
			_init_state, _pg, _enable, _disable)		\
	static struct gpio_switch_regulator_subdev_data gpio_pdata_##_name =  \
	{								\
		.regulator_name	= "gpio-switch-"#_name,			\
		.input_supply	= _input_supply,			\
		.id		= _id,					\
		.gpio_nr	= _gpio_nr,				\
		.pin_group	= _pg,					\
		.active_low	= _active_low,				\
		.init_state	= _init_state,				\
		.voltages	= gpio_switch_##_name##_voltages,	\
		.n_voltages	= ARRAY_SIZE(gpio_switch_##_name##_voltages), \
		.num_consumer_supplies =				\
				ARRAY_SIZE(gpio_switch_##_name##_supply), \
		.consumer_supplies = gpio_switch_##_name##_supply,	\
		.constraints = {					\
			.valid_modes_mask = (REGULATOR_MODE_NORMAL |	\
					REGULATOR_MODE_STANDBY),	\
			.valid_ops_mask = (REGULATOR_CHANGE_MODE |	\
					REGULATOR_CHANGE_STATUS |	\
					REGULATOR_CHANGE_VOLTAGE),	\
		},							\
		.enable_rail = _enable,					\
		.disable_rail = _disable,				\
	}

#define ADD_GPIO_REG(_name)	(&gpio_pdata_##_name)

#define MHL_LDO_EN      TEGRA_GPIO_PV2
#define SENSOR_LDO_EN   TEGRA_GPIO_PX7
#define SENSOR_LDO_EN2  TEGRA_GPIO_PD2
#define VFUSE_LDO_EN	TEGRA_GPIO_PK7

static struct regulator_consumer_supply gpio_switch_ldo_mhl_en_supply[] = {
        REGULATOR_SUPPLY("avdd_vhdmi_vtherm", NULL),
};
static int gpio_switch_ldo_mhl_en_voltages[] = {3300};

static struct regulator_consumer_supply gpio_switch_ldo_sensor_3v0_en_supply[] = {
        REGULATOR_SUPPLY("vdd_nct1008", NULL),
        REGULATOR_SUPPLY("vdd_ina230", NULL),
        REGULATOR_SUPPLY("vdd_3v_ldo", NULL),
};
static int gpio_switch_ldo_sensor_3v0_en_voltages[] = {3000};

static struct regulator_consumer_supply gpio_switch_ldo_sensor_1v8_en_supply[] = {
        REGULATOR_SUPPLY("vlg_1v8_ldo", NULL),
};
static int gpio_switch_ldo_sensor_1v8_en_voltages[] = {1800};

static struct regulator_consumer_supply gpio_switch_ldo_vdd_fuse_3v3_en_supply[] = {
        REGULATOR_SUPPLY("vdd_fuse", NULL),
};
static int gpio_switch_ldo_vdd_fuse_3v3_en_voltages[] = {3300};

GREG_INIT(0, ldo_mhl_en, NULL, MHL_LDO_EN, false, 0, 0, 0, 0);
GREG_INIT(1, ldo_sensor_3v0_en, NULL, SENSOR_LDO_EN, false, 0, 0, 0, 0);
GREG_INIT(2, ldo_sensor_1v8_en, NULL, SENSOR_LDO_EN2, false, 0, 0, 0, 0);
GREG_INIT(3, ldo_vdd_fuse_3v3_en, NULL, VFUSE_LDO_EN, false, 0, 0, 0, 0);

static struct gpio_switch_regulator_subdev_data *gswitch_subdevs[] = {
        ADD_GPIO_REG(ldo_mhl_en),
        ADD_GPIO_REG(ldo_sensor_3v0_en),
        ADD_GPIO_REG(ldo_sensor_1v8_en),
        ADD_GPIO_REG(ldo_vdd_fuse_3v3_en),
};

static struct gpio_switch_regulator_platform_data  gswitch_pdata = {
	.num_subdevs	= ARRAY_SIZE(gswitch_subdevs),
	.subdevs	= gswitch_subdevs,
};

static struct platform_device gswitch_regulator_pdata = {
	.name	= "gpio-switch-regulator",
	.id	= -1,
	.dev	= {
		.platform_data = &gswitch_pdata,
	},
};

static int __init gpio_switch_regulator_init(void)
{
	int i;

	for (i = 0; i < gswitch_pdata.num_subdevs; ++i) {
		struct gpio_switch_regulator_subdev_data *gswitch_data =
						gswitch_pdata.subdevs[i];
		if (gswitch_data->gpio_nr <= TEGRA_NR_GPIOS)
			tegra_gpio_enable(gswitch_data->gpio_nr);
	}
	return platform_device_register(&gswitch_regulator_pdata);
}
#endif // CONFIG_REGULATOR_GPIO_SWITCH

static void x3_power_off(void)
{
	int ret;

	pr_err("Powering off the device\n");

#if defined(CONFIG_MFD_MAX77663)
	/*
	 * power-code should be completed in tps80031
	 */
	ret = max77663_power_off();
#endif
	if (ret)
		pr_err("failed to power off\n");

	while(1);
}


int __init x3_regulator_init(void)
{
	void __iomem *pmc = IO_ADDRESS(TEGRA_PMC_BASE);
	u32 pmc_ctrl;

	/* configure the power management controller to trigger PMU
	 * interrupts when low */

	pmc_ctrl = readl(pmc + PMC_CTRL);
	writel(pmc_ctrl | PMC_CTRL_INTR_LOW, pmc + PMC_CTRL);

	i2c_register_board_info(4, x3_regulators,
			ARRAY_SIZE(x3_regulators));

#if defined(CONFIG_REGULATOR_GPIO_SWITCH)
	gpio_switch_regulator_init();
#endif

	pm_power_off = x3_power_off;
	return 0;
}

static void x3_board_suspend(int lp_state, enum suspend_stage stg)
{
	if ((lp_state == TEGRA_SUSPEND_LP1) && (stg == TEGRA_SUSPEND_BEFORE_CPU))
		tegra_console_uart_suspend();
}

static void x3_board_resume(int lp_state, enum resume_stage stg)
{
	if ((lp_state == TEGRA_SUSPEND_LP1) && (stg == TEGRA_RESUME_AFTER_CPU))
		tegra_console_uart_resume();
}

static struct tegra_suspend_platform_data x3_suspend_data = {
	.cpu_timer	= 2000,
	.cpu_off_timer	= 200,
	.suspend_mode	= TEGRA_SUSPEND_LP0,
	.core_timer	= 0x7e7e,
	.core_off_timer = 0,
	.corereq_high	= true,
	.sysclkreq_high	= true,
//	.wake_low	   =  TEGRA_WAKE_GPIO_PV0,
//	.wake_enb	  = TEGRA_WAKE_GPIO_PV0,
	.board_suspend = x3_board_suspend,
	.board_resume = x3_board_resume,
};

static void x3_init_deep_sleep_mode(void)
{
	struct board_info bi;
	tegra_get_board_info(&bi);
	if (bi.board_id == BOARD_1205 && bi.fab == X3_FAB_A01)
		x3_suspend_data.suspend_mode = TEGRA_SUSPEND_LP1;
}

int __init x3_suspend_init(void)
{
	x3_init_deep_sleep_mode();
	tegra_init_suspend(&x3_suspend_data);

	return 0;
}

#ifdef CONFIG_TEGRA_EDP_LIMITS
int __init x3_edp_init(void)
{
	unsigned int regulator_mA;

	regulator_mA = get_maximum_cpu_current_supported();
	if (!regulator_mA) {
		regulator_mA = 4700; /* regular AP30 */
	}
	pr_info("%s: CPU regulator %d mA\n", __func__, regulator_mA);

	tegra_init_cpu_edp_limits(regulator_mA);
	tegra_init_system_edp_limits(TEGRA_BPC_CPU_PWR_LIMIT);
	return 0;
}
#endif

static struct tegra_bpc_mgmt_platform_data bpc_mgmt_platform_data = {
	.gpio_trigger = TEGRA_BPC_TRIGGER,
	.bpc_mgmt_timeout = TEGRA_BPC_TIMEOUT,
};

static struct platform_device x3_bpc_mgmt_device = {
	.name		= "tegra-bpc-mgmt",
	.id		= -1,
	.dev		= {
		.platform_data = &bpc_mgmt_platform_data,
	},
};

void __init x3_bpc_mgmt_init(void)
{
#ifdef CONFIG_SMP

#if defined(CONFIG_MACH_VU10)
	int tegra_bpc_gpio;
	int int_gpio;

	tegra_bpc_gpio = TEGRA_GPIO_PX6;
	bpc_mgmt_platform_data.gpio_trigger = tegra_bpc_gpio;

	int_gpio = TEGRA_GPIO_TO_IRQ(tegra_bpc_gpio);
	tegra_gpio_enable(tegra_bpc_gpio);
#else
	int int_gpio = TEGRA_GPIO_TO_IRQ(TEGRA_BPC_TRIGGER);

	tegra_gpio_enable(TEGRA_BPC_TRIGGER);

#endif
	cpumask_setall(&(bpc_mgmt_platform_data.affinity_mask));
	irq_set_affinity_hint(int_gpio,
				&(bpc_mgmt_platform_data.affinity_mask));
	irq_set_affinity(int_gpio, &(bpc_mgmt_platform_data.affinity_mask));
#endif
	platform_device_register(&x3_bpc_mgmt_device);

	return;
}

static irqreturn_t max77663_irq_manualreset_warning(int irq, void *data)
{
	printk("%s\n", __func__);

	kmsg_dump(KMSG_DUMP_PANIC);

	return IRQ_HANDLED;
}

int __init x3_power_late_init(void)
{
	int irq;
	int ret;

	if (is_tegra_bootmode()) {
		irq = max7763_pdata.irq_base + MAX77663_IRQ_ONOFF_HRDPOWRN;
		ret = request_threaded_irq(irq , NULL, max77663_irq_manualreset_warning,
				IRQF_ONESHOT, "MAX77663_IRQ_ONOFF_HRDPOWRN",
				NULL);
		if (ret) {
			pr_err("%s: Failed to request irq %d\n", __func__, irq);
			return ret;
		}
	}

	return 0;
}


late_initcall(x3_power_late_init);


