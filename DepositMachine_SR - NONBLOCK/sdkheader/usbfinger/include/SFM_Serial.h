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


#ifndef _SFM_SERIAL_H_
#define _SFM_SERIAL_H_

#include <fcntl.h>
#include <sys/stat.h>
#include <termios.h>

#include "SFM_Type.h"
#include "SFM_Command.h"
#include "SFM_Packet.h"
#include "SFM_Error.h"
#include "SFM_SysParameter.h"
#include "SFM_RS232.h"

extern struct termios options;
extern int g_nBaudrate;
extern int g_sys_timeout;

#ifdef __cplusplus
extern "C"
{
#endif

UF_RET_CODE UF_CloseCommPort(void);
UF_RET_CODE UF_SetBaudrate(int baudrate);
void UF_SetAsciiMode( BOOL asciiMode );
int UF_GetBaudrate(void);

/* For termios */
int cfsetspeed(struct termios *termios_p, speed_t speed);

#ifdef __cplusplus
}
#endif

#endif