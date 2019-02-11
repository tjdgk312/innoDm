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

    File : SFM_Packet.c
    Descriptions : Low-Level Packet API
    API list
    - UF_SendPacket
    - UF_ReceivePacket
    - UF_ReceiveDataPacket
    - UF_SendRawData
    - UF_ReceiveRawData
    - UF_SendDataPacket

    - UF_SendNetworkPacket
    - UF_ReceiveNetworkPacket

    - UF_SetSendPacketCallback
    - UF_SetReceivePacketCallback
    - UF_SetSendDataPacketCallback
    - UF_SetReceiveDataPacketCallback
    - UF_SetSendRawDataCallback
    - UF_SetReceiveRawDataCallback

    - UF_SetDefaultPacketSize
    - UF_GetDefaultPacketSize

*/

/*
 *	Make a packet from components
 */
#include "SFM_Packet.h"

#define MAX_TIMEOUT_COUNT 20

static BOOL s_WriteCanceled = FALSE;
static BOOL s_ReadCanceled = FALSE;

// Callback
void (*s_SendPacketCallback)( BYTE* ) = NULL;
void (*s_ReceivePacketCallback)( BYTE* ) = NULL;
void (*s_SendDataPacketCallback)( int, int ) = NULL;
void (*s_ReceiveDataPacketCallback)( int, int ) = NULL;
void (*s_SendRawDataCallback)( int, int ) = NULL;
void (*s_ReceiveRawDataCallback)( int, int ) = NULL;

// Global variables
int g_ChannelType = UF_SERIAL_CHANNEL;

// Static variables
int s_DefaultPacketSize = SFM_DEFAULT_DATA_PACKET_SIZE;

// Set callback
void UF_SetSendPacketCallback( void (*Callback)( BYTE* ) )
{
	s_SendPacketCallback = Callback;
}

void UF_SetReceivePacketCallback( void (*Callback)( BYTE* ) )
{
	s_ReceivePacketCallback = Callback;
}

void UF_SetSendDataPacketCallback( void (*Callback)( int, int ) )
{
	s_SendDataPacketCallback = Callback;
}

void UF_SetReceiveDataPacketCallback( void (*Callback)( int, int ) )
{
	s_ReceiveDataPacketCallback = Callback;
}

void UF_SetSendRawDataCallback( void (*Callback)( int, int ) )
{
	s_SendRawDataCallback = Callback;
}

void UF_SetReceiveRawDataCallback( void (*Callback)( int, int ) )
{
	s_ReceiveRawDataCallback = Callback;
}

UF_RET_CODE UF_SendPacket( BYTE command, UINT32 param, UINT32 size, BYTE flag, int timeout )
{
    UF_RET_CODE result;

    int writeLen;
	BYTE packet[SFM_PACKET_LEN];

    result = UF_MakePacket(command, param, size, flag, packet );
    if(result != UF_RET_SUCCESS)
    {
        /* Never enter here */
        return result;
    }
    writeLen = UF_WriteData(packet, SFM_PACKET_LEN, timeout );

    if( writeLen < 0 ) {
        if (g_ChannelType == UF_SERIAL_CHANNEL)
            return UF_ERR_CANNOT_WRITE_SERIAL;
    } else if ( writeLen < SFM_PACKET_LEN) {
        if (g_ChannelType == UF_SERIAL_CHANNEL)
            return UF_ERR_WRITE_SERIAL_TIMEOUT;
    }

	if( s_SendPacketCallback )
	{
		(*s_SendPacketCallback)( packet );
	}

    return UF_RET_SUCCESS;
}

UF_RET_CODE UF_ReceivePacket(BYTE* packet, int timeout )
{
    BYTE checksum = 0;

    RecievePacket(packet, SFM_PACKET_LEN, timeout);

    checksum = CalculateDataChecksum( packet, SFM_PACKET_LEN - 2 );

    if( (checksum != SFM_GetPacketValue( SFM_PACKET_CHECKSUM, packet ))
        || (SFM_PACKET_END_CODE != packet[SFM_PACKET_END_CODE_POS]) ) {
            return UF_ERR_CHECKSUM_ERROR;
    }

	if( s_ReceivePacketCallback )
	{
		(*s_ReceivePacketCallback)( packet );
	}

    return UF_RET_SUCCESS;
}

UF_RET_CODE UF_ReceiveDataPacket(BYTE command, BYTE* buf, UINT32 dataSize)
{
    UF_RET_CODE result;
    
	UINT32 checksum1, checksum2;
	BYTE* dataPacketBuf;
	BYTE packet[SFM_NETWORK_PACKET_LEN];
	int numOfPacket, packetIndex;
	int bufPos = 0;
	int packetSize;
    int i;

    UINT32 timeout = 10000;

    if (dataSize == 0) {
        return UF_RET_SUCCESS;
    }

    do {
        if( UF_GetProtocol() == UF_SINGLE_PROTOCOL ) {
            UF_ReceivePacket(packet, timeout );

            packetSize = SFM_GetPacketValue( SFM_PACKET_SIZE, packet );
            numOfPacket = SFM_GetPacketValue( SFM_PACKET_PARAM, packet ) & 0xffff;
            packetIndex = (SFM_GetPacketValue( SFM_PACKET_PARAM, packet ) & 0xffff0000) >> 16;
        } else {
            result = UF_ReceiveNetworkPacket(packet, 10000);
            if (result != UF_RET_SUCCESS) {
                return result;
            }

    		packetSize = UF_GetNetworkPacketValue( SFM_PACKET_SIZE, packet );
			numOfPacket = UF_GetNetworkPacketValue( SFM_PACKET_PARAM, packet ) & 0xffff;
			packetIndex = (UF_GetNetworkPacketValue( SFM_PACKET_PARAM, packet ) & 0xffff0000) >> 16;
        }

        result = UF_ReceiveRawData( buf + bufPos, packetSize + 4, 10000, FALSE );
        if (result != UF_RET_SUCCESS) {
            return result;
        }

        checksum1 = 0;

        for( i = 0; i < packetSize; i++ )
        {
            checksum1 += buf[bufPos + i];
        }

        checksum2 = buf[bufPos + packetSize];
        checksum2 |= ((unsigned)buf[bufPos + packetSize + 1]) << 8;
        checksum2 |= ((unsigned)buf[bufPos + packetSize + 2]) << 16;
        checksum2 |= ((unsigned)buf[bufPos + packetSize + 3]) << 24;

        if( UF_GetProtocol() == UF_SINGLE_PROTOCOL ) {
            UF_SendPacket( command , 0, 0, SFM_PROTO_RET_DATA_OK, 30000 );
        } else {
            usleep( UF_GetNetworkDelay() * 1000 );
            UF_SendNetworkPacket( command, UF_GetNetworkPacketValue( SFM_PACKET_TERMINAL_ID, packet ), 0, 0, SFM_PROTO_RET_DATA_OK, UF_GetGenericCommandTimeout() );
        }
           
        bufPos += packetSize;

		if( s_ReceiveDataPacketCallback )
		{
			(*s_ReceiveDataPacketCallback)( packetIndex, numOfPacket );
		}

    } while( packetIndex + 1 < numOfPacket && bufPos < dataSize );

    return UF_RET_SUCCESS;
}

UF_RET_CODE UF_SendRawData(BYTE* buf, UINT32 size, int timeout)
{
    BYTE endCode = SFM_PACKET_END_CODE;
    int i;
    int ret;

    if( s_SendRawDataCallback ) {
        UF_SetSerialWriteCallback( s_SendRawDataCallback );
    }

    for (i=0; i < size; i++)
    {
        ret = Uart_Tx(*(buf + i));
        if(!ret) {
            return UF_ERR_CANNOT_WRITE_SERIAL;
        }
    }
    

    if( s_SendRawDataCallback ) {
        UF_SetSerialWriteCallback( NULL );
    }

    ret = Uart_Tx( endCode );
    if(!ret) {
        return UF_ERR_CANNOT_WRITE_SERIAL;
    }

    return UF_RET_SUCCESS;
}

UF_RET_CODE UF_ReceiveRawData( BYTE* buf, UINT32 size, int timeout, BOOL checkEndCode )
{
    int readLen;
    BYTE endCode;
    int i;
    int tryCount = 0;
    int bytes_avail = 0;

	if( checkEndCode && s_ReceiveRawDataCallback )
	{
		UF_SetSerialReadCallback( s_ReceiveRawDataCallback );
	}

    UINT32 currentTime;
    currentTime = GetTickCount();

    for (i = 0; i < size ; i++) {
        while (1) {
            ioctl(g_fdRS232, FIONREAD, &bytes_avail);
            if (bytes_avail > 0)
                break;
            if (tryCount++ > TRIAL_COUNT) {
                return UF_ERR_READ_SERIAL_TIMEOUT;
            }
                
        }
        if (read(g_fdRS232, buf+i, 1) != 1) {
            return UF_ERR_CANNOT_READ_SERIAL;
        }
    }

#ifdef _PACKET_FILE_
    for (i = 0; i < size; i++) {
        printf(" %X", buf[i]);
    }
    printf("\n");
#endif

    if( checkEndCode && s_ReceiveRawDataCallback ) {
        UF_SetSerialReadCallback( NULL );
    }

    if (checkEndCode) {
        int ret = UF_ReadData( &endCode, 1, timeout );

        if(ret == 0) {
            return UF_RET_SUCCESS;
        } else {
            return UF_ERR_CHECKSUM_ERROR;
        }
    }

    return UF_RET_SUCCESS;
}

UF_RET_CODE UF_SendDataPacket( BYTE command, BYTE* buf, UINT32 dataSize, UINT32 dataPacketSize )
{
	UF_RET_CODE result;

	UINT32 checksum;
	BYTE* dataPacketBuf;
	BYTE packet[SFM_PACKET_LEN];
    BYTE rcv_packet[SFM_PACKET_LEN];
    BYTE flag;

	int numOfPacket, sentLen, written;
    int i;

    UINT32 timeout = 10000;

	if( dataSize == 0 || dataPacketSize == 0 )
	{
		return UF_RET_SUCCESS;
	}

    numOfPacket = dataSize / dataPacketSize;

	if( dataSize % dataPacketSize > 0 )
	{
		numOfPacket++;
	}

    sentLen = 0;
    dataPacketBuf = buf;

    for( i = 0; i < numOfPacket; i++ )
    {
        if( dataSize - sentLen < dataPacketSize ) {
            dataPacketSize = dataSize - sentLen;
        }

        UF_SendPacket(command, (i << 16) | numOfPacket, dataPacketSize, 0, timeout );

		//
		// Send raw data
		//
        written = UF_WriteData( dataPacketBuf, dataPacketSize, timeout );

        sentLen += written;

        checksum = CalculateDataChecksum( dataPacketBuf, dataPacketSize );

        written = UF_WriteData( (BYTE*)&checksum, sizeof(checksum), timeout );

        dataPacketBuf += dataPacketSize;

        RecievePacket(rcv_packet, SFM_PACKET_LEN, timeout );

        flag = SFM_GetPacketValue(SFM_PACKET_FLAG, rcv_packet);
         
        if (flag != SFM_PROTO_RET_DATA_OK) {
            return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE)flag );
        }

        if( s_SendDataPacketCallback )
		{
			(*s_SendDataPacketCallback)( i, numOfPacket );
		}
        
        usleep(10000);
    }
    return UF_RET_SUCCESS;
}

UF_RET_CODE UF_SendNetworkPacket( BYTE command, USHORT terminalID, UINT32 param, UINT32 size, BYTE flag, int timeout )
{
	int writeLen;
	BYTE packet[SFM_NETWORK_PACKET_LEN];

	UF_MakeNetworkPacket( command, terminalID, param, size, flag, packet );

	writeLen = UF_WriteData( packet, SFM_NETWORK_PACKET_LEN, timeout );

	if( writeLen < 0 )
	{
        if (g_ChannelType == UF_SERIAL_CHANNEL)
		    return UF_ERR_CANNOT_WRITE_SERIAL;
	}
	else if( writeLen < SFM_NETWORK_PACKET_LEN )
	{
        if (g_ChannelType == UF_SERIAL_CHANNEL)
		    return UF_ERR_WRITE_SERIAL_TIMEOUT;
	}

	if( s_SendPacketCallback )
	{
		(*s_SendPacketCallback)( packet );
	}

	return UF_RET_SUCCESS;
}

UF_RET_CODE UF_ReceiveNetworkPacket( BYTE* packet, int timeout )
{
    BYTE checksum = 0;
    RecievePacket(packet, SFM_NETWORK_PACKET_LEN, timeout);

    checksum = CalculateDataChecksum( packet, SFM_NETWORK_PACKET_LEN - 2 );
    if( (checksum != SFM_GetPacketValue( SFM_PACKET_CHECKSUM, packet ))
        || (SFM_PACKET_END_CODE != packet[SFM_PACKET_END_CODE_POS]) ) {
            return UF_ERR_CHECKSUM_ERROR;
    }

	if( s_ReceivePacketCallback )
	{
		(*s_ReceivePacketCallback)( packet );
	}

    return UF_RET_SUCCESS;
}


void UF_SetDefaultPacketSize( int defaultSize )
{
	s_DefaultPacketSize = defaultSize;
}

int UF_GetDefaultPacketSize(void)
{
	return s_DefaultPacketSize;
}

UF_RET_CODE UF_MakePacket(BYTE command, UINT32 param, UINT32 size, BYTE flag, BYTE* packet )
{
	packet[SFM_PACKET_START_CODE_POS] = SFM_PACKET_START_CODE;
	packet[SFM_PACKET_COMMAND_POS] = command;
	packet[SFM_PACKET_PARAM_POS] = (BYTE)param;
	packet[SFM_PACKET_PARAM_POS + 1] = (BYTE)(param >> 8);
	packet[SFM_PACKET_PARAM_POS + 2] = (BYTE)(param >> 16);
	packet[SFM_PACKET_PARAM_POS + 3] = (BYTE)(param >> 24);
	packet[SFM_PACKET_SIZE_POS] = (BYTE)size;
	packet[SFM_PACKET_SIZE_POS + 1] = (BYTE)(size >> 8);
	packet[SFM_PACKET_SIZE_POS + 2] = (BYTE)(size >> 16);
	packet[SFM_PACKET_SIZE_POS + 3] = (BYTE)(size >> 24);
	packet[SFM_PACKET_FLAG_POS] = flag;
	packet[SFM_PACKET_CHECKSUM_POS] = SFM_CalculateChecksum( packet, SFM_PACKET_CHECKSUM_POS );
	packet[SFM_PACKET_END_CODE_POS] = SFM_PACKET_END_CODE;

    return UF_RET_SUCCESS;
}

UF_RET_CODE UF_MakeNetworkPacket( BYTE command, USHORT terminalID, UINT32 param, UINT32 size, BYTE flag, BYTE* packet )
{
	packet[UF_NETWORK_PACKET_START_CODE_POS] = SFM_NETWORK_PACKET_START_CODE;
	packet[UF_NETWORK_PACKET_TERMINALID_POS] = (BYTE)terminalID;
	packet[UF_NETWORK_PACKET_TERMINALID_POS + 1] = (BYTE)(terminalID >> 8);
	packet[UF_NETWORK_PACKET_COMMAND_POS] = command;
	packet[UF_NETWORK_PACKET_PARAM_POS] = (BYTE)param;
	packet[UF_NETWORK_PACKET_PARAM_POS + 1] = (BYTE)(param >> 8);
	packet[UF_NETWORK_PACKET_PARAM_POS + 2] = (BYTE)(param >> 16);
	packet[UF_NETWORK_PACKET_PARAM_POS + 3] = (BYTE)(param >> 24);
	packet[UF_NETWORK_PACKET_SIZE_POS] = (BYTE)size;
	packet[UF_NETWORK_PACKET_SIZE_POS + 1] = (BYTE)(size >> 8);
	packet[UF_NETWORK_PACKET_SIZE_POS + 2] = (BYTE)(size >> 16);
	packet[UF_NETWORK_PACKET_SIZE_POS + 3] = (BYTE)(size >> 24);
	packet[UF_NETWORK_PACKET_FLAG_POS] = flag;
	packet[UF_NETWORK_PACKET_CHECKSUM_POS] = SFM_CalculateChecksum( packet, UF_NETWORK_PACKET_CHECKSUM_POS );
	packet[UF_NETWORK_PACKET_END_CODE_POS] = SFM_PACKET_END_CODE;

	return UF_RET_SUCCESS;
}

/**
 *  Calculate checksum
 */
BYTE SFM_CalculateChecksum( BYTE* packet, int size )
{
	int i;
	int checksum = 0;

	for( i = 0; i < size; i++ )
	{
		checksum += packet[i];
	}

	return (checksum & 0xff);
}

/**
 *	Get a component of a packet
 */
UINT32 SFM_GetPacketValue( int component, BYTE* packet )
{
	UINT32 value = 0;

	switch( component ) {
		case SFM_PACKET_COMMAND:
			value = (UINT32)packet[SFM_PACKET_COMMAND_POS];
			break;

		case SFM_PACKET_PARAM:
			value = (UINT32)packet[SFM_PACKET_PARAM_POS];
			value |= (UINT32)packet[SFM_PACKET_PARAM_POS + 1] << 8;
			value |= (UINT32)packet[SFM_PACKET_PARAM_POS + 2] << 16;
			value |= (UINT32)packet[SFM_PACKET_PARAM_POS + 3] << 24;
			break;

		case SFM_PACKET_SIZE:
			value = (UINT32)packet[SFM_PACKET_SIZE_POS];
			value |= (UINT32)packet[SFM_PACKET_SIZE_POS + 1] << 8;
			value |= (UINT32)packet[SFM_PACKET_SIZE_POS + 2] << 16;
			value |= (UINT32)packet[SFM_PACKET_SIZE_POS + 3] << 24;
			break;

		case SFM_PACKET_FLAG:
			value = (UINT32)packet[SFM_PACKET_FLAG_POS];
			break;

		case SFM_PACKET_CHECKSUM:
			value = (UINT32)packet[SFM_PACKET_CHECKSUM_POS];
			break;

		default:
			break;
	}

	return value;
}

UINT32 UF_GetNetworkPacketValue( int component, BYTE* packet )
{
	UINT32 value = 0;

	switch( component ) {
		case SFM_PACKET_COMMAND:
			value = (UINT32)packet[UF_NETWORK_PACKET_COMMAND_POS];
			break;

		case SFM_PACKET_TERMINAL_ID:
			value = (UINT32)packet[UF_NETWORK_PACKET_TERMINALID_POS];
			value |= (UINT32)packet[UF_NETWORK_PACKET_TERMINALID_POS + 1] << 8;
			break;

		case SFM_PACKET_PARAM:
			value = (UINT32)packet[UF_NETWORK_PACKET_PARAM_POS];
			value |= (UINT32)packet[UF_NETWORK_PACKET_PARAM_POS + 1] << 8;
			value |= (UINT32)packet[UF_NETWORK_PACKET_PARAM_POS + 2] << 16;
			value |= (UINT32)packet[UF_NETWORK_PACKET_PARAM_POS + 3] << 24;
			break;

		case SFM_PACKET_SIZE:
			value = (UINT32)packet[UF_NETWORK_PACKET_SIZE_POS];
			value |= (UINT32)packet[UF_NETWORK_PACKET_SIZE_POS + 1] << 8;
			value |= (UINT32)packet[UF_NETWORK_PACKET_SIZE_POS + 2] << 16;
			value |= (UINT32)packet[UF_NETWORK_PACKET_SIZE_POS + 3] << 24;
			break;

		case SFM_PACKET_FLAG:
			value = (UINT32)packet[UF_NETWORK_PACKET_FLAG_POS];
			break;

		case SFM_PACKET_CHECKSUM:
			value = (UINT32)packet[UF_NETWORK_PACKET_CHECKSUM_POS];
			break;

		default:
			break;
	}

	return value;
}

int RecievePacket(unsigned char *buf, int size, int timout)
{
    int i,n,s,pos;
    unsigned char tempbuf[size];
    unsigned char hexbuf[size*3];

    memset(tempbuf, 0, size);
    memset(hexbuf, 0, size);

    s = GetTickCount();
    n = 0;
    pos = 0;
    while(1)
    {
        
        if (GetTickCount() - s > timout * 1000)
        {
            return UF_ERR_WRITE_SERIAL_TIMEOUT;
        }
        
        n = SFM_RS232_PollComport(tempbuf+pos, 1);
        if(n > 0)
        {
            tempbuf[n + pos] = '\0';   /* always put a "null" at the end of a string! */
            //hexbuf[(n + pos)*2-1] = 0;

            for(i=pos; i < n+pos; i++)
            {
                buf[i] = tempbuf[i];
            }

            //break;
        }

        if(tempbuf[SFM_PACKET_END_CODE_POS]==SFM_PACKET_END_CODE || pos == SFM_PACKET_LEN ){
            break;
        }

        pos += n;

    }

    int _cmd,_param,_size,_flag,_chksum;

#ifdef _PACKET_TRACE_
    printf("%8s : ", "[RECV]");
    for(i=0; i<size ; i++)
        printf("%02X ", buf[i]);
    printf("\r\n");
#endif

    if(buf[SFM_PACKET_END_CODE_POS]==SFM_PACKET_END_CODE && buf[SFM_PACKET_START_CODE_POS]==SFM_PACKET_START_CODE )
    {
        _cmd = SFM_GetPacketValue(SFM_PACKET_COMMAND, buf);
        _param = SFM_GetPacketValue(SFM_PACKET_PARAM, buf);
        _size = SFM_GetPacketValue(SFM_PACKET_SIZE, buf);
        _flag = SFM_GetPacketValue(SFM_PACKET_FLAG, buf);
        _chksum = SFM_GetPacketValue(SFM_PACKET_CHECKSUM, buf);

#ifdef _PACKET_TRACE_
        printf("%8s = 0x%02x(%d)\n","Command", _cmd, _cmd);
        printf("%8s = 0x%08x(%d)\n","Param", _param, _param);
        printf("%8s = 0x%08x(%d)\n","Size", _size, _size);
        printf("%8s = 0x%02x(%d)\n","Flag", _flag, _flag);
        printf("%8s = 0x%02x(%d)\n","CheckSum", _chksum, _chksum);
#endif

    }
    return _size;
}

int UF_WriteSerial( BYTE* buf, int size, int timeout )
{
	unsigned char* newBuf;
	int newSize;
	int i;
	int bufPos = 0;
	int writeLen;
	BOOL writeSuccess = FALSE;
	DWORD currentTime;

	int timeoutCount = 0;
	DWORD comError;

	newBuf = buf;
	newSize = size;

    currentTime = GetTickCount();

    do {
        writeLen = write(g_fdRS232, newBuf + bufPos, newSize - bufPos);

    	if( writeLen > 0 ) {
			timeoutCount = 0;
		} else {
			if( bufPos > 0 ) {
				if( ++timeoutCount > MAX_TIMEOUT_COUNT ) {
					break;
				}
			}
		}

        bufPos += writeLen;

        if( bufPos >= newSize ) {
			writeSuccess = TRUE;
			break;
		}
    } while( (GetTickCount() - currentTime < timeout) && !s_WriteCanceled );

#ifdef _PACKET_TRACE_
    printf("\r[SEND] : ");
    for(i=0; i<size ; i++)
        printf("%02X ", newBuf[i]);
    printf("\r\n");
#endif

    if( bufPos > 0 ) {
        usleep(10000);
        return bufPos;
    } else {
        return -1;
    }
}

int UF_WriteData( unsigned char* buf, int size, int timeout )
{
    if ( g_ChannelType == UF_SERIAL_CHANNEL ) {
        return UF_WriteSerial( buf, size, timeout );
    } else {
        return -1;
    }
}

int UF_ReadData( unsigned char* buf, int size, int timeout )
{
    return UF_ReadSerial( buf, size, timeout );
}

int UF_ReadSerial( unsigned char* buf, int size, int timeout )
{
	unsigned char* newBuf;
	int newSize;
	int i;
	int readLen;
	int bufPos = 0;
	BOOL readSuccess = FALSE;
	DWORD currentTime;

	int timeoutCount = 0;
	DWORD comError;

    newBuf = buf;
	newSize = size;
    currentTime = GetTickCount();

    do {
        readLen = read(g_fdRS232, buf, size);
        if( readLen > 0 ){
            timeoutCount = 0;
        } else {
            if( bufPos > 0 ) {
                if( ++timeoutCount > MAX_TIMEOUT_COUNT ) {
                    break;
                }
            }
        }

        bufPos += readLen;
        if( bufPos >= newSize ) {
            readSuccess = TRUE;
            break;
        }

    } while( (GetTickCount() - currentTime < timeout) && !s_ReadCanceled );

    if( bufPos == 0 || s_ReadCanceled ) {
        return bufPos;
    }

    return bufPos;

}

int Uart_Tx(BYTE Data)
{
    int tryCount = 0;
    while (1) {
        if (write(g_fdRS232, &Data, 1) == 1) {
            tryCount = 1;
            break;
        }
            
        if (tryCount++ > TRIAL_COUNT) {
            break;
        }      
    }

    return tryCount;
}
