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

#ifndef _SFM_VERIFY_H_
#define _SFM_VERIFY_H_

#include <stdlib.h>

#include "SFM_Type.h"
#include "SFM_Error.h"
#include "SFM_Command.h"
#include "SFM_Packet.h"
#include "SFM_Flag.h"
#include "SFM_SysParameter.h"

extern int g_bConnectionStatus;

#ifdef __cplusplus
extern "C"
{
#endif

BOOL UF_VerifyMsgCallback( BYTE errCode );

// API list 
UF_RET_CODE UF_Verify(UINT32 userID, BYTE* subID);
UF_RET_CODE UF_VerifyImage( UINT32 imageSize, BYTE* imageData,  UINT32 userID, BYTE* subID );
UF_RET_CODE UF_VerifyTemplate( UINT32 templateSize, BYTE* templateData, UINT32 userID, BYTE* subID );
UF_RET_CODE UF_VerifyHostTemplate( UINT32 numOfTemplate, UINT32 templateSize, BYTE* templateData );

// New API
SFM_RET_CODE SFM_VerifyImage( UINT32 imageSize, BYTE* imageData,  UINT32 userID, BYTE* subID );
SFM_RET_CODE SFM_VerifyImageEx(UINT32 imageSize, BYTE* imageData, UINT32 userID, BYTE* subID );

#ifdef __cplusplus
}
#endif

#endif