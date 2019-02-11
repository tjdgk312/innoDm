#ifndef GLOBAL_H
#define GLOBAL_H

#include "innosdk.h"

extern InnoSDK *pMain;

//#define SDK_DATABASE

// IDLE 상태는 모두 0으로 통일한다.

// IDLE MODE STEP
#define IDLE_NORMALSTATE        0
#define IDLE_GATECLOSECHK       1

// DEPOSIT MODE STEP
#define DEPOSIT_IDLE            0
#define DEPOSIT_WAITINPUT       1
#define DEPOSIT_WAITCOUNT       2
#define DEPOSIT_WAITCOUNTING    3
#define DEPOSIT_CHECKREJECT     4
#define DEPOSIT_WAITREJECT      5
#define DEPOSIT_BLOCKOPENCHK    6
#define DEPOSIT_ADDDEPOSIT      7
#define DEPOSIT_ADDCOMPLETE     8

// ETC MODE STEP
#define ETC_IDLE            0
#define ETC_GATEOPENCHK       111

// CANCEL MODE STEP
#define CANCEL_IDLE             0
#define CANCEL_GATEOPENCHK      1
#define CANCEL_WAITRETURN       2
#define CANCEL_CHECKCLOSE       3       // 도어닫힘체크

// COMPLETE MODE STEP
#define COMPLETE_IDLE           0
#define COMPLETE_GATEOPENCHK    1
#define COMPLETE_SETIDLECHK     2

// TRANSCASH MODE STEP
#define TRANSCASH_






#endif // GLOBAL_H
