/*
** Buttons Library for AVR microcontrollers
** Copyright (C) 2009 - 2015 Radu Motisan, radu.motisan@gmail.com
** A basic UI button implementation designed for embedded systems
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

#include <string.h>
#include "../ili9341/ili9341.h"
#include "../touch_resistive/TouchScreen.h"

#define MAX_TEXT_LENGTH 20
class Button {
public:
    enum State {
    	BUTTON_DISABLED = -1,
    	BUTTON_UNPRESSED = 0,
    	BUTTON_PRESSED = 1
    };
private:
	ILI9341 *m_lcd;
	uint8_t m_size;
	uint16_t m_x, m_y, m_w, m_h;
	uint16_t m_id;
	State m_state;
	char m_text[MAX_TEXT_LENGTH];
public:
	void create(ILI9341 *lcd, uint16_t x, uint16_t y, uint16_t w, uint16_t h, char *text, uint8_t size, State state, uint16_t id);

	void draw(State state);
	void toggle();
	bool isPressed(uint16_t x, uint16_t y) ;

	uint16_t getId();
};
