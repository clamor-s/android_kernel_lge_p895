/*
 * arch/arm/mach-tegra/board-x3.c
 *
 * Copyright (c) 2011-2012, NVIDIA CORPORATION. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/ctype.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/serial_8250.h>
#include <linux/i2c.h>
#include <linux/dma-mapping.h>
#include <linux/delay.h>
#include <linux/i2c-tegra.h>
#include <linux/gpio.h>
#include <linux/input.h>
#include <linux/spi/spi.h>
#include <linux/tegra_uart.h>
#include <linux/memblock.h>

#include <sound/max98088.h>

#include <mach/clk.h>
#include <mach/iomap.h>
#include <mach/irqs.h>
#include <mach/pinmux.h>
#include <mach/iomap.h>
#include <mach/io.h>
#include <mach/io_dpd.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <mach/i2s.h>
#include <mach/tegra_max98088_pdata.h>
#include <mach/tegra_fiq_debugger.h>

#include <mach-tegra/board.h>
#include <mach-tegra/clock.h>
#include <lge/board-x3.h>
#include <lge/board-x3-usb.h>
#include <lge/board-x3-audio.h>

#include <lge/board-x3-bt.h>
#include <mach-tegra/devices.h>
#include <mach-tegra/gpio-names.h>
#include <mach-tegra/fuse.h>
#include <mach-tegra/pm.h>
#include <mach/thermal.h>

#include <lge/board-x3-gps.h>

#if defined(CONFIG_LGE_BATTERY)
#include <linux/power/lge_battery.h>
#endif

extern void x3_i2c_init(void);
extern int x3_baseband_init(void);
#if defined (CONFIG_REBOOT_MONITOR)
extern void x3_setup_reboot(void);
#else
void x3_setup_reboot(void) {}
#endif

#ifdef CONFIG_TEGRA_THERMAL_THROTTLE
static struct throttle_table throttle_freqs_tj[] = {
	      /*    CPU,    CBUS,    SCLK,     EMC */
	      { 1000000,  NO_CAP,  NO_CAP,  NO_CAP },
	      {  760000,  NO_CAP,  NO_CAP,  NO_CAP },
	      {  760000,  NO_CAP,  NO_CAP,  NO_CAP },
	      {  620000,  NO_CAP,  NO_CAP,  NO_CAP },
	      {  620000,  NO_CAP,  NO_CAP,  NO_CAP },
	      {  620000,  437000,  NO_CAP,  NO_CAP },
	      {  620000,  352000,  NO_CAP,  NO_CAP },
	      {  475000,  352000,  NO_CAP,  NO_CAP },
	      {  475000,  352000,  NO_CAP,  NO_CAP },
	      {  475000,  352000,  250000,  375000 },
	      {  475000,  352000,  250000,  375000 },
	      {  475000,  247000,  204000,  375000 },
	      {  475000,  247000,  204000,  204000 },
	      {  475000,  247000,  204000,  204000 },
	{ CPU_THROT_LOW,  247000,  204000,  102000 },
};
#endif

#ifdef CONFIG_TEGRA_SKIN_THROTTLE
static struct throttle_table throttle_freqs_tskin[] = {
	      /*    CPU,    CBUS,    SCLK,     EMC */
	      { 1000000,  NO_CAP,  NO_CAP,  NO_CAP },
	      {  760000,  NO_CAP,  NO_CAP,  NO_CAP },
	      {  760000,  NO_CAP,  NO_CAP,  NO_CAP },
	      {  620000,  NO_CAP,  NO_CAP,  NO_CAP },
	      {  620000,  NO_CAP,  NO_CAP,  NO_CAP },
	      {  620000,  437000,  NO_CAP,  NO_CAP },
	      {  620000,  352000,  NO_CAP,  NO_CAP },
	      {  475000,  352000,  NO_CAP,  NO_CAP },
	      {  475000,  352000,  NO_CAP,  NO_CAP },
	      {  475000,  352000,  250000,  375000 },
	      {  475000,  352000,  250000,  375000 },
	      {  475000,  247000,  204000,  375000 },
	      {  475000,  247000,  204000,  204000 },
	      {  475000,  247000,  204000,  204000 },
	{ CPU_THROT_LOW,  247000,  204000,  102000 },
};
#endif

static struct balanced_throttle throttle_list[] = {
#ifdef CONFIG_TEGRA_THERMAL_THROTTLE
	{
		.id = BALANCED_THROTTLE_ID_TJ,
		.throt_tab_size = ARRAY_SIZE(throttle_freqs_tj),
		.throt_tab = throttle_freqs_tj,
	},
#endif
#ifdef CONFIG_TEGRA_SKIN_THROTTLE
	{
		.id = BALANCED_THROTTLE_ID_SKIN,
		.throt_tab_size = ARRAY_SIZE(throttle_freqs_tskin),
		.throt_tab = throttle_freqs_tskin,
	},
#endif
};

/* All units are in millicelsius */
static struct tegra_thermal_data thermal_data = {
	.shutdown_device_id = THERMAL_DEVICE_ID_NCT_EXT,
	.temp_shutdown = 90000,
#if defined(CONFIG_TEGRA_EDP_LIMITS) || defined(CONFIG_TEGRA_THERMAL_THROTTLE)
	.throttle_edp_device_id = THERMAL_DEVICE_ID_NCT_EXT,
#endif
#ifdef CONFIG_TEGRA_EDP_LIMITS
	.edp_offset = TDIODE_OFFSET,  /* edp based on tdiode */
	.hysteresis_edp = 3000,
#endif
#ifdef CONFIG_TEGRA_THERMAL_THROTTLE
	.temp_throttle = 85000,
	.tc1 = 0,
	.tc2 = 1,
	.passive_delay = 2000,
#endif
#ifdef CONFIG_TEGRA_SKIN_THROTTLE
	.skin_device_id = THERMAL_DEVICE_ID_SKIN,
	.temp_throttle_skin = 43000,
#endif
};

static __initdata struct tegra_clk_init_table x3_clk_init_table[] = {
	/* name		parent		rate		enabled */
	{ "pll_m",	NULL,		0,		false},
	{ "hda",	"pll_p",	108000000,	false},
	{ "hda2codec_2x","pll_p",	48000000,	false},
	{ "pwm",	"pll_p",	3187500,	false},
	{ "blink",	"clk_32k",	32768,		true},
	{ "pll_a",	NULL,		564480000,	false},
	{ "pll_a_out0",	NULL,		11289600,	false},
	{ "i2s0",	"pll_a_out0",	0,		false},
	{ "i2s1",	"pll_a_out0",	0,		false},
	{ "i2s3",	"pll_a_out0",	0,		false},
	{ "spdif_out",	"pll_a_out0",	0,		false},
	{ "d_audio",	"clk_m",	13000000,	false},
	{ "dam0",	"clk_m",	13000000,	false},
	{ "dam1",	"clk_m",	13000000,	false},
	{ "dam2",	"clk_m",	13000000,	false},
	{ "i2c1",	"pll_p",	3200000,	false},		/* fastmode */	
	{ "audio0",	"i2s0_sync",	0,		false},
	{ "audio1",	"i2s1_sync",	0,		false},
	{ "audio2",	"i2s2_sync",	0,		false},
	{ "audio3",	"i2s3_sync",	0,		false},
	{ "audio4",	"i2s4_sync",	0,		false},
	{ "vi",		"pll_p",	0,		false},
	{ "vi_sensor",	"pll_p",	0,		false},
	{ "i2c5",	"pll_p",	3200000,	false},
	{ "extern3",	"pll_p",	24000000,	true},
	{ "clk_out_3",	"extern3",	24000000,	true},
	{ NULL,		NULL,		0,		0},
};

static __initdata struct tegra_clk_init_table x3_clk_i2s2_table[] = {
	/* name		parent		rate		enabled */
	{ "i2s2",	"pll_a_out0",	0,		false},
	{ NULL,		NULL,		0,		0},
};

#if defined(CONFIG_BD_ADDRESS)
static struct platform_device bd_address_device = {
    .name = "bd_address",
    .id = -1,
};
#endif

static struct platform_device *x3_uart_devices[] __initdata = {
	&tegra_uarta_device,
	&tegra_uartb_device,
	&tegra_uartc_device,
	&tegra_uartd_device,
	&tegra_uarte_device,
};

static struct uart_clk_parent uart_parent_clk[] = {
	[0] = {.name = "clk_m"},
	[1] = {.name = "pll_p"},
#ifndef CONFIG_TEGRA_PLLM_RESTRICTED	
	[2] = {.name = "pll_m"},
#endif	
};
static struct tegra_uart_platform_data x3_uart_pdata;
static struct tegra_uart_platform_data x3_loopback_uart_pdata;
static void __init uart_debug_init(void)
{
	unsigned long rate;
	struct clk *c;

	/* UARTD is the debug port. */
	pr_info("Selecting UARTD as the debug console\n");
	x3_uart_devices[3] = &debug_uartd_device;
	debug_uart_port_base = ((struct plat_serial8250_port *)(
			debug_uartd_device.dev.platform_data))->mapbase;
	debug_uart_clk = clk_get_sys("serial8250.0", "uartd");

	/* Clock enable for the debug channel */
	if (!IS_ERR_OR_NULL(debug_uart_clk)) {
		rate = ((struct plat_serial8250_port *)(
			debug_uartd_device.dev.platform_data))->uartclk;
		pr_info("The debug console clock name is %s\n",
						debug_uart_clk->name);
		c = tegra_get_clock_by_name("pll_p");
		if (IS_ERR_OR_NULL(c))
			pr_err("Not getting the parent clock pll_p\n");
		else
			clk_set_parent(debug_uart_clk, c);

		clk_enable(debug_uart_clk);
		clk_set_rate(debug_uart_clk, rate);
	} else {
		pr_err("Not getting the clock %s for debug console\n",
				debug_uart_clk->name);
	}
}

static void __init x3_uart_init(void)
{
	int i;
	struct clk *c;

	for (i = 0; i < ARRAY_SIZE(uart_parent_clk); ++i) {
		c = tegra_get_clock_by_name(uart_parent_clk[i].name);
		if (IS_ERR_OR_NULL(c)) {
			pr_err("Not able to get the clock for %s\n",
						uart_parent_clk[i].name);
			continue;
		}
		uart_parent_clk[i].parent_clk = c;
		uart_parent_clk[i].fixed_clk_rate = clk_get_rate(c);
	}
	x3_uart_pdata.parent_clk_list = uart_parent_clk;
	x3_uart_pdata.parent_clk_count = ARRAY_SIZE(uart_parent_clk);
	x3_loopback_uart_pdata.parent_clk_list = uart_parent_clk;
	x3_loopback_uart_pdata.parent_clk_count =
						ARRAY_SIZE(uart_parent_clk);
	x3_loopback_uart_pdata.is_loopback = true;
	tegra_uarta_device.dev.platform_data = &x3_uart_pdata;
	tegra_uartb_device.dev.platform_data = &x3_uart_pdata;
	tegra_uartc_device.dev.platform_data = &x3_uart_pdata;
	tegra_uartd_device.dev.platform_data = &x3_uart_pdata;
	/* UARTE is used for loopback test purpose */
	tegra_uarte_device.dev.platform_data = &x3_loopback_uart_pdata;

	/* Register low speed only if it is selected */
	if (!is_tegra_debug_uartport_hs())
		uart_debug_init();

	platform_add_devices(x3_uart_devices,
				ARRAY_SIZE(x3_uart_devices));
}


#if defined(CONFIG_RTC_DRV_TEGRA)	
static struct resource tegra_rtc_resources[] = {
	[0] = {
		.start = TEGRA_RTC_BASE,
		.end = TEGRA_RTC_BASE + TEGRA_RTC_SIZE - 1,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = INT_RTC,
		.end = INT_RTC,
		.flags = IORESOURCE_IRQ,
	},
};

static struct platform_device tegra_rtc_device = {
	.name = "tegra_rtc",
	.id   = -1,
	.resource = tegra_rtc_resources,
	.num_resources = ARRAY_SIZE(tegra_rtc_resources),
};
#endif

static struct platform_device tegra_camera = {
	.name = "tegra_camera",
	.id = -1,
};

struct platform_device hsic_netdevice0 = {
  .name = "hsic_net_device",
  .id = 0,
};

#if defined(CONFIG_LGE_BATTERY)
static struct lge_battery_platform_data lge_battery_plat = {
	.gauge_name     = "fuelgauge",
	.charger_name   = "charger",
#if (CONFIG_ADC_TSC2007)
	.adc_name		= "tsc2007_adc",
#endif	
};

static struct platform_device lge_battery_device = {
	.name   = "lge-battery",
	.id     = -1,
	.dev    = {
		.platform_data  = &lge_battery_plat,
	},
};
#endif

#if defined(CONFIG_BACKLIGHT_LM353X)
struct platform_device keypad_led_device = {
	.name   = "button-backlight",
	.id = -1,
}; 
#endif

#if defined(CONFIG_LEDS_VU10_PWRKEY)
struct platform_device power_led_device = {
	.name   = "power-key-led",
	.id = -1,
};
#endif

struct platform_device lge_mtc_eta_log_device = {
	.name = "lge_mtc_eta_logger",
	.id = -1,		
};

static struct platform_device *x3_devices[] __initdata = {
	&tegra_pmu_device,
#if defined(CONFIG_RTC_DRV_TEGRA)	
	&tegra_rtc_device,
#endif
	&tegra_udc_device,
#if defined(CONFIG_SND_HDA_TEGRA)
	&tegra_hda_device,
#endif
#if defined(CONFIG_TEGRA_IOVMM_SMMU)
	&tegra_smmu_device,
#endif
	&tegra_wdt0_device,
	&tegra_wdt1_device,
	&tegra_wdt2_device,
#if defined(CONFIG_TEGRA_AVP)
	&tegra_avp_device,
#endif
	&tegra_camera,
	&tegra_spi_device4,	
#if defined(CONFIG_MACH_RGB_CONVERTOR_SPI)	
	&tegra_spi_device5, 
#endif	
#if defined(CONFIG_LGE_BATTERY)
	&lge_battery_device,
#endif
#if defined(CONFIG_CRYPTO_DEV_TEGRA_SE)
	&tegra_se_device,
#endif
#if defined(CONFIG_CRYPTO_DEV_TEGRA_AES)
	&tegra_aes_device,
#endif
#if defined(CONFIG_TSPDRV)
	&tegra_pwfm3_device,
#endif
#if defined(CONFIG_BACKLIGHT_LM353X)
	&keypad_led_device,
#endif	
	&lge_mtc_eta_log_device,
#if defined(CONFIG_LEDS_VU10_PWRKEY)
	&power_led_device,
#endif
#if defined(CONFIG_BD_ADDRESS)
	&bd_address_device,
#endif
};

static struct platform_device *x3_audio_devices[] __initdata = {
    &tegra_ahub_device,
    &tegra_dam_device0,
    &tegra_dam_device1,
    &tegra_dam_device2,
    &tegra_i2s_device0,
    &tegra_i2s_device1,
    &tegra_i2s_device2,
    &tegra_i2s_device3,
    &tegra_spdif_device,
    &spdif_dit_device,
    &bluetooth_dit_device,
    &baseband_dit_device,
    &tegra_pcm_device,
    &x3_audio_device,
};
        
static void x3_audio_init(void)
{
#if defined(CONFIG_MACH_VU10)
    if (x3_get_hw_rev_pcb_version() == hw_rev_pcb_type_A)
	((struct tegra_max98088_platform_data *)(x3_audio_device.dev.platform_data))->gpio_int_mic_en = TEGRA_GPIO_PI6;
#endif
    platform_add_devices(x3_audio_devices, ARRAY_SIZE(x3_audio_devices));
}

hw_rev_pcb_type x3_get_hw_rev_pcb_version(void)
{
	return system_rev;
}

static void __init tegra_x3_init(void)
{	
	tegra_clk_init_from_table(x3_clk_i2s2_table);
    	x3_setup_reboot();
	tegra_thermal_init(&thermal_data,
				throttle_list,
				ARRAY_SIZE(throttle_list));
	tegra_clk_init_from_table(x3_clk_init_table);
	x3_pinmux_init();
	x3_uart_init();
	x3_i2c_init();
	x3_regulator_init();
	tegra_io_dpd_init();
	x3_sdhci_init();
	x3_usb_init();
	platform_add_devices(x3_devices, ARRAY_SIZE(x3_devices));
	tegra_ram_console_debug_init();
#ifdef CONFIG_TEGRA_EDP_LIMITS
	x3_edp_init();
#endif
	x3_kbc_init();
	//x3_touch_init();
	x3_gps_init();
if(is_tegra_bootmode())
{
	x3_baseband_init();
}
	x3_panel_init();
	x3_audio_init();
#if defined(CONFIG_BCM4330_RFKILL)
	x3_bt_rfkill();
#endif
	//x3_setup_bluesleep();
	x3_emc_init();
	x3_sensors_init();
	x3_suspend_init();
	x3_bpc_mgmt_init();
	tegra_release_bootloader_fb();
	x3_sensor_input_init();
	tegra_serial_debug_init(TEGRA_UARTD_BASE, INT_WDT_CPU, NULL, -1, -1);
}

static void __init tegra_x3_reserve(void)
{
#if defined(CONFIG_NVMAP_CONVERT_CARVEOUT_TO_IOVMM)
	tegra_reserve(0, SZ_8M, SZ_8M);
#else
	tegra_reserve(SZ_128M, SZ_8M, SZ_8M);
#endif
	tegra_ram_console_debug_reserve(SZ_1M);
}

MACHINE_START(VU10, "vu10")
	.boot_params    = 0x80000100,
	.map_io         = tegra_map_common_io,
	.reserve        = tegra_x3_reserve,
	.init_early	= tegra_init_early,
	.init_irq       = tegra_init_irq,
	.timer          = &tegra_timer,
	.init_machine   = tegra_x3_init,
MACHINE_END
