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


#include "TouchScreen.h"

/*
 * TouchScreen constructor
 * init the GPIO pins and set the orientation (default portrait)
 * XN and YP are ADC pins, YP and YN are general IO
 */
TouchScreen::TouchScreen(ADC10b *adc, DigitalPin *xp, DigitalPin *yn, DigitalPin *xn, DigitalPin *yp, Orientation orientation, uint16_t r) {
	m_adc = adc;
	xp_io = xp;
	yn_io = yn;
	xn_adc = xn;
	yp_adc = yp;
	m_orientation = orientation;
	touchResistance = r;
}

// change the orientation (default portrait)
void TouchScreen::setOrientation(Orientation orientation) {
	m_orientation = orientation;
}

uint16_t TouchScreen::readRawX(void) {
	xn_adc->config(DigitalPin::INPUT);
	*xn_adc = 0;

	xp_io->config(DigitalPin::INPUT);
	*xp_io = 0;

	yn_io->config(DigitalPin::OUTPUT);
	*yn_io = 0;

	yp_adc->config(DigitalPin::OUTPUT);
	*yp_adc = 1;

	return 1024 - m_adc->read(xn_adc->getPin()) - 1;
}


uint16_t TouchScreen::readRawY(void) {
	yn_io->config(DigitalPin::INPUT);
	*yn_io = 0;

	yp_adc->config(DigitalPin::INPUT);
	*yp_adc = 0;

	xn_adc->config(DigitalPin::OUTPUT);
	*xn_adc = 0;

	xp_io->config(DigitalPin::OUTPUT);
	*xp_io = 1;

	return 1024 - m_adc->read(yp_adc->getPin()) - 1;
}

uint16_t TouchScreen::readRawPressure(void) {
	// Set X+ to ground
	xp_io->config(DigitalPin::OUTPUT);
	*xp_io = 0;

	// Set Y- to VCC
	yn_io->config(DigitalPin::OUTPUT);
	*yn_io = 1;

	// Hi-Z X- and Y+
	xn_adc->config(DigitalPin::INPUT);
	*xn_adc = 0;

	yp_adc->config(DigitalPin::INPUT);
	*yp_adc = 0;

	int z1 = m_adc->read(xn_adc->getPin());
	int z2 = m_adc->read(yp_adc->getPin());

	if (touchResistance != 0) {
		// now read the x
		float rtouch;
		rtouch = z2;
		rtouch /= z1;
		rtouch -= 1;
		rtouch *= readRawX();
		rtouch *= touchResistance;
		rtouch /= 1024;

		return rtouch;
	} else
		return 1024 - (z2-z1) -1;
}

bool TouchScreen::read(uint16_t *x, uint16_t *y, uint16_t *pressure) {
	uint32_t calX1 = 142, calY1 = 170, calX2 = 892, calY2 = 866,
	//uint32_t calX1 = 60, calY1 = 87, calX2 = 160, calY2 = 118,
			minPressure = 200;	// minimum pressure to consider a click

	lastRawZ = readRawPressure();

	if (lastRawZ < minPressure) return false;
	// two consequent reads to double check coords and reduce errors
	lastRawX = readRawX();
	if (readRawX() != lastRawX) return false;

	lastRawY = readRawY();
	if (readRawY() != lastRawY) return false;

	if (lastRawX < calX1 || lastRawX > calX2 || lastRawY < calY1 || lastRawY > calY2)
			return false;

	lastRawX = (lastRawX - calX1) * TOUCH_WIDTH / (calX2 - calX1);
	lastRawY = (lastRawY - calY1) * TOUCH_HEIGHT / (calY2 - calY1);

	if (m_orientation == PORTRAIT) {
		*x = lastRawY;
		*y = TOUCH_WIDTH - lastRawX;
	} else {
		*x = lastRawX;
		*y = lastRawY;
	}

	*pressure = lastRawZ;

	return true;
}
