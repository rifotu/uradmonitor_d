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

#include "button.h"

void Button::create(ILI9341 *lcd, uint16_t x, uint16_t y, uint16_t w, uint16_t h, char *text, uint8_t size, State state, uint16_t id) {
	m_lcd = lcd;
	m_x = x; m_y = y; m_w = w; m_h = h;
	m_size = size; m_id = id;
	strncpy(m_text, text, MAX_TEXT_LENGTH);
	draw(state);
}

void Button::toggle() {
	switch (m_state) {
		case BUTTON_PRESSED:
			draw(BUTTON_UNPRESSED);
			break;
		case BUTTON_UNPRESSED:
			draw(BUTTON_PRESSED);
			break;
	}
}
void Button::draw(State state) {
	m_state = state;
	uint8_t len = strlen(m_text), offset = 0;
	uint16_t color_button , color_text, color_f1, color_f2;
	switch (state) {
		case BUTTON_DISABLED: {
			color_button = DARKGREY;
			color_text = LIGHTGREY;
			color_f1 = LIGHTBLACK;
			color_f2 = LIGHTBLACK;
		} break;
		case BUTTON_UNPRESSED: {
			color_button = MEDGREY;
			color_text = WHITE;
			color_f1 = DARKWHITE;
			color_f2 = LIGHTBLACK;
		} break;
		case BUTTON_PRESSED: {
			color_button = MEDGREY;
			color_text = WHITE;
			color_f1 = LIGHTBLACK;
			color_f2 = DARKWHITE;
			offset = 1;
		} break;
	}

	m_lcd->drawRectFilled(m_x, m_y, m_w, m_h, color_button);
	m_lcd->drawLine(m_x, m_y, m_x + m_w, m_y, color_f1);
	m_lcd->drawLine(m_x + m_w, m_y, m_x + m_w, m_y + m_h, color_f2);
	m_lcd->drawLine(m_x, m_y + m_h, m_x + m_w, m_y + m_h, color_f2);
	m_lcd->drawLine(m_x, m_y, m_x, m_y + m_h, color_f1);
	m_lcd->drawString(m_x + (m_w - len * m_size * FONT_WIDTH) / 2 + offset , m_y + (m_h - m_size * FONT_HEIGHT) / 2 + offset, m_size, color_text, TRANSPARENT, m_text);
}

bool Button::isPressed(uint16_t x, uint16_t y) {
	if (m_state == BUTTON_DISABLED)
		return false;
	else
		return (x >= m_x && x < m_x + m_w && y >= m_y && y < m_y + m_h);
}

uint16_t Button::getId() {
	return m_id;
}
