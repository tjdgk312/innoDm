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

    File : SFM_Image.c
    Descriptions : Image API
    API list
    - UF_ConvertToBitmap : Only Windows
    - UF_SaveImage
    - UF_LoadImage

    - UF_ReadImage
    - UF_ReadImageEx
    - UF_ScanImage
	- UF_ScanImageEx

    New API
    - SFM_ReadImage
    - SFM_ReadImageEx
    - SFM_ScanImage
    - SFM_ScanImageEx

*/

#include <stdlib.h>

#include "SFM_Command.h"
#include "SFM_Packet.h"
#include "SFM_Flag.h"
#include "SFM_Module.h"
#include "SFM_SysParameter.h"
#include "SFM_Image.h"

#include "SFM_BMP.h"

// Callback

extern BOOL UF_ScanMsgCallback( BYTE );

UF_RET_CODE UF_ReadImage( UFImage* image )
{
	UF_RET_CODE result;

	UINT32 param = 0;
	UINT32 size = UF_GetDefaultPacketSize();
	BYTE flag = 0;

	result = UF_Command( SFM_COM_RIX, &param, &size, &flag );

	if( result != UF_RET_SUCCESS )
	{
		return result;
	}

	if( flag == SFM_PROTO_RET_SUCCESS )
	{
		return UF_ReceiveDataPacket( SFM_COM_RIX, (BYTE*)image, size );
	}
	else if( flag == SFM_PROTO_RET_UNSUPPORTED )
	{
		param = 0;
		size = 0;
		flag = 0;
		result = UF_Command( SFM_COM_RI, &param, &size, &flag );

		if( result != UF_RET_SUCCESS )
		{
			return result;
		}

		if( flag != SFM_PROTO_RET_SUCCESS )
		{
			return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE)flag );
		}

		return UF_ReceiveRawData( (BYTE*)image, size, UF_CalculateTimeout(size), TRUE );
	}
	else
	{
		return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE)flag );
	}
}

/**
 *	Read image extention
 */

UF_RET_CODE UF_ReadImageEx( UFImage* image, UF_IMAGE_TYPE type, int wsqBitRate )
{
	UF_RET_CODE result;

	UINT32 param = 0;
	UINT32 size = UF_GetDefaultPacketSize();
	BYTE flag = 0;

	if(type == UF_WSQ_HQ_IMAGE || type == UF_WSQ_MQ_IMAGE || type == UF_WSQ_LQ_IMAGE)
	{
		if(wsqBitRate > 0)
			param = wsqBitRate;
		else
			param = 225; // default, 2.25
	}

	result = UF_Command( SFM_COM_RIX, &param, &size, &flag );

	if( result != UF_RET_SUCCESS )
	{
		return result;
	}

	if( flag == SFM_PROTO_RET_SUCCESS )
	{
		return UF_ReceiveDataPacket( SFM_COM_RIX, (BYTE*)image, size );
	}
	else if( flag == SFM_PROTO_RET_UNSUPPORTED )
	{
		param = 0;
		size = 0;
		flag = 0;
		result = UF_Command( SFM_COM_RI, &param, &size, &flag );

		if( result != UF_RET_SUCCESS )
		{
			return result;
		}

		if( flag != SFM_PROTO_RET_SUCCESS )
		{
			return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE)flag );
		}

		return UF_ReceiveRawData( (BYTE*)image, size, UF_CalculateTimeout(size), TRUE );
	}
	else
	{
		return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE)flag );
	}
}

UF_RET_CODE UF_ScanImage( UFImage* image )
{
	UF_RET_CODE result;

	UINT32 param = 0;
	UINT32 size = UF_GetDefaultPacketSize();
	BYTE flag = 0;
	
	result = UF_CommandEx( SFM_COM_SIX, &param, &size, &flag, UF_ScanMsgCallback );

	if( result != UF_RET_SUCCESS )
	{
		return result;
	}

	if( flag == SFM_PROTO_RET_SUCCESS )
	{
		return UF_ReceiveDataPacket( SFM_COM_SIX, (BYTE*)image, size );
	}
	else if( flag == SFM_PROTO_RET_UNSUPPORTED )
	{
		param = 0;
		size = 0;
		flag = 0;
		result = UF_CommandEx( SFM_COM_SI, &param, &size, &flag, UF_ScanMsgCallback );

		if( result != UF_RET_SUCCESS )
		{
			return result;
		}

		if( flag != SFM_PROTO_RET_SUCCESS )
		{
			return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE)flag );
		}

		return UF_ReceiveRawData( (BYTE*)image, size, UF_CalculateTimeout(size), TRUE );
	}
	else
	{
		return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE)flag );
	}
}

UF_RET_CODE UF_ScanImageEx( UFImage* image, UF_IMAGE_TYPE type, int wsqBitRate )
{
	UF_RET_CODE result;

	UINT32 param = 0;
	UINT32 size = UF_GetDefaultPacketSize();
	BYTE flag = 0;

	if(type == UF_WSQ_HQ_IMAGE || type == UF_WSQ_MQ_IMAGE || type == UF_WSQ_LQ_IMAGE)
	{
		if(wsqBitRate > 0)
			param = wsqBitRate;
		else
			param = 225; // default, 2.25
	}

	result = UF_CommandEx( SFM_COM_SIX, &param, &size, &flag, UF_ScanMsgCallback );

	if( result != UF_RET_SUCCESS )
	{
		return result;
	}

	if( flag == SFM_PROTO_RET_SUCCESS )
	{
		return UF_ReceiveDataPacket( SFM_COM_SIX, (BYTE*)image, size );
	}
	else if( flag == SFM_PROTO_RET_UNSUPPORTED )
	{
		param = 0;
		size = 0;
		flag = 0;
		result = UF_CommandEx( SFM_COM_SI, &param, &size, &flag, UF_ScanMsgCallback );

		if( result != UF_RET_SUCCESS )
		{
			return result;
		}

		if( flag != SFM_PROTO_RET_SUCCESS )
		{
			return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE)flag );
		}

		return UF_ReceiveRawData( (BYTE*)image, size, UF_CalculateTimeout(size), TRUE );
	}
	else
	{
		return UF_GetErrorCode( (SFM_PROTOCOL_RET_CODE)flag );
	}
}

UF_RET_CODE UF_SaveImage( const char* fileName, UFImage* image )
{
	int i;
	BITMAPFILEHEADER fileHeader;
	BITMAPINFO* bitmapInfo;
	BYTE* sourceBuf;
	BYTE* targetBuf;
	BYTE sourceValue;
	FILE* fp;
	int size;

	// image->width is always a multiple of 4
	if( image->format == 0 || image->format == 3) // gray
	{
		sourceBuf = image->buffer;
	}
	else
	{
		sourceBuf = (BYTE*)malloc( image->width * image->height );

		if( !sourceBuf )
		{
			return UF_ERR_OUT_OF_MEMORY;
		}

		for( i = 0; i < image->width * image->height; i++ )
		{
			if( image->format == 1 ) // binary
			{
				sourceValue = image->buffer[i/8];
				sourceBuf[i] = ((sourceValue >>(i % 8)) & 1)? 255 : 0;
			}
			else // 4 bit gray
			{
				sourceValue = image->buffer[i/2];

				sourceBuf[i] = (i % 2 == 0)? (sourceValue & 0x0f) << 4 : sourceValue & 0xf0;
			}
		}
	}

	targetBuf = (BYTE*)malloc( image->height * image->width );
	if( !targetBuf )
	{
		if( image->format != 0 && image->format != 3)
		{
			free( sourceBuf );
		}

		return UF_ERR_OUT_OF_MEMORY;
	}

	for( i = 0; i < image->height; i++ )
	{
		memcpy( targetBuf + (image->height - 1 - i) * image->width,
			sourceBuf + i * image->width, image->width );
	}


	bitmapInfo = (BITMAPINFO*)malloc( sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD) );
	if( !bitmapInfo )
	{
		if( image->format != 0 && image->format != 3)
		{
			free( sourceBuf );
		}
		free( targetBuf );

		return UF_RET_SUCCESS;
		//return NULL; hclee
	}

	bitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapInfo->bmiHeader.biWidth = image->width;
	bitmapInfo->bmiHeader.biHeight = image->height;
	bitmapInfo->bmiHeader.biPlanes = 1;
	bitmapInfo->bmiHeader.biBitCount = 8;
	bitmapInfo->bmiHeader.biCompression = BI_RGB;
	bitmapInfo->bmiHeader.biSizeImage = 0;
	bitmapInfo->bmiHeader.biXPelsPerMeter = 0;
	bitmapInfo->bmiHeader.biYPelsPerMeter = 0;
	bitmapInfo->bmiHeader.biClrUsed = 0;
	bitmapInfo->bmiHeader.biClrImportant = 0;

    for ( i = 0 ; i < 256 ; i++ )
	{
        bitmapInfo->bmiColors[i].rgbRed = i;
        bitmapInfo->bmiColors[i].rgbGreen = i;
        bitmapInfo->bmiColors[i].rgbBlue = i;
    }

	// Fill in the fields of the file header
	fileHeader.bfType		= ((WORD) ('M' << 8) | 'B');	// is always "BM"
	fileHeader.bfSize		= sizeof(fileHeader) + sizeof(bitmapInfo) + image->width * image->height;
	fileHeader.bfReserved1 	= 0;
	fileHeader.bfReserved2 	= 0;
	fileHeader.bfOffBits	= (DWORD) (sizeof(fileHeader) + sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD) );

	fp = fopen( fileName, "wb" );

	if( !fp )
	{
		if( image->format != 0 && image->format != 3)
		{
			free( sourceBuf );
		}
		free( bitmapInfo );
		free( targetBuf );
		return UF_ERR_FILE_IO;
	}

	size = sizeof(fileHeader);
	if( fwrite( &fileHeader, 1, size, fp ) == size )
	{
		size = sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD);
		if( fwrite( bitmapInfo, 1, size, fp ) == size )
		{
			size = image->width * image->height;

			if( fwrite( targetBuf, 1, size, fp ) == size )
			{
				fclose( fp );
				if( image->format != 0 && image->format != 3)
				{
					free( sourceBuf );
				}
				free( bitmapInfo );
				free( targetBuf );
				return UF_RET_SUCCESS;
			}
		}
	}

	fclose( fp );

	if( image->format != 0 && image->format != 3)
	{
		free( sourceBuf );
	}

	free( bitmapInfo );
	free( targetBuf );
	return UF_ERR_FILE_IO;
}

/**
 *	Load a bitmap file
 */
UF_RET_CODE UF_LoadImage( const char* fileName, UFImage* image )
{
	int i;
	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER bitmapInfo;
	BYTE* buffer;
	FILE* fp;
	int size;

	fp = fopen( fileName, "rb" );

	if( !fp )
	{
		return UF_ERR_FILE_IO;
	}

	size = sizeof(fileHeader);

	if( fread( &fileHeader, 1, size, fp ) != size )
	{
		fclose( fp );
		return UF_ERR_FILE_IO;
	}

	size = sizeof(BITMAPINFOHEADER);

	if( fread( &bitmapInfo, 1, size, fp ) != size )
	{
		fclose( fp );
		return UF_ERR_FILE_IO;
	}

	if( bitmapInfo.biBitCount != 8 || bitmapInfo.biCompression != BI_RGB )
	{
		fclose( fp );
		return UF_ERR_INVALID_FILE;
	}

	image->width = bitmapInfo.biWidth;
	image->height = bitmapInfo.biHeight;
	image->compressed = 0;
	image->encrypted = 0;
	image->format = 0; // gray
	image->imgLen = image->width * image->height;

	buffer = (BYTE*)malloc( image->imgLen );

	if( !buffer )
	{
		fclose( fp );
		return UF_ERR_OUT_OF_MEMORY;
	}

	fseek( fp, fileHeader.bfOffBits, SEEK_SET );

	if( fread( buffer, 1, image->imgLen, fp ) != image->imgLen )
	{
		fclose( fp );
		free( buffer );
		return UF_ERR_FILE_IO;
	}


	for( i = 0; i < image->height; i++ )
	{
		memcpy( image->buffer + (image->height - 1 - i) * image->width,
			buffer + i * image->width, image->width );
	}

	free( buffer );
	fclose( fp );

	return UF_RET_SUCCESS;
}

SFM_RET_CODE SFM_ReadImage(unsigned char* image)
{
    int i, ret;
    int tryCount = 0;
    UINT32 param = 0;
    BYTE flag = 0;
    BYTE snd_packet[SFM_PACKET_LEN];
    BYTE rcv_packet[SFM_PACKET_LEN];

    int size_header = 28;
    int size = SFM_WIDTH * SFM_HEIGHT + size_header;

    UF_SendPacket( SFM_COM_RI , param, size, flag, 10000 );

    UF_ReceiveRawData(image, size , UF_CalculateTimeout(size), TRUE);

    return SFM_RET_SUCCESS;
}

SFM_RET_CODE SFM_ReadImageEx(BYTE* image, UF_IMAGE_TYPE type, int wsqBitRate)
{
    int i, ret;
    UINT32 param = 0;
    UINT32 timeout = 10000;

    //UINT32 size = (4 * 1024);
    BYTE flag = 0;
    BYTE snd_packet[SFM_PACKET_LEN];
    BYTE rcv_packet[SFM_PACKET_LEN];

    int const_data_packet_size = 0x1000;
    int size;

    if(type == UF_WSQ_HQ_IMAGE || type == UF_WSQ_MQ_IMAGE || type == UF_WSQ_LQ_IMAGE) {
        if(wsqBitRate > 0)
            param = wsqBitRate;
        else
            param = 225;
    }

    SFM_RS232_ClearBuffer();

    UF_SendPacket( SFM_COM_RIX , param, const_data_packet_size, flag, 10000 );    

    UF_ReceivePacket(rcv_packet, timeout );

    UF_ReceiveDataPacket( SFM_COM_RIX, image, const_data_packet_size);

    UF_ReceivePacket(rcv_packet, timeout );
    UF_ReceivePacket(rcv_packet, timeout );
    
    return SFM_RET_SUCCESS;
}

SFM_RET_CODE SFM_ScanImage(unsigned char* image, UINT32* size)
{
    SFM_RET_CODE result;

	UINT32 timeout = 10000;
    UINT32 param = 0;
    BYTE flag = 0;
    BYTE snd_packet[SFM_PACKET_LEN];
    BYTE rcv_packet[SFM_PACKET_LEN];

    UF_SendPacket(SFM_COM_SI, param, *size, flag, 10000);

    UF_ReceivePacket(rcv_packet, timeout );
    UF_ReceivePacket(rcv_packet, timeout );

    *size = SFM_GetPacketValue(SFM_PACKET_SIZE, rcv_packet);

    /* Receive Image */
    UF_ReceiveRawData(image, *size, UF_CalculateTimeout(*size), TRUE);

    return SFM_RET_SUCCESS;
}

SFM_RET_CODE SFM_ScanImageEx( unsigned char* image, UF_IMAGE_TYPE type, int wsqBitRate )
{
    SFM_RET_CODE result;

    UINT32 param = 0;
    UINT32 size = 0x1000;
    UINT32 imagefile_size;
    BYTE flag = 0;

    BYTE rcv_packet[SFM_PACKET_LEN];

    SFM_RS232_ClearBuffer();

	if(type == UF_WSQ_HQ_IMAGE || type == UF_WSQ_MQ_IMAGE || type == UF_WSQ_LQ_IMAGE)
	{
		if(wsqBitRate > 0)
			param = wsqBitRate;
		else
			param = 225; // default, 2.25
	}

    //result = UF_CommandEx( SFM_COM_SIX, &param, &size, &flag, NULL );

    UF_SendPacket(SFM_COM_SIX, param, size, flag, 10000);
/*
    if( result != SFM_RET_SUCCESS ) {
        SFM_RS232_ClearBuffer();
        return result;
    }
*/
    RecievePacket(rcv_packet, SFM_PACKET_LEN, 30000);
    
    RecievePacket(rcv_packet, SFM_PACKET_LEN, 30000);
    flag = SFM_GetPacketValue(SFM_PACKET_FLAG, rcv_packet );

    imagefile_size = SFM_GetPacketValue( SFM_PACKET_SIZE, rcv_packet );
    if (flag == SFM_PROTO_RET_SUCCESS) {
        UF_ReceiveDataPacket( SFM_COM_SIX, image, imagefile_size );
        SFM_RS232_ClearBuffer();
        return SFM_RET_SUCCESS;
    }
}
