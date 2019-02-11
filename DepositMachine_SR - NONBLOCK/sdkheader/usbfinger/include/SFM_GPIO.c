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

    File : SFM_GPIO.c
    Descriptions : GPIO API
    API list 
    - UF_GetGPIOConfiguration
    - UF_SetInputGPIO
    - UF_SetOutputGPIO
    - UF_SetSharedGPIO
    - UF_DisableGPIO
    - UF_ClearAllGPIO
    - UF_SetDefaultGPIO
    - UF_EnableWiegandInput
    - UF_EnableWiegandOutput
    - UF_DisableWiegandInput
    - UF_DisableWiegandOutput
    - UF_MakeGPIOConfiguration

*/

#include "SFM_GPIO.h"

UF_RET_CODE UF_GetGPIOConfiguration( UF_GPIO_PORT port, UF_GPIO_MODE* mode, int* numOfData, UFGPIOData* data )
{
	int i;
	UINT32 param = port;
	UINT32 size = 0;
	BYTE flag = 0;
    UF_RET_CODE result;

    result = UF_Command( SFM_COM_GR, &param, &size, &flag );

    if( result != UF_RET_SUCCESS ) {
        return result;
    }

    if( flag != SFM_PROTO_RET_SUCCESS ) {
        return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE) flag );
    }

    *mode = (UF_GPIO_MODE) ((param & 0xffff0000) >> 16);
    *numOfData = size;

    if( size > 0 ) {
        result = UF_ReceiveRawData( (BYTE*) data, size * UF_GPIO_DATA_SIZE, UF_CalculateTimeout( size * UF_GPIO_DATA_SIZE ), TRUE );

        if( result != UF_RET_SUCCESS ) {
            return result;
        }
    }

    return UF_RET_SUCCESS;
}

UF_RET_CODE UF_SetInputGPIO( UF_GPIO_PORT port, UFGPIOInputData data )
{
	UINT32 param = port | (UF_GPIO_INPUT << 16);
	UINT32 size = 1;
	BYTE flag = 0;
	UF_RET_CODE result;
	
	result = UF_CommandSendData( SFM_COM_GW, &param, &size, &flag, (BYTE*)&data, UF_GPIO_DATA_SIZE );

	if( result != UF_RET_SUCCESS )
	{
		return result;
	}
	
	else if( flag != SFM_PROTO_RET_SUCCESS )
	{
		return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE) flag );
	}

	return UF_RET_SUCCESS;
}

UF_RET_CODE UF_SetOutputGPIO( UF_GPIO_PORT port, int numOfData, UFGPIOOutputData* data )
{
	UINT32 param = port | (UF_GPIO_OUTPUT << 16);
	UINT32 size = numOfData;
	BYTE flag = 0;
	UF_RET_CODE result;
	
	result = UF_CommandSendData( SFM_COM_GW, &param, &size, &flag, (BYTE*)data, numOfData * UF_GPIO_DATA_SIZE );

	if( result != UF_RET_SUCCESS )
	{
		return result;
	}
	
	else if( flag != SFM_PROTO_RET_SUCCESS )
	{
		return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE) flag );
	}

	return UF_RET_SUCCESS;
}

UF_RET_CODE UF_SetBuzzerGPIO( UF_GPIO_PORT port, int numOfData, UFGPIOOutputData* data )
{
	UINT32 param = port | (UF_GPIO_BUZZER << 16);
	UINT32 size = numOfData;
	BYTE flag = 0;
	UF_RET_CODE result;
	
	result = UF_CommandSendData( SFM_COM_GW, &param, &size, &flag, (BYTE*)data, numOfData * UF_GPIO_DATA_SIZE );

	if( result != UF_RET_SUCCESS )
	{
		return result;
	}
	
	else if( flag != SFM_PROTO_RET_SUCCESS )
	{
		return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE) flag );
	}

	return UF_RET_SUCCESS;
}

/* Not support SFM6000 & SFMSlim */
UF_RET_CODE UF_SetSharedGPIO( UF_GPIO_PORT port, UFGPIOInputData inputData, int numOfOutputData, UFGPIOOutputData* outputData )
{
	BYTE* buf;
	UINT32 param = port | (UF_GPIO_SHARED_IO << 16);
	UINT32 size = numOfOutputData + 1;
	BYTE flag = 0;
	//int result;
	UF_RET_CODE result;

	buf = (BYTE*)malloc( (numOfOutputData + 1) * UF_GPIO_DATA_SIZE );
	memcpy( buf, &inputData, UF_GPIO_DATA_SIZE );
	if( numOfOutputData > 0 )
	{
		memcpy( buf + UF_GPIO_DATA_SIZE, outputData, numOfOutputData * UF_GPIO_DATA_SIZE );
	}
	
	result = UF_CommandSendData( SFM_COM_GW, &param, &size, &flag, buf, size * UF_GPIO_DATA_SIZE );

	free( buf );
	
	if( result != UF_RET_SUCCESS )
	{
		return result;
	}
	
	else if( flag != SFM_PROTO_RET_SUCCESS )
	{
		return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE) flag );
	}

	return UF_RET_SUCCESS;
}

UF_RET_CODE UF_DisableGPIO( UF_GPIO_PORT port )
{
	UINT32 param = port | (UF_GPIO_DISABLE << 16);
	UINT32 size = 0;
	BYTE flag = 0;

	//int result = UF_Command( UF_COM_GW, &param, &size, &flag );
	UF_RET_CODE result;
	result = UF_Command( SFM_COM_GW, &param, &size, &flag );

	if( result != UF_RET_SUCCESS )
	{
		return result;
	}

	if( flag != SFM_PROTO_RET_SUCCESS )
	{
		return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE)flag );
	}

	return UF_RET_SUCCESS;
}

UF_RET_CODE UF_ClearAllGPIO()
{
	UINT32 param = 0;
	UINT32 size = 0;
	BYTE flag = 0;

	//int result = UF_Command( UF_COM_GC, &param, &size, &flag );
	UF_RET_CODE result = UF_Command( SFM_COM_GC, &param, &size, &flag );

	if( result != UF_RET_SUCCESS )
	{
		return result;
	}

	if( flag != SFM_PROTO_RET_SUCCESS )
	{
		return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE) flag );
	}

	return UF_RET_SUCCESS;
}

UF_RET_CODE UF_SetDefaultGPIO()
{
	UINT32 param = 0;
	UINT32 size = 0;
	BYTE flag = 0;

	//int result = UF_Command( UF_COM_GD, &param, &size, &flag );
	UF_RET_CODE result = UF_Command( SFM_COM_GD, &param, &size, &flag );

	if( result != UF_RET_SUCCESS )
	{
		return result;
	}

	if( flag != SFM_PROTO_RET_SUCCESS )
	{
		return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE) flag );
	}

	return UF_RET_SUCCESS;
}

#if 0
UF_RET_CODE UF_EnableWiegandInput( UFGPIOWiegandData data )
{

}

UF_RET_CODE UF_EnableWiegandOutput( UFGPIOWiegandData data )
{

}

UF_RET_CODE UF_DisableWiegandInput()
{

}

UF_RET_CODE UF_DisableWiegandOutput()
{

}
#endif

UF_RET_CODE UF_MakeGPIOConfiguration( UFConfigComponentHeader* configHeader, BYTE* configData )
{
	UFConfigGPIO* data;
	int i;
	//int result;
	UF_RET_CODE result; //hclee
	
	configHeader->type = UF_CONFIG_GPIO;
	configHeader->dataSize = 0;
	configHeader->checksum = 0;

	memset( configData, 0, sizeof(UFConfigGPIO) );

	data = (UFConfigGPIO*)configData;

	for( i = 0; i < UF_NUM_OF_GPIO; i++ )
	{
		result = UF_GetGPIOConfiguration( (UF_GPIO_PORT)i, &data->mode[i], &data->numOfData[i], data->gpioData[i] ); //hclee

		if( result != UF_RET_SUCCESS )
		{
			return result;
		}
	}

	configHeader->dataSize = sizeof(UFConfigGPIO);
	
	for( i = 0; i < configHeader->dataSize; i++ )
	{
		configHeader->checksum += configData[i];
	}

	return UF_RET_SUCCESS;
}