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

#ifndef _SFM_IDENTIFY_H_
#define _SFM_IDENTIFY_H_

#include "SFM_Error.h"

/* Extern value */
extern int g_bConnectionStatus;

#ifdef __cplusplus
extern "C"
{
#endif

void UF_SetIdentifyCallback( void (*Callback)( BYTE ) );

// API list
UF_RET_CODE UF_Identify( UINT32* userID, BYTE* subID );
UF_RET_CODE UF_IdentifyTemplate( UINT32 templateSize, BYTE* templateData,  UINT32* userID, BYTE* subID );
UF_RET_CODE UF_IdentifyImage( UINT32 imageSize, BYTE* imageData,  UINT32* userID, BYTE* subID );

// New API
SFM_RET_CODE SFM_IdentifyImage( UINT32 imageSize, BYTE* imageData,  UINT32* userID, BYTE* subID );
SFM_RET_CODE SFM_IdentifyImageEx(UINT32 imageSize, BYTE* imageData, UINT32* userID, BYTE* subID);

#ifdef __cplusplus
}
#endif

#endif