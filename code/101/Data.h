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

#include <stdint.h>

class Data {
public:
	float 			bme280_temp,				// in degrees Celsius
					bme280_temp_min, bme280_temp_max, bme280_temp_last,
					bme280_altitude,			// in meters
					bme280_altitude_min, bme280_altitude_max, bme280_altitude_last;
	uint32_t 		bme280_pressure,			// in Pascals
					bme280_pressure_min, bme280_pressure_max, bme280_pressure_last;
	uint8_t			bme280_humi,
					bme280_humi_min, bme280_humi_max, bme280_humi_last;
	float 			gp2y10_dust,				// in mg/m^3
					gp2y10_dust_min, gp2y10_dust_max, gp2y10_dust_last;
	float 			vz89_co2,					// in ppm=parts per million
					vz89_co2_min, vz89_co2_max, vz89_co2_last,
					vz89_voc,					// in ppb=parts per billion
					vz89_voc_min, vz89_voc_max, vz89_voc_last;
	uint32_t		geiger_pulses,
					geiger_cpm,					// in cpm
					geiger_cpm_min, geiger_cpm_max, geiger_cpm_last;
	uint16_t		geiger_voltage;				// in volts
	uint16_t		geiger_duty;				// in percent per mille
	float			battery_voltage;			// in volts
	uint8_t			time_hour,
					time_minute,
					time_second;
	uint32_t		serial_sent,
					serial_recv,
					wlan_sent,
					wlan_recv;
	// WLAN
	uint8_t			freeAPCount;
	char			freeAPList[6][20];

	Data() {
		// define some initial values , code cleanup required! no time..
		bme280_temp_max = 50;
		bme280_temp_min = 10;
		bme280_pressure_max = 101325;
		bme280_pressure_min = 99325;
		bme280_humi_max = 90;
		bme280_humi_min = 10;
		gp2y10_dust_max = 0.50;
		vz89_co2_max = 800;
		vz89_co2_min = 400;
		vz89_voc_max = 200;
		geiger_cpm_max = 50;
	}

	void setLimits() {
		if (bme280_temp < bme280_temp_min) bme280_temp_min = bme280_temp;
		if (bme280_temp > bme280_temp_max) bme280_temp_max = bme280_temp;
		if (bme280_altitude < bme280_altitude_min) bme280_altitude_min = bme280_altitude;
		if (bme280_altitude > bme280_altitude_max) bme280_altitude_max = bme280_altitude;
		if (bme280_pressure < bme280_pressure_min) bme280_pressure_min = bme280_pressure;
		if (bme280_pressure > bme280_pressure_max) bme280_pressure_max = bme280_pressure;
		if (bme280_humi < bme280_humi_min) bme280_humi_min = bme280_humi;
		if (bme280_humi > bme280_humi_max) bme280_humi_max = bme280_humi;
		if (gp2y10_dust < gp2y10_dust_min) gp2y10_dust_min = gp2y10_dust;
		if (gp2y10_dust > gp2y10_dust_max) gp2y10_dust_max = gp2y10_dust;
		if (vz89_co2 < vz89_co2_min) vz89_co2_min = vz89_co2;
		if (vz89_co2 > vz89_co2_max) vz89_co2_max = vz89_co2;
		if (vz89_voc < vz89_voc_min) vz89_voc_min = vz89_voc;
		if (vz89_voc > vz89_voc_max) vz89_voc_max = vz89_voc;
		if (geiger_cpm < geiger_cpm_min) geiger_cpm_min = geiger_cpm;
		if (geiger_cpm > geiger_cpm_max) geiger_cpm_max = geiger_cpm;
	}
	float scaleFloat(float value, float min, float max, uint8_t height) {
		if (max - min == 0) return 0;
		else
		return (value - min) * height / (max - min);
	}
	float scaleInt(uint32_t value, uint32_t min, uint32_t max, uint8_t height) {
		if (max - min == 0) return 0;
		else
		return (value - min) * height / (max - min);
	}

	float getBME280Temp_scaled(uint8_t height) {
		return scaleFloat(bme280_temp, bme280_temp_min, bme280_temp_max, height);
	}
	float getBME280Altitude_scaled(uint8_t height) {
		return scaleFloat(bme280_altitude, bme280_altitude_min, bme280_altitude_max, height);
	}
	uint32_t getBME280Pressure_scaled(uint8_t height) {
		return scaleInt(bme280_pressure, bme280_pressure_min, bme280_pressure_max, height);
	}
	uint8_t getBME280Humi_scaled(uint8_t height) {
		return scaleInt(bme280_humi, bme280_humi_min, bme280_humi_max, height);
	}
	float getGP2Y10Dust_scaled(uint8_t height) {
		return scaleFloat(gp2y10_dust, gp2y10_dust_min, gp2y10_dust_max, height);
	}
	float getVZ89CO2_scaled(uint8_t height) {
		return scaleFloat(vz89_co2, vz89_co2_min, vz89_co2_max, height);
	}
	float getVZ89VOC_scaled(uint8_t height) {
		return scaleFloat(vz89_voc, vz89_voc_min, vz89_voc_max, height);
	}
	uint32_t getGeigerCPM_scaled(uint8_t height) {
		return scaleInt(geiger_cpm, geiger_cpm_min, geiger_cpm_max, height);
	}

	/*void set(float temp, float altitude, uint32_t pressure, float dust, float co2, float voc, uint32_t pulses, uint32_t cpm, uint16_t voltage, uint16_t duty, float battery) {
		bmp180_temp = temp;
		bmp180_altitude = altitude;
		bmp180_pressure = pressure;
		gp2y10_dust = dust;
		vz89_co2 = co2;
		vz89_voc = voc;
		geiger_pulses = pulses;
		geiger_cpm = cpm;
		geiger_voltage = voltage;
		geiger_duty = duty;
		battery_voltage = battery;
	}

	float getBMP180Temperature() { return bmp180_temp; }
	float getBMP180Altitude() { return bmp180_altitude; }
	uint32_t getBMP180Pressure() { return bmp180_pressure; }
	float getGP2Y10Dust() { return gp2y10_dust; }
	float getVZ89CO2() { return vz89_co2; }
	float getVZ89VOC() { return vz89_voc; }
	uint32_t getGeigerPulses() { return geiger_pulses; }
	uint32_t getGeigerCPM() { return geiger_cpm; }
	uint16_t getGeigerVoltage() { return geiger_voltage; }
	uint16_t getGeigerDuty() { return geiger_duty; }
	float getBatteryVoltage() { return battery_voltage; }*/
};
