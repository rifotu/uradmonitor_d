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
#include "Application.h"


/************************************************************************************************************************************************/
/* Global Objects                                                       																		*/
/************************************************************************************************************************************************/

uint32_t 			deviceID 				= ((uint32_t) ((DEV_MODEL << 4) | DEV_RAD_DETECTOR) << 24) | ((uint32_t) DEV_ID);
// microcontroller pins
DigitalPin			lcdReset(&PORTA, PA0), lcdDC(&PORTA, PA1),  lcdCS(&PORTA, PA2), 							// LCD Pins
					lcdBacklight(&PORTA, PA3),																	// Backlight control, ON
					touchXP(&PORTA, PA4), touchYN(&PORTA, PA5), touchXN(&PORTF, PF6), touchYP(&PORTF, PF7), 	// touchscreen pins
					powerControl(&PORTA, PA6),																	// power control, OFF
					speaker(&PORTA, PA7),																		// speaker pin
					dustFlash(&PORTD, PD7, DigitalPin::OUTPUT, true);											// dust sensor control pin, ON
// various features
AsyncRTC128T0	 	time;										// handles the time interrupts
ADC10b				adc;										// 10bit ADC converter
UART				uart0, 										// connected to FT232 for serial communication and firmware upgrade
					uart1;										// connected to ESP8266 for Internet Access

// display and input
ILI9341 			lcd(&lcdDC, &lcdReset, &lcdCS);																// LCD
TouchScreen			touch(&adc, &touchXP, &touchYN, &touchXN, &touchYP);										// touchscreen

// wifi
ESP8266				wifi(&uart1);								// setup wifi on serial communication
// sensors
BMP180				bmp180;										// temperature and pressure sensor
VZ89				vz89;										// air quality sensor: CO2 and VOCs
GP2Y10				dust;										// air quality sensor: dust particles
// High voltage inverter
Inverter 			inverter(12500, 300, 10, 740, GEIGER_TARGET_VOLTAGE, 2); 									// handles the PWM on Timer1 to drive the Geiger tube inverter
Data				data;
// User Interface
UI					gui(&lcd, &touch, &data);					// class to handle UI, receiving display, touch and data objects

//2^32 = 4294967296 , cpm count range 0..4294967295x
volatile uint32_t 	geigerPulsesPerMin 		= 0,				// geiger: total number of pulses detected
					geigerPulsesInterval	= 0,
					geigerCPM 				= 0,				// geiger: counts per minute
					geigerTotalPulses 		= 0,				// geiger: all time pulses
					geigerCPMAverage 		= 0;				// geiger: all time average
float				geigerTotalDose 		= 0.0;				// geiger: all time dose estimation

volatile bool		cmdSend					= false,
					cmdRefreshText 			= true, 			// to increase speed, refresh text once per second
					cmdRefreshUI			= true,				// refresh entire UI only when needed
					cmdBeep 				= false;

uint8_t				secTimeout 				= 1;
uint16_t 			uiResult 				= 0;				// no user touch event to consume by default
char				szParamServer[50]		= { 0 };			// general purpose buffer
bool				isMuted					= false;

uint8_t				cmdAlarm 				= 0;				// if non zero , alarm will sound
/************************************************************************************************************************************************/
/* Callbacks                                                                                                                                    */
/************************************************************************************************************************************************/

// callback function called from the timecounter object when a full minute has elapsed
void callback_timeMinute() {
	geigerCPM = geigerPulsesPerMin;
	geigerPulsesPerMin = 0;
	geigerTotalDose += CPM2uSVh((uint8_t)DEV_RAD_DETECTOR, geigerCPM) / 60.0;
	// ask to send data online
	cmdSend = 1;
}

// callback function called from the timecounter object when a full second has elapsed
void callback_timeSecond() {
	cmdRefreshText = true;

	// intermediary integration: every 5 sec
	if (time.getSec() % PARTIAL_INTERVAL == 0) {
		// not a full minute yet
		if (time.getSec() != 0) {
			geigerCPM = geigerPulsesInterval * 12; // 60/partial interval
			geigerPulsesInterval =  0;
			/*int intervals = time.getSec() / PARTIAL_INTERVAL;
			if (geigerPulsesPerMin > 0)
				geigerCPM = (60 * geigerPulsesPerMin) / (PARTIAL_INTERVAL * intervals);
				*/
		}
	}
	if (cmdAlarm) {
		if (!isMuted) speaker.toggle();
	}
	secTimeout++;
}

// On our Async T0 , this ISR will be called precisely each second
ISR(TIMER0_OVF_vect) {
	time.TimerEvent();
}

// int4 interrupt handler, triggered each time a rising edge is detected on INT4
// we have a top limit of 2^32-1 pulses. We don't go over it.
ISR (INT4_vect) {
	// count this pulse
	if (geigerPulsesPerMin < MAX_UINT32)
		geigerPulsesPerMin++;
	geigerPulsesInterval ++;
	// compute average
	if (time.getTotalSec() > 0)
		geigerCPMAverage = (60 * geigerTotalPulses) / time.getTotalSec();
	// signal a new pulse, to be handled in the main loop
	cmdBeep = 1;
}

char buffer0[101];
uint8_t index0 = 0;

ISR(USART0_RX_vect){
	volatile uint8_t c = UDR0; //Read the value out of the UART buffer
	//uart0.send(c);
	data.serial_recv++;
	if (c == '\n' || c == '\r' || index0 == 100) {
		serialCommand(buffer0);
		index0 = 0;
	} else {
		buffer0[index0++] = c;
		buffer0[index0] = 0;
	}
}

char buffer1[101];
uint8_t index1 = 0;

ISR(USART1_RX_vect){
	volatile uint8_t c = UDR1; //Read the value out of the UART buffer
	// send WLAN answer over uart0 for debug
	uart0.send(c);
	data.serial_sent++;
	if (c == '\n' || c == '\r' || index1 == 100) {
		serialCommand(buffer1);
		index1 = 0;
	} else {
		buffer1[index1++] = c;
		buffer1[index1] = 0;
	}
}

// must contain quick code only
void serialCommand(char *line) {
	// check uart1/wlan first
	if (strstr(line, "+CWLAP:(0")) {// == 0 && line[8] == '0') {
		// some pointer dangerous magic
		uint8_t apNameLength = strstr(line+11, "\"") - line - 11;
		if (apNameLength > 19) apNameLength = 19; // TODO: trim -> will result in connection error : redo datatypes
		if (data.freeAPCount < 5) {
			memset(data.freeAPList[data.freeAPCount], 0, 19);
			strncpy(data.freeAPList[data.freeAPCount], line+11, apNameLength);
			uart0.send("free ap: %s ", data.freeAPList[data.freeAPCount]);
			data.freeAPCount++;
		}
	}
	// check uart0/user commands first
	if (strcmp(line, "beep") == 0) beep();
	else if (strcmp(line, "shutdown") == 0) shutdown();
	else if (strcmp(line, "listwlan") == 0) wifi.listAP();
}
/************************************************************************************************************************************************/
/* Helper functions that can't go in aux.cpp                                                                                                    */
/************************************************************************************************************************************************/
// produce a short beep
void beep(){
	speaker = 1;
	_delay_us(50);
	speaker = 0;
}

// read voltage on battery via divider
float readBatVoltage() {
	return adc.readDivVoltage(VREF, 220, 220, BATTERY_ADC);
}

// read voltage on Geiger tube via divider
float readTubeVoltage() {
	return adc.readDivVoltage(VREF, 10000, 47, INVERTER_ADC);
}

// pull pin to ground and shut down
void shutdown() {
	beep();
	_delay_ms(2000);
	powerControl = 0;
	powerControl.config(DigitalPin::INPUT);
}

// control backlight on and off
void backlight(bool set) {
	if (set) {
		lcdBacklight.config(DigitalPin::OUTPUT);
		//lcdBacklight = 1;
	} else {
		lcdBacklight.config(DigitalPin::INPUT);
	}
}

// compute battery percentage from voltage
// max voltage on battery is 4.2, but to count the drain we consider it 4.1 . Minimum is 3.3
uint8_t batteryPercentage(float vol) {
	uint8_t result = (vol - BATTERY_LIMIT) * 100 / (4.1 - BATTERY_LIMIT);
	if (result > 100) return 100; else return result;
}


// main entry point, go ahead, have fun!
int main(void) {
	// hold the power button for 3 seconds or shutdown
	//_delay_ms(STARTUP_DELAY);
	// keep power pin High unless we want to shutdown
	powerControl = 1;

	// disable JTAG so we can use PF4,PF5,PF6,PF7 for ADC and GPIO
	MCUCSR |=(1<<JTD);MCUCSR |=(1<<JTD); // two times!!

	// init time RTC
	time.init(callback_timeSecond, callback_timeMinute);
	time.startInterval();

	// microcontroller features
	adc.init();	// needed by gp2y10 and more
	spi_init();	// needed by ILI9341
	i2c_init(); // needed by bmp180 and mics-vz-89t
	uart1.init(1, 9600, 1); // needed by esp8266 . Interrupts are hard on parsing, polling would be easier but blocking
	uart0.init(0, 9600, 1);
	// CONFIGURE INTERRUPT INT4  to count pulses from Geiger Counter
	EICRB |= (1<<ISC00) | (1<<ISC01); // Configure INT4 to trigger on RISING EDGE
	EIMSK |= (1<<INT4); // Configure INT4 to fire interrupts

	// CREATE Timer T1 PWM to drive inverter for regulated Geiger tube voltage
	inverter.initPWM();

	// init display
	lcd.init();
	lcd.setRotation(ILI9341::ROT0);
	//lcd.drawClear(BLACK);
	backlight(true);

	// init sensors
	bmp180.init();
	dust.init(&dustFlash, &adc, PF1);

	beep();
	// start UI

	// enter main menu
	// ## touchscreen calibration code
	/*while (1) {
		uint16_t x = 0, y = 0, z = 0;
				if (touch.read(&x, &y , &z)) {
					lcd.drawPixel(x,y, 2, RED);
				}
		lcd.drawStringF(0,0,2,WHITE, BLACK, "%4u %4u %4u", touch.readRawX(), touch.readRawY(), touch.readRawPressure());
	}*/

	// draw GUI first page with self check
	if (!gui.drawPage(PAGE_INIT))
		shutdown();


	_delay_ms(1000);
	gui.drawPage(PAGE_MAIN);
	// ## main code loop
	while (1) {
		// ## beep
		if (cmdBeep && !cmdAlarm && !isMuted) {
			beep();
			cmdBeep = false;
		}
		// ## read sensors
		// read inverter voltage, via 10M/47K resistive divider, connected to pin ADC2
		data.geiger_voltage = readTubeVoltage();
		inverter.adjustDutyCycle(data.geiger_voltage); // do nothing on failure, we can't reset
		// read battery
		data.battery_voltage = readBatVoltage();

		// turn backlight off when timeout is reached
		if (secTimeout > BACKLIGHT_TIMEOUT) {
			backlight(false);
			secTimeout = 0;
		}
		// ## draw titlebar and refresh data display
		if (cmdRefreshText) {
			// sensor BMP180
			bmp180.readAll(&data.bmp180_temp, &data.bmp180_pressure, &data.bmp180_altitude);
			dust.readDust(&data.gp2y10_dust);
			// sensor MICS-VZ-89T
			uint8_t reactivity = 0;
			// repeat until successful read with timeout?
			//int timeout = 10;
			//while (!vz89.read(&data.vz89_co2, &reactivity, &data.vz89_voc) && timeout) { _delay_ms(1500); timeout--; }
			vz89.read(&data.vz89_co2, &reactivity, &data.vz89_voc);
			// geiger readings
			//float dose = aux_CPM2uSVh((uint8_t)DEV_RAD_DETECTOR, geigerCPM);
			data.geiger_cpm = geigerCPM;

			data.time_hour = time.getHour();
			data.time_minute = time.getMin();
			data.time_second = time.getSec();

			data.setLimits(); // must be changed to proper OOP set/get for all fields
			gui.updateValues();

			// ## alarm condition
			if (geigerCPM >= GEIGER_CPM_ALARM) {
				// threshold to sound alarm reached
				cmdAlarm = ALARM_RADIATION;
			} else if (cmdAlarm) {
				// alarm should be turned off
				cmdAlarm = 0;
				speaker = 0;
			}

			cmdRefreshText = false;

		}
		// ## every minute we can dispatch data over serial or over WLAN to uradmonitor
		if (cmdSend) {
			char tmp[200];
			sprintf(tmp,"{\"data\":{ \"id\":\"%08lX\","
					"\"type\":\"%X\",\"detector\":\"%s\","
					"\"cpm\":%lu,\"temperature\":%.2f,\"uptime\": %lu,"
					"\"pressure\":%lu,\"dust\":%.2f,\"co2\":%.2f,\"voc\":%.2f,"
					"\"battery\":%.2f,\"tube\":%u}}",
					deviceID, DEV_MODEL, aux_detectorName(DEV_RAD_DETECTOR), geigerCPM, data.bmp180_temp, time.getTotalSec(),
					data.bmp180_pressure, data.gp2y10_dust,data.vz89_co2, data.vz89_voc, data.battery_voltage, data.geiger_voltage);
			data.serial_sent += strlen(tmp);
			uart0.send(tmp);

			// internet code here

			sprintf(tmp,"id=%08lX&ts=%ld&inv=%d&ind=%d&s1t=%2.2f&cpm=%ld&voc=%.2f&co2=%.2f",
								deviceID,
								time.getTotalSec(),
								data.geiger_voltage,
								data.geiger_duty,
								data.bmp180_temp,
								geigerCPM,
								data.vz89_voc, data.vz89_co2);

						wifi.sendData(tmp);
			cmdSend = false;
		}

		// ## act on the gui elements
		// read a new touch event only if we are done with previous: useful for handling confirmation "modal" "dialogs"
		if (uiResult == 0) {
			uiResult = gui.readTouchEvent();
			// reset backlight timeout on valid touch
			if (uiResult > 0) {
				secTimeout = 0;
				backlight(true);
				// if screen is pressed while alarm is on, stop alarm
				if (cmdAlarm) {
					cmdAlarm = false;
					speaker = 0;
				}
			}
		}
		// handle special cases: click on wlan AP buttons
		if (uiResult >= ID_BUTTON_WLAN_START && uiResult < ID_BUTTON_WLAN_STOP) {
			uint8_t ap_index = uiResult - ID_BUTTON_WLAN_START;
			// connect and return to main screen
			wifi.connectWiFi(data.freeAPList[ap_index], "");
			uiResult = 0;
			gui.drawPage(PAGE_MAIN);
		}
		// handle regular buttons
		switch (uiResult) {
			case  ID_BUTTON_SHUTDOWN: {
				uint16_t result = gui.showYesNoPopup("Are you sure?");
				if (result == ID_YES)
					shutdown();
				else if (result == ID_NO) {
					uiResult = 0;
					gui.drawPage(PAGE_MAIN);
				}
			} break;
			case ID_BUTTON_MEASURE: {
				uiResult = 0;
				gui.drawPage(PAGE_MEASURE);
			} break;
			case ID_BUTTON_MONITOR: {
				uiResult = 0;
				gui.drawPage(PAGE_MONITOR);
			} break;
			case ID_BUTTON_SETTINGS: {
				uiResult = 0;
				gui.drawPage(PAGE_SETTINGS);
			} break;
			case ID_BUTTON_BACK: {
				uiResult = 0;
				gui.drawPage(PAGE_MAIN);
			} break;
			case ID_BUTTON_MUTE: {
				isMuted = !isMuted;
				if (!isMuted) beep(); // test beep that sound is on
				uiResult = 0;
			} break;
			case ID_BUTTON_CALIBRATE: {
				uiResult = 0;
				gui.drawPage(PAGE_CALIBRATE);
			} break;
			case ID_BUTTON_WLAN: {
				// request list of WLAN APs
				data.freeAPCount = 0;
				//wifi.setMode();
				wifi.listAP();
				uiResult = 0;
				gui.drawPage(PAGE_WLAN);
			}
			break;
			// other commands that don't require a popup so we consume asap
			default:
				uiResult = 0;
		}

		//uint16_t x, y,z;
				//gui.getLastTouch(&x, &y, &z);
				//lcd.drawStringF(0,288, 2, RED, BLACK,"%u %d,%d   ", uiResult, x,y);

	}
}

