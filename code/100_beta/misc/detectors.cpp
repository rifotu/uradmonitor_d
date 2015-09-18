/*
** uRADMonitor radiation detectors
** Version: 	0.1.0
** Date: 		January, 2014
** License:		LGPL v3
** Copyright: 	(C) 2009 - 2015 Radu Motisan, radu.motisan@gmail.com
** Description:	Used in the uRADMonitor code to identify various radiation detectors, mostly Geiger tubes
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

#include "detectors.h"

// not more than 10 chars!
char *aux_detectorName(uint8_t param) {
	switch (param) {
		case GEIGER_TUBE_SBM20: 	return "SBM20";
		case GEIGER_TUBE_SI29BG: 	return "SI29BG";
		case GEIGER_TUBE_SBM19: 	return "SBM19";
		case GEIGER_TUBE_LND712: 	return "LND712";
		case GEIGER_TUBE_STS5: 		return "STS5";
		case GEIGER_TUBE_SI22G: 	return "SI22G";
		case GEIGER_TUBE_SI3BG: 	return "SI3BG";
		case GEIGER_TUBE_SBM21: 	return "SBM21";
		case GEIGER_TUBE_SBT9: 		return "SBT9";
		case GEIGER_TUBE_SI1G:  	return "SI1G";
		default: return "unknown";
	}
}

float aux_detectorFactor(uint8_t param) {
	switch (param) {
		case GEIGER_TUBE_SBM20: 	return 0.006315;
		case GEIGER_TUBE_SI29BG: 	return 0.010000;
		case GEIGER_TUBE_SBM19: 	return 0.001500;
		case GEIGER_TUBE_STS5: 		return 0.006666;
		case GEIGER_TUBE_SI22G: 	return 0.001714;
		case GEIGER_TUBE_SI3BG: 	return 0.631578;
		case GEIGER_TUBE_SBM21: 	return 0.048000;
		case GEIGER_TUBE_LND712: 	return 0.005940;
		case GEIGER_TUBE_SBT9: 		return 0.010900;
		case GEIGER_TUBE_SI1G:		return 0.006000;
		default: 0;
	}
}
