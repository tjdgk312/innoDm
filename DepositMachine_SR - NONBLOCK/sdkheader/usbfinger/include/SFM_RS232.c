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

#include "SFM_Packet.h"
#include "SFM_Command.h"
#include "SFM_SysParameter.h"
#include "SFM_Connect.h"

#define MAX_TIMEOUT_COUNT 20

int g_bConnectionStatus = 0;
int g_fdRS232;

struct termios options;
const char* g_commPort;
int g_nBaudrate;

int SFM_RS232_OpenComport(const char* commPort, int baudrate)
{
    int baudr, status;
    int error;

    switch(baudrate)
    {
        case 9600 : baudr = B9600;
            break;
        case 19200 : baudr = B19200;
            break;
        case   38400 : baudr = B38400;
            break;
        case   57600 : baudr = B57600;
            break;
        case  115200 : baudr = B115200;
            break;
        case  230400 : baudr = B230400;
            break;
        case  460800 : baudr = B460800;
            break;
        case  500000 : baudr = B500000;
            break;
        case  576000 : baudr = B576000;
            break;
        case  921600 : baudr = B921600;
            break;
        default : 
            return(1);
            break;
    }

    g_fdRS232 = open(commPort, O_RDWR | O_NOCTTY | O_NDELAY);

    if (g_fdRS232 == -1) {
        return -1;
    } else {
        g_commPort = commPort;
        g_nBaudrate = baudrate;
    }

    memset(&options, 0, sizeof(options));  /* clear the new struct */

    options.c_cflag = baudr | CS8 | CLOCAL | CREAD;
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    options.c_cc[VMIN] = 0;      /* block untill n bytes are received */
    options.c_cc[VTIME] = 0;     /* block untill a timer expires (n * 100 mSec.) */

    cfsetispeed(&options, baudr);
    cfsetospeed(&options, baudr);

    error = tcsetattr(g_fdRS232, TCSANOW, &options);
    if(error==-1) {
        close(g_fdRS232);
        return -1;
    }

    if(ioctl(g_fdRS232, TIOCMGET, &status) == -1) {
        return(1);
    }

    status |= TIOCM_DTR;    /* turn on DTR */
    status |= TIOCM_RTS;    /* turn on RTS */

    if(ioctl(g_fdRS232, TIOCMSET, &status) == -1) {
        return(1);
    }

    return(0);
}

int SFM_RS232_PollComport(unsigned char *buf, int size)
{
    int n;

    n = read(g_fdRS232, buf, size);

    return(n);
}

void SFM_RS232_CloseComport(void)
{
    int status;

    ioctl(g_fdRS232, TIOCMGET, &status);

    status &= ~TIOCM_DTR;    /* turn off DTR */
    status &= ~TIOCM_RTS;    /* turn off RTS */

    ioctl(g_fdRS232, TIOCMSET, &status);
    close(g_fdRS232);
}

void SFM_RS232_ClearBuffer(void)
{
    tcflush(g_fdRS232,TCIOFLUSH);
}
