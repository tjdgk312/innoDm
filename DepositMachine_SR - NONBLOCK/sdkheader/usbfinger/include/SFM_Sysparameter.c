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

    File : SFM_Sysparameter.c
    Descriptions : System Parameters API
    API list
    - UF_InitSysParameter
    - UF_GetSysParameter
    - UF_SetSysParameter
    - UF_GetMultiSysParameter
    - UF_SetMultiSysParameter
    - UF_Save
    - UF_SaveConfiguration
    - UF_ReadConfigurationHeader
    - UF_LoadConfiguration
    - UF_MakeParameterConfiguration

    New API
    - UF_ResetSystemConfiguration

*/

#include "SFM_SysParameter.h"
#include "SFM_Connect.h"

SysParameter *sysParameter;

//
// Static variables
//

SysParameter s_SysParameter[] =
{
	{ UF_SYS_TIMEOUT, SFM_SYS_PARAMETER_NOT_READ },            //0
	{ UF_SYS_ENROLL_MODE, SFM_SYS_PARAMETER_NOT_READ },        //1
	{ UF_SYS_SECURITY_LEVEL, SFM_SYS_PARAMETER_NOT_READ },     //2
	{ UF_SYS_ENCRYPTION_MODE, SFM_SYS_PARAMETER_NOT_READ },    //3
	{ UF_SYS_SENSOR_TYPE, SFM_SYS_PARAMETER_NOT_READ },        //4
	{ UF_SYS_IMAGE_FORMAT, SFM_SYS_PARAMETER_NOT_READ },       //5
	{ UF_SYS_MODULE_ID, SFM_SYS_PARAMETER_NOT_READ },          //6
	{ UF_SYS_FIRMWARE_VERSION, SFM_SYS_PARAMETER_NOT_READ },   //7
	{ UF_SYS_SERIAL_NUMBER, SFM_SYS_PARAMETER_NOT_READ },      //8
	{ UF_SYS_BAUDRATE, SFM_SYS_PARAMETER_NOT_READ },           //9
//	{ UF_SYS_AUX_BAUDRATE, SFM_SYS_PARAMETER_NOT_READ },
	{ UF_SYS_ENROLLED_FINGER, SFM_SYS_PARAMETER_NOT_READ },
	{ UF_SYS_AVAILABLE_FINGER, SFM_SYS_PARAMETER_NOT_READ },
	{ UF_SYS_SEND_SCAN_SUCCESS, SFM_SYS_PARAMETER_NOT_READ },
	{ UF_SYS_ASCII_PACKET, SFM_SYS_PARAMETER_NOT_READ },
	{ UF_SYS_ROTATE_IMAGE, SFM_SYS_PARAMETER_NOT_READ },
	{ UF_SYS_SENSITIVITY, SFM_SYS_PARAMETER_NOT_READ },
	{ UF_SYS_IMAGE_QUALITY, SFM_SYS_PARAMETER_NOT_READ },
	{ UF_SYS_AUTO_RESPONSE, SFM_SYS_PARAMETER_NOT_READ },
	{ UF_SYS_NETWORK_MODE, SFM_SYS_PARAMETER_NOT_READ },
	{ UF_SYS_FREE_SCAN, SFM_SYS_PARAMETER_NOT_READ },
	{ UF_SYS_PROVISIONAL_ENROLL, SFM_SYS_PARAMETER_NOT_READ },
	{ UF_SYS_PASS_WHEN_EMPTY, SFM_SYS_PARAMETER_NOT_READ },
	{ UF_SYS_RESPONSE_DELAY, SFM_SYS_PARAMETER_NOT_READ },
	{ UF_SYS_MATCHING_TIMEOUT, SFM_SYS_PARAMETER_NOT_READ },
	{ UF_SYS_BUILD_NO, SFM_SYS_PARAMETER_NOT_READ },
	{ UF_SYS_ENROLL_DISPLACEMENT, SFM_SYS_PARAMETER_NOT_READ },
	{ UF_SYS_TEMPLATE_SIZE, SFM_SYS_PARAMETER_NOT_READ },
	{ UF_SYS_ROTATION, SFM_SYS_PARAMETER_NOT_READ },
	{ UF_SYS_LIGHTING_CONDITION, SFM_SYS_PARAMETER_NOT_READ },
	{ UF_SYS_FREESCAN_DELAY, SFM_SYS_PARAMETER_NOT_READ },
//	{ UF_SYS_CARD_ENROLL_MODE, SFM_SYS_PARAMETER_NOT_READ },
	{ UF_SYS_FAST_MODE, SFM_SYS_PARAMETER_NOT_READ },
//	{ UF_SYS_WATCHDOG, SFM_SYS_PARAMETER_NOT_READ },
	{ UF_SYS_TEMPLATE_TYPE, SFM_SYS_PARAMETER_NOT_READ },
	{ UF_SYS_ENHANCED_PRIVACY, SFM_SYS_PARAMETER_NOT_READ },
	{ UF_SYS_FAKE_DETECT, SFM_SYS_PARAMETER_NOT_READ },
	{ UF_SYS_CHECK_LATENT, SFM_SYS_PARAMETER_NOT_READ },
//	{ UF_SYS_VOLTAGE_WARNING, SFM_SYS_PARAMETER_NOT_READ },
//	{ UF_SYS_POWEROFF_TIMEOUT, SFM_SYS_PARAMETER_NOT_READ },
    { UF_SYS_TOUCH_SENSITIVITY, SFM_SYS_PARAMETER_NOT_READ },
    { -1, 0 },
};

char sysParamList[][20]={
"TIMEOUT",
"ENROLL_MODE",
"SECURITY_LEVEL",
"ENCRYPTION_MODE",
"SENSOR_TYPE",
"IMAGE_FORMAT",
"MODULE_ID",
"FIRMWARE_VERSION", //7
"SERIAL_NUMBER", //8
"BAUDRATE",
"AUX_BAUDRATE",
"ENROLLED_FINGER",
"AVAILABLE_FINGER",
"SEND_SCAN_SUCCESS",
"ASCII_PACKET",
"ROTATE_IMAGE",
"SENSITIVITY",
"IMAGE_QUALITY",
"AUTO_RESPONSE",
"NETWORK_MODE",
"FREE_SCAN",
"PROVISIONAL_ENROLL",
"PASS_WHEN_EMPTY",
"RESPONSE_DELAY",
"MATCHING_TIMEOUT",
"BUILD_NO", //25
"ENROLL_DISPLACEMENT",
"TEMPLATE_SIZE",
"ROTATION",
"LIGHTING_CONDITION",
"FREESCAN_DELAY",
"CARD_ENROLL_MODE",
"FAST_MODE",
"WATCHDOG",
"TEMPLATE_TYPE",
"ENHANCED_PRIVACY",
"FAKE_DETECT",
"CHECK_LATENT",
"VOLTAGE_WARNING",
"POWEROFF_TIMEOUT",
"EXPOSURE",
"TOUCH_SENSITIVITY",
"RESERVED"
};

SysParameter* GetSysParamPointer()
{
    return s_SysParameter;
}

/**
 *	Initialize system parameter
 */
UF_RET_CODE UF_InitSysParameter(void)
{
	UF_RET_CODE result;
	int i = 0;

	while( s_SysParameter[i].parameter != -1 )
	{
		s_SysParameter[i++].value = SFM_SYS_PARAMETER_NOT_READ;
	}

    return UF_RET_SUCCESS;
}

UF_RET_CODE UF_GetSysParameter( UF_SYS_PARAM parameter, UINT32* value )
{
	UF_RET_CODE result;
	int i = 0;

	while( s_SysParameter[i].parameter != -1 )
	{
		if( s_SysParameter[i].parameter == parameter )
		{
			if( s_SysParameter[i].value == SFM_SYS_PARAMETER_NOT_FOUND )
			{
				*value = SFM_SYS_PARAMETER_NOT_FOUND;
				return SFM_ERR_NOT_FOUND;
			}

			if( s_SysParameter[i].value != SFM_SYS_PARAMETER_NOT_READ )
			{
				*value = s_SysParameter[i].value;
				return UF_RET_SUCCESS;
			}

			result = UF_ReadSysParameter( parameter, value );

			if( result == UF_RET_SUCCESS )
			{
				s_SysParameter[i].value = *value;

				return UF_RET_SUCCESS;
			}
			else if( result == SFM_ERR_NOT_FOUND )
			{
				s_SysParameter[i].value = SFM_SYS_PARAMETER_NOT_FOUND;
			}

			return result;
		}

		i++;
	}

	return UF_ERR_INVALID_PARAMETER;
}

UF_RET_CODE UF_SetSysParameter( UF_SYS_PARAM parameter, UINT32 value )
{
	UF_RET_CODE result;
	int i = 0;;

	while( s_SysParameter[i].parameter != 0 )
	{
		if( s_SysParameter[i].parameter == parameter )
		{
			if( s_SysParameter[i].value == SFM_SYS_PARAMETER_NOT_FOUND )
			{
				return SFM_ERR_NOT_FOUND;
			}

			if( s_SysParameter[i].value == value )
			{
				return UF_RET_SUCCESS;
			}

			result = UF_WriteSysParameter( parameter, value );

			if( result == UF_RET_SUCCESS )
			{
				s_SysParameter[i].value = value;

				return UF_RET_SUCCESS;
			}
			else if( result == SFM_ERR_NOT_FOUND )
			{
				s_SysParameter[i].value = SFM_SYS_PARAMETER_NOT_FOUND;
			}

			return result;
		}

		i++;
	}

	return UF_ERR_INVALID_PARAMETER;
}


/**
 *	Read a system parameter
 */
UF_RET_CODE UF_ReadSysParameter( UF_SYS_PARAM parameter, UINT32* value )
{
	UF_RET_CODE result;
	UINT32 param = 0;
	UINT32 size = 0;
	BYTE flag = (BYTE)parameter;

	result = UF_Command( SFM_COM_SR, &param, &size, &flag );

	if( result != UF_RET_SUCCESS ) {
		return result;
	}

	if( flag == SFM_PROTO_RET_SUCCESS ) {
		*value = size;

		return UF_RET_SUCCESS;
	} else {
		return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE)flag );
	}
}

/**
 *	Write a system parameter
 */
UF_RET_CODE UF_WriteSysParameter( UF_SYS_PARAM parameter, UINT32 value )
{
	UF_RET_CODE result;
	UINT32 param = 0;
	UINT32 size = value;
	BYTE flag = parameter;

	result = UF_Command( SFM_COM_SW, &param, &size, &flag );

	if( result != UF_RET_SUCCESS )
	{
		return result;
	}

	if( flag == SFM_PROTO_RET_SUCCESS )
	{
		if( parameter == UF_SYS_ASCII_PACKET )
		{
			UF_SetAsciiMode( value - 0x30 );
		}
		else if( parameter == UF_SYS_NETWORK_MODE )
		{
			if( value == 0x30 )
			{
				UF_SetProtocol( UF_SINGLE_PROTOCOL, UF_GetModuleID() );
			}
			else
			{
				UF_SetProtocol( UF_NETWORK_PROTOCOL, UF_GetModuleID() );
			}
		}
		else if( parameter == UF_SYS_MODULE_ID )
		{
			UF_SetProtocol( UF_GetProtocol(), value );
		}

		return UF_RET_SUCCESS;
	}
	else
	{

		return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE)flag );
	}
}

/**
 * 	Get multiple system parameters
 */
UF_RET_CODE UF_GetMultiSysParameter( int parameterCount, UF_SYS_PARAM* parameters, UINT32* values )
{
	UF_RET_CODE result;
	int i;

	for( i = 0; i < parameterCount; i++ )
	{
		result = UF_GetSysParameter( parameters[i], &values[i] );

		if( result != UF_RET_SUCCESS )
		{
			if( result == SFM_ERR_NOT_FOUND )
			{
				values[i] = SFM_SYS_PARAMETER_NOT_FOUND;
			}
			else
			{
				return result;
			}
		}
	}

	return UF_RET_SUCCESS;
}

UF_RET_CODE UF_SetMultiSysParameter( int parameterCount, UF_SYS_PARAM* parameters, UINT32* values )
{
	UF_RET_CODE result;
	int i;

	for( i = 0; i < parameterCount; i++ )
	{
		result = UF_SetSysParameter( parameters[i], values[i] );

		if( result != UF_RET_SUCCESS )
		{
			if( result == SFM_ERR_NOT_FOUND )
			{
				continue;
			}
			else if( result == SFM_ERR_UNSUPPORTED && parameters[i] == UF_SYS_ENCRYPTION_MODE )
			{
				continue;
			}
			else
			{
				return result;
			}
		}
	}

	return UF_RET_SUCCESS;
}

UF_RET_CODE UF_Save(void)
{
	UINT32 param = 0;
	UINT32 size = 0;
	BYTE flag = 0;
	UF_RET_CODE result;

	int origTimeout = UF_GetGenericCommandTimeout();

	UF_SetGenericCommandTimeout( 10000 );

	result = UF_Command( SFM_COM_SF, &param, &size, &flag );

	UF_SetGenericCommandTimeout( origTimeout );

	if( result != UF_RET_SUCCESS )
	{
		return result;
	}
	else if( flag != SFM_PROTO_RET_SUCCESS )
	{
		return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE)flag );
	}

	return UF_RET_SUCCESS;
}

UF_RET_CODE UF_SaveConfiguration( const char* filename, const char* description, int numOfComponent, UFConfigComponentHeader* componentHeader, void** componentData )
{
	FILE* fp;
	UFConfigFileHeader fileHeader;
	int size;
	int descLen;
	int i;

	memset( &fileHeader, 0, sizeof(fileHeader) );

	fileHeader.numOfComponent = numOfComponent;
	descLen = (strlen(description) > 255)? 255 : strlen(description);
	memcpy( fileHeader.description, description, descLen );
	fileHeader.description[descLen] = '\0';

	fp = fopen( filename, "wb" );

	if( !fp )
	{
		return SFM_ERR_FILE_IO;
	}

	size = sizeof(fileHeader);

	if( fwrite( &fileHeader, 1, size, fp ) != size )
	{
		fclose( fp );
		return SFM_ERR_FILE_IO;
	}

	for( i = 0; i < numOfComponent; i++ )
	{
		size = sizeof(UFConfigComponentHeader);
		if( fwrite( &componentHeader[i], 1, size, fp ) != size )
		{
			fclose( fp );
			return SFM_ERR_FILE_IO;
		}

		size = componentHeader[i].dataSize;

		if( fwrite( componentData[i], 1, size, fp ) != size )
		{
			fclose( fp );
			return SFM_ERR_FILE_IO;
		}
	}

	fileHeader.magicNo = UF_VALID_CONFIG_FILE;

	fseek( fp, 0, SEEK_SET );

	size = sizeof(fileHeader);


	if( fwrite( &fileHeader, 1, size, fp ) != size )
	{
		fclose( fp );
		return SFM_ERR_FILE_IO;
	}

	fclose( fp );

	return UF_RET_SUCCESS;
}

UF_RET_CODE UF_ReadConfigurationHeader( const char* filename, UFConfigFileHeader* header )
{
	FILE* fp;
	int size;

	fp = fopen( filename, "rb" );

	if( !fp )
	{
		return SFM_ERR_FILE_IO;
	}

	size = sizeof(UFConfigFileHeader);

	if( fread( header, 1, size, fp ) != size )
	{
		fclose( fp );
		return SFM_ERR_FILE_IO;
	}

	if( header->magicNo != UF_VALID_CONFIG_FILE )
	{
		fclose( fp );
		return SFM_ERR_INVALID_FILE;
	}

	fclose( fp );
	return UF_RET_SUCCESS;
}


UF_RET_CODE UF_LoadConfiguration( const char* filename, int numOfComponent, UF_CONFIG_TYPE* type )
{
	UF_RET_CODE result;

	int i, j, k;
	int size;

	BYTE* componentData;
	UINT32 checksum;
	BOOL foundComponent;

	UFConfigFileHeader fileHeader;
	UFConfigComponentHeader componentHeader;

	FILE* fp;

	fp = fopen( filename, "rb" );

	if( !fp )
	{
		return SFM_ERR_FILE_IO;
	}

	size = sizeof(UFConfigFileHeader);

	if( fread( &fileHeader, 1, size, fp ) != size )
	{
		fclose( fp );
		return SFM_ERR_FILE_IO;
	}

	if( fileHeader.magicNo != UF_VALID_CONFIG_FILE )
	{
		fclose( fp );
		return SFM_ERR_INVALID_FILE;
	}

	for( i = 0; i < fileHeader.numOfComponent; i++ )
	{
		size = sizeof(componentHeader);

		if( fread( &componentHeader, 1, size, fp ) != size )
		{
			fclose( fp );
			return SFM_ERR_FILE_IO;
		}

		foundComponent = FALSE;

		for( j = 0; j < numOfComponent; j++ )
		{
			if( componentHeader.type == type[j] )
			{
				foundComponent = TRUE;

				componentData = (BYTE*)malloc( componentHeader.dataSize );

				if( !componentData )
				{
					fclose( fp );
					return SFM_ERR_OUT_OF_MEMORY;
				}

				if( fread( componentData, 1, componentHeader.dataSize, fp ) != componentHeader.dataSize )
				{
					fclose( fp );
					free( componentData );
					return SFM_ERR_FILE_IO;
				}

				checksum = 0;

				for( k = 0; k < componentHeader.dataSize; k++ )
				{
					checksum += componentData[k];
				}

				if( checksum != componentHeader.checksum )
				{
					fclose( fp );
					free( componentData );

					return SFM_ERR_CHECKSUM_ERROR;
				}

				result = UF_ApplyConfigComponent( &componentHeader, componentData );

				if( result != UF_RET_SUCCESS )
				{
					fclose( fp );
					free( componentData );

					return result;
				}

				free( componentData );

				break;
			}
		}

		if( !foundComponent )
		{
			fseek( fp, componentHeader.dataSize, SEEK_CUR );
		}
	}

	fclose( fp );

	return UF_RET_SUCCESS;

}

UF_RET_CODE UF_ApplyConfigComponent( UFConfigComponentHeader* header, void* data )
{
	UF_RET_CODE result;
	int i;

	UFConfigParameter* parameterConfig;

	switch( header->type )
	{
		case UF_CONFIG_PARAMETERS:
			parameterConfig = (UFConfigParameter*)data;

			UF_InitSysParameter();

			parameterConfig = (UFConfigParameter*)data;
			for( i = 0; i < parameterConfig->numOfParameter; i++ )
			{
				result = UF_SetSysParameter( parameterConfig->parameter[i].parameter, parameterConfig->parameter[i].value );

				if( result != UF_RET_SUCCESS && result != SFM_ERR_NOT_FOUND
					&& !(result == SFM_ERR_UNSUPPORTED && ((parameterConfig->parameter[i].parameter == UF_SYS_ENCRYPTION_MODE)
						|| (parameterConfig->parameter[i].parameter == UF_SYS_TEMPLATE_TYPE)
						|| (parameterConfig->parameter[i].parameter == UF_SYS_ENHANCED_PRIVACY))) )
				{
					return result;
				}
			}

			return UF_RET_SUCCESS;

	}

	return SFM_ERR_UNKNOWN;
}

UF_RET_CODE UF_MakeParameterConfiguration( UFConfigComponentHeader* configHeader, BYTE* configData )
{
	UF_RET_CODE result;

	int numOfParameter;
	int i = 0;

	UFConfigParameter* data;

	configHeader->type = UF_CONFIG_PARAMETERS;
	configHeader->dataSize = 0;
	configHeader->checksum = 0;

	numOfParameter = 0;

	while( s_SysParameter[i].parameter != -1 )
	{
		if( s_SysParameter[i].parameter != UF_SYS_FIRMWARE_VERSION
			&& s_SysParameter[i].parameter != UF_SYS_SERIAL_NUMBER
			&& s_SysParameter[i].parameter != UF_SYS_AVAILABLE_FINGER
			&& s_SysParameter[i].parameter != UF_SYS_SENSOR_TYPE
			&& s_SysParameter[i].parameter != UF_SYS_BUILD_NO )
		{
			numOfParameter++;
		}

		i++;
	}

	configHeader->dataSize = sizeof(int) + numOfParameter * sizeof(UFConfigParameterItem);

	data = (UFConfigParameter*)configData;
	data->numOfParameter = 0;
	i = 0;

	while( s_SysParameter[i].parameter != -1 )
	{
		if( s_SysParameter[i].parameter != UF_SYS_FIRMWARE_VERSION
			&& s_SysParameter[i].parameter != UF_SYS_SERIAL_NUMBER
			&& s_SysParameter[i].parameter != UF_SYS_AVAILABLE_FINGER
			&& s_SysParameter[i].parameter != UF_SYS_SENSOR_TYPE
			&& s_SysParameter[i].parameter != UF_SYS_BUILD_NO )
		{
			data->parameter[data->numOfParameter].parameter = (UF_SYS_PARAM)s_SysParameter[i].parameter;

			result = UF_GetSysParameter( (UF_SYS_PARAM)s_SysParameter[i].parameter, &(data->parameter[data->numOfParameter].value) );

			if( result != UF_RET_SUCCESS && result != SFM_ERR_NOT_FOUND )
			{
				return result;
			}

			data->numOfParameter++;
		}

		i++;
	}

	for( i = 0; i < configHeader->dataSize; i++ )
	{
		configHeader->checksum += configData[i];
	}

	return UF_RET_SUCCESS;

}

UF_RET_CODE UF_ResetSystemConfiguration(void)
{
    int i, ret;
    int tryCount = 0;
    UINT32 param = 0;
    UINT32 size = 0;
    BYTE flag = 0;
    BYTE packet[SFM_PACKET_LEN];
    int bytes_avail = 0;
    BYTE end_code;

    UINT32 timeout = 10000;

    UF_SendPacket( SFM_COM_FR , param, size, flag, 10000 );

    UF_ReceivePacket(packet, timeout );
    return UF_RET_SUCCESS;
}
