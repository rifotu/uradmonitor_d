/*
** uRADMonitor auxiliary code
** Version: 	0.1.0
** Date: 		January, 2014
** License:		LGPL v3
** Copyright: 	(C) 2009 - 2015 Radu Motisan, radu.motisan@gmail.com
** Description:	Used in the uRADMonitor code to serve various purposes
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
#include <avr/interrupt.h>
#include <avr/wdt.h>

#include "../config.h"


// Reset the microcontroller
void aux_softwarereset();

// Proper reset using the watchdog: untested
void aux_wdreset() ;

// not more than 10 chars!
char *aux_detectorName(uint8_t param);

/**
 * Encodes a small float (-127 .. 127) to an 16 bit integer, including sign and two decimals
 */
uint16_t float2int(float val);

// dose equivalent linear approximative conversion from CPM
float CPM2uSVh(uint8_t tube, uint32_t cpm);

// to save display space, we use multipliers
float getDoseMulFactor(float dose);

char getDoseMulSym(float dose);


/**
 * copyBytes
 * copy specified src_bytes from src to dst at specified offset
 * this function is not safe, as dst is not checked for size. Make sure it is big enough!
 */
uint16_t copyBytes(void *dst, uint16_t dst_offset, void *src, uint8_t src_bytes);
