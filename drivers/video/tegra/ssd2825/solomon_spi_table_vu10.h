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

#define MIPI_NON_BURST_MODE  //default(2/27)
//#define HITACHI_CABC_ENABLE
//#define CURRENT_BIAS_MIPI_OUTPUT  //default : no use(2/27)
#define HITACHI_GAMMA_S_CURVE
#define GAMMA_3

/*----------------------------------------
*	DEFINITION FOR 5 INCH DISPLAY
-----------------------------------------*/
#define HITACHI_INVERSION_SELECT
//#define HITACHI_POWER_SETTING
#define COLUMN_INVERSION
#define HITACHI_DIGITAL_CONTRAST_ADJ

#include "ssd2825_bridge.h"

struct spi_cmd_data16 {
	unsigned short delay;
	unsigned short value;
};

struct spi_cmd_data16 solomon_bridge_init_sequence[] = {
		{0, SSD2825_RGB_INTERFACE_CTRL_REG_1}, {0, 0x0105}, {0, 0x0102},
		{0, SSD2825_RGB_INTERFACE_CTRL_REG_2}, {0, 0x0156}, {0, 0x010A},
		{0, SSD2825_RGB_INTERFACE_CTRL_REG_3}, {0, 0x0174}, {0, 0x0118},
		{0, SSD2825_RGB_INTERFACE_CTRL_REG_4}, {0, 0x0100}, {0, 0x0103},
		{0, SSD2825_RGB_INTERFACE_CTRL_REG_5}, {0, 0x0100}, {0, 0x0104},
#ifdef MIPI_NON_BURST_MODE
		{0, SSD2825_RGB_INTERFACE_CTRL_REG_6}, {0, 0x0107}, {0, 0x0100},
#else
		{0, SSD2825_RGB_INTERFACE_CTRL_REG_6}, {0, 0x010B}, {0, 0x0100},
#endif
		{0, SSD2825_LANE_CONFIGURATION_REG}, {0, 0x0103}, {0, 0x0100},
		{0, SSD2825_TEST_REG}, {0, 0x0104}, {0, 0x0100},

		{0, SSD2825_PLL_CTRL_REG}, {0, 0x0100}, {0, 0x0100},
		{0, SSD2825_LINE_CTRL_REG}, {0, 0x0101}, {0, 0x0100},
		{0, SSD2825_DELAY_ADJ_REG_1}, {0, 0x0103}, {0, 0x0121},
		{0, SSD2825_PLL_CONFIGURATION_REG}, {0, 0x01D7}, {0, 0x018C},
		{0, SSD2825_CLOCK_CTRL_REG}, {0, 0x0109}, {0, 0x0100},
		{0, SSD2825_PLL_CTRL_REG}, {0, 0x0101}, {0, 0x0100},
		{0, SSD2825_VC_CTRL_REG}, {0, 0x0100}, {10, 0x0100},

		{0, SSD2825_CONFIGURATION_REG}, {0, 0x0142}, {0, 0x0103},
		{0, SSD2825_VC_CTRL_REG}, {0, 0x0100}, {0, 0x0100},
};

struct spi_cmd_data16 solomon_dsi_panel_init_sequence[] = {
		{0, SSD2825_CONFIGURATION_REG}, {0, 0x0142}, {0, 0x0103},
		{0, SSD2825_VC_CTRL_REG}, {0, 0x0100}, {0, 0x0100},

		{0, SSD2825_PACKET_SIZE_CTRL_REG_1}, {0, 0x0102}, {0, 0x0100},
		{0, SSD2825_PACKET_DROP_REG}, {0, 0x0111}, {80, 0x0100},

		{0, SSD2825_PACKET_SIZE_CTRL_REG_1}, {0, 0x0102}, {0, 0x0100},
		{0, SSD2825_PACKET_DROP_REG}, {0, 0x0136}, {20, 0x0100},

		{0, SSD2825_PACKET_SIZE_CTRL_REG_1}, {0, 0x0102}, {0, 0x0100},
		{0, SSD2825_PACKET_DROP_REG}, {0, 0x013A}, {0, 0x0170},

		{0, SSD2825_CONFIGURATION_REG}, {0, 0x0102}, {0, 0x0103},
		{0, SSD2825_VC_CTRL_REG}, {0, 0x0100}, {0, 0x0100},

		{0, SSD2825_PACKET_SIZE_CTRL_REG_1}, {0, 0x0102}, {0, 0x0100},
		{0, SSD2825_PACKET_DROP_REG}, {0, 0x01B0}, {0, 0x0104},

#ifdef HITACHI_DIGITAL_CONTRAST_ADJ
		{0, SSD2825_PACKET_SIZE_CTRL_REG_1}, {0, 0x0104}, {0, 0x0100},
		{0, SSD2825_PACKET_DROP_REG}, {0, 0x01CC}, {0, 0x01DC}, {0, 0x01B4}, {0, 0x01FF},
#endif

#ifdef HITACHI_GAMMA_S_CURVE
#ifdef GAMMA_3
		/* DO *** S-curve */
		/* gamma setting A - Gamma Setting */
		{0, SSD2825_PACKET_SIZE_CTRL_REG_1}, {0, 0x0119}, {0, 0x0100},
		{0, SSD2825_PACKET_DROP_REG},
		{0, 0x01C8}, {0, 0x010B}, {0, 0x010D},
		{0, 0x0110}, {0, 0x0114}, {0, 0x0113},
		{0, 0x011D}, {0, 0x0120}, {0, 0x0118},
		{0, 0x0112}, {0, 0x0109}, {0, 0x0107},
		{0, 0x0106}, {0, 0x010A}, {0, 0x010C},
		{0, 0x0110}, {0, 0x0114}, {0, 0x0113},
		{0, 0x011D}, {0, 0x0120}, {0, 0x0118},
		{0, 0x0112}, {0, 0x0109}, {0, 0x0107},
		{0, 0x0106},
#elif defined(GAMMA_2)
		/* DV2 *** S-curve */
		/* gamma setting A - Gamma Setting */
		{0, SSD2825_PACKET_SIZE_CTRL_REG_1}, {0, 0x0119}, {0, 0x0100},
		{0, SSD2825_PACKET_DROP_REG},
		{0, 0x01C8}, {0, 0x0100}, {0, 0x0105},
		{0, 0x010B}, {0, 0x010F}, {0, 0x0111},
		{0, 0x011D}, {0, 0x0120}, {0, 0x0118},
		{0, 0x0118}, {0, 0x0109}, {0, 0x0107},
		{0, 0x0106}, {0, 0x0100}, {0, 0x0105},
		{0, 0x010B}, {0, 0x010F}, {0, 0x0111},
		{0, 0x011D}, {0, 0x0120}, {0, 0x0118},
		{0, 0x0118}, {0, 0x0109}, {0, 0x0107},
		{0, 0x0106},
#else
		/* Bring Up 1st *** S-curve */
		/*gamma setting A - Gamma Setting*/
		{0, SSD2825_PACKET_SIZE_CTRL_REG_1}, {0, 0x0119}, {0, 0x0100},
		{0, SSD2825_PACKET_DROP_REG},
		{0, 0x01C8}, {0, 0x0100}, {0, 0x0106},
		{0, 0x010A}, {0, 0x010F}, {0, 0x0114},
		{0, 0x011F}, {0, 0x011F}, {0, 0x0117},
		{0, 0x0112}, {0, 0x010C}, {0, 0x0109},
		{0, 0x0106}, {0, 0x0100}, {0, 0x0106},
		{0, 0x010A}, {0, 0x010F}, {0, 0x0114},
		{0, 0x011F}, {0, 0x011F}, {0, 0x0117},
		{0, 0x0112}, {0, 0x010C}, {0, 0x0109},
		{0, 0x0106},
#endif
#endif //HITACHI_GAMMA_S_CURVE

#ifdef HITACHI_INVERSION_SELECT
		//Panel Driving Setting
		{0, SSD2825_PACKET_SIZE_CTRL_REG_1}, {0, 0x0109}, {0, 0x0100},
		{0, SSD2825_PACKET_DROP_REG},
#ifdef COLUMN_INVERSION
		{0, 0x01C1}, {0, 0x0100}, {0, 0x0150}, //Column
		{0, 0x0103}, {0, 0x0122}, {0, 0x0116},
		{0, 0x0106}, {0, 0x0160}, {0, 0x0111},
#else
		{0, 0x01C1}, {0, 0x0100}, {0, 0x0110}, //2Lines
		{0, 0x0103}, {0, 0x0122}, {0, 0x0116},
		{0, 0x0106}, {0, 0x0160}, {0, 0x0101},
#endif
#endif

#ifdef HITACHI_POWER_SETTING
		//Panel Driving Setting
		{0, SSD2825_PACKET_SIZE_CTRL_REG_1}, {0, 0x0109}, {0, 0x0100},
		{0, SSD2825_PACKET_DROP_REG},
#ifdef COLUMN_INVERSION
		{0, 0x01D1}, {0, 0x018E}, {0, 0x0127}, //Column
		{0, 0x0144}, {0, 0x0163}, {0, 0x0197},
		{0, 0x0163}, {0, 0x01C9}, {0, 0x0106},
#else
		{0, 0x01D1}, {0, 0x016E}, {0, 0x0129}, //2DOT
		{0, 0x0144}, {0, 0x0142}, {0, 0x0175},
		{0, 0x0173}, {0, 0x01EB}, {0, 0x0106},
#endif
#endif
		{0, SSD2825_PACKET_SIZE_CTRL_REG_1}, {0, 0x0102}, {0, 0x0100},
		{0, SSD2825_PACKET_DROP_REG}, {0, 0x01B0}, {0, 0x0103},

		{0, SSD2825_CONFIGURATION_REG}, {0, 0x0142}, {0, 0x0103},
		{0, SSD2825_VC_CTRL_REG}, {0, 0x0100}, {0, 0x0100},

		{0, SSD2825_PACKET_SIZE_CTRL_REG_1}, {0, 0x0102}, {0, 0x0100},
		{0, SSD2825_PACKET_DROP_REG}, {0, 0x0129}, {10, 0x0100},

		{0, SSD2825_CONFIGURATION_REG}, {0, 0x0149}, {0, 0x0103},
		{0, SSD2825_PLL_CTRL_REG}, {0, 0x0101}, {0, 0x0100},
		{0, SSD2825_VC_CTRL_REG}, {0, 0x0100}, {0, 0x0100},
};

static struct spi_cmd_data16 solomon_dsi_panel_power_off_sequence[] = {
		{0, SSD2825_CONFIGURATION_REG}, {0, 0x0149}, {0, 0x0103},
		{0, SSD2825_VC_CTRL_REG}, {0, 0x0100}, {0, 0x0100},

		{0, SSD2825_PACKET_SIZE_CTRL_REG_1}, {0, 0x0102}, {0, 0x0100},
		{0, SSD2825_PACKET_DROP_REG}, {0, 0x0128}, {0, 0x0100},

		{0, SSD2825_PACKET_SIZE_CTRL_REG_1}, {0, 0x0102}, {0, 0x0100},
		{0, SSD2825_PACKET_DROP_REG}, {0, 0x0110}, {100, 0x0100},
};

static struct spi_cmd_data16 solomon_bridge_power_off_sequence[] = {
		{0, SSD2825_CONFIGURATION_REG}, {0, 0x0100}, {0, 0x0103},
		{0, SSD2825_PLL_CTRL_REG}, {0, 0x0100}, {0, 0x0100},
		{0, SSD2825_VC_CTRL_REG}, {0, 0x0100}, {0, 0x0100},
};

/* DCS packets in HS mode */
struct spi_cmd_data16 solomon_reg_read_set4_1[] = {
		{0, SSD2825_MAX_RETURN_SIZE_REG}, {0, 0x010A}, {0, 0x0100},
		{0, SSD2825_CONFIGURATION_REG}, {0, 0x01C9}, {0, 0x0103},
		/*********/
		{0, SSD2825_PACKET_SIZE_CTRL_REG_1}, {0, 0x0101}, {0, 0x0100},
		{0, SSD2825_PACKET_DROP_REG},
};
#endif // _SPI_SOLOMON_TABLE_H_
