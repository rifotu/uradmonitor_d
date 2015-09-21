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
		for (int i=0;i<m_ibutCount;i++)
			if (m_ibuts[i].isPressed(m_touchX, m_touchY)) {
				m_ibuts[i].toggle();
				// valid click on button
				return m_ibuts[i].getId();
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
	m_butCount = 0; m_ibutCount = 0;
	switch (id) {
		case PAGE_INIT: return pageInit();
		case PAGE_MAIN: pageMain(); break;
		case PAGE_DOSIMETER: pageDosimeter(); break;
		case PAGE_AIRQ: pageAirQ(); break;
		case PAGE_MEASURE: pageMeasure(); break;
		case PAGE_MONITOR: pageMonitor(); break;
		case PAGE_SETTINGS: pageSettings(); break;
		case PAGE_CALIBRATE: pageCalibrate(); break;
		case PAGE_WLAN: pageWLAN(); break;
	}
	return true;
}

void UI::drawTitlebar(uint8_t time_h, uint8_t time_m, uint8_t time_s, uint8_t bat) {
	m_lcd->drawStringF(0, 0, 2, WHITE, BLACK_L3, "%02d:%02d:%02d %c%c%c%c%c%c %3d%%",
			time_h, time_m, time_s, 0xB0,0xB0,0xB0,0xB0,0xB0,0xB0,bat);
	switch (cmdAlarm) {
	case ALARM_RADIATION:
		m_lcd->drawString(108, 0, 2, WHITE, RED, " RAD! ");
		break;
	case ALARM_VOC:
		m_lcd->drawString(108, 0, 2, WHITE, GREEN, " VOC! ");
		break;
	case ALARM_CO2:
		m_lcd->drawString(108, 0, 2, WHITE, GREEN, " CO2! ");
		break;
	case ALARM_DUST:
		m_lcd->drawString(108, 0, 2, WHITE, PURPLE, "DUST! ");
		break;
	case ALARM_TEMP:
		m_lcd->drawString(108, 0, 2, WHITE, BLUE, "TEMP! ");
		break;
	case ALARM_HUMI:
		m_lcd->drawString(108, 0, 2, WHITE, BLUE, "HUMI! ");
		break;
	case ALARM_PRESSURE:
		m_lcd->drawString(108, 0, 2, WHITE, BLUE, " ATM! ");
		break;
	}

}

bool UI::pageInit() {
	m_lcd->drawClear(BLACK);
	//m_lcd->drawImageRGB16(20, 100, img_logo100x60, 100, 60, 2);
	m_lcd->drawString (CENTERX, textY(28,1), 1, WHITE, TRANSPARENT, STRING_NAME);
	m_lcd->drawString (CENTERX, textY(29,1), 1, LIGHTGREY, TRANSPARENT, STRING_WEB);
	m_lcd->drawString (CENTERX, textY(31, 1), 1, WHITE, TRANSPARENT, STRING_SELFCHECK);
	m_lcd->drawStringF(CENTERX, textY(32, 1), 1, LIGHTGREY, TRANSPARENT, "DeviceID %08lX", deviceID);
	m_lcd->drawStringF(CENTERX, textY(33, 1), 1, WHITE, TRANSPARENT, "Type:%X HW:%u SW:%u %s", DEV_MODEL, VER_HW, VER_SW, aux_detectorName(DEV_RAD_DETECTOR));
	m_lcd->drawStringF(CENTERX, textY(34, 1), 1, WHITE,	TRANSPARENT, STRING_STARTUPDONE, time.getInterval());
	return true;
}

void UI::pageDosimeter() {
	m_lcd->drawClear(BLACK);
	m_ibuts[m_ibutCount++].create(m_lcd, 60, 18, 120, 100, &m_images[0], "RAD", 2, ImgButton::BUTTON_UNPRESSED, ID_BUTTON_MEASURE);

	m_lcd->drawString(180, 142, 2, BLUE_L1, BLACK, "uSv/h");
	//dose * getDoseMulFactor(dose), getDoseMulSym(dose)

	m_chartX = 0;
	m_lcd->drawRectFilled(0, 188, 240, 90, BLACK_L1);

	m_buts[m_butCount++].create(m_lcd, 60, 285, 120, 30, "BACK", 2, Button::BUTTON_UNPRESSED, ID_BUTTON_BACK);
}

void UI::pageAirQ() {
	m_lcd->drawClear(BLACK);
	m_ibuts[m_ibutCount++].create(m_lcd, 60, 18, 120, 100, &m_images[1], "Air-Q", 2, ImgButton::BUTTON_UNPRESSED, ID_BUTTON_MEASURE);

	m_chartX = 0;
	m_lcd->drawRectFilled(0, 188, 240, 90, BLACK_L1);
	m_buts[m_butCount++].create(m_lcd, 60, 285, 120, 30, "BACK", 2, Button::BUTTON_UNPRESSED, ID_BUTTON_BACK);
}

void UI::pageMain() {
	m_lcd->drawClear(BLACK);
	m_ibuts[m_ibutCount++].create(m_lcd, 0, 18, 120, 100, &m_images[0], "RAD", 2, ImgButton::BUTTON_UNPRESSED, ID_BUTTON_DOSIMETER);
	m_ibuts[m_ibutCount++].create(m_lcd, 120, 18, 120, 100, &m_images[1], "Air-Q", 2, ImgButton::BUTTON_UNPRESSED, ID_BUTTON_AIRQ);
	m_ibuts[m_ibutCount++].create(m_lcd, 0, 118, 120, 100, &m_images[2], "Measure", 2, ImgButton::BUTTON_UNPRESSED, ID_BUTTON_MEASURE);
	m_ibuts[m_ibutCount++].create(m_lcd, 120, 118, 120, 100, &m_images[3], "Monitor", 2, ImgButton::BUTTON_UNPRESSED, ID_BUTTON_MONITOR);
	m_ibuts[m_ibutCount++].create(m_lcd, 0, 218, 120, 100, &m_images[4], "Settings", 2, ImgButton::BUTTON_UNPRESSED, ID_BUTTON_SETTINGS);
	m_ibuts[m_ibutCount++].create(m_lcd, 120, 218, 120, 100, &m_images[5], "Shutdown", 2, ImgButton::BUTTON_UNPRESSED, ID_BUTTON_SHUTDOWN);
}

void UI::pageMeasure() {
	// clean background
	m_lcd->drawRectFilled(0, 16, 240, 16, BLACK);
	for (int i=0;i<7;i++) {
		m_lcd->drawRectFilled(0, 32 + 32 * i, 240, 32, (i%2 == 0)?BLACK_L1:BLACK);
	}
	m_lcd->drawRectFilled(0, 256, 240, 64, BLACK);

	m_buts[m_butCount++].create(m_lcd, 60, 285, 120, 30, "BACK", 2, Button::BUTTON_UNPRESSED, ID_BUTTON_BACK);


	m_lcd->drawString(8,  32 + 1, 1, BLUE_L1, TRANSPARENT, "Temperature");
	m_lcd->drawString(8,  64 + 1, 1, BLUE_L1, TRANSPARENT, "Pressure");
	m_lcd->drawString(8,  96 + 1, 1, BLUE_L1, TRANSPARENT, "Humidity");
	m_lcd->drawString(8, 128 + 1, 1, PURPLE_L1, TRANSPARENT, "Dust");
	m_lcd->drawString(8, 160 + 1, 1, GREEN_L1, TRANSPARENT, "CO2");
	m_lcd->drawString(8, 192 + 1, 1, GREEN_L1, TRANSPARENT, "VOC");
	m_lcd->drawString(8, 224 + 1, 1, RED, TRANSPARENT, "Radiation \xDF\xE0\xE1");
	m_chartX = 120; // start charts from initial position
}

void UI::pageMonitor() {
	m_lcd->drawClear(BLACK);
	m_buts[m_butCount++].create(m_lcd, 60, 285, 120, 30, "BACK", 2, Button::BUTTON_UNPRESSED, ID_BUTTON_BACK);
	m_lcd->drawString(0, 32, 1, WHITE, TRANSPARENT, "While in this mode, data is being sent\nover the USB connection (9600bps).\nIf WLAN is connected, the data also\ngoes to the uRADMonitor server,\nwww.uradmonitor.com");
	m_lcd->drawString(8,  88, 2, BLUE_L1, TRANSPARENT, "Serial");
	m_lcd->drawString(8,  152, 2, GREEN_L1, TRANSPARENT, "WLAN");
}

void UI::pageSettings() {
	m_lcd->drawClear(BLACK);
	m_lcd->drawStringF(CENTERX,  32, 2, WHITE, TRANSPARENT, "Timeout: %ds", BACKLIGHT_TIMEOUT);
	m_lcd->drawString(CENTERX,  48, 2, WHITE, TRANSPARENT, "Baudrate: 9600bps");

	m_buts[m_butCount++].create(m_lcd, 60, getButtonY(64, 0, 30, 4), 120, 30, "MUTE SND", 2, isMuted?Button::BUTTON_PRESSED:Button::BUTTON_UNPRESSED, ID_BUTTON_MUTE);
	m_buts[m_butCount++].create(m_lcd, 60, getButtonY(64, 1, 30, 4), 120, 30, "CALIBRATE", 2, Button::BUTTON_UNPRESSED, ID_BUTTON_CALIBRATE);
	m_buts[m_butCount++].create(m_lcd, 60, getButtonY(64, 2, 30, 4), 120, 30, "WLAN", 2, Button::BUTTON_UNPRESSED, ID_BUTTON_WLAN);
	m_buts[m_butCount++].create(m_lcd, 60, getButtonY(64, 3, 30, 4), 120, 30, "BACK", 2, Button::BUTTON_UNPRESSED, ID_BUTTON_BACK);
}

void UI::pageCalibrate() {
	m_lcd->drawClear(BLACK);
	m_lcd->drawString(CENTERX,  32, 2, WHITE, TRANSPARENT, "Debug screen");
	m_buts[m_butCount++].create(m_lcd, 60, 285, 120, 30, "BACK", 2, Button::BUTTON_UNPRESSED, ID_BUTTON_BACK);
}

void UI::pageWLAN() {
	m_lcd->drawClear(BLACK);
	// do we have APs discovered?
	if (m_data->freeAPCount) {
		for (uint8_t i=0;i<m_data->freeAPCount;i++)
			m_buts[m_butCount++].create(
					m_lcd, 60, getButtonY(16, i, 30, m_data->freeAPCount + 1), 120, 30, m_data->freeAPList[i], 1, Button::BUTTON_UNPRESSED, ID_BUTTON_WLAN_START + i);

		m_buts[m_butCount++].create(m_lcd, 60, getButtonY(16, m_data->freeAPCount, 30, m_data->freeAPCount + 1), 120, 30, "BACK", 2, Button::BUTTON_UNPRESSED, ID_BUTTON_BACK);

	} else {
		m_lcd->drawString(CENTERX,  32, 2, WHITE, TRANSPARENT, "Discovering WLANs...");
		m_buts[m_butCount++].create(m_lcd, 60, 285, 120, 30, "BACK", 2, Button::BUTTON_UNPRESSED, ID_BUTTON_BACK);
	}

}

void UI::updateTitlebar() {
	drawTitlebar(m_data->time_hour, m_data->time_minute, m_data->time_second, batteryPercentage(m_data->battery_voltage));
}

void UI::updateValues() {

	switch (m_page) {
		case PAGE_DOSIMETER: {
			float dose = CPM2uSVh((uint8_t)DEV_RAD_DETECTOR, m_data->geiger_cpm);

			m_lcd->drawStringF(0, 120, 5, BLUE_L1, BLACK, "%06.02f", dose );

			if (dose < DOSE_THRESHOLD_LOW)
				m_lcd->drawString(CENTERX, 160 + 2, 3, GREEN, BLACK, "  LOW  ");
			else if (dose < DOSE_THRESHOLD_NORMAL)
				m_lcd->drawString(CENTERX, 160 + 2, 3, GREEN_L1, BLACK, "NORMAL");
			else if (dose < DOSE_THRESHOLD_HIGH)
				m_lcd->drawString(CENTERX, 160 + 2, 3, ORANGE, BLACK, " HIGH ");
			else
				m_lcd->drawString(CENTERX, 160 + 2, 3, RED, BLACK, "DANGER");

			// chart at 160..250
			uint32_t geiger_cpm = m_data->getGeigerCPM_scaled(88);

			if (m_chartX == 0) {
				m_lcd->drawPixel(m_chartX,278 - geiger_cpm - 1, RED);
			} else {
				m_lcd->drawLine(m_chartX, 278 - m_data->geiger_cpm_last - 1, m_chartX, 278 - geiger_cpm - 1, RED);
			}
			if (m_chartX < 239) {
				m_chartX ++;
				// clear in front
				m_lcd->drawRectFilled(m_chartX, 188, 2, 90, BLACK_L1);
				m_data->geiger_cpm_last = geiger_cpm;
			}
			else
				m_chartX  = 0;

		} break;
		case PAGE_AIRQ: {

			// compute air quality scale score: 0 good, 100 bad
			uint8_t score1 = (uint8_t)(m_data->vz89_voc / 10.0),
					score2 = (uint8_t)(m_data->gp2y10_dust * 222.22),
					score3 = (uint8_t)((m_data->vz89_co2 - 400) / 160.0),
					score4 = (uint8_t)(abs(50 - m_data->bme280_humi) * 2);

			float score = log10((60 * score1 + 30 * score2 + 5* score3 + 5*score4) / 1111.1111 + 1) ;

			// the smallest the better
			m_lcd->drawStringF(CENTERX, 120, 5, BLUE_L1, BLACK, "%05.02f",  score );

			if (score < 0.20)
				m_lcd->drawString(CENTERX, 160 + 2, 3, GREEN, BLACK, "  CLEAN  ");
			else if (score < 0.40)
				m_lcd->drawString(CENTERX, 160 + 2, 3, GREEN_L1, BLACK, " NORMAL ");
			else if (score < 0.50)
				m_lcd->drawString(CENTERX, 160 + 2, 3, ORANGE, BLACK, "POLLUTED");
			else
				m_lcd->drawString(CENTERX, 160 + 2, 3, RED, BLACK, " DANGER ");

			m_lcd->drawString(4, 32, 2, BLUE_L1, BLACK, "VOC");
			m_lcd->drawStringF(4, 48, 1, BLUE_L1, BLACK, "%4.2fppb", m_data->vz89_voc);

			m_lcd->drawString(4, 64, 2, PURPLE_L1, BLACK, "DUST");
			m_lcd->drawStringF(4, 80, 1, PURPLE_L1, BLACK, "%2.2fmgm%c", m_data->gp2y10_dust, 254);

			m_lcd->drawString(152 + 48, 32, 2, YELLOW, BLACK, "CO2");
			m_lcd->drawStringF(152 + 24, 48, 1, YELLOW, BLACK, "%4.2fppm", m_data->vz89_co2);

			m_lcd->drawString(152 + 36, 64, 2, GREEN, BLACK, "HUMI");
			m_lcd->drawStringF(152 + 54, 80, 1, GREEN, BLACK, "%3uRH", m_data->bme280_humi);

//0..88 152..240

			uint8_t scaled1 = m_data->getVZ89VOC_scaled(88),
					scaled2 = m_data->getGP2Y10Dust_scaled(88),
					scaled3 = m_data->getVZ89CO2_scaled(88),
					scaled4 = m_data->getBME280Humi_scaled(88);
			if (m_chartX == 0) {
				m_lcd->drawPixel(m_chartX,278 - scaled1 - 1, BLUE_L1);
				m_lcd->drawPixel(m_chartX,278 - scaled2 - 1, PURPLE_L1);
				m_lcd->drawPixel(m_chartX,278 - scaled3 - 1, YELLOW);
				m_lcd->drawPixel(m_chartX,278 - scaled4 - 1, GREEN);
			} else {
				m_lcd->drawLine(m_chartX, 278 - m_data->vz89_voc_last - 1, m_chartX, 278 - scaled1 - 1, BLUE_L1);
				m_lcd->drawLine(m_chartX, 278 - m_data->gp2y10_dust_last - 1, m_chartX, 278 - scaled2 - 1, PURPLE_L1);
				m_lcd->drawLine(m_chartX, 278 - m_data->vz89_co2_last - 1, m_chartX, 278 - scaled3 - 1, YELLOW);
				m_lcd->drawLine(m_chartX, 278 - m_data->bme280_humi_last - 1, m_chartX, 278 - scaled4 - 1, GREEN);
			}
			if (m_chartX < 239) {
				m_chartX ++;
				// clear in front
				m_lcd->drawRectFilled(m_chartX, 188, 2, 90, BLACK_L1);
				m_data->vz89_voc_last = scaled1;
				m_data->gp2y10_dust_last = scaled2;
				m_data->vz89_co2_last = scaled3;
				m_data->bme280_humi_last = scaled4;
			}
			else
				m_chartX  = 0;

		} break;

		case PAGE_MEASURE: {
			m_lcd->drawStringF(8,  40 + 2, 2, BLUE_L1, BLACK_L1, "%2.2f%cC", m_data->bme280_temp,247);
			m_lcd->drawStringF(8,  72 + 2, 2, BLUE_L1, BLACK, "%4uhPa", m_data->bme280_pressure / 100);
			m_lcd->drawStringF(8, 104 + 2, 2, BLUE_L1, BLACK_L1, "%3uRH", m_data->bme280_humi);
			m_lcd->drawStringF(8, 136 + 2, 2, PURPLE_L1, BLACK, "%2.2fmgm%c", m_data->gp2y10_dust, 254);
			m_lcd->drawStringF(8, 168 + 2, 2, GREEN_L1, BLACK_L1, "%4.2fppm", m_data->vz89_co2);
			m_lcd->drawStringF(8, 200 + 2, 2, GREEN_L1, BLACK, "%4.2fppb", m_data->vz89_voc);
			m_lcd->drawStringF(8, 232 + 2, 2, RED, BLACK_L1, "%6luCPM", m_data->geiger_cpm);

			float bme280_temp = m_data->getBME280Temp_scaled(30),
					gp2y10_dust = m_data->getGP2Y10Dust_scaled(30),
					vz89_co2 = m_data->getVZ89CO2_scaled(30) ,
					vz89_voc = m_data->getVZ89VOC_scaled(30);
			uint32_t bme280_pressure = m_data->getBME280Pressure_scaled(30),
			 		geiger_cpm = m_data->getGeigerCPM_scaled(30);
			uint8_t bme280_humi = m_data->getBME280Humi_scaled(30);

			if (m_chartX == 120) {
				m_lcd->drawPixel(m_chartX, 64 - bme280_temp - 1, BLUE_L1);
				m_lcd->drawPixel(m_chartX, 96 - bme280_pressure - 1, BLUE_L1);
				m_lcd->drawPixel(m_chartX,128 - bme280_humi - 1, BLUE_L1);
				m_lcd->drawPixel(m_chartX,160 - gp2y10_dust - 1, PURPLE_L1);
				m_lcd->drawPixel(m_chartX,192 - vz89_co2 - 1, GREEN_L1);
				m_lcd->drawPixel(m_chartX,224 - vz89_voc - 1, GREEN_L1);
				m_lcd->drawPixel(m_chartX,256 - geiger_cpm - 1, RED);
			} else {
				m_lcd->drawLine(m_chartX, 64 - m_data->bme280_temp_last - 1, m_chartX, 64 - bme280_temp - 1, BLUE_L1);
				m_lcd->drawLine(m_chartX, 96 - m_data->bme280_pressure_last - 1, m_chartX, 96 - bme280_pressure - 1, BLUE_L1);
				m_lcd->drawLine(m_chartX, 128 - m_data->bme280_humi_last - 1, m_chartX, 128 - bme280_humi - 1, BLUE_L1);
				m_lcd->drawLine(m_chartX, 160 - m_data->gp2y10_dust_last - 1, m_chartX, 160 - gp2y10_dust - 1, PURPLE);
				m_lcd->drawLine(m_chartX, 192 - m_data->vz89_co2_last - 1, m_chartX, 192 - vz89_co2 - 1, GREEN_L1);
				m_lcd->drawLine(m_chartX, 224 - m_data->vz89_voc_last - 1, m_chartX, 224 - vz89_voc - 1, GREEN_L1);
				m_lcd->drawLine(m_chartX, 256 - m_data->geiger_cpm_last - 1, m_chartX, 256 - geiger_cpm - 1, RED);
			}

			if (m_chartX < 239) {
				m_chartX ++;
				// clear in front
				for (int i=0;i<7;i++)
					m_lcd->drawRectFilled(m_chartX, 32 + 32*i, 2, 32, (i%2 == 0)?BLACK_L1:BLACK);
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
			m_lcd->drawStringF(8,  104, 2, BLUE_L1, BLACK,      "Sent:    %09luB", m_data->serial_sent);
			m_lcd->drawStringF(8,  120, 2, BLUE_L1, BLACK,      "Received:%09luB", m_data->serial_recv);
			m_lcd->drawStringF(8,  168, 2, GREEN_L1, BLACK, "Sent:    %09luB", m_data->wlan_sent);
			m_lcd->drawStringF(8,  184, 2, GREEN_L1, BLACK, "Received:%09luB", m_data->wlan_recv);
			m_lcd->drawStringF(8,  216, 2, WHITE, BLACK, "Tube: %3uV", m_data->geiger_voltage);
			m_lcd->drawStringF(8,  232, 2, WHITE, BLACK, "Battery: %2.2fV", m_data->battery_voltage);
		} break;
		case PAGE_SETTINGS: {
			// nothing to update here
		} break;
		case PAGE_CALIBRATE: {
			m_lcd->drawStringF(CENTERX,  CENTERY, 2, BLUE_L1, BLACK, "%04d,%04d,%04d", m_touch->lastRawX, m_touch->lastRawY, m_touch->lastRawZ);
			m_lcd->drawPixel(m_touchX, m_touchY,3, RED);
		}
		break;
		case PAGE_WLAN: {
			if (m_butCount != m_data->freeAPCount + 1) // count the back button
				drawPage(PAGE_WLAN);
		}
	}
}
