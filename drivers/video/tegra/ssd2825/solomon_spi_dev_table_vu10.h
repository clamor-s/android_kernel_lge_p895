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

#ifndef _SPI_SOLOMON_TABLE_H_
#define _SPI_SOLOMON_TABLE_H_

/* DUPLICATION OF solomon_spi_table_vu10.h for dev purposes */

#define MIPI_NON_BURST_MODE  //default(2/27)
//#define HITACHI_CABC_ENABLE
//#define CURRENT_BIAS_MIPI_OUTPUT  //default : no use(2/27)
#define HITACHI_GAMMA_S_CURVE
#define GAMMA_3

/* 
 * DEFINITION FOR 5 INCH DISPLAY 
 * 
 * HITACHI/KOE 5" 768x1024 TX13D100VM0EAA MIPI DSI panel
 */
#define HITACHI_INVERSION_SELECT
//#define HITACHI_POWER_SETTING
#define COLUMN_INVERSION
#define HITACHI_DIGITAL_CONTRAST_ADJ

#include "ssd2825_bridge.h"

struct spi_cmd_data16 solomon_init_sequence_set[] = {
		SSD2825_RGB_INTERFACE_CTRL_REG_1, 0x0205, // v_sync_width = 2; h_sync_width = 5
		SSD2825_RGB_INTERFACE_CTRL_REG_2, 0x0A56, // v_back_porch + v_sync_width = 0xA(10); h_back_porch + h_sync_width = 0x56(86) (Because of the Non-burst mode)
		SSD2825_RGB_INTERFACE_CTRL_REG_3, 0x1874, // v_front_porch = 0x18(24); h_front_porch = 0x74(116)
		SSD2825_RGB_INTERFACE_CTRL_REG_4, 0x0300, // 0x0300 = h_active = 768
		SSD2825_RGB_INTERFACE_CTRL_REG_5, 0x0400, // 0x0400 = v_active = 1024
#ifdef MIPI_NON_BURST_MODE
		// video pixel format 24bpp; Non burst mode with sync events
		SSD2825_RGB_INTERFACE_CTRL_REG_6, 0x0007,
#else
		// video pixel format 24bpp; Burst mode
		SSD2825_RGB_INTERFACE_CTRL_REG_6, 0x000B,
#endif
		SSD2825_LANE_CONFIGURATION_REG, 0x0003, // 4 lane mode (DSI_0)
		SSD2825_TEST_REG, 0x0004, // Entry point for the internal buffer used to read the data returned by the MIPI slave.
#ifdef CURRENT_BIAS_MIPI_OUTPUT
		SSD2825_ANALOG_CTRL_REG_1, 0x021C, // MIPI Control the analog Phy input.
#endif
		SSD2825_PLL_CTRL_REG, 0x0000, // PLL power down (bit0); SYS_clk Divider = 1
		SSD2825_LINE_CTRL_REG, 0x0001, // Automatically perform BTA after the next write packet.
		SSD2825_DELAY_ADJ_REG_1, 0x2103, // HZD = 33(dec); HPD = 3(dec)

		SSD2825_PLL_CONFIGURATION_REG, 0x8CD7, // NS=215(dec); MS=12(dec); FR=10 (251 < freq out < 500); 430Mbps

		//change for Hitach ESD
		SSD2825_CLOCK_CTRL_REG, 0x0009, // LP Clock Divider - div by 10

		SSD2825_PLL_CTRL_REG, 0x0001, // PLL power down
		SSD2825_VC_CTRL_REG, 0x0000, // all 0x0

		// set mdelay(10);

		// SSD2825_CONFIGURATION_REG: 0x0342 - 0000 0011 0100 0010
		// bit0 - LP mode on;
		// bit1 - Clock lane will enter HS mode for all the cases.
		// bit2 - Sleep mode is disabled
		// bit3 - Video mode is disabled
		// bit4 - HS clock is enabled
		// bit5 - The clock source is tx_clk
		// bit6 - DCS packet (The packet can be any one of DCS Long Write, DCS Short Write, DCS Read packet, depending on the configuration.)
		// bit7 - Write operation
		// bit8 - ECC CRC Check Disable
		// bit9 - EOT Packet Enable send
		SSD2825_CONFIGURATION_REG, 0x0342,
		SSD2825_VC_CTRL_REG, 0x0000,

		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0002, // Send DSI commands into buffer
		SSD2825_PACKET_DROP_REG, 0x11, 0x00, // MIPI_DCS_EXIT_SLEEP_MODE

		// set mdelay(80);

		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0002, // Send DSI commands into buffer
		SSD2825_PACKET_DROP_REG, 0x36, 0x00, // MIPI_DCS_SET_ADDRESS_MODE

		// set mdelay(20);

		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0002, // Send DSI commands into buffer
		SSD2825_PACKET_DROP_REG, 0x3A, 0x70, // 0x70?; 0x3A = MIPI_DCS_SET_PIXEL_FORMAT

		// SSD2825_CONFIGURATION_REG: 0x0302 - 0000 0011 0000 0010
		// bit0 - LP mode on;
		// bit1 - Clock lane will enter HS mode for all the cases.
		// bit2 - Sleep mode is disabled
		// bit3 - Video mode is disabled
		// bit4 - HS clock is enabled
		// bit5 - The clock source is tx_clk
		// bit6 - Generic packet (The packet can be any one of Generic Long Write, Generic Short Write, Generic Read packet, depending on the configuration.)
		// bit7 - Write operation
		// bit8 - ECC CRC Check Disable
		// bit9 - EOT Packet Enable send
		SSD2825_CONFIGURATION_REG, 0x0302,
		SSD2825_VC_CTRL_REG, 0x0000,

		/* Manufacturer Command Access Protect Off */
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0002,  // Send DSI commands into buffer
		SSD2825_PACKET_DROP_REG, 0xB0, 0x04, // MIPI_DCS_GET_DISPLAY_ID; 0xB0 ?

#if defined(HITACHI_DIGITAL_CONTRAST_ADJ)
		//Digital Contrast Adjustment
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0004, // Send DSI commands into buffer
		SSD2825_PACKET_DROP_REG, 0xCC, 0xDC, 0xB4, 0xFF, // cmd not in MIPI DCS
#endif

#ifdef HITACHI_GAMMA_S_CURVE
		// gamma curves are here check end
#endif

#if defined(HITACHI_INVERSION_SELECT)
		//Panel Driving Setting
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0009, // Send DSI commands into buffer
		SSD2825_PACKET_DROP_REG,
#if defined(COLUMN_INVERSION)
		0xC1, 0x00, 0x50, //Column
		0x03, 0x22, 0x16,
		0x06, 0x60, 0x11,
#else
		0xC1, 0x00, 0x10,//2Lines
		0x03, 0x22, 0x16,
		0x06, 0x60, 0x01,
#endif
#endif

#if defined(HITACHI_POWER_SETTING)
		//Panel Driving Setting
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0009, // Send DSI commands into buffer
		SSD2825_PACKET_DROP_REG,
#if defined(COLUMN_INVERSION)
		0xD1, 0x8E, 0x27, //Column
		0x44, 0x63, 0x97,
		0x63, 0xC9, 0x06,
#else
		0xD1, 0x6E, 0x29, //2DOT
		0x44, 0x42, 0x75,
		0x73, 0xEB, 0x06,
#endif
#endif
		/* Manufacturer Command Access Protect On */
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0002, // Send DSI commands into buffer
		SSD2825_PACKET_DROP_REG, 0xB0, 0x03, // 0x03 = MIPI_DCS_GET_COMPRESSION_MODE; 0xBO ?

		// SSD2825_CONFIGURATION_REG: 0x0342 - 0000 0011 0100 0010
		// bit0 - LP mode on;
		// bit1 - Clock lane will enter HS mode for all the cases.
		// bit2 - Sleep mode is disabled
		// bit3 - Video mode is disabled
		// bit4 - HS clock is enabled
		// bit5 - The clock source is tx_clk
		// bit6 - DCS packet (The packet can be any one of DCS Long Write, DCS Short Write, DCS Read packet, depending on the configuration.)
		// bit7 - Write operation
		// bit8 - ECC CRC Check Disable
		// bit9 - EOT Packet Enable send
		SSD2825_CONFIGURATION_REG, 0x0342,
		SSD2825_VC_CTRL_REG, 0x00, 0x00,

		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0002, // Send DSI commands into buffer
		SSD2825_PACKET_DROP_REG, 0x29, 0x00, // MIPI_DCS_SET_DISPLAY_ON

		// set mdelay(10);

		SSD2825_PLL_CTRL_REG, 0x0001, // PLL power down
		SSD2825_VC_CTRL_REG, 0x0000, 

		// SSD2825_CONFIGURATION_REG: 0x0349 - 0000 0011 0100 1001
		// bit0 - HS mode on;
		// bit1 - Clock lane will enter LP mode, if it is not in reverse direction communication. Clock lane will follow the setting of HCLK, if it is in reverse direction communication.
		// bit2 - Sleep mode is disabled
		// bit3 - Video mode is enabled
		// bit4 - HS clock is enabled
		// bit5 - The clock source is tx_clk
		// bit6 - DCS packet (The packet can be any one of DCS Long Write, DCS Short Write, DCS Read packet, depending on the configuration.)
		// bit7 - Write operation
		// bit8 - ECC CRC Check Disable
		// bit9 - EOT Packet Enable send
		SSD2825_CONFIGURATION_REG, 0x0349,
};

static struct spi_cmd_data16 solomon_power_off_set[] = {
		// SSD2825_CONFIGURATION_REG: 0x0349 - 0000 0011 0100 1001
		// bit0 - HS mode on;
		// bit1 - Clock lane will enter LP mode, if it is not in reverse direction communication. Clock lane will follow the setting of HCLK, if it is in reverse direction communication.
		// bit2 - Sleep mode is disabled
		// bit3 - Video mode is enabled
		// bit4 - HS clock is enabled
		// bit5 - The clock source is tx_clk
		// bit6 - DCS packet (The packet can be any one of DCS Long Write, DCS Short Write, DCS Read packet, depending on the configuration.)
		// bit7 - Write operation
		// bit8 - ECC CRC Check Disable
		// bit9 - EOT Packet Enable send
		SSD2825_CONFIGURATION_REG, 0x0349,

		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0002, // Send DSI commands into buffer
		SSD2825_PACKET_DROP_REG, 0x28, 0x00, // MIPI_DCS_SET_DISPLAY_OFF
		SSD2825_PACKET_DROP_REG, 0x10, 0x00, // MIPI_DCS_ENTER_SLEEP_MODE
		
		// set mdelay(100);

		// SSD2825_CONFIGURATION_REG: 0x0300 - 0000 0011 0000 0000
		// bit0 - LP mode on;
		// bit1 - Clock lane will enter LP mode, if it is not in reverse direction communication. Clock lane will follow the setting of HCLK, if it is in reverse direction communication.
		// bit2 - Sleep mode is disabled
		// bit3 - Video mode is disabled
		// bit4 - HS clock is enabled
		// bit5 - The clock source is tx_clk
		// bit6 - Generic packet (The packet can be any one of Generic Long Write, Generic Short Write, Generic Read packet, depending on the configuration.)
		// bit7 - Write operation
		// bit8 - ECC CRC Check Disable
		// bit9 - EOT Packet Enable send
		SSD2825_CONFIGURATION_REG, 0x0300,
		SSD2825_PLL_CTRL_REG, 0x0000,
};

struct spi_cmd_data16 solomon_reg_read_set[] = {
		SSD2825_OPERATION_CTRL_REG, 0x0100, // SW reset
		// INSERT solomon_init_sequence_set[] here !!!
};

struct spi_cmd_data16 solomon_reg_read_set2[] = {
		SSD2825_SPI_READ_REG, 0x00FA, // SPI read reset
		SSD2825_READ_REG, // internal buffer for MIPI slave
};

struct spi_cmd_data16 solomon_reg_read_set3[] = {
		SSD2825_SPI_READ_REG, 0x00FA, // SPI read reset
		SSD2825_INTERRUPT_STATUS_REG, // ???
};

/* DCS packets in LP mode */
struct spi_cmd_data16 solomon_reg_read_set4[] = {
		// SSD2825_CONFIGURATION_REG: 0x034B - 0000 0011 0100 1011
		// bit0 - HS mode on;
		// bit1 - Clock lane will enter HS mode for all the cases.
		// bit2 - Sleep mode is disabled
		// bit3 - Video mode is enabled
		// bit4 - HS clock is enabled
		// bit5 - The clock source is tx_clk
		// bit6 - DCS packet (The packet can be any one of DCS Long Write, DCS Short Write, DCS Read packet, depending on the configuration.)
		// bit7 - Write operation
		// bit8 - ECC CRC Check Disable
		// bit9 - EOT Packet Enable send
		SSD2825_CONFIGURATION_REG, 0x034B,
		SSD2825_VC_CTRL_REG, 0x0000,

		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0000, // Send DSI commands into buffer
		SSD2825_PACKET_DROP_REG, 0x28, 0x00, // MIPI_DCS_SET_DISPLAY_OFF

		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0002, // Send DSI commands into buffer
		SSD2825_PACKET_DROP_REG, 0x10, 0x00, // MIPI_DCS_ENTER_SLEEP_MODE

		// set mdelay(500);

		SSD2825_LINE_CTRL_REG, 0x0001, // Automatically perform BTA after the next write packet
		SSD2825_MAX_RETURN_SIZE_REG, 0x0020, // Maximum Return Size 0x20 = 32

		// SSD2825_CONFIGURATION_REG: 0x03C2 - 0000 0011 1100 0010
		// bit0 - LP mode on;
		// bit1 - Clock lane will enter HS mode for all the cases.
		// bit2 - Sleep mode is disabled
		// bit3 - Video mode is disabled
		// bit4 - HS clock is enabled
		// bit5 - The clock source is tx_clk
		// bit6 - DCS packet (The packet can be any one of DCS Long Write, DCS Short Write, DCS Read packet, depending on the configuration.)
		// bit7 - Read operation
		// bit8 - ECC CRC Check Disable
		// bit9 - EOT Packet Enable send
		SSD2825_CONFIGURATION_REG, 0x03C2,

		/*********/
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0001, // Send DSI commands into buffer
		SSD2825_PACKET_DROP_REG, 0x0C, 0x00, // MIPI_DCS_GET_PIXEL_FORMAT
		SSD2825_SPI_READ_REG, 0x00FA, // SPI read reset
		SSD2825_RETURN_DATA_COUNT_REG, // Return Data Count – reflect the number of data bytes received from the MIPI slave read response packet.
};

/* DCS packets in HS mode */
struct spi_cmd_data16 solomon_reg_read_set5[] = {
		// SSD2825_CONFIGURATION_REG: 0x034B - 0000 0011 0100 1011
		// bit0 - HS mode on;
		// bit1 - Clock lane will enter HS mode for all the cases.
		// bit2 - Sleep mode is disabled
		// bit3 - Video mode is enabled
		// bit4 - HS clock is enabled
		// bit5 - The clock source is tx_clk
		// bit6 - DCS packet (The packet can be any one of DCS Long Write, DCS Short Write, DCS Read packet, depending on the configuration.)
		// bit7 - Write operation
		// bit8 - ECC CRC Check Disable
		// bit9 - EOT Packet Enable send
		SSD2825_CONFIGURATION_REG, 0x034B, 
		SSD2825_VC_CTRL_REG, 0x0000,

		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0000, // Send DSI commands into buffer
		SSD2825_PACKET_DROP_REG, 0x28, 0x00, // MIPI_DCS_SET_DISPLAY_OFF

		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0002, // Send DSI commands into buffer
		SSD2825_PACKET_DROP_REG, 0x10, 0x00, // MIPI_DCS_ENTER_SLEEP_MODE

		// set mdelay(500);

		SSD2825_LINE_CTRL_REG, 0x0001, // Automatically perform BTA after the next write packet
		SSD2825_MAX_RETURN_SIZE_REG, 0x0020, // Maximum Return Size 0x20 = 32

		// SSD2825_CONFIGURATION_REG: 0x03C3 - 0000 0011 1100 0011
		// bit0 - HS mode on;
		// bit1 - Clock lane will enter HS mode for all the cases.
		// bit2 - Sleep mode is disabled
		// bit3 - Video mode is disabled
		// bit4 - HS clock is enabled
		// bit5 - The clock source is tx_clk
		// bit6 - DCS packet (The packet can be any one of DCS Long Write, DCS Short Write, DCS Read packet, depending on the configuration.)
		// bit7 - Read operation
		// bit8 - ECC CRC Check Disable
		// bit9 - EOT Packet Enable send
		SSD2825_CONFIGURATION_REG, 0x03C3,

		/*********/
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0001, // Send DSI commands into buffer
		SSD2825_PACKET_DROP_REG, 0x0A, 0x00, // MIPI_DCS_GET_POWER_MODE
		SSD2825_SPI_READ_REG, 0x00FA, // SPI read reset
		SSD2825_RETURN_DATA_COUNT_REG, // Return Data Count – reflect the number of data bytes received from the MIPI slave read response packet.
};

/* Generic Packets in LP mode */
struct spi_cmd_data16 solomon_reg_read_set6[] = {
		/* Solomon_enter_sleep_sequence */
		SSD2825_CONFIGURATION_REG, 0x034B,
		SSD2825_VC_CTRL_REG, 0x0000, 

		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0000, // Send DSI commands into buffer
		SSD2825_PACKET_DROP_REG, 0x28, 0x00, // MIPI_DCS_SET_DISPLAY_OFF

		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0002, // Send DSI commands into buffer
		SSD2825_PACKET_DROP_REG, 0x10, 0x00, // MIPI_DCS_ENTER_SLEEP_MODE

		// set mdelay(500);

		SSD2825_CONFIGURATION_REG, 0x0302,

		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0002, // Send DSI commands into buffer
		SSD2825_PACKET_DROP_REG, 0xB0, 0x04, // MIPI_DCS_GET_DISPLAY_ID; 0xB0 ?

		SSD2825_LINE_CTRL_REG, 0x0001,
		SSD2825_MAX_RETURN_SIZE_REG, 0x0020,
		SSD2825_CONFIGURATION_REG, 0x0382,

		/*********/
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0001, // Send DSI commands into buffer
		SSD2825_PACKET_DROP_REG, 0xBF, 0x00, // 0xBF ?
		SSD2825_SPI_READ_REG, 0x00FA,
		SSD2825_RETURN_DATA_COUNT_REG,
};

/*Generic Packets in HS mode*/
struct spi_cmd_data16 solomon_reg_read_set7[] = {
		/*Solomon_enter_sleep_sequence*/
		SSD2825_CONFIGURATION_REG, 0x034B,
		SSD2825_VC_CTRL_REG, 0x0000,

		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0000, // Send DSI commands into buffer
		SSD2825_PACKET_DROP_REG, 0x28, 0x00, // MIPI_DCS_SET_DISPLAY_OFF

		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0002, // Send DSI commands into buffer
		SSD2825_PACKET_DROP_REG, 0x10, 0x00, // MIPI_DCS_ENTER_SLEEP_MODE

		// set mdelay(500);

		SSD2825_CONFIGURATION_REG, 0x0302,

		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0002, // Send DSI commands into buffer
		SSD2825_PACKET_DROP_REG, 0xB0, 0x04, // MIPI_DCS_GET_DISPLAY_ID; 0xB0 ?

		SSD2825_LINE_CTRL_REG, 0x0001,
		SSD2825_MAX_RETURN_SIZE_REG, 0x0020,
		SSD2825_CONFIGURATION_REG, 0x0383,

		/*********/
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0001, // Send DSI commands into buffer
		SSD2825_PACKET_DROP_REG, 0xBF, 0x00, // 0xBF ?
		SSD2825_SPI_READ_REG, 0x00FA,
		SSD2825_RETURN_DATA_COUNT_REG,
};

struct spi_cmd_data16 solomon_reg_read_set8[] = {
		SSD2825_SPI_READ_REG, 0xFA, 0x00,
		SSD2825_DEVICE_ID_REG,
};

struct spi_cmd_data16 solomon_reg_read_set9[] = {
		SSD2825_CONFIGURATION_REG, 0x0302,
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0002, // Send DSI commands into buffer
		SSD2825_PACKET_DROP_REG, 0xB0, 0x03, // MIPI_DCS_GET_COMPRESSION_MODE; 0xB0 ?
};

const char ssd2825_reg_set[] = {
	0xB0,0xB1,0xB2,0xB3,0xB4,
	0xB5,0xB6,0xDD,0xB7,0xB8,
	0xB9,0xBA,0xD5,0xBB,0xBC,
	0xBD,0xBE,0xBF,0xC0,0xC1,
	0xC2,0xC3,0xC4,0xC5,0xC6,
	0xC7,0xC8,0xC9,0xCA,0xCB,
	0xCC,0xCD,0xCE,0xCF,0xD0,
	0xD1,0xD2,0xD3,0xD4,0xD6,
	0xD7,0xD8,0xD9,0xDA,0xDB,
	0xDC,0xDE,0xDF,0xE0,0xE1,
	0xE2,0xE3,0xE4,0xE5,0xE6,
	0xE7,0xE8,0xE9,0xEA,0xEB,
	0xFF
};

/* DCS packets in HS mode */
struct spi_cmd_data16 solomon_reg_read_set4_1[] = {
		SSD2825_MAX_RETURN_SIZE_REG, 0x000A,
		SSD2825_CONFIGURATION_REG, 0x03C9,

		/*********/
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0000, // Send DSI commands into buffer
		SSD2825_PACKET_DROP_REG, // nothing?
};

#if defined(HITACHI_GAMMA_S_CURVE)
#if defined(GAMMA_3)
		/* DO *** S-curve */
		/*gamma setting A - Gamma Setting*/
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0019, // 0x-19 (19 in hex = 25)
		SSD2825_PACKET_DROP_REG,
		0xC8, 0x0B, 0x0D, 0x10, 0x14,
		0x13, 0x1D, 0x20, 0x18, 0x12,
		0x09, 0x07, 0x06, 0x0A, 0x0C,
		0x10, 0x14, 0x13, 0x1D, 0x20,
		0x18, 0x12, 0x09, 0x07, 0x06,
#elif defined(GAMMA_2)
		/* DV2 *** S-curve */
		/*gamma setting A - Gamma Setting*/
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0019,
		SSD2825_PACKET_DROP_REG,
		0xC8, 0x00, 0x05, 0x0B, 0x0F,
		0x11, 0x1D, 0x20, 0x18, 0x18,
		0x09, 0x07, 0x06, 0x00, 0x05,
		0x0B, 0x0F, 0x11, 0x1D, 0x20,
		0x18, 0x18, 0x09, 0x07, 0x06,
#else
		/* Bring Up 1st *** S-curve */
		/*gamma setting A - Gamma Setting*/
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0019,
		SSD2825_PACKET_DROP_REG,
		0xC8, 0x00, 0x06, 0x0A, 0x0F,
		0x14, 0x1F, 0x1F, 0x17, 0x12,
		0x0C, 0x09, 0x06, 0x00, 0x06,
		0x0A, 0x0F, 0x14, 0x1F, 0x1F,
		0x17, 0x12, 0x0C, 0x09, 0x06,
#endif
#endif //HITACHI_GAMMA_S_CURVE

#endif // _SPI_SOLOMON_TABLE_H_
