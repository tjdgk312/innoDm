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

    File : SFM_Verify.c
    Descriptions : Verify API
    API list
    - UF_Verify
    - UF_VerifyTemplate
    - UF_VerifyHostTemplate
    - UF_VerifyImage
    - UF_SetVerifyCallback

    New API
    - SFM_VerifyImageEx
*/

#include "SFM_Verify.h"

void (*s_VerifyCallback)( BYTE ) = NULL;

/**
 * 	Set verification callback
 */
void UF_SetVerifyCallback( void (*Callback)( BYTE ) )
{
	s_VerifyCallback = Callback;
}

/**
 * 	Message callback for verification
 */
BOOL UF_VerifyMsgCallback( BYTE errCode )
{
	if( errCode == SFM_PROTO_RET_SCAN_SUCCESS )
	{
		if( s_VerifyCallback )
		{
			(*s_VerifyCallback)( errCode );
		}

		return FALSE;
	} else {
		return TRUE;
	}
}

UF_RET_CODE UF_Verify( UINT32 userID, BYTE* subID )
{
	UF_RET_CODE result;

	UINT32 param = userID;
	UINT32 size = 0;
	BYTE flag = 0;

    if(g_bConnectionStatus == 0) {
        return UF_ERR_CANNOT_OPEN_SERIAL;
    }

    SFM_RS232_ClearBuffer();

	result = UF_CommandEx( SFM_COM_VS, &param, &size, &flag, UF_VerifyMsgCallback );

	if( result != UF_RET_SUCCESS )
	{
		return result;
	}
	else if( flag != SFM_PROTO_RET_SUCCESS )
	{
		return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE)flag );
	}

	*subID = size;

	return UF_RET_SUCCESS;
}

UF_RET_CODE UF_VerifyImage( UINT32 imageSize, BYTE* imageData,  UINT32 userID, BYTE* subID )
{
	UF_RET_CODE result;

	UINT32 param = userID;
	UINT32 size = imageSize;
	BYTE flag = 0;
	BYTE packet[SFM_NETWORK_PACKET_LEN];

    if(g_bConnectionStatus == 0) {
        return UF_ERR_CANNOT_OPEN_SERIAL;
    }

    SFM_RS232_ClearBuffer();

	result = UF_Command( SFM_COM_VIX, &param, &size, &flag );

	if( result != UF_RET_SUCCESS )
	{
		return result;
	}

	if( flag == SFM_PROTO_RET_SUCCESS )
	{
		result = UF_SendDataPacket( SFM_COM_VIX, imageData, imageSize, UF_GetDefaultPacketSize() );

		if( result != UF_RET_SUCCESS )
		{
			return result;
		}

		while( 1 )
		{
			if( UF_GetProtocol() == UF_SINGLE_PROTOCOL )
			{
				result = UF_ReceivePacket( packet, UF_GetInputCommandTimeout() );

				if( result != UF_RET_SUCCESS )
				{
					return result;
				}

				flag = SFM_GetPacketValue( SFM_PACKET_FLAG, packet );

				if( flag == SFM_PROTO_RET_SCAN_SUCCESS )
				{
					continue;
				}

				if( flag != SFM_PROTO_RET_SUCCESS )
				{
					return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE)flag );
				}

				*subID = SFM_GetPacketValue( SFM_PACKET_SIZE, packet );
				break;

			}
			else
			{
				result = UF_ReceiveNetworkPacket( packet, UF_GetInputCommandTimeout() );

				if( result != UF_RET_SUCCESS )
				{
					return result;
				}

				flag = UF_GetNetworkPacketValue( SFM_PACKET_FLAG, packet );

				if( flag == SFM_PROTO_RET_SCAN_SUCCESS )
				{
					continue;
				}

				if( flag != SFM_PROTO_RET_SUCCESS )
				{
					return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE)flag );
				}

				*subID = UF_GetNetworkPacketValue( SFM_PACKET_SIZE, packet );
				break;
			}
		}

		return UF_RET_SUCCESS;
	}
	else if( flag == SFM_PROTO_RET_UNSUPPORTED )
	{
		param = userID;
		size = imageSize;
		flag = 0;

		result = UF_CommandSendDataEx( SFM_COM_VI, &param, &size, &flag, imageData, imageSize, UF_VerifyMsgCallback, FALSE );

		if( result != UF_RET_SUCCESS )
		{
			return result;
		}
		else if( flag != SFM_PROTO_RET_SUCCESS )
		{
			return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE)flag );
		}

		*subID = size;

		return UF_RET_SUCCESS;
	}
}

UF_RET_CODE UF_VerifyTemplate( UINT32 templateSize, BYTE* templateData,  UINT32 userID, BYTE* subID )
{
	UF_RET_CODE result;

	UINT32 param = userID;
	UINT32 size = templateSize;
	BYTE flag = 0;

    if(g_bConnectionStatus == 0) {
        return UF_ERR_CANNOT_OPEN_SERIAL;
    }

    SFM_RS232_ClearBuffer();

	result = UF_CommandSendDataEx( SFM_COM_VT, &param, &size, &flag, templateData, templateSize, UF_VerifyMsgCallback, FALSE );

	if( result != UF_RET_SUCCESS )
	{
		return result;
	}
	else if( flag != SFM_PROTO_RET_SUCCESS )
	{
		return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE)flag );
	}

	*subID = size;

	return UF_RET_SUCCESS;
}

/**
 *	Verify host templates
 */
UF_RET_CODE UF_VerifyHostTemplate( UINT32 numOfTemplate, UINT32 templateSize, BYTE* templateData )
{
	UF_RET_CODE result;

	UINT32 param = numOfTemplate;
	UINT32 size = templateSize;
	BYTE flag = 0;

	BYTE* buffer;
	int i;

    if(g_bConnectionStatus == 0) {
        return UF_ERR_CANNOT_OPEN_SERIAL;
    }

    SFM_RS232_ClearBuffer();

	buffer = (BYTE*)malloc( numOfTemplate * (templateSize + 1) );
	if( !buffer )
	{
		return UF_ERR_OUT_OF_MEMORY;
	}

	for( i = 0; i < numOfTemplate; i++ )
	{
		memcpy( buffer + i * (templateSize + 1), templateData + i * templateSize, templateSize );
		buffer[i * (templateSize + 1) + templateSize] = SFM_PACKET_END_CODE;
	}

	result = UF_CommandSendDataEx( SFM_COM_VH, &param, &size, &flag, buffer, numOfTemplate * (templateSize + 1) - 1, UF_VerifyMsgCallback, TRUE );

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

SFM_RET_CODE SFM_VerifyImage( UINT32 imageSize, BYTE* imageData,  UINT32 userID, BYTE* subID )
{
    SFM_RET_CODE result;

    int size = imageSize;
    int flag;
    BYTE snd_packet[SFM_PACKET_LEN];
    BYTE rcv_packet[SFM_PACKET_LEN];
    UINT32 timeout = 10000;

    if(g_bConnectionStatus == 0) {
        return SFM_ERR_CANNOT_OPEN_SERIAL;
    }

    SFM_RS232_ClearBuffer();

    UF_SendPacket( SFM_COM_VI , userID, size, flag, 10000 );

    UF_SendRawData(imageData, size, 10000);

	UF_SendPacket( SFM_COM_VI , userID, size, flag, 10000 );

    result = UF_ReceivePacket(rcv_packet, timeout );
    flag = SFM_GetPacketValue(SFM_PACKET_FLAG, rcv_packet);
    if(flag != SFM_PROTO_RET_SCAN_SUCCESS) {
        SFM_RS232_ClearBuffer();
        return result;
    }

    result = UF_ReceivePacket(rcv_packet, timeout );
    flag = SFM_GetPacketValue(SFM_PACKET_FLAG, rcv_packet);
    if(flag != SFM_PROTO_RET_SUCCESS) {
        SFM_RS232_ClearBuffer();
        return result;
    } else {
        *subID = SFM_GetPacketValue(SFM_PACKET_SIZE, rcv_packet);
         SFM_RS232_ClearBuffer();
         return result;
    }
}

SFM_RET_CODE SFM_VerifyImageEx(UINT32 imageSize, BYTE* imageData, UINT32 userID, BYTE* subID )
{
    SFM_RET_CODE result;

    int size = imageSize;
    int flag = 0;

    BYTE snd_packet[SFM_PACKET_LEN];
    BYTE rcv_packet[SFM_PACKET_LEN];

    UINT32 timeout = 10000;
    int const_data_packet_size = 0x1000;

    if(g_bConnectionStatus == 0) {
        return SFM_ERR_CANNOT_OPEN_SERIAL;
    }
    SFM_RS232_ClearBuffer();

    UF_SendPacket( SFM_COM_VIX , userID, size, flag, 10000 );

    result = UF_ReceivePacket(rcv_packet, timeout);
    flag = SFM_GetPacketValue(SFM_PACKET_FLAG, rcv_packet);

    if (flag == SFM_PROTO_RET_SUCCESS) {

        result = UF_SendDataPacket( SFM_COM_VIX , imageData, size, const_data_packet_size);

        free(imageData);

        if(result != SFM_RET_SUCCESS) {
            return result;
        }

        result = UF_ReceivePacket(rcv_packet, timeout);
        flag = SFM_GetPacketValue(SFM_PACKET_FLAG, rcv_packet);
        if(flag != SFM_PROTO_RET_SCAN_SUCCESS) {
            return result;
        }

        result = UF_ReceivePacket(rcv_packet, timeout);
        flag = SFM_GetPacketValue(SFM_PACKET_FLAG, rcv_packet);
        if(flag != SFM_PROTO_RET_SUCCESS) {
            return result;
        } else {
            *subID = SFM_GetPacketValue(SFM_PACKET_SIZE, rcv_packet);
            SFM_RS232_ClearBuffer();
            return result;
        }
    } else {
        SFM_RS232_ClearBuffer();
        return result;
    }  
}