/*
** gp2y10 dust sensor Library for AVR Microcontrollers
** Version: 	0.1.0
** Date: 		July, 2015
** License:		LGPL v3
** Copyright: 	(C) 2009 - 2015 Radu Motisan, radu.motisan@gmail.com
** Description:	Details on http://www.pocketmagic.net/sharp-gp2y10-dust-sensor/
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



#include "gp2y10.h"

void GP2Y10::init(DigitalPin *ledPulser, ADC10b *adc, uint8_t adcPin) {
	m_ledPulser = ledPulser;
	m_adc = adc;
	m_pin = adcPin;
}

uint16_t GP2Y10::readRaw() {
	// shine the sensor IR LED
	*m_ledPulser = 0; // led on
	// wait just a little, this delay can be as big as 500uS
	_delay_us(50);
	// read sensor value
	uint16_t adcValue = m_adc->read(m_pin);
	// turn the IR LED off
	*m_ledPulser = 1;
	// filter values, biggest possible is arround 772
	if (adcValue > 800)
		return 0;
	else
		return adcValue;
}

void GP2Y10::readDust(float *dust) {
	// results are computed for a 5V voltage and a 10bit adc
	float adcVoltage =  readRaw() * (5.0 / 1024);
	// see why we have this limit on: http://www.pocketmagic.net/sharp-gp2y10-dust-sensor/
	if (adcVoltage < 0.583)
		*dust = 0;
	else
	 	*dust =  6 * adcVoltage / 35 - 0.1;
}
