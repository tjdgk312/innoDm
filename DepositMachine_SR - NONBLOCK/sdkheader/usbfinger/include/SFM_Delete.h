#ifndef _SFM_DELETE_H_
#define _SFM_DELETE_H_

#include "SFM_Error.h"

#define SFM_DELETE_ONLY_ONE		0x70
#define SFM_DELETE_MULTIPLE_ID	0x71

#ifdef __cplusplus
extern "C"
{
#endif

BOOL UF_DeleteMsgCallback( BYTE errCode );

UF_RET_CODE UF_Delete(UINT32 userID);
UF_RET_CODE UF_DeleteOneTemplate(UINT32 userID, int subID);
UF_RET_CODE UF_DeleteMultipleTemplates(UINT32 startUserID, UINT32 lastUserID, int* deletedUserID );
UF_RET_CODE UF_DeleteAll(void);
UF_RET_CODE UF_DeleteByScan(UINT32* userID);
UF_RET_CODE UF_DeleteAllAfterVerification(void);

#ifdef __cplusplus
}
#endif

#endif