
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
#ifndef _SFM_GPIO_H_
#define _SFM_GPIO_H_

#include <stdlib.h>

#include "SFM_SysParameter.h"
#include "SFM_Error.h"
#include "SFM_Type.h"
#include "SFM_Command.h"
#include "SFM_Packet.h"

#define UF_NUM_OF_GPIO					8
#define UF_GPIO_DATA_SIZE				8
#define UF_MAX_GPIO_OUTPUT_EVENT		24

typedef enum {
 	UF_GPIO_0 = 0,
 	UF_GPIO_1 = 1,
 	UF_GPIO_2 = 2,
 	UF_GPIO_3 = 3,
 	UF_GPIO_4 = 4,
 	UF_GPIO_5 = 5,
 	UF_GPIO_6 = 6,
 	UF_GPIO_7 = 7,
} UF_GPIO_PORT;

typedef enum {
	UF_GPIO_DISABLE 		= 0,
	UF_GPIO_INPUT			= 1,
	UF_GPIO_OUTPUT			= 2,	
	UF_GPIO_SHARED_IO		= 3,
	UF_GPIO_WIEGAND_INPUT	= 4,
	UF_GPIO_WIEGAND_OUTPUT	= 5,
	UF_GPIO_BUZZER			= 6,
	UF_GPIO_NETWORK_MODE	= 6,
	UF_GPIO_MODE_UNKNOWN	= -1,
} UF_GPIO_MODE;

typedef struct {
	unsigned short	inputFunction;
	unsigned short	activationLevel;
	unsigned short	timeout;
	unsigned short	reserved;
} UFGPIOInputData;

typedef struct {
	unsigned short 	event;
	unsigned short	level;
	unsigned short	interval;
	unsigned short	blinkingPeriod;
} UFGPIOOutputData;

typedef struct {
	unsigned short field0;
	unsigned short field1;
	unsigned short field2;
	unsigned short field3;
} UFGPIOData;

typedef struct {
	UF_GPIO_MODE mode[UF_NUM_OF_GPIO];
	int numOfData[UF_NUM_OF_GPIO];
	UFGPIOData gpioData[UF_NUM_OF_GPIO][UF_MAX_GPIO_OUTPUT_EVENT];
} UFConfigGPIO;

#ifdef __cplusplus
extern "C"
{
#endif

UF_RET_CODE UF_GetGPIOConfiguration( UF_GPIO_PORT port, UF_GPIO_MODE* mode, int* numOfData, UFGPIOData* data );
UF_RET_CODE UF_SetInputGPIO( UF_GPIO_PORT port, UFGPIOInputData data );
UF_RET_CODE UF_SetOutputGPIO( UF_GPIO_PORT port, int numOfData, UFGPIOOutputData* data );
UF_RET_CODE UF_SetBuzzerGPIO( UF_GPIO_PORT port, int numOfData, UFGPIOOutputData* data );
UF_RET_CODE UF_SetSharedGPIO( UF_GPIO_PORT port, UFGPIOInputData inputData, int numOfOutputData, UFGPIOOutputData* outputData );
UF_RET_CODE UF_DisableGPIO( UF_GPIO_PORT port );
UF_RET_CODE UF_ClearAllGPIO();
UF_RET_CODE UF_SetDefaultGPIO();
UF_RET_CODE UF_MakeGPIOConfiguration( UFConfigComponentHeader* configHeader, BYTE* configData );

#ifdef __cplusplus
}
#endif

#endif
