/*
 * arch/arm/mach-tegra/baseband-xmm-power.c
 *
 * Copyright (C) 2011 NVIDIA Corporation
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

//#define DEBUG

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/wakelock.h>
#include <linux/spinlock.h>
#include <linux/usb.h>
#include <linux/pm_runtime.h>
#include <linux/suspend.h>
#include <linux/notifier.h>
#include <linux/reboot.h>

#include <mach/usb_phy.h>
#include "board.h"
#include "devices.h"
#include "gpio-names.h"
#include "baseband-xmm-power.h"

MODULE_LICENSE("GPL");

unsigned long modem_ver = XMM_MODEM_VER_1145;
EXPORT_SYMBOL(modem_ver);

unsigned long modem_flash = 1;
EXPORT_SYMBOL(modem_flash);

unsigned long modem_pm = 1;
EXPORT_SYMBOL(modem_pm);

unsigned long enum_delay_ms = 1000; /* ignored if !modem_flash */
bool enum_success = false;

const char *pwrstate_cmt[] = {
	"UNINT",	/* BBXMM_PS_UNINIT */
	"INIT",		/* BBXMM_PS_INIT */
	"L0",		/* BBXMM_PS_L0 */
	"L0TOL2",	/* BBXMM_PS_L0TOL2 */
	"L2",		/* BBXMM_PS_L2 */
	"L2TOL0",	/* BBXMM_PS_L2TOL0 */
	"L2TOL3",	/* BBXMM_PS_L2TOL3 */
	"L3",		/* BBXMM_PS_L3 */
	"L3TOL0",	/* BBXMM_PS_L3TOL0 */
};

static struct gpio tegra_baseband_gpios[] = {
	{ -1, GPIOF_OUT_INIT_LOW,  "BB_RSTn" },
	{ -1, GPIOF_OUT_INIT_LOW,  "BB_ON"   },
	{ -1, GPIOF_OUT_INIT_LOW,  "IPC_BB_WAKE" },
	{ -1, GPIOF_IN,            "IPC_AP_WAKE" },
	{ -1, GPIOF_OUT_INIT_HIGH, "IPC_HSIC_ACTIVE" },
	{ -1, GPIOF_IN,            "IPC_HSIC_SUS_REQ" },
};

static enum {
	IPC_AP_WAKE_UNINIT,
	IPC_AP_WAKE_IRQ_READY,
	IPC_AP_WAKE_INIT1,
	IPC_AP_WAKE_INIT2,
	IPC_AP_WAKE_L,
	IPC_AP_WAKE_H,
} ipc_ap_wake_state;

enum baseband_xmm_powerstate_t baseband_xmm_powerstate;
static struct workqueue_struct *workqueue;
static struct work_struct init1_work;
static struct baseband_power_platform_data *baseband_power_driver_data;
static bool register_hsic_device;
static int enum_repeat = ENUM_REPEAT_TRY_CNT;

void baseband_xmm_set_power_status(unsigned int status)
{
	struct baseband_power_platform_data *data = baseband_power_driver_data;

	pr_info("%s n(%d),o(%d)\n", __func__, status, baseband_xmm_powerstate );

	if (baseband_xmm_powerstate == status)
		return;

	switch (status) {
	case BBXMM_PS_L0:
		pr_info("PM_ST : L0\n");
		baseband_xmm_powerstate = status;

		gpio_set_value(data->modem.xmm.ipc_hsic_active, 1);
		pr_info("GPIO [W]: L0 Host_active -> 1 \n");

		pr_info("gpio host active high->\n");
		break;

	default:
		baseband_xmm_powerstate = status;
		break;
	}

	pr_info("BB XMM POWER STATE = %s(%d)\n", pwrstate_cmt[status], status);
}
EXPORT_SYMBOL_GPL(baseband_xmm_set_power_status);

static irqreturn_t baseband_xmm_power_ipc_ap_wake_irq(int irq, void *dev_id)
{
	int value;
	struct baseband_power_platform_data *data = baseband_power_driver_data;
	
	value = gpio_get_value(baseband_power_driver_data->modem.xmm.ipc_ap_wake);
	pr_info("%s g(%d), wake_st(%d)\n", __func__, value, ipc_ap_wake_state);

	if (ipc_ap_wake_state == IPC_AP_WAKE_IRQ_READY) {
		if (!value) {
			pr_info("%s - IPC_AP_WAKE_INIT1" " - got falling edge\n", __func__);
			/* go to IPC_AP_WAKE_INIT1 state */
			ipc_ap_wake_state = IPC_AP_WAKE_INIT1;
			/* queue work */
			queue_work(workqueue, &init1_work);
		}
	} else if (ipc_ap_wake_state == IPC_AP_WAKE_INIT1) {
		if (value) {
			pr_info("%s - IPC_AP_WAKE_INIT2" " - got rising edge\n",__func__);
			/* go to IPC_AP_WAKE_INIT2 state */
			ipc_ap_wake_state = IPC_AP_WAKE_INIT2;
		}
	} else {
		if (!value) {
			pr_info("%s - falling\n", __func__);
			/* First check it a CP ack or CP wake  */
			ipc_ap_wake_state = IPC_AP_WAKE_L;
		} else {
			pr_info("%s - rising\n", __func__);
			value = gpio_get_value(data->modem.xmm.ipc_bb_wake);
			if (value) {
				/* Clear the slave wakeup request */
				gpio_set_value(data->modem.xmm.ipc_bb_wake, 0);
				pr_info("GPIO [W]: Slave_wake -> 0 \n"); 
			}

			gpio_set_value(data->modem.xmm.ipc_hsic_active, 1);
			ipc_ap_wake_state = IPC_AP_WAKE_H;
		}
	}

	return IRQ_HANDLED;
}
EXPORT_SYMBOL(baseband_xmm_power_ipc_ap_wake_irq);

//#define ENUM_REPEAT_TRY_CNT 3
//#define MODEM_ENUM_TIMEOUT_500MS 16 /* 8 sec */
//#define MODEM_ENUM_TIMEOUT_200MS 25 /* 5 sec */

static void baseband_xmm_power_flash_pm_ver_ge_1145_recovery(void)
{
	int timeout_500ms = 16; /* 8 sec */
	int timeout_200ms = 0;
	long err;
	
	pr_info("%s {\n", __func__);

	/* check for platform data */
	if (!baseband_power_driver_data)
		return;

	/* waiting ap_wake up */
	while (ipc_ap_wake_state == IPC_AP_WAKE_INIT1 && timeout_500ms--)
		msleep(500);
	
	if (ipc_ap_wake_state != IPC_AP_WAKE_INIT2) {
		pr_err("err : modem boot repeat condition state(%d)\n", ipc_ap_wake_state);
		timeout_200ms = MODEM_ENUM_TIMEOUT_200MS;
	}
	
	/* check if enumeration succeeded */
	/* waiting ttyacm dev to be created */
	do {
		mm_segment_t oldfs;
		struct file *filp;
		oldfs = get_fs();
		set_fs(KERNEL_DS);
		filp = filp_open("/dev/ttyACM0",
			O_RDONLY, 0);
		if (IS_ERR(filp) || (filp == NULL)) {
			msleep(200);
			err = PTR_ERR(filp);
		} else {
			pr_info("ttyACM0 created OK\n");
			enum_success = true;
			enum_repeat = ENUM_REPEAT_TRY_CNT;
			filp_close(filp, NULL);
			break;
		}
		set_fs(oldfs);
	} while (++timeout_200ms <= MODEM_ENUM_TIMEOUT_200MS);

	/*
	TODO:power on/off routine
	if repeat does not work.
	*/
	if (!enum_repeat)
		pr_info("failed to enumerate modem software" " - too many retry attempts\n");

	if (!enum_success && enum_repeat) {
		pr_info("start repeat enum - attempt #%d\n", ENUM_REPEAT_TRY_CNT - --enum_repeat);
		ipc_ap_wake_state = IPC_AP_WAKE_IRQ_READY;

		/* unregister usb host controller */
		if (baseband_power_driver_data->hsic_unregister && register_hsic_device == false)
			baseband_power_driver_data->hsic_unregister(baseband_power_driver_data->modem.xmm.hsic_device);

		register_hsic_device = true;
	
		/* wait X ms */
		msleep(500);

		/* set IPC_HSIC_ACTIVE low */
		gpio_set_value(baseband_power_driver_data->modem.xmm.ipc_hsic_active, 0);

		pr_info("GPIO [W]: Host_active -> 0 \n"); 
	}
}

static void baseband_xmm_power_init1_work(struct work_struct *work)
{
	int value;

	pr_info("%s {\n", __func__);
	
	if (register_hsic_device && baseband_power_driver_data->hsic_register) {
		pr_info("%s: register usb host controller\n", __func__);
		baseband_power_driver_data->modem.xmm.hsic_device =
				baseband_power_driver_data->hsic_register();
		register_hsic_device = false;

		baseband_xmm_power_flash_pm_ver_ge_1145_recovery();
	}
      
	pr_info("%s }\n", __func__);
}

static void baseband_xmm_power(void)
{
	pr_info("%s\n", __func__);

	gpio_set_value(baseband_power_driver_data->
			modem.xmm.ipc_hsic_active, 1);

	pr_info("GPIO [W]: Host_active -> 1 \n"); 
			
	/* reset / power on sequence */
	gpio_set_value(baseband_power_driver_data->modem.xmm.bb_on, 0);
	mdelay(50);
	gpio_set_value(baseband_power_driver_data->modem.xmm.bb_rst, 0);
	mdelay(200);
	gpio_set_value(baseband_power_driver_data->modem.xmm.bb_rst, 1);	
	mdelay(50);
	
	gpio_set_value(baseband_power_driver_data->modem.xmm.bb_on, 1);
	udelay(60);
	gpio_set_value(baseband_power_driver_data->modem.xmm.bb_on, 0);

	gpio_set_value(baseband_power_driver_data->
			modem.xmm.ipc_hsic_active, 0);			
	pr_info("%s: ver > 1145:" "GPIO [W]: Host_active -> 0 \n",__func__); 
}

static int baseband_xmm_power_driver_probe(struct platform_device *device)
{
	struct baseband_power_platform_data *data
		= (struct baseband_power_platform_data *)device->dev.platform_data;
	unsigned long flags;	
	int err;

	pr_info("%s\n", __func__);
	pr_info("[XMM] enum_delay_ms=%ld\n", enum_delay_ms);

	/* check for platform data */
	if (!data)
		return -ENODEV;

	/* check if supported modem */
	if (data->baseband_type != BASEBAND_XMM) {
		pr_err("unsuppported modem\n");
		return -ENODEV;
	}

	/* save platform data */
	baseband_power_driver_data = data;

	/* request baseband gpio(s) */
	tegra_baseband_gpios[0].gpio = baseband_power_driver_data->modem.xmm.bb_rst;
	tegra_baseband_gpios[1].gpio = baseband_power_driver_data->modem.xmm.bb_on;
	tegra_baseband_gpios[2].gpio = baseband_power_driver_data->modem.xmm.ipc_bb_wake;
	tegra_baseband_gpios[3].gpio = baseband_power_driver_data->modem.xmm.ipc_ap_wake;
	tegra_baseband_gpios[4].gpio = baseband_power_driver_data->modem.xmm.ipc_hsic_active;
	tegra_baseband_gpios[5].gpio = baseband_power_driver_data->modem.xmm.ipc_hsic_sus_req;

	err = gpio_request_array(tegra_baseband_gpios, ARRAY_SIZE(tegra_baseband_gpios));
	if (err < 0) {
		pr_err("%s - request gpio(s) failed\n", __func__);
		return -ENODEV;
	}

	/* request baseband irq(s) */
	pr_info("%s: request_irq IPC_AP_WAKE_IRQ\n", __func__);
	err = request_threaded_irq(gpio_to_irq(data->modem.xmm.ipc_ap_wake), NULL,
				   baseband_xmm_power_ipc_ap_wake_irq, 
				   IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, 
				   "IPC_AP_WAKE_IRQ", NULL);
	if (err < 0) {
		pr_err("%s - request irq IPC_AP_WAKE_IRQ failed\n",__func__);
		return err;
	}

	pr_info("%s: ver > 1145: AP_WAKE_READY\n", __func__);
	/* ver 1145 or later starts in READY state */
	/* ap_wake keeps low util CP starts to initiate hsic hw. */
	/* ap_wake goes up during cp hsic init and then */
	/* it goes down when cp hsic ready */
	ipc_ap_wake_state = IPC_AP_WAKE_IRQ_READY;

	/* init work queue */
	workqueue = create_singlethread_workqueue("baseband_xmm_power_workqueue");
	if (!workqueue) {
		pr_err("cannot create workqueue\n");
		return -1;
	}
	
	baseband_xmm_power();

	/* init work objects */
	INIT_WORK(&init1_work, baseband_xmm_power_init1_work);

	/* init state variables */
	register_hsic_device = true;
	baseband_xmm_powerstate = BBXMM_PS_UNINIT;

	pr_info("%s }\n", __func__);

	return 0;
}

static int baseband_xmm_power_driver_remove(struct platform_device *device)
{
	struct baseband_power_platform_data *data
		= (struct baseband_power_platform_data *)
			device->dev.platform_data;

	pr_info("%s\n", __func__);

	/* check for platform data */
	if (!data)
		return 0;

	/* free baseband irq(s) */
	free_irq(gpio_to_irq(baseband_power_driver_data->modem.xmm.ipc_ap_wake), NULL);

	/* free baseband gpio(s) */
	gpio_free_array(tegra_baseband_gpios,ARRAY_SIZE(tegra_baseband_gpios));

	/* destroy wake lock */
	destroy_workqueue(workqueue);

	/* unregister usb host controller */
	if (data->hsic_unregister)
		data->hsic_unregister(data->modem.xmm.hsic_device);
	else
		pr_err("%s: hsic_unregister is missing\n", __func__);

	return 0;
}

static int baseband_xmm_power_driver_shutdown(struct platform_device *device)
{
	pr_info("%s\n", __func__);

	disable_irq(gpio_to_irq(baseband_power_driver_data->modem.xmm.ipc_ap_wake));
	
	gpio_set_value(baseband_power_driver_data->modem.xmm.bb_on, 0);
	udelay(200);	
	gpio_set_value(baseband_power_driver_data->modem.xmm.bb_rst, 0);
	mdelay(20);

	return 0;
}

static struct platform_driver baseband_power_driver = {
	.driver = {
		.name = "baseband_xmm_power",
	},
	.probe = baseband_xmm_power_driver_probe,
	.remove = baseband_xmm_power_driver_remove,
	.shutdown = baseband_xmm_power_driver_shutdown,
};
module_platform_driver(baseband_power_driver);

MODULE_DESCRIPTION("Baseband power supply driver");
MODULE_LICENSE("GPL");
