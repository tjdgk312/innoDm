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

#ifndef _SFM_UTILS_H_
#define _SFM_UTILS_H_

#include "SFM_Type.h"

#define BinaryToAscii( data ) ( ( data ) >= 10 ? ( data ) - 10 + 'A' : ( data ) + '0' )
#define AsciiToBinary( data ) ( ( data ) >= 'A' ? ( data ) - 'A' + 10 : ( data ) - '0' )


#ifdef __cplusplus
extern "C"
{
#endif
int usleep(UINT32 usec);

void BinToHex(unsigned char c , unsigned char *buf);
void BinToHexPacket(unsigned char* inbuf, unsigned char *outbuf, int inbuf_size);
unsigned int GetTickCount();
int CalculateDataChecksum( unsigned char* packet, int size );

#ifdef __cplusplus
}
#endif

#endif
