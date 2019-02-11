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
#ifndef _SFM_IMAGE_H_
#define _SFM_IMAGE_H_

#include "SFM_Error.h"

typedef enum {
    UF_GRAY_IMAGE			= 0x30,
    UF_BINARY_IMAGE			= 0x31,
    UF_4BIT_GRAY_IMAGE		= 0x32,
    UF_WSQ_IMAGE			= 0x33,
    UF_WSQ_HQ_IMAGE			= 0x33,
    UF_WSQ_MQ_IMAGE			= 0x34,
    UF_WSQ_LQ_IMAGE			= 0x35,
} UF_IMAGE_TYPE;

typedef struct {
	int	width;
	int	height;
	int	compressed;
	int	encrypted;
	int	format;
	int	imgLen;
	int	templateLen;
	BYTE buffer[1];
} UFImage;

extern int g_fdRS232;
extern SysParameter *sysParameter;

/* BITMAP Header */
#define DIB_DPM2DPI(x)	((unsigned int)(((float(x) / 100.0f) * 2.54f) + 0.5f))
#define DIB_DPI2DPM(x)	((unsigned int)(((float(x) / 2.54f) * 100.0f) + 0.5f))

#define DIB_LOCK(x)		((PBITMAPINFOHEADER)GlobalLock(x))
#define DIB_UNLOCK(x)	GlobalUnlock(x)

#ifdef __cplusplus
extern "C"
{
#endif

UF_RET_CODE UF_ReadImage( UFImage* image );
UF_RET_CODE UF_ReadImageEx( UFImage* image, UF_IMAGE_TYPE type, int wsqBitRate );
UF_RET_CODE UF_ScanImage( UFImage* image );

SFM_RET_CODE SFM_ReadImage(unsigned char* image);
SFM_RET_CODE SFM_ReadImageEx(BYTE* image, UF_IMAGE_TYPE type, int wsqBitRate);
SFM_RET_CODE SFM_ScanImage(unsigned char* image, UINT32* size);
SFM_RET_CODE SFM_ScanImageEx( unsigned char* image, UF_IMAGE_TYPE type, int wsqBitRate );

#ifdef __cplusplus
}
#endif

#endif