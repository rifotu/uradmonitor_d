/*
** Time counter Library for Atmega128 using an external 32.768kHz crystal
** Version: 	0.1.0
** Date: 		February 03, 2015
** License:		LGPL v3
** Copyright: 	(C) 2009 - 2015 Radu Motisan, radu.motisan@gmail.com
** Description:	mega128 features a RTC module that can provide accurate time measurement by using only a 32.768kHz crystal connected to TOSC1 / TOSC2
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

#include "AsyncRTC128T0.h"

#include "../config.h"

// init function used to set a callback to be called every new minute
void  AsyncRTC128T0::init(TimeCallback callSec, TimeCallback callMin) {
	ptrFuncSec = callSec;
	ptrFuncMin = callMin;
	
	//Wait for external clock crystal to stabilize;
	_delay_ms(20);

	// By setting the AS0 bit in ASSR, Timer/Counter0 is asynchronously clocked from the TOSC1 pin.
	// When AS0 is set, pins TOSC1 and TOSC2 are disconnected from Port C.
	ASSR |= (1<<AS0);
	TCCR0 |= (1<<CS02) | (1<<CS00); // prescaler set to 128 32768/128=256 Hz
	TIMSK |= (1<<TOIE0); //TOIE0 bit in the TIMSK (Timer/Counter Interrupt Mask Register) is then set to enable Timer/Counter0 Overflow Interrupt.
	sei();


	/*TIMSK &= ~((1<<TOIE0)|(1<<OCIE0));						//Make sure all TC0 interrupts are disabled
	ASSR |= (1<<AS0);										//set Timer/counter0 to be asynchronous from the CPU clock
															//with a second external clock (32,768kHz)driving it.
	TCNT0 =0;												//Reset timer
	TCCR0 =(1<<CS00)|(1<<CS02);								//Prescale the timer to be clock source/128 to make it
															//exactly 1 second for every overflow to occur
	while (ASSR & ((1<<TCN0UB)|(1<<OCR0UB)|(1<<TCR0UB)));	//Wait until TC0 is updated

	TIMSK |= (1<<TOIE0);
	sei();*/
}

void AsyncRTC128T0::TimerEvent() {
	total_sec++;

	if ((total_sec>0) && (total_sec % 60 == 0)) {
		total_min++;
		if (ptrFuncMin != 0) ptrFuncMin();
	}
	if (ptrFuncSec != 0) ptrFuncSec();
}

uint8_t AsyncRTC128T0::getSec() { return total_sec % 60; }
uint8_t AsyncRTC128T0::getMin() { return (total_sec / 60) % 60; }
uint8_t AsyncRTC128T0::getHour() { return (total_sec / 3600) % 24; }

void AsyncRTC128T0::startInterval() {
	timeKey = total_sec;
}

uint32_t AsyncRTC128T0::getInterval() {
	return total_sec - timeKey;
}
