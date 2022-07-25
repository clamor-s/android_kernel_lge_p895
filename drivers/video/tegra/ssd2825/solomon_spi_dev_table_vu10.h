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
 * ----------------------------------------
 *	DEFINITION FOR 5 INCH DISPLAY
 * ----------------------------------------
 */
#define HITACHI_INVERSION_SELECT
//#define HITACHI_POWER_SETTING
#define COLUMN_INVERSION
#define HITACHI_DIGITAL_CONTRAST_ADJ

// VIEW5_MANUFACTURE_COMMAND_ACCESS
// enable : before specific setting
//#define VIEW5_MANUFACTURE_COMMAND_ACCESS

// VIEW5_CABC_ON
// enable : CABC ON
//#define VIEW5_CABC_ON

#include "ssd2825_bridge.h"

struct spi_cmd_data16 {
	unsigned short delay;
	unsigned short value;
};

struct spi_cmd_data16 solomon_init_sequence_set[] = {
		// Horizontal and Vertical Pulse Width
		SSD2825_RGB_INTERFACE_CTRL_REG_1, 0x0105, 0x0102,

		// HBP + width(Because of the Non-burst mode)
		// VBP + width(Because of the Non-burst mode)
		SSD2825_RGB_INTERFACE_CTRL_REG_2, 0x0156, 0x010A,	

		SSD2825_RGB_INTERFACE_CTRL_REG_3, 0x0174, 0x0118,	// HFP + VFP

		// 0xB4 : 0x0300 : 768	// 0xB5 : 0x0400 : 1024
		SSD2825_RGB_INTERFACE_CTRL_REG_4, 0x0100, 0x0103,
		SSD2825_RGB_INTERFACE_CTRL_REG_5, 0x0100, 0x0104,

#ifdef MIPI_NON_BURST_MODE
		// default Non-burst mode
		SSD2825_RGB_INTERFACE_CTRL_REG_6, 0x0107, 0x0100,
#else
		// burst mode for power consumption:0x010B,
		SSD2825_RGB_INTERFACE_CTRL_REG_6, 0x010B, 0x0100,
#endif

		SSD2825_LANE_CONFIGURATION_REG, 0x0103, 0x0100,
		SSD2825_TEST_REG, 0x0104, 0x0100,

#ifdef CURRENT_BIAS_MIPI_OUTPUT
		SSD2825_ANALOG_CTRL_REG_1, 0x011C, 0x0102,
#endif
		SSD2825_PLL_CTRL_REG, 0x0100, 0x0100,
		SSD2825_LINE_CTRL_REG, 0x0101, 0x0100,
		SSD2825_DELAY_ADJ_REG_1, 0x0103, 0x0121,

		// 430Mbps
		SSD2825_PLL_CONFIGURATION_REG, 0x01D7, 0x018C,

		//change for Hitach ESD
		SSD2825_CLOCK_CTRL_REG, 0x0109, 0x0100,

		SSD2825_PLL_CTRL_REG, 0x0101, 0x0100,
		SSD2825_VC_CTRL_REG, 0x0100, 0x0100,

		// set mdelay(10);

		SSD2825_CONFIGURATION_REG, 0x0142, 0x0103,
		SSD2825_VC_CTRL_REG, 0x0100, 0x0100,

		/* exit_sleep_mode */
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0102, 0x0100,
		SSD2825_PACKET_DROP_REG, 0x0111, 0x0100,

		// set mdelay(80);

		/* set_address_mode */
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0102, 0x0100,
		SSD2825_PACKET_DROP_REG, 0x0136, 0x0100,

		// set mdelay(20);

		/* set_pixel_format */
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0102, 0x0100,
		SSD2825_PACKET_DROP_REG, 0x013A, 0x0170,

		SSD2825_CONFIGURATION_REG, 0x0102, 0x0103,
		SSD2825_VC_CTRL_REG, 0x0100, 0x0100,

		/* Manufacturer Command Access Protect Off */
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0102, 0x0100,
		SSD2825_PACKET_DROP_REG, 0x01B0, 0x0104,

#if defined(HITACHI_DIGITAL_CONTRAST_ADJ)
		//Digital Contrast Adjustment
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0104, 0x0100,
		SSD2825_PACKET_DROP_REG, 0x01CC, 0x01DC, 0x01B4, 0x01FF,
#endif

#ifdef HITACHI_GAMMA_S_CURVE
		// gamma curves are here check end
#endif

#if defined(HITACHI_INVERSION_SELECT)
		//Panel Driving Setting
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0109, 0x0100,
		SSD2825_PACKET_DROP_REG,
#if defined(COLUMN_INVERSION)
		0x01C1, 0x0100, 0x0150, //Column
		0x0103, 0x0122, 0x0116,
		0x0106, 0x0160, 0x0111,
#else
		0x01C1, 0x0100, 0x0110,//2Lines
		0x0103, 0x0122, 0x0116,
		0x0106, 0x0160, 0x0101,
#endif
#endif

#if defined(HITACHI_POWER_SETTING)
		//Panel Driving Setting
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0109, 0x0100,
		SSD2825_PACKET_DROP_REG,
#if defined(COLUMN_INVERSION)
		0x01D1, 0x018E, 0x0127, //Column
		0x0144, 0x0163, 0x0197,
		0x0163, 0x01C9, 0x0106,
#else
		0x01D1, 0x016E, 0x0129, //2DOT
		0x0144, 0x0142, 0x0175,
		0x0173, 0x01EB, 0x0106,
#endif
#endif
		/* Manufacturer Command Access Protect On */
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0102, 0x0100,
		SSD2825_PACKET_DROP_REG, 0x01B0, 0x0103,

		SSD2825_CONFIGURATION_REG, 0x0142, 0x0103,
		SSD2825_VC_CTRL_REG, 0x0100, 0x0100,

		/* Display on */
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0102, 0x0100,
		SSD2825_PACKET_DROP_REG, 0x0129, 0x0100,

		// set mdelay(10);

		SSD2825_PLL_CTRL_REG, 0x0101, 0x0100,
		SSD2825_VC_CTRL_REG, 0x0100, 0x0100,
		SSD2825_CONFIGURATION_REG, 0x0149, 0x0103,
};

static struct spi_cmd_data16 solomon_power_off_set[] = {
		SSD2825_CONFIGURATION_REG, 0x0149, 0x0103,
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0102, 0x0100,
		SSD2825_PACKET_DROP_REG, 0x0128, 0x0100, //set_display_off
		SSD2825_PACKET_DROP_REG, 0x0110, 0x0100, //enter_sleep_mode
		
		// set mdelay(100); 
		
		SSD2825_CONFIGURATION_REG, 0x0100, 0x0103,
		SSD2825_PLL_CTRL_REG, 0x0100, 0x0100,
};

struct spi_cmd_data16 solomon_reg_read_set[] = {
		// SW reset
		SSD2825_OPERATION_CTRL_REG, 0x0100, 0x0101,

		// Horizontal and Vertical Pulse Width
		SSD2825_RGB_INTERFACE_CTRL_REG_1, 0x0105, 0x0102,

		// HBP + width(Because of the Non-burst mode)
		// VBP + width(Because of the Non-burst mode)
		SSD2825_RGB_INTERFACE_CTRL_REG_2, 0x0156, 0x010A,	

		SSD2825_RGB_INTERFACE_CTRL_REG_3, 0x0174, 0x0118,	// HFP + VFP

		// 0xB4 : 0x0300 : 768	// 0xB5 : 0x0400 : 1024
		SSD2825_RGB_INTERFACE_CTRL_REG_4, 0x0100, 0x0103,
		SSD2825_RGB_INTERFACE_CTRL_REG_5, 0x0100, 0x0104,

#ifdef MIPI_NON_BURST_MODE
		// default Non-burst mode
		SSD2825_RGB_INTERFACE_CTRL_REG_6, 0x0107, 0x0100,
#else
		// burst mode for power consumption:0x010B,
		SSD2825_RGB_INTERFACE_CTRL_REG_6, 0x010B, 0x0100,
#endif

		SSD2825_LANE_CONFIGURATION_REG, 0x0103, 0x0100,
		SSD2825_TEST_REG, 0x0104, 0x0100,

#ifdef CURRENT_BIAS_MIPI_OUTPUT
		SSD2825_ANALOG_CTRL_REG_1, 0x011C, 0x0102,
#endif

		SSD2825_PLL_CTRL_REG, 0x0100, 0x0100,
		SSD2825_LINE_CTRL_REG, 0x0101, 0x0100,
		SSD2825_DELAY_ADJ_REG_1, 0x0103, 0x0121,

		// 430Mbps
		SSD2825_PLL_CONFIGURATION_REG, 0x01D7, 0x018C,

		SSD2825_CLOCK_CTRL_REG, 0x0109, 0x0100,

		SSD2825_PLL_CTRL_REG, 0x0101, 0x0100,
		SSD2825_VC_CTRL_REG, 0x0100, 0x0100,
	
		// set mdelay(10);

		SSD2825_CONFIGURATION_REG, 0x0142, 0x0103,
		SSD2825_VC_CTRL_REG, 0x0100, 0x0100,

		/*exit_sleep_mode*/
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0102, 0x0100,
		SSD2825_PACKET_DROP_REG, 0x0111, 0x0100,

		// set mdelay(80);

		/*set_address_mode*/
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0102, 0x0100,
		SSD2825_PACKET_DROP_REG, 0x0136, 0x0100,

		// set mdelay(20);

		/*set_pixel_format*/
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0102, 0x0100,
		SSD2825_PACKET_DROP_REG, 0x013A, 0x0170,

		/*Solomon_leave_sleep_sequence*/
		SSD2825_CONFIGURATION_REG, 0x0102, 0x0103,			//CHECK !!!!
		SSD2825_VC_CTRL_REG, 0x0100, 0x0100,

		/*Manufacturer Command Access Protect Off*/
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0102, 0x0100,
		SSD2825_PACKET_DROP_REG, 0x01B0, 0x0104,

#if defined(HITACHI_DIGITAL_CONTRAST_ADJ)
		//Digital Contrast Adjustment
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0104, 0x0100,
		SSD2825_PACKET_DROP_REG, 0x01CC, 0x01DC, 0x01B4, 0x01FF,

#endif

#ifdef HITACHI_GAMMA_S_CURVE
		// gamma curves are here check end
#endif

#if defined(HITACHI_INVERSION_SELECT)
		//Panel Driving Setting
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0109, 0x0100,
		SSD2825_PACKET_DROP_REG,
#if defined(COLUMN_INVERSION)
		0x01C1, 0x0100, 0x0150, //Column
		0x0103, 0x0122, 0x0116,
		0x0106, 0x0160, 0x0111,
#else
		0x01C1, 0x0100, 0x0110, //2Lines
		0x0103, 0x0122, 0x0116,
		0x0106, 0x0160, 0x0101,
#endif
#endif

#if defined(HITACHI_POWER_SETTING)
		//Panel Driving Setting
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0109, 0x0100, //0x01-09 9 data blocks 
		SSD2825_PACKET_DROP_REG,
#if defined(COLUMN_INVERSION)
		0x01D1, 0x018E, 0x0127, //Column
		0x0144, 0x0163, 0x0197,
		0x0163, 0x01C9, 0x0106,
#else
		0x01D1, 0x016E, 0x0129, //2DOT
		0x0144, 0x0142, 0x0175,
		0x0173, 0x01EB, 0x0106,
#endif
#endif
		/*Manufacturer Command Access Protect On*/
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0102, 0x0100,
		SSD2825_PACKET_DROP_REG, 0x01B0, 0x0103,

		//change by DCS, display on
		SSD2825_CONFIGURATION_REG, 0x0142, 0x0103, // change by generic
		SSD2825_VC_CTRL_REG, 0x0100, 0x0100,

		/*Display on*/
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0102, 0x0100,
		SSD2825_PACKET_DROP_REG, 0x0129, 0x0100,

		// set mdelay(10);

		SSD2825_PLL_CTRL_REG, 0x0101, 0x0100,
		SSD2825_VC_CTRL_REG, 0x0100, 0x0100,
		SSD2825_CONFIGURATION_REG, 0x0149, 0x0103,

};

struct spi_cmd_data16 solomon_reg_read_set2[] = {
		SSD2825_SPI_READ_REG, 0x01FA, 0x0100,
		SSD2825_READ_REG,
};

struct spi_cmd_data16 solomon_reg_read_set3[] = {
		SSD2825_SPI_READ_REG, 0x01FA, 0x0100,
		SSD2825_INTERRUPT_STATUS_REG,
};

/* DCS packets in LP mode */
struct spi_cmd_data16 solomon_reg_read_set4[] = {
		/* Solomon_enter_sleep_sequence */
		SSD2825_CONFIGURATION_REG, 0x014B, 0x0103,
		SSD2825_VC_CTRL_REG, 0x0100, 0x0100,
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0100, 0x0100,

		/* Display off */
		SSD2825_PACKET_DROP_REG, 0x0128, 0x0100,

		/* enter_sleep_mode */
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0102, 0x0100,
		SSD2825_PACKET_DROP_REG, 0x0110, 0x0100,

		// set mdelay(500);

		SSD2825_LINE_CTRL_REG, 0x0101, 0x0100,
		SSD2825_MAX_RETURN_SIZE_REG, 0x0120, 0x0100,

		//0x01c2:DCS in LP, 0x0182: Generic in LP, 0x0183: Generic in HS ,0x01c3:DCS in HS
		SSD2825_CONFIGURATION_REG, 0x01C2, 0x0103,

		/*********/
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0101, 0x0100,
		SSD2825_PACKET_DROP_REG, 0x010C, 0x0100,

		SSD2825_SPI_READ_REG, 0x01FA, 0x0100,
		SSD2825_RETURN_DATA_COUNT_REG,
};

/* DCS packets in HS mode */
struct spi_cmd_data16 solomon_reg_read_set5[] = {
		/* Solomon_enter_sleep_sequence */
		SSD2825_CONFIGURATION_REG, 0x014B, 0x0103,
		SSD2825_VC_CTRL_REG, 0x0100, 0x0100,
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0100, 0x0100,

		/* Display off */
		SSD2825_PACKET_DROP_REG, 0x0128, 0x0100,

		/* enter_sleep_mode */
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0102, 0x0100,
		SSD2825_PACKET_DROP_REG, 0x0110, 0x0100,

		// set mdelay(500);

		SSD2825_LINE_CTRL_REG, 0x0101, 0x0100,
		SSD2825_MAX_RETURN_SIZE_REG, 0x0120, 0x0100,
		SSD2825_CONFIGURATION_REG, 0x01C3, 0x0103,

		/*********/
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0101, 0x0100,
		SSD2825_PACKET_DROP_REG, 0x010A, 0x0100,

		SSD2825_SPI_READ_REG, 0x01FA, 0x0100,
		SSD2825_RETURN_DATA_COUNT_REG,
};

/* Generic Packets in LP mode */
struct spi_cmd_data16 solomon_reg_read_set6[] = {
		/* Solomon_enter_sleep_sequence */
		SSD2825_CONFIGURATION_REG, 0x014B, 0x0103,
		SSD2825_VC_CTRL_REG, 0x0100, 0x0100,
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0100, 0x0100,

		/*Display off*/
		SSD2825_PACKET_DROP_REG, 0x0128, 0x0100,

		/*enter_sleep_mode*/
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0102, 0x0100,
		SSD2825_PACKET_DROP_REG, 0x0110, 0x0100,

		// set mdelay(500);

		SSD2825_CONFIGURATION_REG, 0x0102, 0x0103,
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0102, 0x0100,
		SSD2825_PACKET_DROP_REG, 0x01B0, 0x0104,

		SSD2825_LINE_CTRL_REG, 0x0101, 0x0100,
		SSD2825_MAX_RETURN_SIZE_REG, 0x0120, 0x0100,
		SSD2825_CONFIGURATION_REG, 0x0182, 0x0103,
		/*********/
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0101, 0x0100,
		SSD2825_PACKET_DROP_REG, 0x01BF, 0x0100,

		SSD2825_SPI_READ_REG, 0x01FA, 0x0100,
		SSD2825_RETURN_DATA_COUNT_REG,
};

/*Generic Packets in HS mode*/
struct spi_cmd_data16 solomon_reg_read_set7[] = {
		/*Solomon_enter_sleep_sequence*/
		SSD2825_CONFIGURATION_REG, 0x014B, 0x0103,
		SSD2825_VC_CTRL_REG, 0x0100, 0x0100,
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0100, 0x0100,

		/*Display off*/
		SSD2825_PACKET_DROP_REG, 0x0128, 0x0100,

		/*enter_sleep_mode*/
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0102, 0x0100,
		SSD2825_PACKET_DROP_REG, 0x0110, 0x0100,

		// set mdelay(500);

		SSD2825_CONFIGURATION_REG, 0x0102, 0x0103,
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0102, 0x0100,
		SSD2825_PACKET_DROP_REG, 0x01B0, 0x0104,

		SSD2825_LINE_CTRL_REG, 0x0101, 0x0100,
		SSD2825_MAX_RETURN_SIZE_REG, 0x0120, 0x0100,
		SSD2825_CONFIGURATION_REG, 0x0183, 0x0103,
		/*********/
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0101, 0x0100,
		SSD2825_PACKET_DROP_REG, 0x01BF, 0x0100,

		SSD2825_SPI_READ_REG, 0x01FA, 0x0100,
		SSD2825_RETURN_DATA_COUNT_REG,
};

struct spi_cmd_data16 solomon_reg_read_set8[] = {
		SSD2825_SPI_READ_REG, 0x01FA, 0x0100,
		SSD2825_DEVICE_ID_REG,
};

struct spi_cmd_data16 solomon_reg_read_set9[] = {
		SSD2825_CONFIGURATION_REG, 0x0102, 0x0103,
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0102, 0x0100,
		SSD2825_PACKET_DROP_REG, 0x01B0, 0x0103,
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
		SSD2825_MAX_RETURN_SIZE_REG, 0x010A, 0x0100,
		SSD2825_CONFIGURATION_REG, 0x01C9, 0x0103,
		/*********/
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0101, 0x0100,
		SSD2825_PACKET_DROP_REG,
};

#if defined(HITACHI_GAMMA_S_CURVE)
#if defined(GAMMA_3)
		/* DO *** S-curve */
		/*gamma setting A - Gamma Setting*/
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0119, 0x0100, // 0x01-19 (19 in hex = 25)
		SSD2825_PACKET_DROP_REG,
		0x01C8, 0x010B, 0x010D, 0x0110, 0x0114,
		0x0113, 0x011D, 0x0120, 0x0118, 0x0112,
		0x0109, 0x0107, 0x0106, 0x010A, 0x010C,
		0x0110, 0x0114, 0x0113, 0x011D, 0x0120,
		0x0118, 0x0112, 0x0109, 0x0107, 0x0106,
#elif defined(GAMMA_2)
		/* DV2 *** S-curve */
		/*gamma setting A - Gamma Setting*/
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0119, 0x0100,
		SSD2825_PACKET_DROP_REG,
		0x01C8, 0x0100, 0x0105, 0x010B, 0x010F,
		0x0111, 0x011D, 0x0120, 0x0118, 0x0118,
		0x0109, 0x0107, 0x0106, 0x0100, 0x0105,
		0x010B, 0x010F, 0x0111, 0x011D, 0x0120,
		0x0118, 0x0118, 0x0109, 0x0107, 0x0106,
#else
		/* Bring Up 1st *** S-curve */
		/*gamma setting A - Gamma Setting*/
		SSD2825_PACKET_SIZE_CTRL_REG_1, 0x0119, 0x0100,
		SSD2825_PACKET_DROP_REG,
		0x01C8, 0x0100, 0x0106, 0x010A, 0x010F,
		0x0114, 0x011F, 0x011F, 0x0117, 0x0112,
		0x010C, 0x0109, 0x0106, 0x0100, 0x0106,
		0x010A, 0x010F, 0x0114, 0x011F, 0x011F,
		0x0117, 0x0112, 0x010C, 0x0109, 0x0106,
#endif
#endif //HITACHI_GAMMA_S_CURVE

#endif // _SPI_SOLOMON_TABLE_H_
