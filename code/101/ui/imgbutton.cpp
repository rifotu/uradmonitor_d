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

#include "imgbutton.h"

void ImgButton::create(ILI9341 *lcd, uint16_t x, uint16_t y, uint16_t w, uint16_t h, Image *image, char *text, uint8_t size, State state, uint16_t id) {
	m_lcd = lcd;
	m_x = x; m_y = y; m_w = w; m_h = h;
	m_size = size; m_id = id;
	m_image = image;
	strncpy(m_text, text, MAX_TEXT_LENGTH);
	draw(state);
}

void ImgButton::toggle() {
	switch (m_state) {
		case BUTTON_PRESSED:
			draw(BUTTON_UNPRESSED);
			break;
		case BUTTON_UNPRESSED:
			draw(BUTTON_PRESSED);
			break;
	}
}
void ImgButton::draw(State state) {
	m_state = state;
	uint16_t color_button , color_text;
	switch (state) {
		case BUTTON_DISABLED: {
			color_button = BLACK;
			color_text = BLACK_L3;
		} break;
		case BUTTON_UNPRESSED: {
			color_button = BLACK;
			color_text = WHITE;
		} break;
		case BUTTON_PRESSED: {
			color_button = BLACK;
			color_text = DARKGREY;

		} break;
	}
	uint16_t imageHeight = m_image!=NULL? m_image->getHeight() : 0;

	uint16_t sp = (m_h - m_size * imageHeight - m_size * FONT_HEIGHT ) /3;
	m_lcd->drawRectFilled(m_x, m_y, m_w, m_h, color_button);

	if (m_image != NULL)
		m_image->draw(m_lcd, m_x + (m_w - m_size * m_image->getWidth()) / 2 , m_y + sp, m_size);

	m_lcd->drawString(m_x + (m_w - strlen(m_text) * m_size * FONT_WIDTH) / 2  , m_y + 2 * sp + m_size * imageHeight, m_size, color_text, TRANSPARENT, m_text);
}

bool ImgButton::isPressed(uint16_t x, uint16_t y) {
	if (m_state == BUTTON_DISABLED)
		return false;
	else
		return (x >= m_x && x < m_x + m_w && y >= m_y && y < m_y + m_h);
}

uint16_t ImgButton::getId() {
	return m_id;
}
