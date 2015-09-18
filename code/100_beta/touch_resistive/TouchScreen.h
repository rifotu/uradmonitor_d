/*
** Resistive touchscreen driver for AVR Microcontrollers
** Version: 	0.1.0
** Date: 		August, 2015
** License:		LGPL v3
** Copyright: 	(C) 2009 - 2015 Radu Motisan, radu.motisan@gmail.com
** Description:	UART interrupt driven serial communication class file for atmega128
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

#include <stdint.h>
#include "../gpio/DigitalPin.h"
#include "../adc/adc.h"

#define TOUCH_HEIGHT			 					240	// touchscreen height in landscape mode
#define TOUCH_WIDTH 								320	// touchscreen width in landscape mode

class TouchScreen {
public:
	enum Orientation {
				PORTRAIT = 0,		// portrait orientation
				LANDSCAPE = 1		// landscape orientation
			};

	/*
	 * TouchScreen constructor
	 * init the GPIO pins and set the orientation (default portrait)
	 * XN and YP are ADC pins, YP and YN are general IO
	 */
	TouchScreen(ADC10b *adc, DigitalPin *xp, DigitalPin *yn, DigitalPin *xn, DigitalPin *yp, Orientation orientation = PORTRAIT, uint16_t r = 0);

	// change the orientation (default portrait)
	void	setOrientation(Orientation orientation = PORTRAIT);

	uint16_t readRawPressure(void);
	uint16_t readRawY();
	uint16_t readRawX();

	bool	read(uint16_t *x, uint16_t *y, uint16_t *pressure);
	uint16_t	lastRawX, lastRawY, lastRawZ;
private:
	ADC10b 		*m_adc;
	DigitalPin 	*xn_adc, *yp_adc, *xp_io, *yn_io;
	uint16_t 	touchResistance;
	Orientation m_orientation;
};
