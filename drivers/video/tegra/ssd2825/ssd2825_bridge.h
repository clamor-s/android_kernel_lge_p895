/*
 * Xilinx SPI device driver API and platform data header file
 *
 * Copyright (c) 2009 Intel Corporation
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef _SPI_RGB_BRIDGE_H_
#define _SPI_RGB_BRIDGE_H_

#include <linux/spi/spi.h>

#define STR_RGB_BRIDGE	"rgb_bridge_spi"

#define SSD2825_DEVICE_ID_REG			0xB0
#define SSD2825_RGB_INTERFACE_CTRL_REG_1	0xB1
#define SSD2825_RGB_INTERFACE_CTRL_REG_2	0xB2
#define SSD2825_RGB_INTERFACE_CTRL_REG_3	0xB3
#define SSD2825_RGB_INTERFACE_CTRL_REG_4	0xB4
#define SSD2825_RGB_INTERFACE_CTRL_REG_5	0xB5
#define SSD2825_RGB_INTERFACE_CTRL_REG_6	0xB6
#define SSD2825_CONFIGURATION_REG		0xB7
#define SSD2825_VC_CTRL_REG			0xB8
#define SSD2825_PLL_CTRL_REG			0xB9
#define SSD2825_PLL_CONFIGURATION_REG		0xBA
#define SSD2825_CLOCK_CTRL_REG			0xBB
#define SSD2825_PACKET_SIZE_CTRL_REG_1		0xBC
#define SSD2825_PACKET_SIZE_CTRL_REG_2		0xBD
#define SSD2825_PACKET_SIZE_CTRL_REG_3		0xBE
#define SSD2825_PACKET_DROP_REG			0xBF
#define SSD2825_OPERATION_CTRL_REG		0xC0
#define SSD2825_MAX_RETURN_SIZE_REG		0xC1
#define SSD2825_RETURN_DATA_COUNT_REG		0xC2
#define SSD2825_ACK_RESPONSE_REG		0xC3
#define SSD2825_LINE_CTRL_REG			0xC4
#define SSD2825_INTERRUPT_CTRL_REG		0xC5
#define SSD2825_INTERRUPT_STATUS_REG		0xC6
#define SSD2825_ERROR_STATUS_REG		0xC7
#define SSD2825_DATA_FORMAT_REG			0xC8
#define SSD2825_DELAY_ADJ_REG_1			0xC9
#define SSD2825_DELAY_ADJ_REG_2			0xCA
#define SSD2825_DELAY_ADJ_REG_3			0xCB
#define SSD2825_DELAY_ADJ_REG_4			0xCC
#define SSD2825_DELAY_ADJ_REG_5			0xCD
#define SSD2825_DELAY_ADJ_REG_6			0xCE
#define SSD2825_HS_TX_TIMER_REG_1		0xCF
#define SSD2825_HS_TX_TIMER_REG_2		0xD0
#define SSD2825_LP_RX_TIMER_REG_1		0xD1
#define SSD2825_LP_RX_TIMER_REG_2		0xD2
#define SSD2825_TE_STATUS_REG			0xD3
#define SSD2825_SPI_READ_REG			0xD4
#define SSD2825_PLL_LOCK_REG			0xD5
#define SSD2825_TEST_REG			0xD6
#define SSD2825_TE_COUNT_REG			0xD7
#define SSD2825_ANALOG_CTRL_REG_1		0xD8
#define SSD2825_ANALOG_CTRL_REG_2		0xD9
#define SSD2825_ANALOG_CTRL_REG_3		0xDA
#define SSD2825_ANALOG_CTRL_REG_4		0xDB
#define SSD2825_INTERRUPT_OUT_CTRL_REG		0xDC
#define SSD2825_RGB_INTERFACE_CTRL_REG_7	0xDD
#define SSD2825_LANE_CONFIGURATION_REG		0xDE
#define SSD2825_DELAY_ADJ_REG_7			0xDF
#define SSD2825_INPUT_PIN_CTRL_REG_1		0xE0
#define SSD2825_INPUT_PIN_CTRL_REG_2		0xE1
#define SSD2825_BIDIR_PIN_CTRL_REG_1		0xE2
#define SSD2825_BIDIR_PIN_CTRL_REG_2		0xE3
#define SSD2825_BIDIR_PIN_CTRL_REG_3		0xE4
#define SSD2825_BIDIR_PIN_CTRL_REG_4		0xE5
#define SSD2825_BIDIR_PIN_CTRL_REG_5		0xE6
#define SSD2825_BIDIR_PIN_CTRL_REG_6		0xE7
#define SSD2825_BIDIR_PIN_CTRL_REG_7		0xE8
#define SSD2825_CABC_BRIGHTNESS_CTRL_REG_1	0xE9
#define SSD2825_CABC_BRIGHTNESS_CTRL_REG_2	0xEA
#define SSD2825_CABC_BRIGHTNESS_STATUS_REG	0xEB
#define SSD2825_READ_REG			0xFF

#define SSD2825_CONF_REG_HS	BIT(0)	// 0 - LP mode; 1 - HS mode
#define SSD2825_CONF_REG_CKE	BIT(1)	// 0 - Clock lane can enter LP; 1 - Clock lane will enter HS
#define SSD2825_CONF_REG_SLP	BIT(2)	// Sleep mode switch
#define SSD2825_CONF_REG_VEN	BIT(3)	// Video mode switch
#define SSD2825_CONF_REG_HCLK	BIT(4)	// HS clock switch
#define SSD2825_CONF_REG_CSS	BIT(5)	// 0 - The clock source is tx_clk; 1 - The clock source is pclk
#define SSD2825_CONF_REG_DCS	BIT(6)	// 0 - Generic packet in use; 1 - DCS packet in use
#define SSD2825_CONF_REG_REN	BIT(7)	// 0 - Write operation; 1 - Read operation
#define SSD2825_CONF_REG_ECD	BIT(8)	// ECC CRC Check switch
#define SSD2825_CONF_REG_EOT	BIT(9)	// EOT Packet send switch
#define SSD2825_CONF_REG_LPE	BIT(10)	// Long Packet enable switch

struct bridge_platform_data{
	u16 lcd_en;
#if defined(CONFIG_MACH_VU10)
	u16 lcd_en_3v;
#endif
	u16 bridge_en;
	u16 lcd_reset_n;
	u16 bridge_reset_n;
	u8 mode;
	u8 bits_per_word;
	u32 max_speed_hz;
};

int ssd2825_bridge_enable(void);
int ssd2825_bridge_disable(void);

#endif // _SPI_RGB_BRIDGE_H_
