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

#ifndef _SFM_TEMPLATE_H_
#define _SFM_TEMPLATE_H_

#include "SFM_Error.h"

#define MAXIMUM_TEMPLATE_SIZE	384
#define SFM_ADD_CHECKSUM		0x70

typedef enum {
	UF_AUTH_FINGERPRINT = 0x00,
	UF_AUTH_BYPASS = 0x01,
	UF_AUTH_REJECT = 0x03,
} UF_AUTH_TYPE;

typedef enum {
	UF_USER_SECURITY_DEFAULT				= 0,
	UF_USER_SECURITY_1_TO_1000			= 1,
	UF_USER_SECURITY_3_TO_10000			= 2,
	UF_USER_SECURITY_1_TO_10000			= 3,
	UF_USER_SECURITY_3_TO_100000		= 4,
	UF_USER_SECURITY_1_TO_100000		= 5,
	UF_USER_SECURITY_3_TO_1000000		= 6,
	UF_USER_SECURITY_1_TO_1000000		= 7,
	UF_USER_SECURITY_3_TO_10000000		= 8,
	UF_USER_SECURITY_1_TO_10000000		= 9,
	UF_USER_SECURITY_3_TO_100000000		= 10,
	UF_USER_SECURITY_1_TO_100000000		= 11,
} UF_USER_SECURITY_LEVEL;

typedef enum {
	UF_ADMIN_LEVEL_NONE			= 0,
	UF_ADMIN_LEVEL_ENROLL		= 1,
	UF_ADMIN_LEVEL_DELETE		= 2,
	UF_ADMIN_LEVEL_ALL			= 3
} UF_ADMIN_LEVEL;

typedef struct {
	UINT32 	userID;
	BYTE	numOfTemplate;
	BYTE	adminLevel;
	BYTE	reserved[2]; // reserved[0] is used for user security level
} UFUserInfo;

typedef struct {
	UINT32 	userID;
	UINT32 	checksum[10];
	BYTE	numOfTemplate;
	BYTE	authMode : 4;
	BYTE	adminLevel : 4;
	BYTE	duress[10];
	BYTE	securityLevel;
} UFUserInfoEx;

typedef struct {
	UINT32 	magicNo;
	UINT32 	numOfUser;
	UINT32 	numOfTemplate;
	UINT32	templateSize;
	UINT32	dataSize;
} UFTemplateDBHeader;

typedef struct {
	UINT32	userID;
	UINT32	userLevel; // entLimit << 24 | securityLevel << 16 | authMode << 8 | adminLevel
	UINT32 	numOfTemplate;
	UINT32	checksum;
} UFUserItemHeader;

#ifdef __cplusplus
extern "C"
{
#endif

void UF_SortUserInfo( UFUserInfo* userInfo, int numOfUser );
int UF_SortByUserID( const void* elem1, const void* elem2 );

UF_RET_CODE UF_GetNumOfTemplate( UINT32* numOfTemplate );
UF_RET_CODE UF_GetMaxNumOfTemplate( UINT32* maxNumOfTemplate );
UF_RET_CODE UF_GetAllUserInfo( UFUserInfo* userInfo, UINT32* numOfUser, UINT32* numOfTemplate );
UF_RET_CODE UF_GetAllUserInfoEx( UFUserInfoEx* userInfo, UINT32* numOfUser, UINT32* numOfTemplate );

UF_RET_CODE UF_SetAdminLevel( UINT32 userID, UF_ADMIN_LEVEL adminLevel );
UF_RET_CODE UF_GetAdminLevel( UINT32 userID, UF_ADMIN_LEVEL* adminLevel );
UF_RET_CODE UF_ClearAllAdminLevel(void);
UF_RET_CODE UF_ScanTemplate(BYTE* templateData, UINT32* templateSize, UINT32* imageQuality);
UF_RET_CODE UF_ReadOneTemplate( UINT32 userID, int subID, BYTE* templateData );
UF_RET_CODE UF_ReadTemplate( UINT32 userID, UINT32* numOfTemplate, BYTE* templateData );
UF_RET_CODE UF_CheckTemplate( UINT32 userID, UINT32* numOfTemplate );
UF_RET_CODE UF_FixProvisionalTemplate(void);
UF_RET_CODE UF_SetSecurityLevel( UINT32 userID, UF_USER_SECURITY_LEVEL securityLevel );
UF_RET_CODE UF_GetSecurityLevel( UINT32 userID, UF_USER_SECURITY_LEVEL* securityLevel );
UF_RET_CODE UF_FormatUserDatabase(void);

UF_RET_CODE UF_SaveDB( const char* fileName );
UF_RET_CODE UF_LoadDB( const char* fileName );

SFM_RET_CODE SFM_Read_template(int userID, unsigned char* template1, unsigned char* template2);

#ifdef __cplusplus
}
#endif

#endif