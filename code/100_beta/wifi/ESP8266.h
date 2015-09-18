/*
** ESP8266 Library for AVR microcontrollers
** Copyright (C) 2009 - 2015 Radu Motisan, radu.motisan@gmail.com
** Interface for the ESP8266 Library over a serial line
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

#include "../config.h"
#include "../uart/uart.h"
#include <stdio.h>
#include <string.h>

#define AT_RESET 			"AT+RST\r\n"
#define AT_FIRMWARE			"AT+GMR\r\n"
#define AT_WIFI_MODE 		"AT+CWMODE"//AT+CWMODE=3 access point + station
#define AT_JOIN_AP 			"AT+CWJAP\r\n"
#define AT_LIST_AP 			"AT+CWLAP\r\n"
#define AT_QUIT_AP 			"AT+CWQAP\r\n"
#define AT_SET_AP_PARAMS 	"AT+CWSAP\r\n"
#define AT_GET_CON_STATUS 	"AT+CIPSTATUS"
#define AT_START_CON 		"AT+CIPSTART"
#define AT_SEND 			"AT+CIPSEND"
#define AT_CLOSE 			"AT+CIPCLOSE"
#define AT_SET_MUL_CON 		"AT+CIPUX"
#define AT_SET_SERVER 		"AT+CIPSERVER"
#define AT_RECEIVED_DATA	"+IP86.82.199.83D"
#define AT_CHANGESSID		"AT+CWSAP=Test123\r\n"

class ESP8266 {
	UART *m_serial;
public:
	ESP8266(UART *serial) {
		m_serial = serial;
	}

	void reset() {
		m_serial->send(AT_RESET);
	}
	void firmware() {
		m_serial->send(AT_CHANGESSID);
	}
	void setMode() {
		m_serial->send("%s=3\r\n", AT_WIFI_MODE);
	}
	void listAP() {
		m_serial->send(AT_LIST_AP);
	}
	void sendCustom(char *cmd) {
		m_serial->send(cmd);
	}


	bool connectWiFi(char *ssid, char *password) {
		//m_serial->send("AT+CWMODE=1\r\n");
		m_serial->send("AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, password);
		//m_serial->send("AT_CWJAP=\"%s\"\r\n", ssid);
		return true;
	}

	// no time: quick and dirty uRADMonitor server connect
	void sendData(char *data) {
		m_serial->send("AT+CIPSTART=\"TCP\",\"23.239.13.18\",80\r\n");
		char tmp[255] = {0};
		sprintf(tmp, "GET /upload/0.1/upload.php?%s HTTP/1.1\r\nHost: data.uradmonitor.com\r\n\r\n", data);
		m_serial->send("AT+CIPSEND=%d\r\n", strlen(tmp));
		m_serial->send(tmp);

		m_serial->send("AT+CIPCLOSE\r\n");

	}
};
