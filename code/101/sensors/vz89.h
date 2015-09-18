/*
** MiCS-VZ-89 Library for AVR Microcontrollers
** Version: 	0.1.0
** Date: 		July, 2015
** License:		LGPL v3
** Copyright: 	(C) 2009 - 2015 Radu Motisan, radu.motisan@gmail.com
** Description:	Detection Method Semiconductor gas sensor, detecting a wide range of VOCs
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

/*
Detection Method Semiconductor gas sensor, detecting a wide range of VOCs
Monitoring Range
400-2000 ppm equivalent CO2
0-1000 ppb isobutylene equivalent tVOCs
I2C Output (VZ-89)
Pin 2 and 4 ; see VZ I2C SPEC rev B for details of operation
Pull-up of 4.7 kOhms on master SDA and SCL
Response Time
Equivalent to conventional NDIR-CO2 sensors
< 5 seconds for tVOC
Refresh Output Frequency 1 Hz
Supply Voltage
5V DC, regulated +/- 5% for F version
3.3V DC regulated +/- 5% for T version
Operating Power
190 mW for F version (5V DC)
125 mW for T version (3.3V DC)
Warm-up Time 15 min
Operating Temperature 0°C to 50°C
Operating Humidity 0%RH to 95%RH (non condensing)
Storage Temperature -40°C to 80°C
Storage Humidity 0%RH to 95%RH (non condensing)
*/
#pragma  once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <avr/io.h>

#include "../config.h"
//#include <util/delay.h>
#include "../i2c/i2c.h"

#define VZ89_ADDR (0x70<<1) //0x70 default I2C address


//registers
#define VZ89_CMD_SETPPMCO2 	0x8	// This command is used to send a ppmCO2 value from an analyser to the VZ89 in order to recalibrate its outputs.
#define VZ89_CMD_GETSTATUS	0x9	// This command is used to read the VZ89 status coded with 6 bytes:

class VZ89 {
	void writemem(uint8_t reg, uint8_t value);
	void readmem(uint8_t reg, uint8_t buff[], uint8_t bytes);

public:
	// read the 6 bytes data over I2C
	void readRaw(uint8_t rawData[6]);
	// return CO2 equivalent [ppm] and tVOC equivalent referred to the isobutylene sensitivity unit [ppb].
	bool read(float *co2, uint8_t *reactivity, float *tvoc);
};
