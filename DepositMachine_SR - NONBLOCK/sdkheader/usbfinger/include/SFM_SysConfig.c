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

#include "SFM_Connect.h"
#include "SFM_Serial.h"

extern int g_fdRS232;

/* Category : System Configuration 
 *            without System Parameter prtocol
 */

SFM_RET_CODE Check_System_Status(void)
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

    UINT32 Status_Module;
    UINT32 Status_Sensor;

    UF_SendPacket( SFM_COM_SS , param, size, flag, timeout );

    UF_ReceivePacket(packet, timeout );
    Status_Module = SFM_GetPacketValue(SFM_PACKET_PARAM, packet);
    Status_Sensor = SFM_GetPacketValue(SFM_PACKET_SIZE, packet);

    return SFM_RET_SUCCESS;
}

SFM_RET_CODE Get_Module_ID(void)
{
    int i, ret;
    int tryCount = 0;
    UINT32 param = 0;
    UINT32 size = 0;
    BYTE flag = 0;
    BYTE packet[SFM_PACKET_LEN];
    UINT32 timeout = 10000;
    int bytes_avail = 0;
    BYTE end_code;

    UINT32 Status_Module;
    UINT32 Status_Sensor;

    UF_SendPacket( SFM_COM_ID , param, size, flag, timeout );

    UF_ReceivePacket(packet, timeout );
    
    return SFM_RET_SUCCESS;
}

SFM_RET_CODE Cancel(void)
{
    UINT32 timeout = 10000;

    UINT32 param = 0;
    UINT32 size = 0;
    BYTE flag = 0;
    BYTE packet[SFM_PACKET_LEN];

    UF_SendPacket( SFM_COM_CA , param, size, flag, 10000 );

    UF_ReceivePacket(packet, timeout ); /* Cancel Flag for Request command  */
    UF_ReceivePacket(packet, timeout ); /* Response for Cancle command  */

    return SFM_RET_SUCCESS;
}

UF_RET_CODE UF_Reset(void)
{
	UF_RET_CODE result;

	UINT32 param = 0;
	UINT32 size = 0;
	BYTE flag = 0;

	result = UF_Command( SFM_COM_RS, &param, &size, &flag );

	if( result != SFM_RET_SUCCESS ) {
		return result;
	}

	if( flag != SFM_PROTO_RET_SUCCESS ) {
		return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE)flag );
	}

	return SFM_RET_SUCCESS;
}

UF_RET_CODE UF_Upgrade( const char* firmwareFilename, int dataPacketSize )
{
	UF_RET_CODE result;
	
	int fileSize;
	UINT32 param = 0;
	UINT32 size = 0;
	BYTE flag = 0;
	BYTE* buf;
	BYTE packet[SFM_NETWORK_PACKET_LEN];

    struct stat stat_buf;

    if(g_bConnectionStatus == 0) {
        return SFM_ERR_CANNOT_OPEN_SERIAL;
    }
    
    SFM_RS232_ClearBuffer();

	FILE* fp = fopen( firmwareFilename, "rb" );

	if( !fp )
	{
		return SFM_ERR_FILE_IO;
	}

	stat( firmwareFilename, &stat_buf );

	fileSize = stat_buf.st_size;

	buf = (BYTE*)malloc( fileSize );

	if( !buf )
	{
		fclose( fp );
		return SFM_ERR_OUT_OF_MEMORY;
	}

	if( fread( buf, 1, fileSize, fp ) != fileSize )
	{
		fclose( fp );
		free( buf );

		return SFM_ERR_FILE_IO;
	}

	size = fileSize;
	result = UF_Command( SFM_COM_UG, &param, &size, &flag );

	if( result != UF_RET_SUCCESS )
	{
		fclose( fp );
		free( buf );

		return result;
	}
	else if( flag != SFM_PROTO_RET_SUCCESS )
	{
		fclose( fp );
		free( buf );

		return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE)flag );
	}

	result = UF_SendDataPacket( SFM_COM_UG, buf, fileSize, dataPacketSize );

	fclose( fp );
	free( buf );

	if( result != UF_RET_SUCCESS )
	{
		return result;
	}

	sleep(3); // wait for writing the firmware into flash

	if( UF_GetProtocol() == UF_SINGLE_PROTOCOL )
	{
		result = UF_ReceivePacket( packet, UF_GetGenericCommandTimeout() * 4 );	// Wait long time until complete upgrade

		if( result != UF_RET_SUCCESS )
		{
			return result;
		}
		else if( SFM_GetPacketValue( SFM_PACKET_FLAG, packet ) != SFM_PROTO_RET_SUCCESS )
		{
			return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE)SFM_GetPacketValue( SFM_PACKET_FLAG, packet ) );
		}
	}
	else
	{
		result = UF_ReceiveNetworkPacket( packet, UF_GetGenericCommandTimeout() * 4 );

		if( result != UF_RET_SUCCESS )
		{
			return result;
		}
		else if( UF_GetNetworkPacketValue( SFM_PACKET_FLAG, packet ) != SFM_PROTO_RET_SUCCESS )
		{
			return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE)UF_GetNetworkPacketValue( SFM_PACKET_FLAG, packet ) );
		}
	}
	return UF_RET_SUCCESS;
}