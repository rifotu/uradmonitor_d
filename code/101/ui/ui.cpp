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

#include "ui.h"

void UI::showLogo() {

}

uint16_t UI::readTouchEvent() {

	if (m_touch->read(&m_touchX, &m_touchY , &m_touchZ)) {
		for (int i=0;i<m_butCount;i++)
			if (m_buts[i].isPressed(m_touchX, m_touchY)) {
				//m_buts[i].draw(Button::BUTTON_PRESSED);
				m_buts[i].toggle();
				// valid click on button
				return m_buts[i].getId();
			}
		// valid click elsewhere on the screen
		return 0xFFFF;
	} else
		return 0;
}

void UI::getLastTouch(uint16_t *x, uint16_t *y, uint16_t *z) {
	*x = m_touchX;
	*y = m_touchY;
	*z = m_touchZ;
}

uint16_t UI::showYesNoPopup(char *text) {
	if (!popupVisible) {
		m_lcd->drawRectFilled(0,80, 240, 160, DARKGREY);
		m_lcd->drawString(CENTERX, 120, 2, WHITE, TRANSPARENT, text);
		m_butCount = 0;
		m_buts[m_butCount++].create(m_lcd, 10, 180, 100, 30, "YES", 2, Button::BUTTON_UNPRESSED, ID_YES);
		m_buts[m_butCount++].create(m_lcd, 130, 180, 100, 30, "NO", 2, Button::BUTTON_UNPRESSED, ID_NO);
		popupVisible = true;
	}
	if (m_touch->read(&m_touchX, &m_touchY , &m_touchZ)) {
		for (int i=0;i<m_butCount;i++)
			if (m_buts[i].isPressed(m_touchX, m_touchY)) {
				popupVisible = false;
				m_buts[i].draw(Button::BUTTON_PRESSED);
				return m_buts[i].getId();
			}
	}
	return 0;
}

// general purpose return code, true by default, false in case of some failure
bool UI::drawPage(uint16_t id) {
	m_page = id;
	m_butCount = 0;
	switch (id) {
		case PAGE_INIT: return pageInit();
		case PAGE_MAIN: pageMain(); break;
		case PAGE_MEASURE: pageMeasure(); break;
		case PAGE_MONITOR: pageMonitor(); break;
		case PAGE_SETTINGS: pageSettings(); break;
		case PAGE_CALIBRATE: pageCalibrate(); break;
		case PAGE_WLAN: pageWLAN(); break;
	}
	return true;
}

void UI::drawTitlebar(uint8_t time_h, uint8_t time_m, uint8_t time_s, uint8_t bat) {
	m_lcd->drawStringF(0, 0, 2, WHITE, LIGHTBLACK, "%02d:%02d:%02d %c%c%c%c%c%c %3d%%",
			time_h, time_m, time_s, 0xB0,0xB0,0xB0,0xB0,0xB0,0xB0,bat);
	switch (cmdAlarm) {
	case ALARM_RADIATION:
		m_lcd->drawString(120, 0, 2, WHITE, RED, "RAD!");
		break;
	}
}

bool UI::pageInit() {
	m_lcd->drawClear(BLACK);

	m_lcd->drawImageRGB16(20, 100, img_logo100x60, 100, 60, 2);
	m_lcd->drawString(CENTERX, textY(28,1), 1, WHITE, TRANSPARENT, STRING_NAME);
	m_lcd->drawString(CENTERX, textY(29,1), 1, LIGHTGREY, TRANSPARENT, STRING_WEB);


	uint8_t start = 31;
	m_lcd->drawString(CENTERX, textY(start++, 1), 1, WHITE, TRANSPARENT, STRING_SELFCHECK);
	m_lcd->drawStringF(CENTERX, textY(start++, 1), 1, LIGHTGREY, TRANSPARENT, "DeviceID %08lX", deviceID);
	m_lcd->drawStringF(CENTERX, textY(start++, 1), 1, WHITE, TRANSPARENT, "Type:%X HW:%u SW:%u %s", DEV_MODEL, VER_HW, VER_SW, aux_detectorName(DEV_RAD_DETECTOR));
	float voltageBat = readBatVoltage();
	m_lcd->drawStringF(CENTERX, textY(start++, 1), 1, WHITE, TRANSPARENT, STRING_BATVOLTAGE1, voltageBat);
	if (voltageBat < BATTERY_LIMIT) {
		m_lcd->drawString (CENTERX, textY(start++, 1), 1, RED, TRANSPARENT, STRING_BATDISCHARGED);
		return false;
	}
	m_lcd->drawStringF(CENTERX, textY(start++, 1), 1, WHITE,	TRANSPARENT, STRING_STARTUPDONE, time.getInterval());
	return true;
}

void UI::pageMain() {
	m_lcd->drawClear(WHITE);
	m_buts[m_butCount++].create(m_lcd, 60, getButtonY(16, 0, 30, 4), 120, 30, "MEASURE", 2, Button::BUTTON_UNPRESSED, ID_BUTTON_MEASURE);
	m_buts[m_butCount++].create(m_lcd, 60, getButtonY(16, 1, 30, 4), 120, 30, "MONITOR", 2, Button::BUTTON_UNPRESSED, ID_BUTTON_MONITOR);
	m_buts[m_butCount++].create(m_lcd, 60, getButtonY(16, 2, 30, 4), 120, 30, "SETTINGS", 2, Button::BUTTON_UNPRESSED, ID_BUTTON_SETTINGS);
	m_buts[m_butCount++].create(m_lcd, 60, getButtonY(16, 3, 30, 4), 120, 30, "SHUTDOWN", 2, Button::BUTTON_UNPRESSED, ID_BUTTON_SHUTDOWN);
}

void UI::pageMeasure() {
	m_lcd->drawClear(WHITE);
	m_buts[m_butCount++].create(m_lcd, 60, 270, 120, 30, "BACK", 2, Button::BUTTON_UNPRESSED, ID_BUTTON_BACK);
	m_lcd->drawString(8,  32, 1, BLUE, WHITE, "Temperature");
	m_lcd->drawString(8,  64, 1, BLUE, WHITE, "Pressure");
	//m_lcd->drawString(8,  96, 1, BLUE, WHITE, "Altitude");
	m_lcd->drawString(8,  96, 1, BLUE, WHITE, "Humidity");
	m_lcd->drawString(8, 128, 1, PURPLE, WHITE, "Dust");
	m_lcd->drawString(8, 160, 1, DARKGREEN, WHITE, "CO2");
	m_lcd->drawString(8, 192, 1, DARKGREEN, WHITE, "VOC");
	m_lcd->drawString(8, 224, 1, RED, WHITE, "Radiation \xDF\xE0\xE1");
	m_chartX = 120; // start charts from initial position
}

void UI::pageMonitor() {
	m_lcd->drawClear(WHITE);
	m_buts[m_butCount++].create(m_lcd, 60, 270, 120, 30, "BACK", 2, Button::BUTTON_UNPRESSED, ID_BUTTON_BACK);
	m_lcd->drawString(0, 32, 1, BLACK, TRANSPARENT, "While in this mode, data is being sent\nover the USB connection (9600bps).\nIf WLAN is connected, the data also\ngoes to the uRADMonitor server,\nwww.uradmonitor.com");
	m_lcd->drawString(8,  88, 2, BLUE, TRANSPARENT, "Serial");
	m_lcd->drawString(8,  152, 2, DARKGREEN, TRANSPARENT, "WLAN");
}

void UI::pageSettings() {
	m_lcd->drawClear(WHITE);
	m_lcd->drawStringF(CENTERX,  32, 2, BLACK, TRANSPARENT, "Timeout: %ds", BACKLIGHT_TIMEOUT);
	m_lcd->drawString(CENTERX,  48, 2, BLACK, TRANSPARENT, "Baudrate: 9600bps");

	m_buts[m_butCount++].create(m_lcd, 60, getButtonY(64, 0, 30, 4), 120, 30, "MUTE SND", 2, isMuted?Button::BUTTON_PRESSED:Button::BUTTON_UNPRESSED, ID_BUTTON_MUTE);
	m_buts[m_butCount++].create(m_lcd, 60, getButtonY(64, 1, 30, 4), 120, 30, "CALIBRATE", 2, Button::BUTTON_UNPRESSED, ID_BUTTON_CALIBRATE);
	m_buts[m_butCount++].create(m_lcd, 60, getButtonY(64, 2, 30, 4), 120, 30, "WLAN", 2, Button::BUTTON_UNPRESSED, ID_BUTTON_WLAN);
	m_buts[m_butCount++].create(m_lcd, 60, getButtonY(64, 3, 30, 4), 120, 30, "BACK", 2, Button::BUTTON_UNPRESSED, ID_BUTTON_BACK);
}

void UI::pageCalibrate() {
	m_lcd->drawClear(WHITE);
	m_lcd->drawString(CENTERX,  32, 2, BLACK, TRANSPARENT, "Debug screen");
	m_buts[m_butCount++].create(m_lcd, 60, 270, 120, 30, "BACK", 2, Button::BUTTON_UNPRESSED, ID_BUTTON_BACK);
}

void UI::pageWLAN() {
	m_lcd->drawClear(WHITE);
	// do we have APs discovered?
	if (m_data->freeAPCount) {
		for (uint8_t i=0;i<m_data->freeAPCount;i++)
			m_buts[m_butCount++].create(
					m_lcd, 60, getButtonY(16, i, 30, m_data->freeAPCount + 1), 120, 30, m_data->freeAPList[i], 1, Button::BUTTON_UNPRESSED, ID_BUTTON_WLAN_START + i);

		m_buts[m_butCount++].create(m_lcd, 60, getButtonY(16, m_data->freeAPCount, 30, m_data->freeAPCount + 1), 120, 30, "BACK", 2, Button::BUTTON_UNPRESSED, ID_BUTTON_BACK);

	} else {
		m_lcd->drawString(CENTERX,  32, 2, BLACK, TRANSPARENT, "Discovering WLANs...");
		m_buts[m_butCount++].create(m_lcd, 60, 270, 120, 30, "BACK", 2, Button::BUTTON_UNPRESSED, ID_BUTTON_BACK);
	}

}
void UI::updateValues() {
	drawTitlebar(m_data->time_hour, m_data->time_minute, m_data->time_second, batteryPercentage(m_data->battery_voltage));
	m_lcd->drawStringF(140,16, 2, RED, WHITE, "%2.2f %d  ", m_data->battery_voltage, m_data->geiger_voltage);

	// debug
	//m_lcd->drawStringF(0,312, 1, RED, WHITE, "%2.2f %d  ", m_data->battery_voltage, m_data->geiger_voltage);

	switch (m_page) {
		case PAGE_MEASURE: {
			m_lcd->drawStringF(8,  40, 2, BLUE, WHITE, "%2.2f%cC", m_data->bme280_temp,247);
			m_lcd->drawStringF(8,  72, 2, BLUE, WHITE, "%4uhPa", m_data->bme280_pressure / 100);
			//m_lcd->drawStringF(8, 104, 2, BLUE, WHITE, "%4.2fm", m_data->bme280_altitude);
			m_lcd->drawStringF(8, 104, 2, BLUE, WHITE, "%03uRH", m_data->bme280_humi);
			m_lcd->drawStringF(8, 136, 2, PURPLE, WHITE, "%2.2fmgm%c", m_data->gp2y10_dust, 254);
			m_lcd->drawStringF(8, 168, 2, DARKGREEN, WHITE, "%4.2fppm", m_data->vz89_co2);
			m_lcd->drawStringF(8, 200, 2, DARKGREEN, WHITE, "%4.2fppb", m_data->vz89_voc);
			m_lcd->drawStringF(8, 232, 2, RED, WHITE, "%luCPM", m_data->geiger_cpm);

			float bme280_temp = m_data->getBME280Temp_scaled(),
					bme280_altitude = m_data->getBME280Altitude_scaled(),
					gp2y10_dust = m_data->getGP2Y10Dust_scaled(),
					vz89_co2 = m_data->getVZ89CO2_scaled() ,
					vz89_voc = m_data->getVZ89VOC_scaled();
			uint32_t bme280_pressure = m_data->getBME280Pressure_scaled(),
			 		geiger_cpm = m_data->getGeigerCPM_scaled();
			uint8_t bme280_humi = m_data->getBME280Humi_scaled();

			if (m_chartX == 120) {
				m_lcd->drawPixel(m_chartX, 64 - bme280_temp, BLUE);
				m_lcd->drawPixel(m_chartX, 96 - bme280_pressure, BLUE);
				//m_lcd->drawPixel(m_chartX,128 - bme280_altitude, BLUE);
				m_lcd->drawPixel(m_chartX,128 - bme280_humi, BLUE);
				m_lcd->drawPixel(m_chartX,160 - gp2y10_dust, PURPLE);
				m_lcd->drawPixel(m_chartX,192 - vz89_co2, DARKGREEN);
				m_lcd->drawPixel(m_chartX,224 - vz89_voc, DARKGREEN);
				m_lcd->drawPixel(m_chartX,256 - geiger_cpm, RED);
			} else {
				m_lcd->drawLine(m_chartX, 64 - m_data->bme280_temp_last, m_chartX, 64 - bme280_temp, BLUE);
				m_lcd->drawLine(m_chartX, 96 - m_data->bme280_pressure_last, m_chartX, 96 - bme280_pressure, BLUE);
				//m_lcd->drawLine(m_chartX, 128 - m_data->bme280_altitude_last, m_chartX, 128 - bme280_altitude, BLUE);
				m_lcd->drawLine(m_chartX, 128 - m_data->bme280_humi_last, m_chartX, 128 - bme280_humi, BLUE);
				m_lcd->drawLine(m_chartX, 160 - m_data->gp2y10_dust_last, m_chartX, 160 - gp2y10_dust, PURPLE);
				m_lcd->drawLine(m_chartX, 192 - m_data->vz89_co2_last, m_chartX, 192 - vz89_co2, DARKGREEN);
				m_lcd->drawLine(m_chartX, 224 - m_data->vz89_voc_last, m_chartX, 224 - vz89_voc, DARKGREEN);
				m_lcd->drawLine(m_chartX, 256 - m_data->geiger_cpm_last, m_chartX, 256 - geiger_cpm, RED);
			}

			if (m_chartX < 240) {
				m_chartX ++;
				// clear in front
				m_lcd->drawRectFilled(m_chartX, 32, 1, 225, WHITE);
				// save last values
				m_data->bme280_temp_last = bme280_temp;
				m_data->bme280_pressure_last = bme280_pressure;
				//m_data->bme280_altitude_last = bme280_altitude;
				m_data->bme280_humi_last = bme280_humi;
				m_data->gp2y10_dust_last = gp2y10_dust;
				m_data->vz89_co2_last = vz89_co2;
				m_data->vz89_voc_last = vz89_voc;
				m_data->geiger_cpm_last = geiger_cpm;

			}
			else
				m_chartX  = 120;
		}
		break;
		case PAGE_MONITOR: {
			m_lcd->drawStringF(8,  104, 2, BLUE, WHITE,      "Sent:    %09luB", m_data->serial_sent);
			m_lcd->drawStringF(8,  120, 2, BLUE, WHITE,      "Received:%09luB", m_data->serial_recv);
			m_lcd->drawStringF(8,  168, 2, DARKGREEN, WHITE, "Sent:    %09luB", m_data->wlan_sent);
			m_lcd->drawStringF(8,  184, 2, DARKGREEN, WHITE, "Received:%09luB", m_data->wlan_recv);
			m_lcd->drawStringF(8,  216, 2, BLACK, WHITE, "Tube: %3uV", m_data->geiger_voltage);
			m_lcd->drawStringF(8,  232, 2, BLACK, WHITE, "Battery: %2.2fV", m_data->battery_voltage);
		} break;
		case PAGE_SETTINGS: {
			// nothing to update here
		} break;
		case PAGE_CALIBRATE: {
			m_lcd->drawStringF(CENTERX,  CENTERY, 2, BLUE, WHITE, "%04d,%04d,%04d", m_touch->lastRawX, m_touch->lastRawY, m_touch->lastRawZ);
			m_lcd->drawPixel(m_touchX, m_touchY,3, RED);
		}
		break;
		case PAGE_WLAN: {
			if (m_butCount != m_data->freeAPCount + 1) // count the back button
				drawPage(PAGE_WLAN);
		}
	}
}
