/*
** UI Library for AVR microcontrollers
** Copyright (C) 2009 - 2015 Radu Motisan, radu.motisan@gmail.com
** A user interface system using a display and touchscreen
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

#include "../Language.h"
#include "../ili9341/ili9341.h"
#include "../touch_resistive/TouchScreen.h"
#include "../ili9341/img_logo.h"
#include "../Application.h"
#include "button.h"
#include "image.h"
#include "imgbutton.h"

#define PAGE_INIT				0x1
#define PAGE_MAIN 				0x2
#define PAGE_MEASURE			0x3
#define PAGE_MONITOR			0x4
#define PAGE_SETTINGS			0x5
#define PAGE_CALIBRATE			0x6
#define PAGE_WLAN				0x7
#define PAGE_DOSIMETER			0x8
#define PAGE_AIRQ				0x9

#define ID_YES 					0x1
#define ID_NO 					0x2

// define a few unique IDs for the controls in our user interface
#define ID_BUTTON_MEASURE 		0x1
#define ID_BUTTON_MONITOR 		0x2
#define ID_BUTTON_SETTINGS 		0x3
#define ID_BUTTON_SHUTDOWN		0x4
#define ID_BUTTON_DOSIMETER		0x5
#define ID_BUTTON_AIRQ			0x6
#define ID_BUTTON_BACK			0x7
#define ID_BUTTON_MUTE			0x8
#define ID_BUTTON_CALIBRATE		0x9
#define ID_BUTTON_WLAN			0x10

#define	ID_BUTTON_WLAN_START	0x50
#define	ID_BUTTON_WLAN_STOP		0x60



class UI {
private:
	ILI9341 				*m_lcd;
	TouchScreen 			*m_touch;
	uint16_t 				m_touchX, m_touchY, m_touchZ;		// last coordinates received
	Button 					m_buts[7];
	ImgButton				m_ibuts[7];
	Image					m_images[6] = {
								Image(icon_rad_32x32, 32, 32),
								Image(icon_airq_32x32, 32, 32),
								Image(icon_measure_32x32, 32, 32),
								Image(icon_monitor_32x32, 32, 32),
								Image(icon_settings_32x32, 32, 32),
								Image(icon_shutdown_32x32, 32, 32)
							};
	uint8_t					m_butCount,
							m_ibutCount,
							m_chartX;
	// max buttons we'll use on one page
	bool 					popupVisible;
	Data					*m_data;
	uint16_t 				m_page;								// id of current page

	// helper to split a few buttons on screen evenly
	uint16_t getButtonY(uint16_t offset, uint8_t index, uint16_t height, uint8_t total) {
		uint16_t spacer = (m_lcd->getHeight() - offset - height * total) / (total + 1);
		return offset + spacer * (index + 1) + height * index;
	}

	// various pages
	bool pageInit();
	void pageMain();
	void pageDosimeter();
	void pageAirQ();
	void pageMeasure();
	void pageMonitor();
	void pageSettings();
	void pageCalibrate();
	void pageWLAN();

public:
	UI(ILI9341 *lcd, TouchScreen *touch, Data *data) {
		m_lcd = lcd;
		m_touch = touch;
		m_touchX = 0;
		m_touchY = 0;
		m_touchZ = 0;
		popupVisible = false;
		m_data = data;
		m_chartX = 120;

	}

	void showLogo();

	// checks touch press and then all controls on current page, returning the id of pressed one or zero
	uint16_t readTouchEvent();




	// get last coordinates retrieved from the touchscreen after readTouchEvent() function was called
	void getLastTouch(uint16_t *x, uint16_t *y, uint16_t *z);

	uint16_t showYesNoPopup(char *text);

	void drawTitlebar(uint8_t time_h, uint8_t time_m, uint8_t time_s, uint8_t bat);
	// general purpose return code, true by default, false in case of some failure
	bool drawPage(uint16_t id);

	void updateTitlebar();
	void updateValues();
};
