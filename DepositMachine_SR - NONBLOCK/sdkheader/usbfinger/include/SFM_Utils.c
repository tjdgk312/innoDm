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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/stat.h>

#include "SFM_Utils.h"

#define TOHEX(a, b)	{*b++ = hextable[a >> 4];*b++ = hextable[a&0xf];}
char hextable[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

void BinToHex(unsigned char c , unsigned char *buf)
{
    TOHEX(c, buf);
}

void BinToHexPacket(unsigned char* inbuf, unsigned char *outbuf, int inbuf_size)
{
    int i;

    for(i=0; i<inbuf_size; i++)
    {
        BinToHex(inbuf[i], &outbuf[i*3]);
        outbuf[i*3+2] = 32;
    }
}

unsigned int GetTickCount()
{
	struct timeval gettick;
	unsigned int tick;

	gettimeofday(&gettick, NULL);

	tick = gettick.tv_sec*1000 + gettick.tv_usec/1000;

	return tick;
}



int CalculateDataChecksum( unsigned char* packet, int size )
{
	int i;
	int checksum = 0;

	for( i = 0; i < size; i++ )
	{
		checksum += packet[i];
	}

	return checksum;
}

