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


#include "ds18b20.h"

void DS18B20::init( volatile  uint8_t  *port, uint8_t  dq) {
	// save globals
	m_pport = port;
	m_dq = dq;
	m_pddr = Port2DDR(m_pport);
	m_ppin = Port2PIN(m_pport);
}


uint8_t DS18B20::therm_ds18b20_reset()
{
	uint8_t i;
	//Pull line low and wait for 480uS
	*m_pport &= ~(1<<m_dq);
	*m_pddr |= (1<<m_dq);
	_delay_us(480);
	//Release line and wait for 60uS
	*m_pddr &= ~(1<<m_dq);
	_delay_us(60);
	//Store line value and wait until the completion of 480uS period
	i = *m_ppin & (1<<m_dq);
	_delay_us(480);
	//Return the value read from the presence pulse (0=OK, 1=WRONG)
	return i;
}

void DS18B20::therm_ds18b20_write_bit(uint8_t bit)
{
	//Pull line low for 1uS
	*m_pport &= ~(1<<m_dq);
	*m_pddr |= (1<<m_dq);
	_delay_us(1);
	//If we want to write 1, release the line (if not will keep low)
	if(bit) *m_pddr &= ~(1<<m_dq);
	//Wait for 60uS and release the line
	_delay_us(60);
	*m_pddr &= ~(1<<m_dq);
}

uint8_t DS18B20::therm_ds18b20_read_bit(void)
{
	uint8_t bit=0;
	//Pull line low for 1uS
	*m_pport &= ~(1<<m_dq);
	*m_pddr |= (1<<m_dq);
	_delay_us(1);
	//Release line and wait for 14uS
	*m_pddr &= ~(1<<m_dq);
	_delay_us(14);
	//Read line value
	if(*m_ppin & (1<<m_dq)) bit = 1;
	//Wait for 45uS to end and return read value
	_delay_us(45);
	return bit;
}

uint8_t DS18B20::therm_ds18b20_read_byte(void)
{
	uint8_t i=8, n=0;
	while(i--) {
		//Shift one position right and store read value
		n>>=1;
		n|=(therm_ds18b20_read_bit()<<7);
	}
	return n;
}

void DS18B20::therm_ds18b20_write_byte(uint8_t byte)
{
	uint8_t i=8;
	while(i--) {
		//Write actual bit and shift one position right to make the next bit ready
		therm_ds18b20_write_bit(byte&1);
		byte>>=1;
	}
}

int DS18B20::therm_ds18b20_read_temperature(float *temp)
{
	//Reset, skip ROM and start temperature conversion
	if (THERM_OK != therm_ds18b20_reset()) THERM_ERR;
	therm_ds18b20_write_byte(THERM_DS18B20_CMD_SKIPROM);
	therm_ds18b20_write_byte(THERM_DS18B20_CMD_CONVERTTEMP);

	//Wait until conversion is complete
	int to = 1000;
	while(!therm_ds18b20_read_bit()) {
		to--;
		if (to <= 0) return THERM_ERR;
	}
	//Reset, skip ROM and send command to read Scratchpad
	if (THERM_OK!= therm_ds18b20_reset()) return THERM_ERR;
	therm_ds18b20_write_byte(THERM_DS18B20_CMD_SKIPROM);
	therm_ds18b20_write_byte(THERM_DS18B20_CMD_RSCRATCHPAD);

	
	uint8_t l = therm_ds18b20_read_byte();
	uint8_t h = therm_ds18b20_read_byte();
	*temp = ( (h << 8) + l )*0.0625;
	if (*temp >= 80.0) return THERM_ERR;

	if (THERM_OK!= therm_ds18b20_reset()) return THERM_ERR;
	return THERM_OK;

}
