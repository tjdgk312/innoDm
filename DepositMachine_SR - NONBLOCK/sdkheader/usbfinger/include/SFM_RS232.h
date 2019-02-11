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


#ifndef _SFM_RS232_H_
#define _SFM_RS232_H_

#include <stdio.h>
#include <string.h>

#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>

#include "SFM_Type.h"

typedef enum {
	UF_SINGLE_PROTOCOL		= 0,
	UF_NETWORK_PROTOCOL		= 1,
} UF_PROTOCOL;

typedef enum {
	UF_SERIAL_CHANNEL		= 0,
	UF_SOCKET_CHANNEL		= 1,
} UF_CHANNEL_TYPE;

extern const char* g_commPort;
extern int g_nBaudrate;
extern int g_fdRS232;

#define UART_READ_WRITE_TRIAL_COUNT (10 * 60 * 1000 * 1000)
#define TRIAL_COUNT UART_READ_WRITE_TRIAL_COUNT

/* To Do : Customer */
#define USB_INTERFACE_DEV0 "/dev/ttyACM0"       // USB
#define USB_INTERFACE_DEV1 "/dev/ttyACM1"       // USB
#define UART_INTERFACE_DEV0 "/dev/ttyUSB0"      // Serial
#define UART_INTERFACE_DEV1 "/dev/ttyUSB1"      // Serial

#ifdef __cplusplus
extern "C"
{
#endif

int SFM_RS232_OpenComport(const char* commPort, int baudrate);
int SFM_RS232_PollComport(unsigned char *buf, int size);
void SFM_RS232_ClearBuffer(void);
void SFM_RS232_CloseComport(void);
#ifdef __cplusplus
}
#endif

#endif