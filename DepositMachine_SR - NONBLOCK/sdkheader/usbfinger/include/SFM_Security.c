#include <stdlib.h>
#include <pthread.h>


#include "SFM_Security.h"

SFM_RET_CODE SFM_SetEncryptionKey(BYTE* enc_keys)
{
	SFM_RET_CODE result;

	BYTE rcv_packet[SFM_PACKET_LEN];
	int flag;

    UINT32 timeout = 10000;

	result = UF_SendPacket( SFM_COM_KW , 0, 0, 0, 10000 );
	if( result != SFM_RET_SUCCESS )
	{
		return result;
	}

	result = UF_SendRawData(enc_keys, 32, 10000);
	if( result != SFM_RET_SUCCESS )
	{
		return result;
	}

	result = UF_ReceivePacket(rcv_packet, timeout );
	if (result == SFM_RET_SUCCESS) {
		flag = SFM_GetPacketValue(SFM_PACKET_FLAG, rcv_packet);
		return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE)flag );
	}
	
	return result;
}