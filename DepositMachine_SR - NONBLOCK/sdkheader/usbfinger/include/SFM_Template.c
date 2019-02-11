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


    File : SFM_Template.c
    Descriptions : Template Management API
    API list
	- UF_GetNumOfTemplate
	- UF_GetMaxNumOfTemplate
	- UF_GetAllUserInfo
	- UF_GetAllUserInfoEx
	- UF_SortUserInfo
	- UF_SetAdminLevel
	- UF_GetAdminLevel
	- UF_ClearAllAdminLevel
	- UF_CheckTemplate
	- UF_ReadTemplate
	- UF_ReadOneTemplate
	- UF_ScanTemplate
	- UF_FixProvisionalTemplate
	- UF_SetSecurityLevel
	- UF_GetSecurityLevel

	- UF_SetUserInfoCallback
	- UF_SetScanCallback

	New API
	- SFM_Read_template
	- UF_FormatUserDatabase

	Go to SFM_DB.c
	- UF_SaveDB		
	- UF_LoadDB
*/

#include "SFM_Module.h"
#include "SFM_Command.h"
#include "SFM_Packet.h"
#include "SFM_Error.h"
#include "SFM_Flag.h"
#include "SFM_SysParameter.h"

#include "SFM_Template.h"

void (*s_ScanCallback)( BYTE ) = NULL;
void (*s_UserInfoCallback)( int, int ) = NULL;

/**
 *	Set user info callback
 */
void UF_SetUserInfoCallback( void (*callback)( int index, int numOfTemplate ) )
{
	s_UserInfoCallback = callback;
}

/**
 * 	Set scan callback
 */
void UF_SetScanCallback( void (*Callback)( BYTE ) )
{
	s_ScanCallback = Callback;
}

/**
 * 	Message callback for scanning template
 */
BOOL UF_ScanMsgCallback( BYTE errCode )
{
	if( errCode == SFM_PROTO_RET_SCAN_SUCCESS ) {
		if( s_ScanCallback ) {
			(*s_ScanCallback)( errCode );
		}

		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

UF_RET_CODE UF_GetNumOfTemplate( UINT32* numOfTemplate )
{
	UF_RET_CODE result;

	UINT32 value;

	result = UF_GetSysParameter( UF_SYS_ENROLLED_FINGER, &value );

	if( result != UF_RET_SUCCESS )
	{
		return result;
	}

	*numOfTemplate = value;

	return UF_RET_SUCCESS;
}

UF_RET_CODE UF_GetMaxNumOfTemplate( UINT32* maxNumOfTemplate )
{
	UF_RET_CODE result;

	UINT32 value;

	result = UF_GetSysParameter( UF_SYS_AVAILABLE_FINGER, &value );

	if( result != UF_RET_SUCCESS )
	{
		return result;
	}

	*maxNumOfTemplate = value;

	result = UF_GetSysParameter( UF_SYS_ENROLLED_FINGER, &value );

	if( result != UF_RET_SUCCESS )
	{
		return result;
	}

	*maxNumOfTemplate += value;

	return UF_RET_SUCCESS;
}

UF_RET_CODE UF_GetAllUserInfo( UFUserInfo* userInfo, UINT32* numOfUser, UINT32* numOfTemplate )
{
	UF_RET_CODE result;

	UINT32 param = 0;
	UINT32 size = UF_GetDefaultPacketSize();
	BYTE flag = 0;
	BYTE* data;
	UINT32* userID;
	BOOL foundID;

	int i, j;
	int bufPos;
	int oldTimeout = UF_GetGenericCommandTimeout();

	UF_SetGenericCommandTimeout( 20000 );

	result = UF_Command( SFM_COM_LTX, &param, &size, &flag );
	if (result != UF_RET_SUCCESS) {
		UF_SetGenericCommandTimeout( oldTimeout );
		return result;
	}

	if( flag == SFM_PROTO_RET_SUCCESS ) {
		data = (BYTE*)malloc( size + 8 );

		if ( !data ) {
			UF_SetGenericCommandTimeout( oldTimeout );
			return SFM_ERR_OUT_OF_MEMORY;
		}

		result = UF_ReceiveDataPacket( SFM_COM_LTX, data, size );

		if ( result != UF_RET_SUCCESS ) {
			free(data);
			UF_SetGenericCommandTimeout(oldTimeout);
			return result;
		}

		*numOfTemplate = param;
		*numOfUser = 0;

		bufPos = 0;

		for( i = 0; i < *numOfTemplate; i++ )
		{
			foundID = FALSE;
			userID = (UINT32*)(data + bufPos);
		
			if( s_UserInfoCallback ) {
				(*s_UserInfoCallback)( i, *numOfTemplate );
			}

			for( j = 0; j < *numOfUser; j++ )
			{
				if( *userID == userInfo[j].userID ) {
					userInfo[j].numOfTemplate++;
					foundID = TRUE;
					break;
				}
			}

			if( !foundID ) {
				userInfo[*numOfUser].userID = *userID;
				userInfo[*numOfUser].numOfTemplate = 1;
				userInfo[*numOfUser].reserved[0] = (*(data + bufPos + 4) & 0xf0) >> 4; // security level
				userInfo[*numOfUser].adminLevel = (*(data + bufPos + 5) & 0x30) >> 4;
				
				(*numOfUser)++;
			}
			
			bufPos += 8;
		}
		
		free(data);
		UF_SetGenericCommandTimeout( oldTimeout );
		return UF_RET_SUCCESS;
	
	} else if( flag == SFM_PROTO_RET_UNSUPPORTED ) {
		UF_SetGenericCommandTimeout( oldTimeout );

		param = 0;
		size = 0;
		flag = 0;

		result = UF_Command(SFM_COM_LT, &param, &size, &flag );

		if (result != UF_RET_SUCCESS) {
			return result;
		}

		if (flag != SFM_PROTO_RET_SUCCESS) {
			return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE)flag );
		}

		if (param == 0) {
			*numOfUser = 0;
			*numOfTemplate = 0;

			return UF_RET_SUCCESS;
		}

		*numOfTemplate = param;

		data = (BYTE*)malloc(size);
		if(!data) {
			return SFM_ERR_OUT_OF_MEMORY;
		}

		result = UF_ReceiveRawData(data, size, UF_CalculateTimeout(size), TRUE );
		if (result != UF_RET_SUCCESS) {
			free( data );
			return result;
		}

		*numOfUser = 0;
		userID = (UINT32*)data;

		for (i = 0; i < *numOfTemplate; i++ ) {
			foundID = FALSE;

			if( s_UserInfoCallback ) {
				(*s_UserInfoCallback)( i, *numOfTemplate );
			}

			for( j = 0; j < *numOfUser; j++ )
			{
				if( userID[i] == userInfo[j].userID ) {
					userInfo[j].numOfTemplate++;
					foundID = TRUE;
					break;
				}
			}

			if( !foundID ) {
				userInfo[*numOfUser].userID = userID[i];
				userInfo[*numOfUser].numOfTemplate = 1;

				result = UF_GetAdminLevel( userID[i], (UF_ADMIN_LEVEL*)(&userInfo[*numOfUser].adminLevel) );

				if( result == SFM_ERR_UNSUPPORTED ){
					userInfo[*numOfUser].adminLevel = UF_ADMIN_LEVEL_NONE;
				} else if( result != UF_RET_SUCCESS ) {
					free( data );
					return result;
				}

				(*numOfUser)++;
			}
		}

		free(data);
		return UF_RET_SUCCESS;
	} else {
		return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE)flag );
	}
}

UF_RET_CODE UF_GetAllUserInfoEx( UFUserInfoEx* userInfo, UINT32* numOfUser, UINT32* numOfTemplate )
{
	UF_RET_CODE result;

	UINT32 param = 0;
	UINT32 size = UF_GetDefaultPacketSize();
	BYTE flag = 0;
	BYTE* data;
	UINT32* userID;
	BOOL foundID;
	int i, j;
	int bufPos;
	UF_ADMIN_LEVEL adminLevel;

	int oldTimeout = UF_GetGenericCommandTimeout();

	UF_SetGenericCommandTimeout( 10000 );

	result = UF_Command( SFM_COM_LTX, &param, &size, &flag );
	if( result != UF_RET_SUCCESS )
	{
		UF_SetGenericCommandTimeout( oldTimeout );
		return result;
	}

	if( flag == SFM_PROTO_RET_SUCCESS )
	{
		data = (BYTE*)malloc( size + 8 );

		if( !data )
		{
			UF_SetGenericCommandTimeout( oldTimeout );
			return SFM_ERR_OUT_OF_MEMORY;
		}

		result = UF_ReceiveDataPacket( SFM_COM_LTX, data, size );

		if( result != UF_RET_SUCCESS )
		{
			free( data );
			UF_SetGenericCommandTimeout( oldTimeout );
			return result;
		}

		*numOfTemplate = param;
		*numOfUser = 0;

		bufPos = 0;

		for( i = 0; i < *numOfTemplate; i++ )
		{
			foundID = FALSE;
			userID = (UINT32*)(data + bufPos);

			if( s_UserInfoCallback )
			{
				(*s_UserInfoCallback)( i, *numOfTemplate );
			}

			for( j = 0; j < *numOfUser; j++ )
			{
				if( *userID == userInfo[j].userID )
				{
					userInfo[j].duress[userInfo[j].numOfTemplate] = *(data + bufPos + 4 ) & 0x0f;
					userInfo[j].checksum[userInfo[j].numOfTemplate] = *(data + bufPos + 6);
					userInfo[j].checksum[userInfo[j].numOfTemplate] |= (unsigned)(*(data + bufPos + 7)) << 8;
					userInfo[j].numOfTemplate++;
					foundID = TRUE;
					break;
				}
			}

			if( !foundID )
			{
				userInfo[*numOfUser].userID = *userID;
				userInfo[*numOfUser].numOfTemplate = 1;
				userInfo[*numOfUser].duress[0] = *(data + bufPos + 4 ) & 0x0f;
				userInfo[*numOfUser].securityLevel = (*(data + bufPos + 4 ) & 0xf0) >> 4;
				userInfo[*numOfUser].adminLevel = (*(data + bufPos + 5) & 0x30) >> 4;
				userInfo[*numOfUser].authMode = (*(data + bufPos + 5) & 0xc0) >> 6;
				userInfo[*numOfUser].checksum[0] = *(data + bufPos + 6);
				userInfo[*numOfUser].checksum[0] |= (unsigned)(*(data + bufPos + 7)) << 8;

				(*numOfUser)++;
			}

			bufPos += 8;
		}

		free( data );

		UF_SetGenericCommandTimeout( oldTimeout );
		return UF_RET_SUCCESS;
	}
	else if( flag == SFM_PROTO_RET_UNSUPPORTED )
	{
		UF_SetGenericCommandTimeout( oldTimeout );

		param = 0;
		size = 0;
		flag = 0;

		result = UF_Command( SFM_COM_LT, &param, &size, &flag );

		if( result != UF_RET_SUCCESS )
		{
			return result;
		}

		if( flag != SFM_PROTO_RET_SUCCESS )
		{
			return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE)flag );
		}

		if( param == 0 ) // no user
		{
			*numOfUser = 0;
			*numOfTemplate = 0;

			return UF_RET_SUCCESS;
		}

		*numOfTemplate = param;


		data = (BYTE*)malloc( size );
		if( !data )
		{
			return SFM_ERR_OUT_OF_MEMORY;
		}

		result = UF_ReceiveRawData( data, size, UF_CalculateTimeout(size), TRUE );

		if( result != UF_RET_SUCCESS )
		{
			free( data );
			return result;
		}

		*numOfUser = 0;
		userID = (UINT32*)data;

		for( i = 0; i < *numOfTemplate; i++ )
		{
			foundID = FALSE;

			if( s_UserInfoCallback )
			{
				(*s_UserInfoCallback)( i, *numOfTemplate );
			}

			for( j = 0; j < *numOfUser; j++ )
			{
				if( userID[i] == userInfo[j].userID )
				{
					userInfo[j].numOfTemplate++;
					foundID = TRUE;
					break;
				}
			}

			if( !foundID )
			{
				userInfo[*numOfUser].userID = userID[i];
				userInfo[*numOfUser].numOfTemplate = 1;

				result = UF_GetAdminLevel( userID[i], &adminLevel );
				userInfo[*numOfUser].adminLevel = adminLevel;

				if( result == SFM_ERR_UNSUPPORTED )
				{
					userInfo[*numOfUser].adminLevel = UF_ADMIN_LEVEL_NONE;
				}
				else if( result != UF_RET_SUCCESS )
				{
					free( data );
					return result;
				}

				(*numOfUser)++;

			}
		}

		free( data );

		return UF_RET_SUCCESS;
	}
	else
	{
		return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE)flag );
	}

}

void UF_SortUserInfo( UFUserInfo* userInfo, int numOfUser )
{
	qsort( userInfo, numOfUser, sizeof(UFUserInfo), UF_SortByUserID );
}

int UF_SortByUserID( const void* elem1, const void* elem2 )
{
	UFUserInfo* info1 = (UFUserInfo*)elem1;
	UFUserInfo* info2 = (UFUserInfo*)elem2;

	return info1->userID - info2->userID;
}

UF_RET_CODE UF_SetAdminLevel( UINT32 userID, UF_ADMIN_LEVEL adminLevel )
{
	UF_RET_CODE result;

	BYTE flag;

	UINT32 param = userID;
	UINT32 size = adminLevel;

	result = UF_Command( SFM_COM_AW, &param, &size, &flag );

	if( result != UF_RET_SUCCESS )
	{
		return result;
	}

	if( flag == SFM_PROTO_RET_SUCCESS )
	{
		return UF_RET_SUCCESS;
	}
	else
	{
		return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE)flag );
	}
}

UF_RET_CODE UF_GetAdminLevel( UINT32 userID, UF_ADMIN_LEVEL* adminLevel )
{
	UF_RET_CODE result;

	BYTE flag;

	UINT32 param = userID;
	UINT32 size = 0;

	result = UF_Command( SFM_COM_AR, &param, &size, &flag );

	if( result != UF_RET_SUCCESS )
	{
		return result;
	}

	if( flag == SFM_PROTO_RET_SUCCESS )
	{
		*adminLevel = (UF_ADMIN_LEVEL)size;
		return UF_RET_SUCCESS;
	}
	else
	{
		return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE)flag );
	}
}

UF_RET_CODE UF_ClearAllAdminLevel(void)
{
	UF_RET_CODE result;

	BYTE flag = 0;
	UINT32 param = 0;
	UINT32 size = 0;

	result = UF_Command( SFM_COM_AC, &param, &size, &flag );

	if( result != UF_RET_SUCCESS )
	{
		return result;
	}

	if( flag == SFM_PROTO_RET_SUCCESS )
	{
		return UF_RET_SUCCESS;
	}
	else
	{
		return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE)flag );
	}
}

UF_RET_CODE UF_ScanTemplate( BYTE* templateData, UINT32* templateSize, UINT32* imageQuality )
{
	UF_RET_CODE result;

	UINT32 param = 0;
	UINT32 size = 0;
	BYTE flag = SFM_ADD_CHECKSUM;
	UINT32 version;
	BYTE buf[5];
	unsigned checksum = 0;
	int i;

	result = UF_GetSysParameter( UF_SYS_FIRMWARE_VERSION, &version );

	if( result != UF_RET_SUCCESS )
	{
		return result;
	}

    result = UF_CommandEx( SFM_COM_ST, &param, &size, &flag, UF_ScanMsgCallback );

	if( result != UF_RET_SUCCESS )
	{
		return result;
	}
	else if( flag != SFM_PROTO_RET_SUCCESS )
	{
		return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE)flag );
	}

	*templateSize = size;
	*imageQuality = param;

	if( size > 0 )
	{
		if( ((version & 0xff0000) >> 16) == '1' && ((version & 0xff00) >> 8) - '0' < UF_VERSION_1_5 )
		{
			result = UF_ReceiveRawData( templateData, size, UF_CalculateTimeout( size ), TRUE );

			if( result != UF_RET_SUCCESS )
			{
				return result;
			}
		}
		else
		{
			result = UF_ReceiveRawData( templateData, size, UF_CalculateTimeout( size ), FALSE );

			if( result != UF_RET_SUCCESS )
			{
				return result;
			}

//			result = UF_ReceiveRawData( buf, 4, UF_CalculateTimeout( 5 ), TRUE );

//			if( result != UF_RET_SUCCESS )
//			{
//				return result;
//			}

			for( i = 0; i < size; i++ )
			{
				checksum += templateData[i];
			}

			if( checksum != *((UINT32*)buf) )
			{
				return SFM_ERR_DATA_ERROR;
			}
		}
	}

	return UF_RET_SUCCESS;
}

/**
 * 	Read one template of a user ID
 */
UF_RET_CODE UF_ReadOneTemplate( UINT32 userID, int subID, BYTE* templateData )
{
	UF_RET_CODE result;

	UINT32 param = userID;
	UINT32 size = (subID << 16) | MAXIMUM_TEMPLATE_SIZE;
	BYTE flag = 1; // use sub index
	BYTE rcv_packet[SFM_PACKET_LEN];

	result = UF_Command(SFM_COM_RTX, &param, &size, &flag);

	if (result != UF_RET_SUCCESS) {
		return result; // UF_RET_SUCCESS
	}

	if (flag == SFM_PROTO_RET_SUCCESS) {
		result = UF_ReceiveDataPacket(SFM_COM_RTX, templateData, size & 0xffff);
		return result;
	} else {
		return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE)flag );
	}
}

/**
 * 	Read templates of a user ID
 */
UF_RET_CODE UF_ReadTemplate( UINT32 userID, UINT32* numOfTemplate, BYTE* templateData )
{
	UF_RET_CODE result;

	UINT32 param = userID;
	UINT32 size = MAXIMUM_TEMPLATE_SIZE;
	BYTE flag = 0;
	int bufPos = 0;
	int templateSize;
	BYTE packet[SFM_NETWORK_PACKET_LEN];

	result = UF_Command( SFM_COM_RTX, &param, &size, &flag );

	if( result != UF_RET_SUCCESS )
	{
		return result;
	}

	if( flag == SFM_PROTO_RET_SUCCESS )
	{
		*numOfTemplate = (size & 0xffff0000) >> 16;
		templateSize = size & 0xffff;
		
		return UF_ReceiveDataPacket( SFM_COM_RTX, templateData, *numOfTemplate * templateSize );

	} else {
		return result;
	}
}

UF_RET_CODE UF_CheckTemplate( UINT32 userID, UINT32* numOfTemplate )
{
	UF_RET_CODE result;

	UINT32 param = userID;
	UINT32 size = 0;
	BYTE flag = 0;

	result = UF_Command( SFM_COM_CT, &param, &size, &flag );

	if( result != UF_RET_SUCCESS )
	{
		return result;
	}

	if ( size > 0) {
		*numOfTemplate = size;
		return UF_RET_SUCCESS;
	}

	if( flag != SFM_PROTO_RET_EXIST_ID ) {
		return SFM_ERR_NOT_FOUND;
	}
}

UF_RET_CODE UF_FixProvisionalTemplate(void)
{
	UF_RET_CODE result;

	UINT32 param = 0;
	UINT32 size = 0;
	BYTE flag = 0;

	result = UF_Command( SFM_COM_FP, &param, &size, &flag );

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

/**
 *	Set security level of a user
 */
UF_RET_CODE UF_SetSecurityLevel( UINT32 userID, UF_USER_SECURITY_LEVEL securityLevel )
{
	UF_RET_CODE result;

	BYTE flag;
	UINT32 param = userID;
	UINT32 size = securityLevel;

	result = UF_Command( SFM_COM_WSL, &param, &size, &flag );

	if( result != UF_RET_SUCCESS )
	{
		return result;
	}

	if( flag == SFM_PROTO_RET_SUCCESS )
	{
		return UF_RET_SUCCESS;
	}
	else
	{
		return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE)flag );
	}
}

/**
 *	Get security level of a user
 */
UF_RET_CODE UF_GetSecurityLevel( UINT32 userID, UF_USER_SECURITY_LEVEL* securityLevel )
{
	UF_RET_CODE result;
	
	BYTE flag;
	UINT32 param = userID;
	UINT32 size = 0;

	result = UF_Command( SFM_COM_RSL, &param, &size, &flag );

	if( result != UF_RET_SUCCESS )
	{
		return result;
	}

	if( flag == SFM_PROTO_RET_SUCCESS )
	{
		*securityLevel = (UF_USER_SECURITY_LEVEL)size;
		return UF_RET_SUCCESS;
	}
	else
	{
		return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE)flag );
	}
}

UF_RET_CODE UF_FormatUserDatabase(void)
{
	int result;
	BYTE snd_packet[SFM_PACKET_LEN];
	BYTE rcv_packet[SFM_PACKET_LEN];

    UINT32 timeout = 10000;

	UF_SendPacket( SFM_COM_FF , 0, 0, 0, timeout );

	while(1) {
		UF_ReceivePacket(rcv_packet, timeout );
		result = SFM_GetPacketValue(SFM_PACKET_FLAG, rcv_packet);
		if(result == SFM_PROTO_RET_SUCCESS) {
			break;
		}
	}
	
	return UF_RET_SUCCESS;
}

SFM_RET_CODE SFM_Read_template(int userID, unsigned char* template1, unsigned char* template2)
{
	int readLen;
	char strID[10];
	int flag = 0;
	int size = 0;
	int i;

	int tryCount = 0;
	int bytes_avail = 0;

	BYTE snd_packet[SFM_PACKET_LEN];
	BYTE* rcv_packet;

    UINT32 timeout = 10000;

	rcv_packet = (BYTE *)malloc(1000);
	memset(rcv_packet, 0xFF, 1000);

	UF_SendPacket( SFM_COM_RT , userID, size, flag, 10000 );

	UF_ReceivePacket(rcv_packet, timeout );
	size = SFM_GetPacketValue(SFM_PACKET_SIZE, rcv_packet);

	for (i = 0; i < size + 1; i++) {
		while (1) {
			ioctl(g_fdRS232, FIONREAD, &bytes_avail);
            if (bytes_avail > 0)
                break;
            if (tryCount++ > TRIAL_COUNT)
                return SFM_ERR_CANNOT_READ_SERIAL;
		}
		readLen = read(g_fdRS232, template1 + i, 1);
		if (readLen == -1) {
            return SFM_ERR_CANNOT_READ_SERIAL;
        }
	}

	sleep(1); // Need sleep

	UF_ReceivePacket(rcv_packet, timeout );
	size = SFM_GetPacketValue(SFM_PACKET_SIZE, rcv_packet);

	for (i = 0; i < size + 1; i++) {
		while (1) {
			ioctl(g_fdRS232, FIONREAD, &bytes_avail);
            if (bytes_avail > 0)
                break;
            if (tryCount++ > TRIAL_COUNT)
                return SFM_ERR_CANNOT_READ_SERIAL;
		}
		readLen = read(g_fdRS232, template2 + i, 1);
		if (readLen == -1) {
            return SFM_ERR_CANNOT_READ_SERIAL;
        }
	}

	free(rcv_packet);

	return SFM_RET_SUCCESS;
}
