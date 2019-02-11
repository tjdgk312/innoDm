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

    File : SFM_Enroll.c
    Descriptions : Enroll API
    API list
    - UF_Enroll
    - UF_EnrollContinue
    - UF_EnrollAfterVerification
    - UF_EnrollTemplate
    - UF_EnrollMultipleTemplates
    - UF_EnrollImage
    - UF_SetEnrollCallback

    New API
    - SFM_Enroll_Scan
    - SFM_Enroll_Template
    - SFM_Enroll_ImageEx
    - SFM_Enroll_TemplateEx

*/

#include <stdlib.h>

#include "SFM_Command.h"
#include "SFM_Packet.h"
#include "SFM_Flag.h"

#include "SFM_Enroll.h"
#include "SFM_Image.h"

/* Static value */
static UF_ENROLL_MODE s_EnrollMode = UF_ENROLL_TWO_TEMPLATES1;
static int s_NumOfSuccess = 0;

static void (*s_EnrollCallback)( BYTE, UF_ENROLL_MODE enrollMode, int numOfSuccess ) = NULL;

/**
 * 	Set enroll callback
 */
void UF_SetEnrollCallback( void (*Callback)( BYTE, UF_ENROLL_MODE, int ) )
{
	s_EnrollCallback = Callback;
}

/**
 * 	Message callback for enroll
 */
BOOL UF_EnrollMsgCallback( BYTE errCode )
{
	if( errCode == SFM_PROTO_RET_SCAN_SUCCESS )
	{
		if( s_EnrollCallback )
		{
			(*s_EnrollCallback)( errCode, s_EnrollMode, s_NumOfSuccess );
		}

		return FALSE;
	}
	else if( errCode == SFM_PROTO_RET_SUCCESS )
	{
		if( s_EnrollMode == UF_ENROLL_ONE_TIME
			|| s_EnrollMode == UF_ENROLL_TWO_TEMPLATES2
			|| s_EnrollMode == UF_ENROLL_TWO_TIMES2 )
		{
			return TRUE;
		}

		if( ++s_NumOfSuccess == 2 )
		{
			return TRUE;
		}

		if( s_EnrollCallback )
		{
			(*s_EnrollCallback)( errCode, s_EnrollMode, s_NumOfSuccess );
		}

		return FALSE;
	}
	else if( errCode == SFM_PROTO_RET_CONTINUE )
	{
		if( s_EnrollCallback )
		{
			(*s_EnrollCallback)( errCode, s_EnrollMode, s_NumOfSuccess );
		}

		return FALSE;
	}
}

UF_RET_CODE UF_Enroll( UINT32 userID, UF_ENROLL_OPTION option, UINT32* enrollID, UINT32* imageQuality )
{
    UF_RET_CODE result;

	UINT32 param = userID;
	UINT32 size = 0;
	BYTE flag = option;

    result = UF_GetSysParameter( UF_SYS_ENROLL_MODE, (UINT32*)&s_EnrollMode );

	if( result != UF_RET_SUCCESS )
	{
		return result;
	}

	s_NumOfSuccess = 0;

	result = UF_CommandEx( SFM_COM_ES, &param, &size, &flag, UF_EnrollMsgCallback );

	if( result != UF_RET_SUCCESS )
	{
		return result;
	}
	else if( flag != SFM_PROTO_RET_SUCCESS )
	{
		return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE)flag );
	}

	*enrollID = param;
	*imageQuality = size;

	return UF_RET_SUCCESS;
}

/**
 *	Enroll continue for 2 times II and 2 templates II modes
 */
UF_RET_CODE UF_EnrollContinue( UINT32 userID,  UINT32* enrollID, UINT32* imageQuality )
{
    UF_RET_CODE result;

	UINT32 param = userID;
	UINT32 size = 0;
	BYTE flag = SFM_PROTO_RET_CONTINUE;

	result = UF_CommandEx( SFM_COM_ES, &param, &size, &flag, UF_EnrollMsgCallback );

	if( result != UF_RET_SUCCESS )
	{
		return result;
	}
	else if( flag != SFM_PROTO_RET_SUCCESS )
	{
		return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE)flag );
	}

	*enrollID = param;
	*imageQuality = size;

	return UF_RET_SUCCESS;
}

/**
 *	Enroll after administrator's verification
 */
UF_RET_CODE UF_EnrollAfterVerification( UINT32 userID, UF_ENROLL_OPTION option, UINT32* enrollID, UINT32* imageQuality )
{
    UF_RET_CODE result;

	UINT32 param = userID;
	UINT32 size = 0;
	BYTE flag = option;

    if(g_bConnectionStatus == 0)
    {
        return result;
    }

    SFM_RS232_ClearBuffer();

    result = UF_GetSysParameter( UF_SYS_ENROLL_MODE, (UINT32*)&s_EnrollMode );

	if( result != UF_RET_SUCCESS )
	{
		return result;
	}

	s_NumOfSuccess = 0;

	result = UF_CommandEx( SFM_COM_ESA, &param, &size, &flag, UF_EnrollMsgCallback );

	if( result != UF_RET_SUCCESS ) {
		return result;
	} else if( flag != SFM_PROTO_RET_SUCCESS ) {
		return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE)flag );
	}

	*enrollID = param;
	*imageQuality = size;

	return UF_RET_SUCCESS;
}

UF_RET_CODE UF_EnrollTemplate( UINT32 userID, UF_ENROLL_OPTION option, UINT32 templateSize, BYTE* templateData, UINT32* enrollID )
{
	UF_RET_CODE result;

	UINT32 param = userID;
	UINT32 size = templateSize;
	BYTE flag = option;

	s_EnrollMode = UF_ENROLL_ONE_TIME;

	result = UF_EnrollMultipleTemplates( userID, option, 1, templateSize, templateData, enrollID );

	if( result == SFM_ERR_UNSUPPORTED )
	{
		result = UF_CommandSendDataEx( SFM_COM_ET, &param, &size, &flag, templateData, templateSize, UF_EnrollMsgCallback, FALSE );

		if( result != UF_RET_SUCCESS )
		{
			return result;
		}

		else if( flag != SFM_PROTO_RET_SUCCESS )
		{
			return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE)flag );
		}

		*enrollID = param;

		return UF_RET_SUCCESS;
	}
	else
	{
		return result;
	}
}

/**
 *	Enroll multiple templates
 */
UF_RET_CODE UF_EnrollMultipleTemplates( UINT32 userID, UF_ENROLL_OPTION option, int numOfTemplate, UINT32 templateSize, BYTE* templateData, UINT32* enrollID )
{
    UF_RET_CODE result;

    int enroll_timeout = 10000;

	UINT32 param = userID;
	UINT32 size;
	BYTE flag = option;
	BYTE packet[SFM_NETWORK_PACKET_LEN];

    if(g_bConnectionStatus == 0) {
        return SFM_ERR_CANNOT_OPEN_SERIAL;
    }

    SFM_RS232_ClearBuffer();

    if( numOfTemplate > 1 ) {
        size = (numOfTemplate << 24) | templateSize;
    } else {
        size = templateSize;
    }

    s_EnrollMode = UF_ENROLL_ONE_TIME;

    result = UF_Command( SFM_COM_ETX, &param, &size, &flag );

    if (result != UF_RET_SUCCESS) {
        return result;
    }

    if (flag == SFM_PROTO_RET_SUCCESS) {
        result = UF_SendDataPacket(SFM_COM_ETX, templateData, templateSize * numOfTemplate, templateSize);

        if (result != UF_RET_SUCCESS) {
            return result;
        }

        while(1)
        {
            if( UF_GetProtocol() == UF_SINGLE_PROTOCOL ) {
                result = UF_ReceivePacket( packet, UF_GetGenericCommandTimeout() );
                
                if (result != UF_RET_SUCCESS) {
                    return result;
                }

                flag = SFM_GetPacketValue(SFM_PACKET_FLAG, packet);

                if ( flag == SFM_PROTO_RET_SCAN_SUCCESS) {
                    continue;
                }

                if ( flag != SFM_PROTO_RET_SUCCESS ) {
                    return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE)flag );
                }

                *enrollID = SFM_GetPacketValue(SFM_PACKET_PARAM, packet);
                break;

            } else {

            }
        }

        return UF_RET_SUCCESS;
    } else {
        return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE)flag );
    }
}

UF_RET_CODE UF_EnrollImage(UINT32 userID, UF_ENROLL_OPTION enroll_option, UINT32 imageSize, BYTE* imageData, UINT32 *enrollID, UINT32 *Image_Quality)
{
    UF_RET_CODE result;

    int enroll_mode;
    int index;
    int param = 0;
    int size = imageSize;
    int flag = enroll_option; // default : UF_ENROLL_AUTO_ID

    UINT32 enroll_timeout = 10000;
    BYTE snd_packet[SFM_PACKET_LEN];
    BYTE rcv_packet[SFM_PACKET_LEN];

    if(g_bConnectionStatus == 0)
    {
        return result;
    }

    SFM_RS232_ClearBuffer();

    UF_SendPacket( SFM_COM_EI , userID, size, flag, 10000 );

    UF_SendRawData(imageData, size, 10000);
    
    result = UF_ReceivePacket(rcv_packet, enroll_timeout);
    result = UF_ReceivePacket(rcv_packet, enroll_timeout);
    flag = SFM_GetPacketValue(SFM_PACKET_FLAG, rcv_packet);
    if(flag != SFM_PROTO_RET_SUCCESS) {
        SFM_RS232_ClearBuffer();
        return result;
    } else {
        *enrollID = SFM_GetPacketValue(SFM_PACKET_PARAM, rcv_packet);
        *Image_Quality = SFM_GetPacketValue(SFM_PACKET_SIZE, rcv_packet);

        SFM_RS232_ClearBuffer();
        return result;
    }
}

SFM_RET_CODE SFM_Enroll_Scan( UINT32 userID, UF_ENROLL_OPTION option, UINT32* enrollID, UINT32* imageQuality, int enroll_mode )
{
    SFM_RET_CODE result;

    int i;
    int index;
    int param = userID;
    int size = 0;
    int flag = option; // default : SFM_ENROLL_AUTO_ID
    UINT32  enroll_timeout = 10000;

    BYTE snd_packet[SFM_PACKET_LEN];
    BYTE rcv_packet[SFM_PACKET_LEN];

    if(g_bConnectionStatus == 0) 
    {
        return result;
    }

    SFM_RS232_ClearBuffer();

    /* Enroll by Scan */
	result = UF_SendPacket( SFM_COM_ES , param, size, flag, 10000 );
    if(result != SFM_RET_SUCCESS) {
        return result;
    }

    if(enroll_mode == UF_ENROLL_TWO_TEMPLATES1 || enroll_mode == UF_ENROLL_TWO_TIMES1)
    {

        result = UF_ReceivePacket(rcv_packet, enroll_timeout);

        flag = SFM_GetPacketValue(SFM_PACKET_FLAG, rcv_packet);
        if(flag != SFM_PROTO_RET_SCAN_SUCCESS) {

            SFM_RS232_ClearBuffer();
            return result;
        }

        result = UF_ReceivePacket(rcv_packet, enroll_timeout);
        flag = SFM_GetPacketValue(SFM_PACKET_FLAG, rcv_packet);
        if(flag != SFM_PROTO_RET_SUCCESS) {

            SFM_RS232_ClearBuffer();
            return result;
        } else {
            *enrollID = SFM_GetPacketValue(SFM_PACKET_PARAM, rcv_packet);
            *imageQuality = SFM_GetPacketValue(SFM_PACKET_SIZE, rcv_packet);
            SFM_RS232_ClearBuffer();
        }
    }

    if(enroll_mode == UF_ENROLL_TWO_TIMES2 || enroll_mode == UF_ENROLL_TWO_TEMPLATES2)
    {
        UF_SendPacket( SFM_COM_ES , param, size, UF_ENROLL_CONTINUE, 10000 );
    }

    memset(rcv_packet, 0x0, SFM_PACKET_LEN);

    result = UF_ReceivePacket(rcv_packet, enroll_timeout);

    flag = SFM_GetPacketValue(SFM_PACKET_FLAG, rcv_packet);
    if(flag != SFM_PROTO_RET_SCAN_SUCCESS) {
        SFM_RS232_ClearBuffer();
        return result;
    }

    result = UF_ReceivePacket(rcv_packet, enroll_timeout);

    flag = SFM_GetPacketValue(SFM_PACKET_FLAG, rcv_packet);
    if(flag != SFM_PROTO_RET_SUCCESS) {
        
        SFM_RS232_ClearBuffer();
        return result;
    } else {
        *enrollID = SFM_GetPacketValue(SFM_PACKET_PARAM, rcv_packet);
        *imageQuality = SFM_GetPacketValue(SFM_PACKET_SIZE, rcv_packet);

        SFM_RS232_ClearBuffer();
        return result;
    }
}

/* SFM6000 & SFMSlim */
SFM_RET_CODE SFM_Enroll_Template(UINT32 userID, UF_ENROLL_OPTION enroll_option, UINT32 templateSize, BYTE* templateData, UINT32 *enrollID, UINT32 *Number_Of_Feature)
{
    SFM_RET_CODE result;
    int enroll_mode;
    int index;
    int size = templateSize;
    int param = 0;
    int flag = enroll_option; // default : SFM_ENROLL_AUTO_ID
    UINT32  enroll_timeout = 10000; // 1000ms = 1sec
    BYTE packet[SFM_PACKET_LEN];
    BYTE rcv_packet[SFM_PACKET_LEN];
 
    if(g_bConnectionStatus == 0)
    {
        return result;
    }

    SFM_RS232_ClearBuffer();

    UF_SendPacket( SFM_COM_ET , userID, size, flag, 10000 );

    UF_SendRawData(templateData, size, 10000);

    result = UF_ReceivePacket(rcv_packet, enroll_timeout);

    flag = SFM_GetPacketValue(SFM_PACKET_FLAG, rcv_packet);
    if(flag != SFM_PROTO_RET_SCAN_SUCCESS) {
        SFM_RS232_ClearBuffer();
        return result;
    }

    result = UF_ReceivePacket(rcv_packet, enroll_timeout);
    flag = SFM_GetPacketValue(SFM_PACKET_FLAG, rcv_packet);
    if(flag == SFM_PROTO_RET_SUCCESS) {

        *enrollID = SFM_GetPacketValue(SFM_PACKET_PARAM, rcv_packet);
        *Number_Of_Feature = SFM_GetPacketValue(SFM_PACKET_SIZE, rcv_packet);

        SFM_RS232_ClearBuffer();
        return result;
    }
}

SFM_RET_CODE SFM_Enroll_ImageEx(UINT32 userID, UF_ENROLL_OPTION enroll_option, UINT32 imageSize, BYTE* imageData, UINT32 *enrollID, UINT32 *Image_Quality)
{
    SFM_RET_CODE result;
    int enroll_mode;
    int index;
    int param = 0;
    int size = imageSize;
    int flag = enroll_option;
    int const_data_packet_size = 0x1000;

    UINT32 enroll_timeout = 10000;

    BYTE snd_packet[SFM_PACKET_LEN];
    BYTE rcv_packet[SFM_PACKET_LEN];

    if(g_bConnectionStatus == 0)
    {
        return result;
    }

    SFM_RS232_ClearBuffer();

    UF_SendPacket( SFM_COM_EIX , userID, size, flag, 10000 );

    UF_ReceivePacket(rcv_packet, enroll_timeout);
    flag = (SFM_GetPacketValue(SFM_PACKET_FLAG, rcv_packet));

    if (flag == SFM_PROTO_RET_SUCCESS) {
        result = UF_SendDataPacket(SFM_COM_EIX, imageData, size, const_data_packet_size);

        if(result != SFM_RET_SUCCESS) {
            SFM_RS232_ClearBuffer();

            return result;
        }

        result = UF_ReceivePacket(rcv_packet, enroll_timeout);
        result = UF_ReceivePacket(rcv_packet, enroll_timeout);
        flag = SFM_GetPacketValue(SFM_PACKET_FLAG, rcv_packet);
        if(flag == SFM_PROTO_RET_SUCCESS) {

            *enrollID = SFM_GetPacketValue(SFM_PACKET_PARAM, rcv_packet);
            *Image_Quality = SFM_GetPacketValue(SFM_PACKET_SIZE, rcv_packet);
            SFM_RS232_ClearBuffer();

            return result;
        }
    }
}

/* SFM6000 & SFMSlim */
SFM_RET_CODE SFM_Enroll_TemplateEx(UINT32 userID, UF_ENROLL_OPTION enroll_option, UINT32 templateSize, BYTE* templateData, UINT32 *enrollID, UINT32 *Number_Of_Feature)
{
    SFM_RET_CODE result;
    int enroll_mode;
    int index;
    int param = 0;
    int size = MAXIMUM_TEMPLATE_SIZE;
    int flag = enroll_option;

    UINT32  enroll_timeout = 10000; // 1000ms = 1sec
    BYTE packet[SFM_PACKET_LEN];
    BYTE rcv_packet[SFM_PACKET_LEN];
    int Count;

    int num_of_packet;
    int const_data_packet_size = 0x1000;
    int data_packet_size;

    if(g_bConnectionStatus == 0)
    {
        return result;
    }

    SFM_RS232_ClearBuffer();

    num_of_packet = size / const_data_packet_size;
    if ((size % const_data_packet_size) > 0) {
        num_of_packet = num_of_packet + 1;
    }

    UF_SendPacket( SFM_COM_ETX , userID, size, flag, 10000 );

    UF_ReceivePacket(rcv_packet, enroll_timeout);

    flag = SFM_GetPacketValue(SFM_PACKET_FLAG, rcv_packet);

    if (flag == SFM_PROTO_RET_SUCCESS) {
        result = UF_SendDataPacket(SFM_COM_ETX, templateData, size, const_data_packet_size);

        if(result != SFM_RET_SUCCESS) {
            SFM_RS232_ClearBuffer();

            return result;
        }

        UF_ReceivePacket(rcv_packet, enroll_timeout);
        flag = SFM_GetPacketValue(SFM_PACKET_FLAG, rcv_packet);
        if (!flag) {
            SFM_RS232_ClearBuffer();
            return result;
        }

        UF_ReceivePacket(rcv_packet, enroll_timeout);
        flag = SFM_GetPacketValue(SFM_PACKET_FLAG, rcv_packet);
        if(flag == SFM_PROTO_RET_SUCCESS) {
            *enrollID = SFM_GetPacketValue(SFM_PACKET_PARAM, rcv_packet);
            *Number_Of_Feature = SFM_GetPacketValue(SFM_PACKET_SIZE, rcv_packet);
            SFM_RS232_ClearBuffer();

            return SFM_RET_SUCCESS;
        }
    }
}

SFM_RET_CODE Get_Enroll_Mode(int *enroll_mode)
{
    SFM_RET_CODE result;
    UINT32 value;
    BYTE rcv_packet[SFM_PACKET_LEN];

    /* ++ Get Enroll Mode ++ */
    result = UF_GetSysParameter( UF_SYS_ENROLL_MODE, &value );
    if(result != SFM_RET_SUCCESS) {
        return result;
    }

    *enroll_mode = value;

    /* -- Get Enroll Mode -- */

    return result;
}