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

#ifndef _SFM_ENROLL_H_
#define _SFM_ENROLL_H_

#include "SFM_Error.h"
#include "SFM_SysParameter.h"
#include "SFM_Template.h"

/* Extern value */
extern int g_bConnectionStatus;

extern int g_fdRS232;
extern SysParameter *sysParameter;

typedef enum {
	UF_ENROLL_ONE_TIME 			= 0x30,
	UF_ENROLL_TWO_TIMES1			= 0x31,
	UF_ENROLL_TWO_TIMES2			= 0x32,
	UF_ENROLL_TWO_TEMPLATES1		= 0x41,
	UF_ENROLL_TWO_TEMPLATES2		= 0x42,
} UF_ENROLL_MODE;

typedef enum {
	UF_ENROLL_ADD_NEW				= 0x71,
	UF_ENROLL_AUTO_ID				= 0x79,
	UF_ENROLL_CONTINUE				= 0x74,
	UF_ENROLL_CHECK_ID				= 0x70,
	UF_ENROLL_CHECK_FINGER 			= 0x84,
	UF_ENROLL_CHECK_FINGER_AUTO_ID 	= 0x85,
	UF_ENROLL_DURESS					= 0x92,
} UF_ENROLL_OPTION;

typedef enum {
	SFM_ENROLL_ONE_TIME 			= 0x30,
	SFM_ENROLL_TWO_TIMES1			= 0x31,
	SFM_ENROLL_TWO_TIMES2			= 0x32,
	SFM_ENROLL_TWO_TEMPLATES1		= 0x41,
	SFM_ENROLL_TWO_TEMPLATES2		= 0x42,
} SFM_ENROLL_MODE;

typedef enum {
	SFM_ENROLL_ADD_NEW				= 0x71,
	SFM_ENROLL_AUTO_ID				= 0x79,
	SFM_ENROLL_CONTINUE				= 0x74,
	SFM_ENROLL_CHECK_ID				= 0x70,
	SFM_ENROLL_CHECK_FINGER 			= 0x84,
	SFM_ENROLL_CHECK_FINGER_AUTO_ID 	= 0x85,
	SFM_ENROLL_DURESS					= 0x92,
} SFM_ENROLL_OPTION;

#ifdef __cplusplus
extern "C"
{
#endif

UF_RET_CODE UF_Enroll( UINT32 userID, UF_ENROLL_OPTION option, UINT32* enrollID, UINT32* imageQuality );
UF_RET_CODE UF_EnrollImage(UINT32 userID, UF_ENROLL_OPTION enroll_option, UINT32 imageSize, BYTE* imageData, UINT32 *enrollID, UINT32 *Image_Quality);
UF_RET_CODE UF_EnrollTemplate( UINT32 userID, UF_ENROLL_OPTION option, UINT32 templateSize, BYTE* templateData, UINT32* enrollID );
UF_RET_CODE UF_EnrollMultipleTemplates( UINT32 userID, UF_ENROLL_OPTION option, int numOfTemplate, UINT32 templateSize, BYTE* templateData, UINT32* enrollID );

SFM_RET_CODE SFM_Enroll_Scan( UINT32 userID, UF_ENROLL_OPTION option, UINT32* enrollID, UINT32* imageQuality, int enroll_mode );
SFM_RET_CODE SFM_Enroll_Template(UINT32 userID, UF_ENROLL_OPTION enroll_option, UINT32 templateSize, BYTE* templateData, UINT32 *enrollID, UINT32 *Number_Of_Feature);
SFM_RET_CODE SFM_Enroll_ImageEx(UINT32 userID, UF_ENROLL_OPTION enroll_option, UINT32 imageSize, BYTE* imageData, UINT32 *enrollID, UINT32 *Image_Quality);
SFM_RET_CODE SFM_Enroll_TemplateEx(UINT32 userID, UF_ENROLL_OPTION enroll_option, UINT32 templateSize, BYTE* templateData, UINT32 *enrollID, UINT32 *Number_Of_Feature);
SFM_RET_CODE Get_Enroll_Mode(int *enroll_mode);

#ifdef __cplusplus
}
#endif

#endif