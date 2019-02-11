#ifndef GLOBAL_H
#define GLOBAL_H

#include "mainwindow.h"
#include "sdkheader/vault.h"
#include "sdkheader/user.h"
#include <QDebug>
#include "numpad/numpad.h"
#include "keypad/keypad.h"
#include "QMessageBox"

#include <QStyledItemDelegate>

#include <QDateTime>

#include <QTextCodec>

#define PROGRAM_VERSION "1.00_W"

// pi test
#define PATH_ROOT "/home/pi/INNOSTEC/"
#define PATH_DB "/home/pi/INNOSTEC/Deposit_DB_S.db"
#define INFO_TABLE     "table_info_S"
#define RECORD_TABLE     "table_record_S"
#define INFO2_TABLE     "table_info2_S"
#define ALARM_TABLE     "alarm_record"

#define DEFAULTCOUNTRY "03"                             // 버젼 국가설정   03
#define TEXTCOUNTRY "KRW"                             // 버젼 국가설정   03

#define TWISTPERIOD 1000                 // 봉투 흔들기 주기


typedef enum
{
    GATE_STOP = 0   ,
    GATE_OPEN       ,
    GATE_CHK_OPEN   ,
    GATE_OPENED     ,
    GATE_CLOSE      ,
    GATE_CHK_CLOSE  ,
    GATE_CLOSED     ,
    GATE_REOPEN     ,
    GATE_BRAKE      ,
    GATE_FAILED     ,
    GATE_CHK_STAT   ,
    GATE_ERROR      ,
    GATE_TWIST      ,
    GATE_TWISTED    ,
    GATE_CHGVAR     ,
    GATE_CHKSEN     ,
    GATE_DOPENED    ,
    GATE_DCLOSED    ,
} Gate_TypeDef;


// FINGER IDENTIFY
#define FINGERTYPE 1            // 0 = Device ,  1 = Pc

extern MainWindow *p_Main;

extern int countryCount;
extern QStringList countryList;

extern QTranslator *translator;

//numpad
extern numpad  *num_pad;

//keypad
extern keypad  *key_pad;

extern QStyledItemDelegate  *itemDelegate;              // 콤보박스  Style 적용을 위한 전역

#define ROLLLEFT 10     // 용지 잔여 리미트 제한  - 이 수량 미만일 시  롤교체여부 지속체크
// main page define
#define LOGINPAGE    0
#define MENUPAGE    1
#define LOADINGPAGE 2
#define RESULTPAGE  3
#define SETTINGPAGE 4
#define MANAGEPAGE 5
#define TRANSPAGE 6
#define USERPAGE 7
#define SEARCHPAGE 8
#define EVENTVIEWPAGE 9
#define REPAIRPAGE 10
#define SIGNPAGE   11
#define VISITORPAGE   12
#define ETCPAGE   13
#define MANAGERMAIN    997
#define SETCURPAGE  998
#define TESTPAGE    999

// deposit flag
#define ESCROWDEPOSIT   1
#define BULKDEPOSIT     2
#define ETCDEPOSIT      3

// system size
#define MAIN_WITH       800
#define MAIN_HEIGHT     480

// manual deposit step
#define IDLE_DEPOSIT    0
#define WAIT_COUNTRYSEL 1
#define WAIT_INPUT      2
#define WAIT_COUNTING      3
#define WAIT_STARTCOUNT      9
#define CHECK_REJECT    4
#define WAIT_REJECT     5
#define WAIT_OUTREJECT  6
#define ERROR_CHECK     7
#define WAIT_CANCELREJECT 8
#define DISPLAY_COMPLETE     10
#define WAIT_SETTLECOMPLETE    11
#define WAIT_TRDOOROPEN    12
#define WAIT_TRDOORCLOSE    13
#define DISPLAY_TRCOMPLETE    14
#define WAIT_FINGERPRINT    15
#define WAIT_CHANGEBAGBARCODE    16
#define CHECK_TRDOORLOCK 17
#define DISPLAY_CANCELCOMPLETE 18
#define WAIT_COUPONPRINT 19
#define COUPONPRINT_COMPLETE 20
#define WAIT_COMPLETE 21
#define CHECK_TRDOOROPEN    22
#define WAIT_DBRECEIVE    23
#define WAIT_ADDDEPOSIT    24
#define WAIT_ADDCOMPLETE    25

#define WAIT_RFIDASSIGN   27
#define WAIT_OUTRETRY     28

#define ETC_OUTOPEN    30
#define ETC_OUTCLOSE    31
#define ETC_CANCELCLOSE    32

#define CANCEL_OUTOPEN    40
#define CANCEL_WAITRETURN 41
#define CANCEL_OUTCLOSE  42

#define COMP_BLOPEN 50
#define COMP_INOPEN 51
#define COMP_CHKPOC 52
#define COMP_INCLOSE 53
#define COMP_BLCLOSE 54

#define MID_BLOPEN 60
#define MID_INOPEN 61
#define MID_CHKPOC 62
#define MID_INCLOSE 63
#define MID_BLCLOSE 64

#define CAL_BLOPEN 70
#define CAL_INOPEN  71
#define CAL_CHKPOC  72
#define CAL_INCLOSE 73
#define CAL_BLCLOSE 74

#define TR_UNLOCK   80
#define BAG_TWIST   81
#define WAIT_INCLOSE 82  // 에러상태처리대기
#define WAIT_OUTCLOSE 83  // 에러상태처리대기
#define WAIT_CLEARJAM 84  // 에러상태처리대기
#define AUTO_INCLOSE 85  // 에러상태처리대기
#define AUTO_DOORLOCK 86  // 에러상태처리대기

//  Server Protocol Define

#define WAIT_INFORECEIVE   999
#define WAIT_EDITRECEIVE   998

//정보전송 또는 요청 S_  응답측은 R_

#define S_DEVICEINIT 0x01       // 장비초기데이터 요청
#define R_DEVICEINIT 0x02

#define S_SENDINFO   0x31       // 이벤트 데이터전달
#define R_SENDINFO   0x32

#define S_REQDBDATA 0x33
#define R_REQDBDATA 0x34

#define S_TRANSSIGN 0x35
#define R_TRANSSIGN 0x36

#define S_FINGASSIGN 0x37
#define R_FINGASSIGN 0x38


#define S_SENDCHANGED 0x41
#define R_SENDCHANGED 0x42

#define S_SETPW 0x45          // 매니저 PW 변경 요청
#define R_SETPW 0x46

#define S_CHECKDEV    0x51
#define S_TRANSACTION     0x52
#define R_TRANSACTION     0x53


// 기기상태 점검항목으로 통합---------------------------------------
#define S_DEVSTATE   0x60             // check state poll
#define R_DEVSTATE   0x61             // check state poll

#define S_DOORSTATE 0x47          // 도어 상태 변경 정보 전송
#define R_DOORSTATE 0x48
#define S_PRINTSTATE 0x49          // 프린터상태
#define R_PRINTSTATE 0x50
#define S_ALARM 0x54
#define R_ALARM 0x55

#define FINGERCOMP 0x56

#endif // GLOBAL_H
