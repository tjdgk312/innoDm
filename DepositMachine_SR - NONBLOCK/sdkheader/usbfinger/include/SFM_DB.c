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

    File : SFM_DB.c
    Descriptions : Template DB API
    API list
    - UF_SaveDB
	- UF_LoadDB

*/

#include <stdlib.h>
#include <pthread.h>

#include "SFM_Module.h"
#include "SFM_Command.h"
#include "SFM_Packet.h"
#include "SFM_Flag.h"
#include "SFM_SysParameter.h"

#include "SFM_Enroll.h"
#include "SFM_Template.h"

#define UF_VALID_TEMPLATE_DB	0x1f2f3f4f

extern void (*s_ScanCallback)( BYTE );
extern void (*s_UserInfoCallback)( int, int );
extern void UF_SetScanCallback( void (*Callback)( BYTE ) );
extern BOOL UF_ScanMsgCallback( BYTE errCode );

/**
 *	Set auth type of a user
 */
UF_RET_CODE UF_SetAuthType( UINT32 userID, UF_AUTH_TYPE authType )
{
	UF_RET_CODE result;

	BYTE flag;
	UINT32 param = userID;
	UINT32 size = authType;

	result = UF_Command( SFM_COM_UW, &param, &size, &flag );

	if( result != UF_RET_SUCCESS )
	{
		return result;
	}

	if( flag == UF_PROTO_RET_SUCCESS )
	{
		return UF_RET_SUCCESS;
	}
	else
	{
		return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE)flag );
	}
}

/**
 *	Get auth type of a user
 */
UF_RET_CODE UF_GetAuthType( UINT32 userID, UF_AUTH_TYPE* authType )
{
	UF_RET_CODE result;

	BYTE flag;
	UINT32 param = userID;
	UINT32 size = 0;

	result = UF_Command( SFM_COM_UR, &param, &size, &flag );

	if( result != UF_RET_SUCCESS )
	{
		return result;
	}

	if( flag == UF_PROTO_RET_SUCCESS )
	{
		*authType = (UF_AUTH_TYPE)size;
		return UF_RET_SUCCESS;
	}
	else
	{
		return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE)flag );
	}
}

/**
 *	Get user IDs of a specific auth type
 */
UF_RET_CODE UF_GetUserIDByAuthType( UF_AUTH_TYPE authType, int* numOfID, UINT32* userID )
{
	UF_RET_CODE result;

	UINT32 param = 0;
	UINT32 size = authType;
	BYTE flag = 0;

	result = UF_Command( SFM_COM_UL, &param, &size, &flag );

	if( result != UF_RET_SUCCESS )
	{
		return result;
	}

	if( flag != UF_PROTO_RET_SUCCESS )
	{
		return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE)flag );
	}

	if( param == 0 ) // no user
	{
		*numOfID = 0;

		return UF_RET_SUCCESS;
	}

	*numOfID = param;

	result = UF_ReceiveRawData( (BYTE* )userID, size, UF_CalculateTimeout(size), TRUE );

	if( result != UF_RET_SUCCESS )
	{
		return result;
	}

	return UF_RET_SUCCESS;
}

/**
 *	Reset auth type to AUTH_FINGERPRINT
 */
UF_RET_CODE UF_ResetAllAuthType()
{
	UF_RET_CODE result;

	UINT32 param = 0;
	UINT32 size = 0;
	BYTE flag = 0;

	result = UF_Command( SFM_COM_UC, &param, &size, &flag );

	if( result != UF_RET_SUCCESS )
	{
		return result;
	}

	if( flag == UF_PROTO_RET_SUCCESS )
	{
		return UF_RET_SUCCESS;
	}
	else
	{
		return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE)flag );
	}
}

/**
 *	Set entrance limit
 */
UF_RET_CODE UF_SetEntranceLimit( UINT32 userID, int entranceLimit )
{
	UF_RET_CODE result;

	UINT32 param = userID;
	UINT32 size = entranceLimit;
	BYTE flag = 0;

	result = UF_Command( SFM_COM_WME, &param, &size, &flag );

	if( result != UF_RET_SUCCESS )
	{
		return result;
	}

	if( flag == UF_PROTO_RET_SUCCESS )
	{
		return UF_RET_SUCCESS;
	}
	else
	{
		return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE)flag );
	}
}

/**
 *	Get entrance limit
 */
UF_RET_CODE UF_GetEntranceLimit( UINT32 userID, int* entranceLimit, int *entranceCount )
{
	UF_RET_CODE result;

	UINT32 param = userID;
	UINT32 size = 0;
	BYTE flag = 0;

	result = UF_Command( SFM_COM_RME, &param, &size, &flag );

	if( result != UF_RET_SUCCESS )
	{
		return result;
	}

	if( flag == UF_PROTO_RET_SUCCESS )
	{
		*entranceCount = param;
		*entranceLimit = size;
		return UF_RET_SUCCESS;
	}
	else
	{
		return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE)flag );
	}
}

/**
 *	Clear all entrance limits
 */
UF_RET_CODE UF_ClearAllEntranceLimit(void)
{
	UF_RET_CODE result;

	UINT32 param = 0;
	UINT32 size = 0;
	BYTE flag = 0;

	result = UF_Command( SFM_COM_CME, &param, &size, &flag );

	if( result != UF_RET_SUCCESS )
	{
		return result;
	}

	if( flag == UF_PROTO_RET_SUCCESS )
	{
		return UF_RET_SUCCESS;
	}
	else
	{
		return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE)flag );
	}
}

/**
 *	Save all the templates into a file
 */
UF_RET_CODE UF_SaveDB( const char* fileName )
{
	UF_RET_CODE result;

	int i, j;
	int maximumUser;
	int size;
	int entranceLimit, entranceCount;
	BYTE templateData[MAXIMUM_TEMPLATE_SIZE * 10 + 1];

	FILE* fp;
	UFTemplateDBHeader dbHeader;
	UFUserItemHeader userHeader;
	UFUserInfoEx* userInfo;

	memset( &dbHeader, 0, sizeof(dbHeader) );

	result = UF_GetSysParameter( UF_SYS_ENROLLED_FINGER, (UINT32*)&maximumUser );
	if( result != UF_RET_SUCCESS )
	{
		return result;
	}

	userInfo = (UFUserInfoEx*)malloc( maximumUser * sizeof(UFUserInfoEx) );
	if( !userInfo )
	{
		return UF_ERR_OUT_OF_MEMORY;
	}

	result = UF_GetAllUserInfoEx( userInfo, &dbHeader.numOfUser, &dbHeader.numOfTemplate );
	if( result != UF_RET_SUCCESS )
	{
		free( userInfo );
		return result;
	}

	result = UF_GetSysParameter( UF_SYS_TEMPLATE_SIZE, (UINT32*)&dbHeader.templateSize );
	if( result != UF_RET_SUCCESS )
	{
		free( userInfo );
		return result;
	}

	fp = fopen( fileName, "wb" );
	if( !fp )
	{
		free( userInfo );
		return UF_ERR_FILE_IO;
	}

	size = sizeof(dbHeader);

	if( fwrite( &dbHeader, 1, size, fp ) != size )
	{
		free( userInfo );
		fclose( fp );

		return UF_ERR_FILE_IO;
	}

	for( i = 0; i < dbHeader.numOfUser; i++ )
	{
		if( s_UserInfoCallback )
		{
			(*s_UserInfoCallback)( i, dbHeader.numOfUser );
		}

		userHeader.userID = userInfo[i].userID;
		userHeader.userLevel = userInfo[i].adminLevel;
		userHeader.userLevel = (unsigned)(userInfo[i].authMode) << 8;
		userHeader.userLevel |= (unsigned)(userInfo[i].securityLevel) << 16;

		result = UF_GetEntranceLimit( userHeader.userID, &entranceLimit, &entranceCount );

		if( result == UF_RET_SUCCESS )
		{
			userHeader.userLevel |= entranceLimit << 24;
		}

		userHeader.checksum = 0;

		result = UF_ReadTemplate( userInfo[i].userID, &userHeader.numOfTemplate, templateData );

		if( result != UF_RET_SUCCESS )
		{
			free( userInfo );
			fclose( fp );

			return result;
		}

		for( j = 0; j < userHeader.numOfTemplate * dbHeader.templateSize; j++ )
		{
			userHeader.checksum += templateData[j];
		}

		size = sizeof(userHeader);

		if( fwrite( &userHeader, 1, size, fp ) != size )
		{
			free( userInfo );
			fclose( fp );
			return UF_ERR_FILE_IO;
		}

		dbHeader.dataSize += size;

		size = userHeader.numOfTemplate * dbHeader.templateSize;

		if( fwrite( templateData, 1, size, fp ) != size )
		{
			free( userInfo );
			fclose( fp );
			return UF_ERR_FILE_IO;
		}

		dbHeader.dataSize += size;
	}

	dbHeader.magicNo = UF_VALID_TEMPLATE_DB;
	fseek( fp, 0, SEEK_SET );

	size = sizeof(dbHeader);
	if( fwrite( &dbHeader, 1, size, fp ) != size )
	{
		free( userInfo );
		fclose( fp );
		return UF_ERR_FILE_IO;
	}

	fclose( fp );
	free( userInfo );

	return UF_RET_SUCCESS;
}

/**
 *	Load all the templates from a file
 */
UF_RET_CODE UF_LoadDB( const char* fileName )
{
	UF_RET_CODE result;

	UINT32 userID;
	UINT32 checksum;
	UINT32 scanSuccess;
	UINT32 version;
	int i, j;
	int size;
	int flag;

	FILE* fp;
	UFTemplateDBHeader dbHeader;
	UFUserItemHeader userHeader;

	BYTE templateData[MAXIMUM_TEMPLATE_SIZE * 10];
	BYTE rcv_packet[SFM_PACKET_SIZE];

	fp = fopen( fileName, "rb" );
	if( !fp )
	{
		return UF_ERR_FILE_IO;
	}

	size = sizeof(dbHeader);
	if( fread( &dbHeader, 1, size, fp ) != size )
	{
		fclose( fp );
		return UF_ERR_FILE_IO;
	}

	if( dbHeader.magicNo != UF_VALID_TEMPLATE_DB )
	{
		fclose( fp );
		return UF_ERR_INVALID_FILE;
	}

	/* ++ Delete All ++*/
	result = UF_SendPacket( SFM_COM_DA , 0, 0, 0, 10000 );
	if (result != UF_RET_SUCCESS) {
		return result;
	}

	result = UF_ReceivePacket(rcv_packet, 10000);
	if (result != UF_RET_SUCCESS) {
		return result;
	}
	/* -- Delete All --*/

	result = UF_GetSysParameter( UF_SYS_FIRMWARE_VERSION, &version );

	if( result != UF_RET_SUCCESS )
	{
		fclose( fp );
		return result;
	}

	result = UF_GetSysParameter( UF_SYS_SEND_SCAN_SUCCESS, &scanSuccess );

	if( result != UF_RET_SUCCESS )
	{
		fclose( fp );
		return result;
	}

	UF_SetSysParameter( UF_SYS_SEND_SCAN_SUCCESS, 0x30 );

	for( i = 0; i < dbHeader.numOfUser; i++ )
	{
		if( s_UserInfoCallback )
		{
			(*s_UserInfoCallback)( i, dbHeader.numOfUser );
		}

		size = sizeof(userHeader);

		if( fread( &userHeader, 1, size, fp ) != size )
		{
			fclose( fp );
			UF_SetSysParameter( UF_SYS_SEND_SCAN_SUCCESS, scanSuccess );
			return UF_ERR_FILE_IO;
		}

		size = userHeader.numOfTemplate * dbHeader.templateSize;

		if( fread( templateData, 1, size, fp ) != size )
		{
			fclose( fp );
			UF_SetSysParameter( UF_SYS_SEND_SCAN_SUCCESS, scanSuccess );
			return UF_ERR_FILE_IO;
		}

		checksum = 0;

		for( j = 0; j < size; j++ )
		{
			checksum += templateData[j];
		}

		if( checksum != userHeader.checksum )
		{
			fclose( fp );
			UF_SetSysParameter( UF_SYS_SEND_SCAN_SUCCESS, scanSuccess );
			return UF_ERR_CHECKSUM_ERROR;
		}

#if 0
		if( ((version & 0xff00) >> 8) - '0' < UF_VERSION_1_5 )
		{
			for( j = 0; j < userHeader.numOfTemplate; j++ )
			{
				result = UF_EnrollTemplate( userHeader.userID, UF_ENROLL_ADD_NEW, dbHeader.templateSize, templateData + j * dbHeader.templateSize, &userID );
				if( result != UF_RET_SUCCESS )
				{
					fclose( fp );
					UF_SetSysParameter( UF_SYS_SEND_SCAN_SUCCESS, scanSuccess );
					return result;
				}
			}
		}
		else
		{
			result = UF_EnrollMultipleTemplates( userHeader.userID, UF_ENROLL_ADD_NEW, userHeader.numOfTemplate, dbHeader.templateSize, templateData, &userID );
			if( result != UF_RET_SUCCESS )
			{
				fclose( fp );
				UF_SetSysParameter( UF_SYS_SEND_SCAN_SUCCESS, scanSuccess );
				return result;
			}
		}
#else
		result = UF_EnrollMultipleTemplates( userHeader.userID, UF_ENROLL_ADD_NEW, userHeader.numOfTemplate, dbHeader.templateSize, templateData, &userID );
		if( result != UF_RET_SUCCESS )
		{
			fclose( fp );
			UF_SetSysParameter( UF_SYS_SEND_SCAN_SUCCESS, scanSuccess );
			return result;
		}

#endif
		result = UF_SetAdminLevel( userHeader.userID, (UF_ADMIN_LEVEL)(userHeader.userLevel & 0xff) );

		if( result != UF_RET_SUCCESS && result != UF_ERR_UNSUPPORTED )
		{
			fclose( fp );
			UF_SetSysParameter( UF_SYS_SEND_SCAN_SUCCESS, scanSuccess );
			return result;
		}

		result = UF_SetAuthType( userHeader.userID,(UF_AUTH_TYPE)((userHeader.userLevel & 0xff00) >> 8));

		if( result != UF_RET_SUCCESS && result != UF_ERR_UNSUPPORTED )
		{
			fclose( fp );
			UF_SetSysParameter( UF_SYS_SEND_SCAN_SUCCESS, scanSuccess );
			return result;
		}

		result = UF_SetSecurityLevel( userHeader.userID, (UF_USER_SECURITY_LEVEL)((userHeader.userLevel & 0x00ff0000) >> 16));

		if( result != UF_RET_SUCCESS && result != UF_ERR_UNSUPPORTED )
		{
			fclose( fp );
			UF_SetSysParameter( UF_SYS_SEND_SCAN_SUCCESS, scanSuccess );
			return result;
		}

		result = UF_SetEntranceLimit( userHeader.userID, (userHeader.userLevel & 0xff000000) >> 24 );

		if( result != UF_RET_SUCCESS && result != UF_ERR_UNSUPPORTED )
		{
			fclose( fp );
			UF_SetSysParameter( UF_SYS_SEND_SCAN_SUCCESS, scanSuccess );
			return result;
		}

	}

	fclose( fp );

	UF_SetSysParameter( UF_SYS_SEND_SCAN_SUCCESS, scanSuccess );

	return UF_RET_SUCCESS;
}
