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

    File : SFM_Delete.c
    Descriptions : Delete API
	API list
	- UF_Delete
	- UF_DeleteOneTemplate
	- UF_DeleteMultipleTemplates
	- UF_DeleteAll
	- UF_DeleteByScan
	- UF_SetDeleteCallback
	- UF_DeleteAllAfterVerification

*/

/*
 *	Protocol - Delete 
 */

#include <stdlib.h>

#include "SFM_Command.h"
#include "SFM_Packet.h"
#include "SFM_Flag.h"
#include "SFM_SysParameter.h"

#include "SFM_Delete.h"

//
// Static variables
//

static void (*s_DeleteCallback)( BYTE ) = NULL;

/**
 * 	Set delete callback
 */
void UF_SetDeleteCallback( void (*Callback)( BYTE ) )
{
	s_DeleteCallback = Callback;
}


/**
 * 	Message callback for delete
 */
BOOL UF_DeleteMsgCallback( BYTE errCode )
{
	if( errCode == SFM_PROTO_RET_SCAN_SUCCESS ) {
		if( s_DeleteCallback ) {
			(*s_DeleteCallback)( errCode );
		}

		return FALSE;
	} else if( errCode == SFM_PROTO_RET_CONTINUE ) {
		if( s_DeleteCallback ) {
			(*s_DeleteCallback)( errCode );
		}

		return FALSE;
	}

	return TRUE;
}

UF_RET_CODE UF_Delete(UINT32 userID)
{
	UF_RET_CODE result;

	UINT32 param = userID;
	UINT32 size = 0;
	BYTE flag = 0;
	BYTE packet[SFM_PACKET_LEN];

	UINT32 timeout = 10000;

	result = UF_SendPacket( SFM_COM_DT , param, size, flag, timeout );
	if( result != UF_RET_SUCCESS ) {
		return result;
	}

	UF_ReceivePacket(packet, timeout );

    return UF_RET_SUCCESS;
}

UF_RET_CODE UF_DeleteOneTemplate(UINT32 userID, int subID)
{
	UF_RET_CODE result;

	UINT32 param = userID;
	UINT32 size = subID;
	BYTE flag = SFM_DELETE_ONLY_ONE; // DELETE_ONLY_ONE
	BYTE packet[SFM_PACKET_LEN];

	UINT32 timeout = 10000;

	result = UF_SendPacket( SFM_COM_DT , param, size, flag, timeout );
	if( result != UF_RET_SUCCESS ) {
		return result;
	}

	UF_ReceivePacket(packet, timeout );

    return UF_RET_SUCCESS;
}

UF_RET_CODE UF_DeleteMultipleTemplates(UINT32 startUserID, UINT32 lastUserID, int* deletedUserID )
{
	UF_RET_CODE result;

	char strID[10];
	UINT32 userID = 0;
	UINT32 to_userID = 0;
	UINT32 size = 0;
	BYTE flag = SFM_DELETE_MULTIPLE_ID; // DELETE_MULTIPLE_ID
	BYTE packet[SFM_PACKET_LEN];
	BYTE rcv_packet[SFM_PACKET_LEN];

	UINT32 timeout = 10000;

	result = UF_SendPacket( SFM_COM_DT , startUserID, lastUserID, flag, timeout );
	if( result != UF_RET_SUCCESS ) {
		return result;
	}

	UF_ReceivePacket(rcv_packet, timeout );
	size = SFM_GetPacketValue(SFM_PACKET_SIZE, rcv_packet);
	*deletedUserID = size;

    return UF_RET_SUCCESS;
}

UF_RET_CODE UF_DeleteAll(void)
{
	UF_RET_CODE result;

	int DA_Timeout = 300 * 1000;
	UINT32 param = 0;
	UINT32 size = 0;
	BYTE flag = 0;
	BYTE packet[SFM_PACKET_SIZE];
	BYTE rcv_packet[SFM_PACKET_SIZE];

	UINT32 timeout = 10000;

	result = UF_SendPacket( SFM_COM_DA , 0, size, flag, timeout );
	if( result != UF_RET_SUCCESS ) {
		return result;
	}

	UF_ReceivePacket(packet, timeout);

	return UF_RET_SUCCESS;
}

UF_RET_CODE UF_DeleteByScan(UINT32* userID)
{
	UF_RET_CODE result;
	int flag;

	BYTE packet[SFM_PACKET_LEN];

	UINT32 timeout = 10000;

	result = UF_SendPacket( SFM_COM_DS , 0, 0, 0, timeout );
	if( result != UF_RET_SUCCESS ) {
		return result;
	}

	result = UF_ReceivePacket(packet, timeout );

	flag = SFM_GetPacketValue(SFM_PACKET_FLAG, packet);
    if( flag != SFM_PROTO_RET_SCAN_SUCCESS) {
        return result;
    }

	UF_ReceivePacket(packet, timeout );

	*userID = SFM_GetPacketValue(SFM_PACKET_PARAM, packet);

	return UF_RET_SUCCESS;
}

/**
 *	Delete all templates after admin's verification
 */
UF_RET_CODE UF_DeleteAllAfterVerification(void)
{
	UF_RET_CODE result;

	UINT32 param = 0;
	UINT32 size = 0;
	BYTE flag = 0;

	result = UF_CommandEx( SFM_COM_DAA, &param, &size, &flag, UF_DeleteMsgCallback );
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
