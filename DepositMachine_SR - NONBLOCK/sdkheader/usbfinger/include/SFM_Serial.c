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


    File : SFM_Serial.c
    Descriptions : Serial Communication API
    API list
    - UF_InitCommPort
    - UF_CloseCommPort
    - UF_Reconnect
    - UF_SetBaudrate
    - UF_SetAsciiMode

*/

#include "SFM_Connect.h"
#include "SFM_Serial.h"

#define FIRST_OPERATING_TIMEOUT 10      // first timeout 10 seconds for connection

/* Static */
static BOOL s_AsciiMode = FALSE;

void (*s_SerialWriteCallback)( int, int ) = NULL;
void (*s_SerialReadCallback)( int, int ) = NULL;
/**
 * 	Set callback
 */
void UF_SetSerialWriteCallback( void (*Callback)( int, int ) )
{
	s_SerialWriteCallback = Callback;
}

void UF_SetSerialReadCallback( void (*Callback)( int, int ) )
{
	s_SerialReadCallback = Callback;
}

UF_RET_CODE UF_InitCommPort(const char* commPort, int baudrate, BOOL asciiMode)
{
    //Connection
    if(SFM_RS232_OpenComport(commPort, baudrate)) {
        SFM_RS232_ClearBuffer();

        return UF_ERR_CANNOT_OPEN_SERIAL;
    } else {
        unsigned char send_buf[SFM_PACKET_LEN];
        unsigned char recv_buf[SFM_PACKET_LEN];

        int flag;
        int chksum;
        UF_RET_CODE result;

        result = UF_SendPacket(SFM_COM_SS, 0, 0, 0, 1000);
        if(result != UF_RET_SUCCESS) {
            return result;
        }

        result = UF_ReceivePacket(recv_buf, FIRST_OPERATING_TIMEOUT);
        if(result != UF_RET_SUCCESS) {
            return result;
        }

        flag = SFM_GetPacketValue(SFM_PACKET_FLAG, recv_buf);

        if(flag == ERROR_SUCCESS)
        {
            g_bConnectionStatus = 1;
        }
        SFM_RS232_ClearBuffer();
        return UF_RET_SUCCESS;
    }
}

UF_RET_CODE UF_CloseCommPort(void)
{
    if(g_bConnectionStatus ==  1) {
        SFM_RS232_CloseComport();
        g_bConnectionStatus = 0;
        return UF_RET_SUCCESS;
    } else {
        return UF_ERR_CANNOT_SETUP_SERIAL;
    }
}

void UF_Reconnect(void)
{
	UF_InitSysParameter();
}

UF_RET_CODE UF_SetBaudrate(int baudrate)
{
    UF_RET_CODE result;

    int baudr;
    int sfm_baudrate;
    BYTE* packet[SFM_PACKET_LEN];

    switch(baudrate)
    {
        case 9600 : baudr = B9600;
            sfm_baudrate = 0x31;
            break;
        case 19200 : baudr = B19200;
            sfm_baudrate = 0x32;
            break;
        case 38400 : baudr = B38400;
            sfm_baudrate = 0x33;
            break;
        case 57600 : baudr = B57600;
            sfm_baudrate = 0x34;
            break;
        case 115200 : baudr = B115200;
            sfm_baudrate = 0x35;
            break;
        case 230400 : baudr = B230400;
            sfm_baudrate = 0x36;
            break;
        case 460800 : baudr = B460800;
            sfm_baudrate = 0x37;
            break;
        case 921600 : baudr = B921600;
            sfm_baudrate = 0x38;
            break;
        default :
            return UF_ERR_CANNOT_SETUP_SERIAL;
            break;
    }

    cfsetspeed(&options, baudr);
    g_nBaudrate = baudrate;

    /* Set System Parameter - BaudRate */
    result = UF_SetSysParameter(UF_SYS_BAUDRATE, sfm_baudrate);
    if(result != UF_RET_SUCCESS) {
        return result;
    }

    /* Save System Parameter */
    result = UF_Save();
    if(result != UF_RET_SUCCESS) {
        return result;
    }

    UF_CloseCommPort();
    UF_InitCommPort(g_commPort, g_nBaudrate, 0);

    return UF_RET_SUCCESS;
}

void UF_SetAsciiMode( BOOL asciiMode )
{
	s_AsciiMode = asciiMode;
}

int UF_GetBaudrate(void)
{
	return g_nBaudrate;
}