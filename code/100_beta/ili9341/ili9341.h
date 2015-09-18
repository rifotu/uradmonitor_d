/*
** ILI9341 Library for AVR Microcontrollers
** Version: 	0.1.0
** Date: 		July, 2015
** License:		LGPL v3
** Copyright: 	(C) 2009 - 2015 Radu Motisan, radu.motisan@gmail.com
** Description:	ILI9341 2.4" serial 240x320 SPI Color TFT LCD Module display Library
**
** www.pocketmagic.net
**
** This file is a part of "Portable Environmental Monitor" open source project presented on
** https://hackaday.io/project/4977-portable-environmental-monitor
**
** This project is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published
** by the Free Software Foundation; either version 3 of the License,
** or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program. If not, see <http://www.gnu.org/licenses/>.
*/


#pragma once

#include <avr/io.h>
#include <stdio.h>
#include "font5x8.h"
#include <string.h>
#include "../gpio/DigitalPin.h"
#include "colors.h"

// define size
#define ILI9341_HEIGHT 								240	// screen height in landscape mode
#define ILI9341_WIDTH 								320 // screen width in landscape mode

// The scrolling area must be a integral multiple of TEXT_HEIGHT
#define TEXT_HEIGHT 16 // Height of text to be printed and scrolled
#define BOT_FIXED_AREA 0 // Number of lines in bottom fixed area (lines counted from bottom of screen)
#define TOP_FIXED_AREA 16 // Number of lines in top fixed area (lines counted from top of screen)

/* Level 1 Commands (from the display Datasheet) */
#define ILI9341_CMD_NOP                             0x00
#define ILI9341_CMD_SOFTWARE_RESET                  0x01
#define ILI9341_CMD_READ_DISP_ID                    0x04
#define ILI9341_CMD_READ_DISP_STATUS                0x09
#define ILI9341_CMD_READ_DISP_MADCTRL               0x0B
#define ILI9341_CMD_READ_DISP_PIXEL_FORMAT          0x0C
#define ILI9341_CMD_READ_DISP_IMAGE_FORMAT          0x0D
#define ILI9341_CMD_READ_DISP_SIGNAL_MODE           0x0E
#define ILI9341_CMD_READ_DISP_SELF_DIAGNOSTIC       0x0F
#define ILI9341_CMD_ENTER_SLEEP_MODE                0x10
#define ILI9341_CMD_SLEEP_OUT                       0x11
#define ILI9341_CMD_PARTIAL_MODE_ON                 0x12
#define ILI9341_CMD_NORMAL_DISP_MODE_ON             0x13
#define ILI9341_CMD_DISP_INVERSION_OFF              0x20
#define ILI9341_CMD_DISP_INVERSION_ON               0x21
#define ILI9341_CMD_GAMMA_SET                       0x26
#define ILI9341_CMD_DISPLAY_OFF                     0x28
#define ILI9341_CMD_DISPLAY_ON                      0x29
#define ILI9341_CMD_COLUMN_ADDRESS_SET              0x2A
#define ILI9341_CMD_PAGE_ADDRESS_SET                0x2B
#define ILI9341_CMD_MEMORY_WRITE                    0x2C
#define ILI9341_CMD_COLOR_SET                       0x2D
#define ILI9341_CMD_MEMORY_READ                     0x2E
#define ILI9341_CMD_PARTIAL_AREA                    0x30
#define ILI9341_CMD_VERT_SCROLL_DEFINITION          0x33
#define ILI9341_CMD_TEARING_EFFECT_LINE_OFF         0x34
#define ILI9341_CMD_TEARING_EFFECT_LINE_ON          0x35
#define ILI9341_CMD_MEMORY_ACCESS_CONTROL           0x36
#define ILI9341_CMD_VERT_SCROLL_START_ADDRESS       0x37
#define ILI9341_CMD_IDLE_MODE_OFF                   0x38
#define ILI9341_CMD_IDLE_MODE_ON                    0x39
#define ILI9341_CMD_COLMOD_PIXEL_FORMAT_SET         0x3A
#define ILI9341_CMD_WRITE_MEMORY_CONTINUE           0x3C
#define ILI9341_CMD_READ_MEMORY_CONTINUE            0x3E
#define ILI9341_CMD_SET_TEAR_SCANLINE               0x44
#define ILI9341_CMD_GET_SCANLINE                    0x45
#define ILI9341_CMD_WRITE_DISPLAY_BRIGHTNESS        0x51
#define ILI9341_CMD_READ_DISPLAY_BRIGHTNESS         0x52
#define ILI9341_CMD_WRITE_CTRL_DISPLAY              0x53
#define ILI9341_CMD_READ_CTRL_DISPLAY               0x54
#define ILI9341_CMD_WRITE_CONTENT_ADAPT_BRIGHTNESS  0x55
#define ILI9341_CMD_READ_CONTENT_ADAPT_BRIGHTNESS   0x56
#define ILI9341_CMD_WRITE_MIN_CAB_LEVEL             0x5E
#define ILI9341_CMD_READ_MIN_CAB_LEVEL              0x5F
#define ILI9341_CMD_READ_ID1                        0xDA
#define ILI9341_CMD_READ_ID2                        0xDB
#define ILI9341_CMD_READ_ID3                        0xDC

/* Level 2 Commands (from the display Datasheet) */
#define ILI9341_CMD_RGB_SIGNAL_CONTROL              0xB0
#define ILI9341_CMD_FRAME_RATE_CONTROL_NORMAL       0xB1
#define ILI9341_CMD_FRAME_RATE_CONTROL_IDLE_8COLOR  0xB2
#define ILI9341_CMD_FRAME_RATE_CONTROL_PARTIAL      0xB3
#define ILI9341_CMD_DISPLAY_INVERSION_CONTROL       0xB4
#define ILI9341_CMD_BLANKING_PORCH_CONTROL          0xB5
#define ILI9341_CMD_DISPLAY_FUNCTION_CONTROL        0xB6
#define ILI9341_CMD_ENTRY_MODE_SET                  0xB7
#define ILI9341_CMD_BACKLIGHT_CONTROL_1             0xB8
#define ILI9341_CMD_BACKLIGHT_CONTROL_2             0xB9
#define ILI9341_CMD_BACKLIGHT_CONTROL_3             0xBA
#define ILI9341_CMD_BACKLIGHT_CONTROL_4             0xBB
#define ILI9341_CMD_BACKLIGHT_CONTROL_5             0xBC
#define ILI9341_CMD_BACKLIGHT_CONTROL_6             0xBD
#define ILI9341_CMD_BACKLIGHT_CONTROL_7             0xBE
#define ILI9341_CMD_BACKLIGHT_CONTROL_8             0xBF
#define ILI9341_CMD_POWER_CONTROL_1                 0xC0
#define ILI9341_CMD_POWER_CONTROL_2                 0xC1
#define ILI9341_CMD_VCOM_CONTROL_1                  0xC5
#define ILI9341_CMD_VCOM_CONTROL_2                  0xC7
#define ILI9341_CMD_POWER_ON_SEQ_CONTROL            0xCB
#define ILI9341_CMD_POWER_CONTROL_A                 0xCD
#define ILI9341_CMD_POWER_CONTROL_B                 0xCF
#define ILI9341_CMD_NVMEM_WRITE                     0xD0
#define ILI9341_CMD_NVMEM_PROTECTION_KEY            0xD1
#define ILI9341_CMD_NVMEM_STATUS_READ               0xD2
#define ILI9341_CMD_READ_ID4                        0xD3
#define ILI9341_CMD_POSITIVE_GAMMA_CORRECTION       0xE0
#define ILI9341_CMD_NEGATIVE_GAMMA_CORRECTION       0xE1
#define ILI9341_CMD_DIGITAL_GAMMA_CONTROL_1         0xE2
#define ILI9341_CMD_DIGITAL_GAMMA_CONTROL_2         0xE3
#define ILI9341_CMD_DRIVER_TIMING_CONTROL_A         0xE8
#define ILI9341_CMD_DRIVER_TIMING_CONTROL_B         0xEA
#define ILI9341_CMD_POWER_ON_SEQUENCE				0xED
#define ILI9341_CMD_ENABLE_3_GAMMA_CONTROL          0xF2
#define ILI9341_CMD_INTERFACE_CONTROL               0xF6
#define ILI9341_CMD_PUMP_RATIO_CONTROL              0xF7

// Radu Motisan: we need to scale the 8bit values to 5bit (for blue or red) / 6bit (for green)
inline uint16_t rgb24to16(uint8_t r, uint8_t g, uint8_t b) {
	return ((r>>3) << 11) | ((g>>2) << 5) | (b>>3);
}

// This routine takes a row number from 0 to 20 and returns the x coordinate on the screen (0-127)
inline uint16_t textX(uint8_t x, int size) { return x * size * FONT_WIDTH; }

// This routine takes a column number from 0 to 20 and returns the y coordinate on the screen (0-127)
inline uint16_t textY(uint8_t y, int size) { return y * size * FONT_HEIGHT; }

#define CENTERX -1
#define CENTERY -1

class ILI9341 {
	// define data pins
	DigitalPin *m_dc, *m_reset, *m_cs;
	uint16_t width, height;
	uint16_t iScrollStart;

	// The initial y coordinate of the top of the scrolling area
	uint16_t yStart = TOP_FIXED_AREA;
	// yArea must be a integral multiple of TEXT_HEIGHT
	uint16_t yArea = 320 - TOP_FIXED_AREA-BOT_FIXED_AREA;
	// The initial y coordinate of the top of the bottom text line
	uint16_t yDraw = 320 - BOT_FIXED_AREA - TEXT_HEIGHT;

public:
	enum Rotation {
		ROT0 = 0,	// portrait
		ROT90 = 1,	// landscape
		ROT180 = 2,	// flipped portrait
		ROT270 = 3	// flipped landscape
	};
	ILI9341(DigitalPin *dc, DigitalPin *reset, DigitalPin *cs) {
		m_dc = dc;
		m_reset = reset;
		m_cs = cs;
	}
	void init();

	void writeCommand(uint8_t com);
	void writeData(uint8_t data)  __attribute__((always_inline));
	void writeData16(uint16_t data) __attribute__((always_inline));
	void setAddress(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
	void reset(void);

	void drawPixel(uint16_t x, uint16_t y, uint16_t colour);
	void drawPixel(uint16_t x, uint16_t y, uint8_t size, uint16_t colour);
	void drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t colour);
	void drawRect(uint16_t x,uint16_t y,uint16_t w,uint16_t h,uint16_t colour);
	void drawRectFilled(uint16_t x,uint16_t y,uint16_t w,uint16_t h,uint16_t colour);
	void drawClear(uint16_t colour);

	void setRotation(Rotation rotation);
	void drawCircle(uint16_t x, uint16_t y, uint16_t radius, uint16_t colour);
	void drawChar(uint16_t x, uint16_t y,  char c, uint8_t size, uint16_t color, uint16_t bg );
	void drawString(uint16_t x, uint16_t y, uint8_t size, uint16_t colour, uint16_t bg, const char *string);
	void drawStringF(uint16_t x, uint16_t y, uint8_t size, uint16_t colour, uint16_t bg, char *szFormat, ...);

	void setupScrollArea(uint16_t TFA, uint16_t BFA) ;
	void scrollAddress(uint16_t VSP);
	int scrollLine();

	uint16_t getWidth() {
		return width;
	}
	uint16_t getHeight() {
		return height;
	}
};

