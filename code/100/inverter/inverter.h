/*
** uRADMonitor High Voltage Inverter Library
** Version: 	0.1.0
** Date: 		January, 2014
** License:		LGPL v3
** Copyright: 	(C) 2009 - 2015 Radu Motisan, radu.motisan@gmail.com
** Description:	Used in uRADMonitor for handling the ferrite core inverter that generates High Voltage
** for the Geiger tubes. It was designed to allow configuration of Frequency and Duty cycle, and also provides
** a regulated voltage output mechanism.
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
#include <stdio.h>
// This class assumes the inverter signal is connected to pin PB1 on an ATmega uC

/************************************************************************/
/* Configuration                                                        */
/************************************************************************/

#define INVERTER_DEFAULT_DUTY	11			// start with low duty to allow powering the unit from higher voltages as well
#define INVERTER_DUTY_MIN		10			// inverter duty cycle will not go below 1%
#define INVERTER_DUTY_MAX		900			// inverter duty cycle will not go above 80% - with discharge transistor!
#define INVERTER_FREQUENCY		15000UL		// inverter frequency in Hertz //15500
//extern uint32_t INVERTER_FREQUENCY;

// SBM20 : 380V (350 - 475V)
// SI29BG: 380V (350 - 475V)
// LND712: 500V (450 - 650V)
#define INVERTER_THRESHOLD		500			// inverter output voltage target
#define INVERTER_TOLERANCE		5			// tolerance for adjusting the duty cycle to control the voltage

class Inverter {
	uint16_t duty, 							// used to set the duty cycle to adjust the output voltage
			dutymin,						// minimum duty cycle for the PWM signal (in per-mille percent)
			dutymax,						// maximum duty cycle for the PWM signal (in per-mille percent)
			target,							// the voltage we want to get - we adjust the duty cycle to match this voltage
			tolerance;						// the tolerance to the target voltage (usually +- 5V)
	uint32_t freq;							// frequency to operate at (in hertz)
	
	uint16_t highestVoltage,				// there's a slope of the output voltage in regards to the duty cycle. From a point, it starts descending. We
				highestVoltageDuty;			// want to remember that point so we can return in case we've missed it and we got to dutyMax.
	public:

	/*
	 * Constructor to create the inverter object
	 * frequency the frequency to operate at
	 * dutyDefault is the PWM duty cycle to start with (in per-mille percent)
	 * dutyMin is the minimum PWM duty cycle allowed (in per-mille percent)
	 * dutyMax is the minimum PWM duty cycle allowed (in per-mille percent)
	 * voltageTarget is the voltage we want to get, it is used in adjusting the duty cycle to match the target, in function adjustDutyCycle
	 */
	Inverter(uint32_t frequency = INVERTER_FREQUENCY,
			uint16_t dutyDefault = INVERTER_DEFAULT_DUTY, uint16_t dutyMin = INVERTER_DUTY_MIN, uint16_t dutyMax = INVERTER_DUTY_MAX,
			uint16_t voltageTarget = INVERTER_THRESHOLD, uint16_t voltageTolerance = INVERTER_TOLERANCE) {
		freq = frequency;
		duty = dutyDefault;
		dutymin = dutyMin;
		dutymax = dutyMax;
		target = voltageTarget;
		tolerance = voltageTolerance;

		highestVoltage = 0;
	}

	// CREATE Timer T1 PWM to drive inverter for regulated Geiger tube voltage 
	void initPWM();

	// sets the duty cycle for the PWM signal
	void setDuty(uint16_t d);
	
	// returns current duty cycle value
	uint16_t getDuty();

	// check tube voltage and adjust duty cycle to match output given threshold level
	// return false on critical error (when we hit the duty max), true if all ok
	bool adjustDutyCycle(int measuredVoltage);

	uint16_t getDutyMin();
	uint16_t getDutyMax();
	uint32_t getFrequency();
	void setFrequency(uint32_t  f);
};
