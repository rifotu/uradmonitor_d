/*
** Portable Environmental Monitor
** Copyright (C) 2009 - 2015 Radu Motisan, radu.motisan@gmail.com
** A battery powered handheld device that measures radiation (alpha,beta,gamma), and air parameters:
** temperature, pressure, humidity, content of dust particles, CO2 levels, tVOCs levels
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

#define __AVR_ATmega128A__

//#include <avr/io.h>
//#include <avr/iom128a.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include "config.h"
// microcontroller interface
#include "gpio/DigitalPin.h"
#include "misc/aux.h"
#include "time/AsyncRTC128T0.h"
#include "i2c/i2c.h"
#include "spi/spi.h"
#include "uart/uart.h"
#include "adc/adc.h"
// lcd
#include "ili9341/ili9341.h"
// various pheripherals
#include "misc/detectors.h"
#include "misc/models.h"
#include "inverter/inverter.h"
#include "touch_resistive/TouchScreen.h"
#include "wifi/ESP8266.h"
#include "sensors/bmp180.h"
#include "sensors/bme280.h"
#include "sensors/vz89.h"
#include "sensors/gp2y10.h"
#include "Data.h"
// UI
#include "ui/ui.h"


/************************************************************************************************************************************************/
/* Unique ID configurator                                                                                                                       */
/************************************************************************************************************************************************/
#define				VER_HW					101 				//108
#define				VER_SW					100
#define				DEV_MODEL				MODEL_D				// 1=model_A, 2=model_B, 3=model_C, 4=model_A2, 9=model_0
#define				DEV_RAD_DETECTOR		GEIGER_TUBE_LND712 	// 1=geiger_SBM20 , 2=geiger_SI29BG, 3=geiger_SBM19
#define				DEV_ID					0x6	 				// change here for new devices

/************************************************************************************************************************************************/
/* Constants                                                            																		*/
/************************************************************************************************************************************************/
#define				INVERTER_ADC			PF0
#define 			BATTERY_ADC				PF5
#define				STARTUP_DELAY			3000				// hold the power button for 3 seconds or shutdown
#define				GEIGER_TARGET_VOLTAGE	450					// configure voltage to generate for geiger tube
#define 			VREF 					3.3					// 3.3 regulated Voltage ref thanks to AMS1117 3.3V
#define				BATTERY_LIMIT			3.3					// shutdown if voltage on battery is lower
#define 			BACKLIGHT_TIMEOUT		60					// 5seconds to timeout backlight, tap on screen to restart
#define				PARTIAL_INTERVAL		5					// seconds to display first results: must divide 60 for precise calculations
#define				MAX_UINT32				0xFFFFFFFF			// max uint32 value
#define 			ALARM_TEMP				1
#define 			ALARM_PRESSURE			2
#define				ALARM_HUMI				3
#define 			ALARM_DUST				4
#define				ALARM_CO2				5
#define				ALARM_VOC				6
#define				ALARM_RADIATION			7
#define 			DOSE_THRESHOLD_LOW 		0.10
#define 			DOSE_THRESHOLD_NORMAL 	0.20
#define 			DOSE_THRESHOLD_HIGH 	1.00

/************************************************************************************************************************************************/
/* Global Objects                                                       																		*/
/************************************************************************************************************************************************/

extern uint32_t 			deviceID;
// various features
extern AsyncRTC128T0	 	time;										// handles the time interrupts
extern bool isMuted;
extern uint8_t cmdAlarm;
/************************************************************************************************************************************************/
/* Helper functions that can't go in aux.cpp                                                                                                    */
/************************************************************************************************************************************************/
// produce a short beep
void beep();

float readBatVoltage() ;

// read voltage on Geiger tube via divider
float readTubeVoltage();

// pull pin to ground and shut down
void shutdown();

// control backlight on and off
void backlight(bool set);

// compute battery percentage from voltage
// max voltage on battery is 4.2, but to count the drain we consider it 4.1 . Minimum is 3.3
uint8_t batteryPercentage(float vol);

void serialCommand(char *cmd);

void setup();
