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


#ifndef _SFM_TYPE_H_
#define _SFM_TYPE_H_

#ifndef TRUE
#define TRUE 1
#endif // TRUE

#ifndef FALSE
#define FALSE 0
#endif // FALSE

// Type definition
#ifndef LONG
typedef unsigned long LONG;
#endif

#ifndef DWORD
typedef unsigned long DWORD;
#endif

#ifndef WORD
typedef unsigned short WORD;
#endif

#ifndef BYTE
typedef unsigned char BYTE;
#endif

#ifndef BOOL
typedef unsigned int BOOL;
#endif

#ifndef USHORT
typedef unsigned short USHORT;
#endif

#ifndef UINT32
#include <stdint.h>
typedef uint32_t UINT32;
#endif

#ifdef __cplusplus
extern "C" 
{
#endif

#undef BOOL

#include <stdbool.h>

#ifdef __cplusplus
}
#endif

#endif