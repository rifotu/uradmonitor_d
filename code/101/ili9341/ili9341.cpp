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

#include "ili9341.h"

#include "../config.h"
#include "../spi/spi.h"





//set up display using predefined command sequence
void ILI9341::init() {

	// set reset high
	*m_reset = 1;


	width = ILI9341_WIDTH;
	height = ILI9341_HEIGHT;

	reset();
	writeCommand(ILI9341_CMD_SOFTWARE_RESET);//soft reset
	_delay_ms(10);
	//power control A
	writeCommand(ILI9341_CMD_POWER_ON_SEQ_CONTROL);
	writeData(0x39);
	writeData(0x2C);
	writeData(0x00);
	writeData(0x34);
	writeData(0x02);

	//power control B
	writeCommand(ILI9341_CMD_POWER_CONTROL_B);
	writeData(0x00);
	writeData(0xC1);
	writeData(0x30);

	//driver timing control A
	writeCommand(ILI9341_CMD_DRIVER_TIMING_CONTROL_A);
	writeData(0x85);
	writeData(0x00);
	writeData(0x78);

	//driver timing control B
	writeCommand(ILI9341_CMD_DRIVER_TIMING_CONTROL_B);
	writeData(0x00);
	writeData(0x00);

	//power on sequence control
	writeCommand(ILI9341_CMD_POWER_ON_SEQUENCE);
	writeData(0x64);
	writeData(0x03);
	writeData(0x12);
	writeData(0x81);

	//pump ratio control
	writeCommand(ILI9341_CMD_PUMP_RATIO_CONTROL);
	writeData(0x20);

	//power control,VRH[5:0]
	writeCommand(ILI9341_CMD_POWER_CONTROL_1);
	writeData(0x23);

	//Power control,SAP[2:0];BT[3:0]
	writeCommand(ILI9341_CMD_POWER_CONTROL_2);
	writeData(0x10);

	//vcm control
	writeCommand(ILI9341_CMD_VCOM_CONTROL_1);
	writeData(0x3E);
	writeData(0x28);

	//vcm control 2
	writeCommand(ILI9341_CMD_VCOM_CONTROL_2);
	writeData(0x86);

	//memory access control
	writeCommand(ILI9341_CMD_MEMORY_ACCESS_CONTROL);
	writeData(0x48);

	//pixel format
	writeCommand(ILI9341_CMD_COLMOD_PIXEL_FORMAT_SET);
	writeData(0x55);

	//framerate control,normal mode full colors
	writeCommand(ILI9341_CMD_FRAME_RATE_CONTROL_NORMAL);
	writeData(0x00);
	writeData(0x18);

	//display function control
	writeCommand(ILI9341_CMD_DISPLAY_FUNCTION_CONTROL);
	writeData(0x08);
	writeData(0x82);
	writeData(0x27);

	//3gamma function disable
	writeCommand(ILI9341_CMD_ENABLE_3_GAMMA_CONTROL);
	writeData(0x00);

	//gamma curve selected
	writeCommand(ILI9341_CMD_GAMMA_SET);
	writeData(0x01);

	//set positive gamma correction
	writeCommand(ILI9341_CMD_POSITIVE_GAMMA_CORRECTION);
	writeData(0x0F);
	writeData(0x31);
	writeData(0x2B);
	writeData(0x0C);
	writeData(0x0E);
	writeData(0x08);
	writeData(0x4E);
	writeData(0xF1);
	writeData(0x37);
	writeData(0x07);
	writeData(0x10);
	writeData(0x03);
	writeData(0x0E);
	writeData(0x09);
	writeData(0x00);

	//set negative gamma correction
	writeCommand(ILI9341_CMD_NEGATIVE_GAMMA_CORRECTION);
	writeData(0x00);
	writeData(0x0E);
	writeData(0x14);
	writeData(0x03);
	writeData(0x11);
	writeData(0x07);
	writeData(0x31);
	writeData(0xC1);
	writeData(0x48);
	writeData(0x08);
	writeData(0x0F);
	writeData(0x0C);
	writeData(0x31);
	writeData(0x36);
	writeData(0x0F);

	//exit sleep
	writeCommand(ILI9341_CMD_SLEEP_OUT);
	_delay_ms(120);
	//display on
	writeCommand(ILI9341_CMD_DISPLAY_ON);

}

// write command byte
void ILI9341::writeCommand(uint8_t cmd) {
	//dc and cs both low to send command
	*m_cs = 0;
	*m_dc = 0;
	//_delay_us(5);
	spi_send(cmd); // send data by SPI
	//pull high cs
	*m_dc = 1;
	*m_cs = 1;
}

// write data byte
void ILI9341::writeData(uint8_t data) {
	//set cs low for operation
	*m_cs = 0;
	spi_send(data);
	//pull high cs
	*m_cs = 1;
}

// write data byte
void ILI9341::writeData16(uint16_t word) {
	//set cs low for operation
	*m_cs = 0;
	spi_send((word >> 8) & 0x00FF);
	spi_send(word & 0x00FF);
	//pull high cs
	*m_cs = 1;
}

//set coordinate for print or other function
void ILI9341::setAddress(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2) {
	writeCommand(ILI9341_CMD_COLUMN_ADDRESS_SET);
	writeData16(x1);
	writeData16(x2);

	writeCommand(ILI9341_CMD_PAGE_ADDRESS_SET);
	writeData16(y1);
	writeData16(y2);
	// memory write
	writeCommand(ILI9341_CMD_MEMORY_WRITE);
}

// hard reset display
void ILI9341::reset(void) {
	// pull reset high if low previously
	*m_reset = 1;
	_delay_ms(20);
	*m_reset = 0;
	_delay_ms(20);
	//again pull high for normal operation
	*m_reset = 1;
	_delay_ms(20);
}

// draw pixel
// pixels will always be counted from right side.x is representing LCD width which will always be less than 240.Y is representing LCD height which will always be less than 320
void ILI9341::drawPixel(uint16_t x, uint16_t y, uint16_t colour) {
	if ((x < 0) || (x >= width) || (y < 0) || (y >= height)) return;
	setAddress(x, y, x + 1, y + 1);
	writeData16(colour);
}

void ILI9341::drawPixel(uint16_t x, uint16_t y, uint8_t size, uint16_t colour) {
	if (size == 1) // default size or big size
		drawPixel(x, y, colour);
	else
		drawRectFilled(x, y, size, size, colour);
}

// Draw a line from x0, y0 to x1, y1
// Note:    This is a version of Bresenham's line drawing algorithm
//          It only draws lines from left to right!
void ILI9341::drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t colour) {
    int16_t dy = y1 - y0;
    int16_t dx = x1 - x0;
    int16_t stepx, stepy;

    if (dy < 0) {
        dy = -dy; stepy = -1;
    }
    else stepy = 1;

    if (dx < 0) {
        dx = -dx; stepx = -1;
    }
    else stepx = 1;

    dy <<= 1;                           // dy is now 2*dy
    dx <<= 1;                           // dx is now 2*dx

    drawPixel(x0, y0, colour);

    if (dx > dy) {
        int fraction = dy - (dx >> 1);  // same as 2*dy - dx
        while (x0 != x1) {
            if (fraction >= 0) {
                y0 += stepy;
                fraction -= dx;         // same as fraction -= 2*dx
            }
            x0 += stepx;
            fraction += dy;                 // same as fraction -= 2*dy
            drawPixel(x0, y0, colour);
        }
    } else {
        int fraction = dx - (dy >> 1);
        while (y0 != y1) {
            if (fraction >= 0) {
                x0 += stepx;
                fraction -= dy;
            }
            y0 += stepy;
            fraction += dx;
            drawPixel(x0, y0, colour);
        }
    }
}

/*void ILI9341::drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t size, uint16_t colour) {
    int16_t dy = y1 - y0;
    int16_t dx = x1 - x0;
    int16_t stepx, stepy;

    if (dy < 0) {
        dy = -dy; stepy = -1;
    }
    else stepy = 1;

    if (dx < 0) {
        dx = -dx; stepx = -1;
    }
    else stepx = 1;

    dy <<= 1;                           // dy is now 2*dy
    dx <<= 1;                           // dx is now 2*dx

    drawPixel(x0, y0, size,colour);

    if (dx > dy) {
        int fraction = dy - (dx >> 1);  // same as 2*dy - dx
        while (x0 != x1) {
            if (fraction >= 0) {
                y0 += stepy;
                fraction -= dx;         // same as fraction -= 2*dx
            }
            x0 += stepx;
            fraction += dy;                 // same as fraction -= 2*dy
            drawPixel(x0, y0, size, colour);
        }
    } else {
        int fraction = dx - (dy >> 1);
        while (y0 != y1) {
            if (fraction >= 0) {
                x0 += stepx;
                fraction -= dy;
            }
            y0 += stepy;
            fraction += dx;
            drawPixel(x0, y0, size, olour);
        }
    }
}*/

void ILI9341::drawRect(uint16_t x,uint16_t y,uint16_t w,uint16_t h,uint16_t colour) {
	// two horizontal lines
	drawLine(x, y, x + w, y, colour);
	drawLine(x, y + h, x + w, y + h, colour);
	// two vertical lines
	drawLine(x, y, x, y + h, colour);
	drawLine(x + w, y, x + w, y + h, colour);
}

//draw colour filled rectangle
void ILI9341::drawRectFilled(uint16_t x,uint16_t y,uint16_t w,uint16_t h,uint16_t colour) {
	if ((x >= width) || (y >= height)) return;
	if ((x + w - 1) >= width) w = width - x;
	if ((y + h - 1) >= height) h = height - y;
	setAddress(x, y, x + w - 1, y + h - 1);
	for (y = h; y > 0; y--)
		for (x = w; x > 0; x--)
			writeData16(colour);
}

//clear lcd and fill with colour
void ILI9341::drawClear(uint16_t colour) {
	drawRectFilled(0,0,width, height, colour);
}

//rotate screen at desired orientation
void ILI9341::setRotation(Rotation rotation) {
	writeCommand(ILI9341_CMD_MEMORY_ACCESS_CONTROL);
	switch (rotation)  {
		case ROT0:
			writeData(0x40 | 0x08);
			width = ILI9341_HEIGHT;
			height = ILI9341_WIDTH;
		break;
		case ROT90:
			writeData(0x20 | 0x08);
			width  = ILI9341_WIDTH;
			height = ILI9341_HEIGHT;
		break;
		case ROT180:
			writeData(0x80 | 0x08);
			width  = ILI9341_HEIGHT;
			height = ILI9341_WIDTH;
		break;
		case ROT270:
			writeData(0x40 | 0x80 | 0x20 | 0x08);
			width  = ILI9341_WIDTH;
			height = ILI9341_HEIGHT;
		break;
	}
}

void ILI9341::drawCircle(uint16_t x, uint16_t y, uint16_t radius, uint16_t colour) {
    int16_t xp = 0, yp = radius;
    int16_t d = 3 - (2 * radius);
    while(xp <= yp) {
        drawPixel(x + xp, y + yp, colour);
        drawPixel(x + yp, y + xp, colour);
        drawPixel(x - xp, y + yp, colour);
        drawPixel(x + yp, y - xp, colour);
        drawPixel(x - xp, y - yp, colour);
        drawPixel(x - yp, y - xp, colour);
        drawPixel(x + xp, y - yp, colour);
        drawPixel(x - yp, y + xp, colour);
        if (d < 0)
        	d += (4 * xp) + 6;
        else {
            d += (4 * (xp - yp)) + 10;
            y -= 1;
        }
        xp++;
    }
}

void ILI9341::drawChar(uint16_t x, uint16_t y, char c, uint8_t size, uint16_t colour, uint16_t bg) {
	// draw. optimisation:6th font line is set as 0, to lower font array size
	for (int8_t i=0; i < FONT_WIDTH; i++ ) {
		uint8_t line = (i == FONT_WIDTH-1)? 0 : pgm_read_byte(font5x8 + (c * (FONT_WIDTH - 1)) + i);
		for (int8_t j = 0; j < FONT_HEIGHT; j++) {
			if (line & 0x1) {
				if (colour != TRANSPARENT) drawPixel(x + i*size, y + j*size, size, colour);
			}
			else {
				if (bg != TRANSPARENT) drawPixel(x + i*size, y + j*size, size, bg);
			}
			line >>= 1;
		}
	}
}


void ILI9341::drawString(uint16_t x, uint16_t y, uint8_t size, uint16_t colour, uint16_t bg,  const char *string) {
	if (x == CENTERX) {
		x = (width - size * FONT_WIDTH * strlen(string)) / 2;
	}
	if (y == CENTERY) {
		y = (height - size * FONT_HEIGHT) / 2;
	}
    unsigned char c;
    while ( (c = *string++) ) {
		if (c == '\r') continue;
		if (c != '\n') drawChar(x, y, c, size, colour, bg);
		// new line check
        if(c == '\n' || x > width - 2 * FONT_WIDTH * size) {
        	x = 0;
			y += size * FONT_HEIGHT;
        } else {
        	x += size * FONT_WIDTH;
        }
    }
}

void ILI9341::drawStringF(uint16_t x, uint16_t y, uint8_t size, uint16_t colour, uint16_t bg, char *szFormat, ...) {
    char szBuffer[256]; //in this buffer we form the message
    int NUMCHARS = sizeof(szBuffer) / sizeof(szBuffer[0]);
    int LASTCHAR = NUMCHARS - 1;
    va_list pArgs;
    va_start(pArgs, szFormat);
    vsnprintf(szBuffer, NUMCHARS - 1, szFormat, pArgs);
    va_end(pArgs);

    drawString(x, y, size, colour, bg, szBuffer);
}

// Setup a portion of the screen for vertical scrolling
// We are using a hardware feature of the display, so we can only scroll in portrait orientation
void ILI9341::setupScrollArea(uint16_t TFA, uint16_t BFA) {
  writeCommand(ILI9341_CMD_VERT_SCROLL_DEFINITION); // Vertical scroll definition
  writeData16(TFA);
  writeData16(320-TFA-BFA);
  writeData16(BFA);
}

// Setup the vertical scrolling start address
void ILI9341::scrollAddress(uint16_t VSP) {
	writeCommand(ILI9341_CMD_VERT_SCROLL_START_ADDRESS);
	writeData16(VSP);
}

// Call this function to scroll the display one text line
int ILI9341::scrollLine() {
	int yTemp = yStart; // Store the old yStart, this is where we draw the next line
	// fill remaining space
	drawRectFilled(0,yStart,width,TEXT_HEIGHT, BLACK);
	// Change the top of the scroll area
	yStart+=TEXT_HEIGHT;
	// The value must wrap around as the screen memory is a circular buffer
	if (yStart >= 320 - BOT_FIXED_AREA) yStart = TOP_FIXED_AREA + (yStart - 320 + BOT_FIXED_AREA);
	// Now we can scroll the display
	scrollAddress(yStart);
	return  yTemp;
}

void ILI9341::drawImageRGB16(uint16_t ltx, uint16_t lty, const uint16_t *image, uint16_t width, uint16_t height, uint8_t size) {
	for (int y=0;y<height;y++)
		for (int x=0;x<width; x++)
			drawPixel(ltx + x * size, lty +  y * size, size,  pgm_read_word(&image[x+y*width]));
}
