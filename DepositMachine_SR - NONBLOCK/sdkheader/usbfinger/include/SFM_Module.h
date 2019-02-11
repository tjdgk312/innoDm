/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _SFM_MODULE_H_
#define _SFM_MODULE_H_

#define _CONFIG_SENSOR_OP_

#ifdef _CONFIG_SENSOR_OP_
#define SFM_WIDTH	272
#define SFM_HEIGHT	320
#endif

#ifdef _CONFIG_SENSOR_OC_
#define SFM_WIDTH	288
#define SFM_HEIGHT	288
#endif

#ifdef _CONFIG_SENSOR_TC1_
#define SFM_WIDTH   256
#define SFM_HEIGHT  360
#endif

#ifdef _CONFIG_SENSOR_TC2_
#define SFM_WIDTH   208
#define SFM_HEIGHT  288
#endif

#ifdef _CONFIG_SENSOR_T2S_
#define SFM_WIDTH   208
#define SFM_HEIGHT  288
#endif

#ifdef _CONFIG_SENSOR_SLIM_
#define SFM_WIDTH   300
#define SFM_HEIGHT  400
#endif

typedef enum {
	UF_MODULE_3000		= 0,
	UF_MODULE_3500		= 1,
	UF_BIOENTRY_SMART		= 2,
	UF_BIOENTRY_PASS		= 3,
	UF_SFOD_3100			= 4,
	UF_3000FC				= 5,
	UF_MODULE_4000		= 6,
	UF_MODULE_5000		= 7,
	UF_MODULE_5500		= 8,
	UF_MODULE_UNKNOWN		= -1,
} UF_MODULE_TYPE;

typedef enum {
	UF_VERSION_1_0		= 0,
	UF_VERSION_1_1		= 1,
	UF_VERSION_1_2		= 2,
	UF_VERSION_1_3		= 3,
	UF_VERSION_1_4		= 4,
	UF_VERSION_1_5		= 5,
	UF_VERSION_1_6		= 6,
	UF_VERSION_1_7		= 7,
	UF_VERSION_1_8		= 8,
	UF_VERSION_1_9		= 9,
	UF_VERSION_2_0		= 10,
	UF_VERSION_2_1		= 11,
	UF_VERSION_UNKNOWN	= -1,
} UF_MODULE_VERSION;

typedef enum {
	UF_SENSOR_FL			= 0,
	UF_SENSOR_FC			= 1,
	UF_SENSOR_OP			= 2,
	UF_SENSOR_OC			= 3,
	UF_SENSOR_TC			= 4,
	UF_SENSOR_OC2			= 5,
	UF_SENSOR_TS			= 6,
	UF_SENSOR_OL			= 7,
	UF_SENSOR_OH			= 8,

	UF_SENSOR_UNKNOWN		= -1,
} UF_MODULE_SENSOR;

#endif
