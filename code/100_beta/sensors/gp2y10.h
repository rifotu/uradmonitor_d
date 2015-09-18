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


#pragma  once
#include "../adc/adc.h"
#include "../gpio/DigitalPin.h"
#include "../adc/adc.h"

class GP2Y10 {
	DigitalPin *m_ledPulser;
	ADC10b *m_adc;
	uint8_t m_pin;
public:
	// sets the pulser GPIO pin and the ADC converter, both to be used with this sensor
	void init(DigitalPin *ledPulser, ADC10b *adc, uint8_t adcPin);
	// does the probing and returns the 10bit ADC value representing dust
	uint16_t readRaw();
	// converting the raw value to dust quantity in mg/m^3
	void readDust(float *dust);
};

/*
 * http://www.howmuchsnow.com/arduino/airquality/
3000 +     = VERY POOR
1050-3000  = POOR
300-1050   = FAIR
150-300    = GOOD
75-150     = VERY GOOD
0-75       = EXCELLENT
*/
