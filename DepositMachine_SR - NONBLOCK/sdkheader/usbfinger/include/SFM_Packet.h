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

#ifndef _SFM_PACKET_H_
#define _SFM_PACKET_H_

#include "SFM_Type.h"
#include "SFM_Error.h"
#include "SFM_Utils.h"
#include "SFM_Command.h"
#include "SFM_RS232.h"

#define SFM_PACKET_LEN              13
#define SFM_NETWORK_PACKET_LEN       15

#define	SFM_PACKET_COMMAND			0
#define	SFM_PACKET_TERMINAL_ID		1
#define	SFM_PACKET_PARAM			2
#define	SFM_PACKET_SIZE				3
#define	SFM_PACKET_FLAG				4
#define	SFM_PACKET_CHECKSUM			5

//
// Constants
//
#define	SFM_PACKET_START_CODE			0x40
#define	SFM_NETWORK_PACKET_START_CODE	0x41
#define	SFM_PACKET_END_CODE				0x0A

//
// Byte position of packet components
//
#define	SFM_PACKET_START_CODE_POS	0
#define	SFM_PACKET_COMMAND_POS		1
#define	SFM_PACKET_PARAM_POS		2
#define	SFM_PACKET_SIZE_POS			6
#define	SFM_PACKET_FLAG_POS			10
#define	SFM_PACKET_CHECKSUM_POS		11
#define	SFM_PACKET_END_CODE_POS		12

#define	UF_NETWORK_PACKET_START_CODE_POS	0
#define	UF_NETWORK_PACKET_TERMINALID_POS	1
#define	UF_NETWORK_PACKET_COMMAND_POS		3
#define	UF_NETWORK_PACKET_PARAM_POS		4
#define	UF_NETWORK_PACKET_SIZE_POS			8
#define	UF_NETWORK_PACKET_FLAG_POS			12
#define	UF_NETWORK_PACKET_CHECKSUM_POS		13
#define	UF_NETWORK_PACKET_END_CODE_POS		14

//
// Data packet
//
#define SFM_DEFAULT_DATA_PACKET_SIZE		(4 * 1024)

// extern

extern int g_ChannelType;

#ifdef __cplusplus
extern "C"
{
#endif

/* Un-Official */
int RecievePacket(unsigned char *buf, int size, int timout);
void UF_SetDefaultPacketSize( int defaultSize );
int UF_GetDefaultPacketSize(void);

/* Official */
BYTE SFM_CalculateChecksum( BYTE* packet, int size );
UF_RET_CODE UF_SendPacket( BYTE command, UINT32 param, UINT32 size, BYTE flag, int timeout );
UF_RET_CODE UF_SendDataPacket( BYTE command, BYTE* buf, UINT32 dataSize, UINT32 dataPacketSize );
UF_RET_CODE UF_MakePacket(BYTE command, UINT32 param, UINT32 size, BYTE flag, BYTE* packet );
UF_RET_CODE UF_SendRawData(BYTE* buf, UINT32 size, int timeout);
UF_RET_CODE UF_ReceivePacket(BYTE* packet, int timeout );
UF_RET_CODE UF_ReceiveDataPacket(BYTE command, BYTE* buf, UINT32 dataSize);
UF_RET_CODE UF_ReceiveRawData( BYTE* buf, UINT32 size, int timeout, BOOL checkEndCode );

int UF_WriteData( unsigned char* buf, int size, int timeout );
int UF_ReadData( unsigned char* buf, int size, int timeout );
int UF_ReadSerial( unsigned char* buf, int size, int timeout );

UINT32 SFM_GetPacketValue( int component, BYTE* packet );

/* Netwrok */
UF_RET_CODE UF_MakeNetworkPacket( BYTE command, USHORT terminalID, UINT32 param, UINT32 size, BYTE flag, BYTE* packet );
UF_RET_CODE UF_SendNetworkPacket( BYTE command, USHORT terminalID, UINT32 param, UINT32 size, BYTE flag, int timeout );
UF_RET_CODE UF_ReceiveNetworkPacket( BYTE* packet, int timeout );
UINT32 UF_GetNetworkPacketValue( int component, BYTE* packet );

void UF_SetSerialWriteCallback( void (*Callback)( int writtenLen, int totalSize ) );
void UF_SetSerialReadCallback( void (*Callback)( int readLen, int totalSize ) );

int Uart_Tx(BYTE Data);

#ifdef __cplusplus
}
#endif

#endif