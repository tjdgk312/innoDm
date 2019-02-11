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

    File : SFM_Command.c
    Descriptions : General Command API
    API list
    - UF_Command
    - UF_CommandEx
    - UF_CommandSendData
    - UF_CommandSendDataEx

    - UF_Cancel
    - UF_SetProtocol
    - UF_GetProtocol
    - UF_GetModuleID
    - UF_SetGenericCommandTimeout
    - UF_SetInputCommandTimeout
    - UF_GetGenericCommandTimeout
    - UF_GetInputCommandTimeout

    - UF_SetNetworkDelay
    - UF_GetNetworkDelay
*/

#include "SFM_Packet.h"
#include "SFM_Command.h"

//
// Static variables
//
static UF_PROTOCOL s_Protocol = UF_SINGLE_PROTOCOL;
static UINT32 s_ModuleID = 0;

static int s_GenericCommandTimeout = 30000;
static int s_InputCommandTimeout = 30000;

static int s_NetworkDelay =  80;

/*
* UF_SetProtocol : sets the type of packet protocol.
*/
void UF_SetProtocol( UF_PROTOCOL protocol, UINT32 moduleID )
{
	if( protocol == UF_SINGLE_PROTOCOL || protocol == UF_NETWORK_PROTOCOL )
	{
		s_Protocol = protocol;
	}

	s_ModuleID = moduleID;
}

/*
* UF_GetProtocol: gets the type of packet protocol
*/
UF_PROTOCOL UF_GetProtocol(void)
{
	return s_Protocol;
}

/*
* UF_GetModuleID: gets the module ID.
*/
UINT32 UF_GetModuleID(void)
{
	return s_ModuleID;
}

UF_RET_CODE UF_Command(BYTE command, UINT32* param, UINT32* size, BYTE* flag )
{
    UF_RET_CODE result;

    BYTE packet[SFM_NETWORK_PACKET_LEN];

    SFM_RS232_ClearBuffer();

    if( s_Protocol == UF_SINGLE_PROTOCOL ) {
        result = UF_SendPacket( command, *param, *size, *flag, s_GenericCommandTimeout );

        if( result != UF_RET_SUCCESS )
        {
            return result;
        }

        do {
            result = UF_ReceivePacket( packet, s_GenericCommandTimeout );

            if( result != UF_RET_SUCCESS )
            {
                return result;
            }

            *param = SFM_GetPacketValue( SFM_PACKET_PARAM, packet );
            *size = SFM_GetPacketValue( SFM_PACKET_SIZE, packet );
            *flag = SFM_GetPacketValue( SFM_PACKET_FLAG, packet );

        } while( command != SFM_GetPacketValue( SFM_PACKET_COMMAND, packet ) );

        return UF_RET_SUCCESS;
    } else {
        usleep( s_NetworkDelay * 1000 );

        result = UF_SendNetworkPacket( command, s_ModuleID & 0xffff, *param, *size, *flag, s_GenericCommandTimeout );

		if( result != UF_RET_SUCCESS )
		{
			return result;
		}

//		Sleep( s_NetworkDelay );

		do {
			result = UF_ReceiveNetworkPacket( packet, s_GenericCommandTimeout );

			if( result != UF_RET_SUCCESS )
			{
				return result;
			}

			*param = UF_GetNetworkPacketValue( SFM_PACKET_PARAM, packet );
			*size = UF_GetNetworkPacketValue( SFM_PACKET_SIZE, packet );
			*flag = UF_GetNetworkPacketValue( SFM_PACKET_FLAG, packet );
		} while( command != UF_GetNetworkPacketValue( SFM_PACKET_COMMAND, packet ) );

		return UF_RET_SUCCESS;
    }
}

UF_RET_CODE UF_CommandEx( BYTE command, UINT32* param, UINT32* size, BYTE* flag, BOOL (*msgCallback)(BYTE) )
{
    UF_RET_CODE result;
    BYTE packet[SFM_NETWORK_PACKET_LEN];

	SFM_RS232_ClearBuffer();

    if( s_Protocol == UF_SINGLE_PROTOCOL ) {

        result = UF_SendPacket( command, *param, *size, *flag, s_GenericCommandTimeout );

        if( result != UF_RET_SUCCESS ) {
            return result;
        }

        while( 1 )
        {
            result = UF_ReceivePacket( packet, s_GenericCommandTimeout );

            if( result != UF_RET_SUCCESS )
            {
                return result;
            }

            if( !msgCallback )
            {
                break;
            }

            if( (*msgCallback)( SFM_GetPacketValue( SFM_PACKET_FLAG, packet ) ) )
            {
                break;
            }
        }

        *param = SFM_GetPacketValue( SFM_PACKET_PARAM, packet );
        *size = SFM_GetPacketValue( SFM_PACKET_SIZE, packet );
        *flag = SFM_GetPacketValue( SFM_PACKET_FLAG, packet );

        return UF_RET_SUCCESS;
    } else {

        usleep( s_NetworkDelay * 1000 );

        result = UF_SendNetworkPacket( command, s_ModuleID & 0xffff, *param, *size, *flag, s_GenericCommandTimeout );

		if( result != UF_RET_SUCCESS )
		{
			return result;
		}

//		Sleep( s_NetworkDelay );

		while( 1 )
		{
			result = UF_ReceiveNetworkPacket( packet, s_InputCommandTimeout );

			if( result != UF_RET_SUCCESS )
			{
				return result;
			}

			if( !msgCallback )
			{
				break;
			}

			if( (*msgCallback)( UF_GetNetworkPacketValue( SFM_PACKET_FLAG, packet ) ) )
			{
				break;
			}
		}

		*param = UF_GetNetworkPacketValue( SFM_PACKET_PARAM, packet );
		*size = UF_GetNetworkPacketValue( SFM_PACKET_SIZE, packet );
		*flag = UF_GetNetworkPacketValue( SFM_PACKET_FLAG, packet );

		return UF_RET_SUCCESS;
    }
}

UF_RET_CODE UF_CommandSendData( BYTE command, UINT32* param, UINT32* size, BYTE* flag, BYTE* data, UINT32 dataSize )
{
    UF_RET_CODE result;
    int timeout;

    BYTE packet[SFM_NETWORK_PACKET_LEN];

    SFM_RS232_ClearBuffer();

    if( s_Protocol == UF_SINGLE_PROTOCOL ) {
        result = UF_SendPacket( command, *param, *size, *flag, s_GenericCommandTimeout );

        if( result != UF_RET_SUCCESS ) {
            return result;
        }

        if( dataSize > 0 ) {
            timeout = UF_CalculateTimeout( dataSize );
            result = UF_SendRawData( data, dataSize, timeout );

            if(result != UF_RET_SUCCESS) {
                return result;
            }
        }

        result = UF_ReceivePacket( packet, s_GenericCommandTimeout );

        if (result != UF_RET_SUCCESS) {
            return result;
        }
        
    	*param = SFM_GetPacketValue( SFM_PACKET_PARAM, packet );
		*size = SFM_GetPacketValue( SFM_PACKET_PARAM, packet );
		*flag = SFM_GetPacketValue( SFM_PACKET_PARAM, packet );
        
        return UF_RET_SUCCESS;
    } else { // UF_NETWORK_PROTOCOL
        usleep( s_NetworkDelay * 1000 );

		result = UF_SendNetworkPacket( command, s_ModuleID & 0xffff, *param, *size, *flag, s_GenericCommandTimeout );

		if( result != UF_RET_SUCCESS )
		{
			return result;
		}

		if( dataSize > 0 )
		{
			timeout = UF_CalculateTimeout( dataSize );

			result = UF_SendRawData( data, dataSize, timeout );
			if( result != UF_RET_SUCCESS )
			{
				return result;
			}
		}

//		Sleep( s_NetworkDelay );

		result = UF_ReceiveNetworkPacket( packet, s_GenericCommandTimeout );

		if( result != UF_RET_SUCCESS )
		{
			return result;
		}

		*param = UF_GetNetworkPacketValue( SFM_PACKET_PARAM, packet );
		*size = UF_GetNetworkPacketValue( SFM_PACKET_SIZE, packet );
		*flag = UF_GetNetworkPacketValue( SFM_PACKET_FLAG, packet );

        return UF_RET_SUCCESS;
    }
}

UF_RET_CODE UF_CommandSendDataEx( BYTE command, UINT32* param, UINT32* size, BYTE* flag, BYTE* data, UINT32 dataSize, BOOL (*msgCallback)(BYTE), BOOL waitUserInput )
{
    UF_RET_CODE result;
    int timeout;

    BYTE packet[SFM_NETWORK_PACKET_LEN];

    SFM_RS232_ClearBuffer();

    if( s_Protocol == UF_SINGLE_PROTOCOL ) {
        result = UF_SendPacket( command, *param, *size, *flag, s_GenericCommandTimeout );

        if( result != UF_RET_SUCCESS ) {
            return result;
        }

        timeout = UF_CalculateTimeout(dataSize);

        result = UF_SendRawData(data, dataSize, timeout);
        if( result != UF_RET_SUCCESS ) {
            return result;
        }

        while(1)
        {
            timeout = (waitUserInput)? s_InputCommandTimeout : (s_GenericCommandTimeout * 2);
            result = UF_ReceivePacket(packet, timeout);

            if (result != UF_RET_SUCCESS ) {
                return result;
            }

            if (!msgCallback) {
                break;
            }

            if( (*msgCallback)( SFM_GetPacketValue( SFM_PACKET_FLAG, packet ) ) ) {
                break;
            }
        }

        *param = SFM_GetPacketValue(SFM_PACKET_PARAM, packet);
		*size = SFM_GetPacketValue( SFM_PACKET_SIZE, packet );
		*flag = SFM_GetPacketValue( SFM_PACKET_FLAG, packet );

        return UF_RET_SUCCESS;
    } else {
        usleep( s_NetworkDelay * 1000 );

        result = UF_SendNetworkPacket( command, s_ModuleID & 0xffff, *param, *size, *flag, s_GenericCommandTimeout );

        if( result != UF_RET_SUCCESS ) {
            return result;
        }

        while( 1 )
        {
            result = UF_ReceiveNetworkPacket( packet, s_InputCommandTimeout );

            if( result != UF_RET_SUCCESS ) {
                return result;
            }

            if (!msgCallback) {
                break;
            }

            if( (*msgCallback)( SFM_GetPacketValue( SFM_PACKET_FLAG, packet ) ) ) {
                break;
            }
        }

        *param = UF_GetNetworkPacketValue( SFM_PACKET_PARAM, packet );
        *size = UF_GetNetworkPacketValue( SFM_PACKET_SIZE, packet );
        *flag = UF_GetNetworkPacketValue( SFM_PACKET_FLAG, packet );

        return UF_RET_SUCCESS;
    }
}

UF_RET_CODE UF_Cancel(BOOL receivePacket)
{
    UF_RET_CODE result;

    BYTE packet[SFM_NETWORK_PACKET_LEN];
    BYTE errorCode;

    SFM_RS232_ClearBuffer();

    if( s_Protocol == UF_SINGLE_PROTOCOL ) {
        result = UF_SendPacket( SFM_COM_CA, 0, 0, 0, s_GenericCommandTimeout );

        if( result != UF_RET_SUCCESS ) {
            return result;
        }

        if( receivePacket ) {
            result = UF_ReceivePacket( packet, s_GenericCommandTimeout / 2);

            if (result != UF_RET_SUCCESS) {
                return result;
            }

            if ( SFM_GetPacketValue(SFM_PACKET_COMMAND, packet) != SFM_COM_CA ) {
                result = UF_ReceivePacket( packet, s_GenericCommandTimeout / 2);

                if (result != UF_RET_SUCCESS) {
                    return result;
                }
            }

            errorCode = SFM_GetPacketValue(SFM_PACKET_FLAG, packet);

            if (errorCode != UF_PROTO_RET_SUCCESS) {
                return UF_GetErrorCode((SFM_PROTOCOL_RET_CODE)errorCode);
            }
        }

        return UF_RET_SUCCESS;
    } else { // UF_NETWORK_PROTOCOL
        usleep( s_NetworkDelay * 1000 );

		result = UF_SendNetworkPacket( SFM_COM_CA, s_ModuleID & 0xffff, 0, 0, 0, s_GenericCommandTimeout );

		if( result != UF_RET_SUCCESS )
		{
			return result;
		}

		if( receivePacket )
		{
//			Sleep( s_NetworkDelay );

			result = UF_ReceiveNetworkPacket( packet, s_GenericCommandTimeout / 2);

			if( result != UF_RET_SUCCESS )
			{
				return result;
			}

			if( UF_GetNetworkPacketValue( SFM_PACKET_COMMAND, packet ) != SFM_COM_CA )
			{
				result = UF_ReceiveNetworkPacket( packet, s_GenericCommandTimeout / 2);

				if( result != UF_RET_SUCCESS )
				{
					return result;
				}
			}

			errorCode = UF_GetNetworkPacketValue( SFM_PACKET_FLAG, packet );

			if( errorCode != UF_PROTO_RET_SUCCESS )
			{
				return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE)errorCode );
			}
		}
		return UF_RET_SUCCESS;
    }
}

/**
 *	UF_CalculateTimeout : Calculate timeout
 */
int UF_CalculateTimeout( int dataSize )
{
	if( g_ChannelType == UF_SERIAL_CHANNEL )
	{
		return s_GenericCommandTimeout + dataSize * 1000 * 2 / (UF_GetBaudrate() / 8);
	}
	else
	{
		return s_GenericCommandTimeout + dataSize * 1000 * 2 / (9600  / 8);
	}
}

/**
 *	UF_SetGenericCommandTimeout : sets the timeout for generic commands.
 */
void UF_SetGenericCommandTimeout( int timeout )
{
	s_GenericCommandTimeout = timeout;
}

/*
* UF_SetInputCommandTimeout: sets the timeout for commands which require user inputs.
*/
void UF_SetInputCommandTimeout( int timeout )
{
	s_InputCommandTimeout = timeout;
}

/*
* UF_GetGenericCommandTimeout: gets the timeout for generic commands.
*/
int UF_GetGenericCommandTimeout(void)
{
	return s_GenericCommandTimeout;
}

/*
* UF_GetInputCommandTimeout: gets the timeout for commands which require user inputs.
*/
int UF_GetInputCommandTimeout(void)
{
	return s_InputCommandTimeout;
}

/*
* UF_SetNetworkDelay: sets the delay for the Network Packet Protocol.
*/
void UF_SetNetworkDelay( int delay )
{
	s_NetworkDelay = delay;
}

/*
* UF_GetNetworKDelay: gets the delay for the Network Packet Protocol.
*/
int UF_GetNetworkDelay(void)
{
	if( g_ChannelType == UF_SERIAL_CHANNEL ) {
		return s_NetworkDelay;
	} else {
		return 0;
	}
}
