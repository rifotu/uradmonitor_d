/*
** DS18B20 Library for AVR Microcontrollers
** Version: 	0.1.0
** Date: 		May, 2012
** License:		LGPL v3
** Copyright: 	(C) 2009 - 2012 Radu Motisan, radu.motisan@gmail.com
** Description:	DS18B20 Temperature Sensor Minimal interface for Atmega microcontrollers
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


/*********************************************
 * CONFIGURE THE WORKING PIN
 *********************************************/

#pragma once

#include <avr/io.h> 
#include <stdio.h>

#include "../config.h"

/* list of these commands translated into C defines:*/
#define THERM_DS18B20_CMD_CONVERTTEMP 0x44
#define THERM_DS18B20_CMD_RSCRATCHPAD 0xbe
#define THERM_DS18B20_CMD_WSCRATCHPAD 0x4e
#define THERM_DS18B20_CMD_CPYSCRATCHPAD 0x48
#define THERM_DS18B20_CMD_RECEEPROM 0xb8
#define THERM_DS18B20_CMD_RPWRSUPPLY 0xb4
#define THERM_DS18B20_CMD_SEARCHROM 0xf0
#define THERM_DS18B20_CMD_READROM 0x33
#define THERM_DS18B20_CMD_MATCHROM 0x55
#define THERM_DS18B20_CMD_SKIPROM 0xcc
#define THERM_DS18B20_CMD_ALARMSEARCH 0xec

#define THERM_OK 0
#define THERM_ERR 1

/* constants */
#define THERM_DS18B20_DECIMAL_STEPS_12BIT 625 //.0625

class DS18B20 {
	volatile uint8_t *m_pddr, *m_ppin, *m_pport;
	uint8_t m_dq;
	
	volatile uint8_t* Port2DDR(volatile uint8_t *port) {
		return port - 1;
	}
	volatile uint8_t* Port2PIN(volatile uint8_t *port) {
		return port - 2;
	}
	
	uint8_t therm_ds18b20_reset();
	
	void therm_ds18b20_write_bit(uint8_t bit);
	uint8_t therm_ds18b20_read_bit(void);
	uint8_t therm_ds18b20_read_byte(void);
	void therm_ds18b20_write_byte(uint8_t byte);
	//uint16_t therm_ds18b20_read_word(void);
	//void therm_ds18b20_write_word(uint16_t word);
public:
	void init( volatile uint8_t  *port, uint8_t  dq) ;
	//void therm_read_temperature(int *digi, int *deci);
	int therm_ds18b20_read_temperature(float *temp);
	//int therm_ds18b20_read_temperature2(float *temp);
};	
