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

    File : SFM_Identify.c
    Descriptions : Identify API
    API list
    - UF_Identify
    - UF_IdentifyTemplate
    - UF_IdentifyImage
    - UF_SetIdentifyCallback

    New API
    - SFM_IdentifyImageEx
*/

#include <stdlib.h>
#include <dirent.h>

#include "SFM_Command.h"
#include "SFM_Packet.h"

#include "SFM_Flag.h"
#include "SFM_SysParameter.h"

#include "SFM_Identify.h"
#include "SFM_Template.h"


void (*s_IdentifyCallback)( BYTE ) = NULL;

void UF_SetIdentifyCallback( void (*Callback)( BYTE ) )
{
	s_IdentifyCallback = Callback;
}

/**
 * 	Message callback for identification
 */
BOOL UF_IdentifyMsgCallback( BYTE errCode )
{
	if( errCode == SFM_PROTO_RET_SCAN_SUCCESS )
	{
		if( s_IdentifyCallback )
		{
			(*s_IdentifyCallback)( errCode );
		}

		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

UF_RET_CODE UF_Identify( UINT32* userID, BYTE* subID )
{
    UF_RET_CODE result;

	UINT32 param = 0;
	UINT32 size = 0;
	BYTE flag = 0;

    if(g_bConnectionStatus == 0) {
        return SFM_ERR_CANNOT_OPEN_SERIAL;
    }

    SFM_RS232_ClearBuffer();

	result = UF_CommandEx( SFM_COM_IS, &param, &size, &flag, UF_IdentifyMsgCallback );

	if( result != SFM_RET_SUCCESS )
	{
		return result;
	}
	else if( flag != SFM_PROTO_RET_SUCCESS )
	{
		return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE)flag );
	}

	*userID = param;
	*subID = size;

	return UF_RET_SUCCESS;
}

UF_RET_CODE UF_IdentifyTemplate( UINT32 templateSize, BYTE* templateData,  UINT32* userID, BYTE* subID )
{
    UF_RET_CODE result;

	UINT32 param = 0;
	UINT32 size = templateSize;
	BYTE flag = 0;

	result = UF_CommandSendDataEx( SFM_COM_IT, &param, &size, &flag, templateData, templateSize, UF_IdentifyMsgCallback, TRUE ); // FALSE );

	*userID = param;
	*subID = size;

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

/**
 *	Identify an image
 */
UF_RET_CODE UF_IdentifyImage( UINT32 imageSize, BYTE* imageData,  UINT32* userID, BYTE* subID )
{
    UF_RET_CODE result;

	UINT32 param = 0;
	UINT32 size = imageSize;
	BYTE flag = 0;
	BYTE packet[SFM_NETWORK_PACKET_LEN];

    if(g_bConnectionStatus == 0) {
        return SFM_ERR_CANNOT_OPEN_SERIAL;
    }

    SFM_RS232_ClearBuffer();

	result = UF_Command( SFM_COM_IIX, &param, &size, &flag );

	if( result != UF_RET_SUCCESS )
	{
		return result;
	}

	if( flag == SFM_PROTO_RET_SUCCESS )
	{
		result = UF_SendDataPacket( SFM_COM_IIX, imageData, imageSize, UF_GetDefaultPacketSize() );

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

				*userID = SFM_GetPacketValue( SFM_PACKET_PARAM, packet );
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

				*userID = UF_GetNetworkPacketValue( SFM_PACKET_FLAG, packet );
				*subID = UF_GetNetworkPacketValue( SFM_PACKET_SIZE, packet );
				break;
			}
		}

		return UF_RET_SUCCESS;
	}
	else if( flag == SFM_PROTO_RET_UNSUPPORTED )
	{
		param = 0;
		size = imageSize;
		flag = 0;

		result = UF_CommandSendDataEx( SFM_COM_II, &param, &size, &flag, imageData, imageSize, UF_IdentifyMsgCallback, TRUE ); // FALSE );

		if( result != UF_RET_SUCCESS )
		{
			return result;
		}
		else if( flag != SFM_PROTO_RET_SUCCESS )
		{
			return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE)flag );
		}

		*userID = param;
		*subID = size;

		return UF_RET_SUCCESS;
	}
}


SFM_RET_CODE SFM_IdentifyImage( UINT32 imageSize, BYTE* imageData,  UINT32* userID, BYTE* subID )
{
    SFM_RET_CODE result;

    int param = 0;
    int size = imageSize;
    int flag = 0;
	UINT32 timeout = 10000;

    BYTE snd_packet[SFM_PACKET_LEN];
    BYTE rcv_packet[SFM_PACKET_LEN];

    if(g_bConnectionStatus == 0) {
        return SFM_ERR_CANNOT_OPEN_SERIAL;
    }
    
    SFM_RS232_ClearBuffer();

	UF_SendPacket( SFM_COM_II , param, size, flag, 10000 );

    UF_SendRawData(imageData, size, 30000);
    UF_ReceivePacket(rcv_packet, timeout );
    
    flag = (SFM_GetPacketValue(SFM_PACKET_FLAG, rcv_packet));
    if(SFM_PROTO_RET_SCAN_SUCCESS != flag) {
        SFM_RS232_ClearBuffer();
        return SFM_ERR_SCAN_FAIL;
    }

    UF_ReceivePacket(rcv_packet, timeout );
    flag = (SFM_GetPacketValue(SFM_PACKET_FLAG, rcv_packet));
    if(SFM_PROTO_RET_SUCCESS == flag) {
        *userID = SFM_GetPacketValue(SFM_PACKET_PARAM, rcv_packet);
        *subID = SFM_GetPacketValue(SFM_PACKET_SIZE, rcv_packet);
        SFM_RS232_ClearBuffer();
        return SFM_RET_SUCCESS;
    }
}

SFM_RET_CODE SFM_IdentifyImageEx(UINT32 imageSize, BYTE* imageData, UINT32* userID, BYTE* subID)
{
    SFM_RET_CODE result;

	int i;
    int param = 0;
    int size = imageSize;
    int flag = 0;
    int const_data_packet_size = 0x1000;
    UINT32 timeout = 10000;

    BYTE snd_packet[SFM_PACKET_LEN];
    BYTE rcv_packet[SFM_PACKET_LEN];

    if(g_bConnectionStatus == 0) {
        return SFM_ERR_CANNOT_OPEN_SERIAL;
    }
    
    SFM_RS232_ClearBuffer();

	UF_SendPacket( SFM_COM_IIX , param, size, flag, 10000 );

    UF_ReceivePacket(rcv_packet,  timeout );

    flag = SFM_GetPacketValue(SFM_PACKET_FLAG, rcv_packet);

    if (flag == SFM_PROTO_RET_SUCCESS) {

	    result = UF_SendDataPacket( SFM_COM_IIX , imageData, size, const_data_packet_size);

        if(result != SFM_RET_SUCCESS) {
            SFM_RS232_ClearBuffer();
            return result;
        }

        UF_ReceivePacket(rcv_packet, timeout );
        flag = (SFM_GetPacketValue(SFM_PACKET_FLAG, rcv_packet));
        if(SFM_PROTO_RET_SCAN_SUCCESS != flag) {
            SFM_RS232_ClearBuffer();
            return SFM_ERR_SCAN_FAIL;
        }

        UF_ReceivePacket(rcv_packet, timeout );

        flag = (SFM_GetPacketValue(SFM_PACKET_FLAG, rcv_packet));
        if(SFM_PROTO_RET_SUCCESS == flag) {
            *userID = SFM_GetPacketValue(SFM_PACKET_PARAM, rcv_packet);
            *subID = SFM_GetPacketValue(SFM_PACKET_SIZE, rcv_packet);
            SFM_RS232_ClearBuffer();
            return SFM_RET_SUCCESS;
        }
    } 
}
