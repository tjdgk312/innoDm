#ifndef DEFINITION_H
#define DEFINITION_H


#define HMI_VERSION   "VERSION 0.1"

#define     MAX_NOTE_NUM    12

// Device ID
#define     ID_CURRENCY     0x0031
#define     ID_RECEIPT      0x0032
#define     ID_FINGER       0x0033
#define     ID_BARCODE      0x0034
#define     ID_COMM         0x0035
#define     ID_MOT1         0x0036
#define     ID_MOT2         0x0037
#define     ID_DOOR         0x0038

// Module ID
#define     ID_OUTGATE      "6O"    // O
#define     ID_INGATE       "6I"    // I
#define     ID_BLOCK        "7B"    // B
#define     ID_SEAL         "7H"    // B
#define     ID_LOCK         "7L"    // L
#define     ID_DOOR_UNLOCK  "8U"    // U
#define     ID_DOOR_LOCK    "8L"    // L
#define     ID_DOOR_EM      "8E"    // L
#define     ID_DOOR_SEN     "8S"    // Check Sensor

// Return ID
#define     ID_MOT1_BD      0x41
#define     ID_MOT2_BD      0x42
#define     ID_DOOR_BD      0x43
#define     ID_CURR_BD      0x31

#define     ERR_OUTGATE     1
#define     ERR_INGATE      2
#define     ERR_BLOCK       3
#define     ERR_SEAL        4
#define     ERR_LOCK        5

// Buffer Size
#define     CMD_BUF_SIZE    245

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

typedef enum
{
    CURRENCY_IDLE = 0       ,
    CURRENCY_READY          ,
    CURRENCY_STACK          ,
    CURRENCY_RESULT_REQ     ,
    CURRENCY_SETUP          ,
    CURRENCY_CHK_OPEN       ,
    CURRENCY_OPENED         ,
    CURRENCY_SET_LIVE       ,
    CURRENCY_REQ_STOP       ,
    CURRENCY_REQ_STOPED     ,
    CURRENCY_REQ_START      ,
    CURRENCY_REQ_STARTED    ,
    CURRENCY_NOCON          ,
    CURRENCY_DEV_SETUP2     ,
    CURRENCY_STATE_DATA2    ,
    CURRENCY_RESULT_CLEAR   ,
    CURRENCY_ERROR          ,
    CURRENCY_CHANGE         ,
    CURRENCY_COUNT_STOP
} Curr_TypeDef;

typedef enum
{
    MACHINE_INIT = 0        ,
    MACHINE_IDLE            ,
    MACHINE_WAIT            ,
    MACHINE_OUTOPEN         ,
    MACHINE_OUTOPENED       ,
    MACHINE_OUTCLOSE        ,
    MACHINE_OUTCLOSED       ,
    MACHINE_INOPEN          ,
    MACHINE_INOPENED        ,
    MACHINE_INCLOSE         ,
    MACHINE_INCLOSED        ,
    MACHINE_BLOCKOPEN       ,
    MACHINE_BLOCKOPENED     ,
    MACHINE_BLOCKCLOSE      ,
    MACHINE_BLOCKCLOSED     ,
    MACHINE_READY           ,
    MACHINE_WAIT_MONEY      ,
    MACHINE_DEPOSIT         ,
    MACHINE_DEPOSITING      ,
    MACHINE_DEPOSITED       ,
    MACHINE_RETURN          ,
    MACHINE_CHK_RECEIPT     ,
    MACHINE_WS_DEPOSIT      ,
    MACHINE_WS_DEPOSITED    ,
    MACHINE_SEAL            ,
    MACHINE_CHK_UNLOCK      ,
    MACHINE_WAIT_SEAL       ,
    MACHINE_CHK_SEAL        ,
    MACHINE_SEALED          ,
    MACHINE_CHK_DOOR_UNLOCK ,
    MACHINE_DOOR_UNLOCKED   ,
    MACHINE_CHK_DOOR_LOCK   ,
    MACHINE_DOOR_LOCKED     ,
    MACHINE_OPENBLOCK      ,
    MACHINE_BULKMODE      ,
    MACHINE_ERROR
} Machine_StatDef;

#define RECEIPT_CHK_SEN     0x0010
#define RECEIPT_OK          0x0030
#define RECEIPT_PAPER_EMPTY 0x0001
#define RECEIPT_HEAD_EMPTY  0x0004
#define RECEIPT_PRINT1      0x0020
#define RECEIPT_PRINT2      0x0021
#define RECEIPT_PRINTED     0x0022
#define RECEIPT_CUT         0x0023

#define BARCODE_IDLE        0x0040
#define BARCODE_LIGHT_ON    0x0041
#define BARCODE_TIMEOUT     0x0042
#define BARCODE_NO_CON      0x0043

#define FINGER_IDENTIFY     0x0040
#define FINGER_LOAD         0x0041
#define FINGER_SAVE         0x0042
#define FINGER_DELETE       0x0043
#define FINGER_ENROLL       0x0044

#define COMM_UPPERDOOR       0x0050

//DEFINE Receive Data from Machine


//DEFINE COMMAND
#define     CMD_OUTGATE_OPEN    0x1001
#define     CMD_OUTGATE_CLOSE   0x1002
#define     CMD_INGATE_OPEN     0x1003
#define     CMD_INGATE_CLOSE    0x1004
#define     CMD_BLOCK_OPEN      0x1005
#define     CMD_BLOCK_CLOSE     0x1006
#define     CMD_SEAL_OPEN       0x1007
#define     CMD_SEAL_CLOSE      0x1008
#define     CMD_LOCK_OPEN       0x1009
#define     CMD_LOCK_CLOSE      0x100A
#define     CMD_RECEIPT_PRINT   0x100B
#define     CMD_WS_START        0x100C
#define     CMD_BARCODE_ON      0x100D
#define     CMD_FINGER_IDT      0x100E
#define     CMD_FINGER_ENROLL   0x100F

#define     CMD_CURRENCY_SET_LIVE       0x1010
#define     CMD_CURRENCY_STOP           0x1011
#define     CMD_CURRENCY_START          0x1012
#define     CMD_CURRENCY_DEVICE_SETUP2  0x1013
#define     CMD_CURRENCY_STATE_DATA2    0x1014
#define     CMD_CURRENCY_CHK_DEV_STAT2  0x1015
#define     CMD_CURRENCY_CHK_SENSOR     0x1016
#define     CMD_CURRENCY_CLEAR_JAM      0x1017
#define     CMD_CURRENCY_CHK_CNT_RELSULT    0x1018
#define     CMD_CURRENCY_CHANGE         0x1019
#define     CMD_CURRENCY_COUNT_STOP      0x1020
#define     CMD_CURRENCY_CLEAR_STACKER     0x1021

#define     CMD_COMM_UPPERSEN     0x1022


// Machine Status
#define     CMD_IDLE                0x1100
#define     CMD_INIT_MACHINE        0x1101
#define     CMD_READY               0x1110
#define     CMD_DEPOSIT             0x1120
#define     CMD_DEPOSITED           0x1121
#define     CMD_RETURN              0x1122
#define     CMD_BULK_DEPOSIT        0x1123
#define     CMD_SENDELAY            0x1124

#define     CMD_DOOR_LOCK           0x1130
#define     CMD_DOOR_UNLOCK         0x1131
#define     CMD_DOOR_EM             0x1132
#define     CMD_DOOR_SEN            0x1133

#define     CMD_SEAL_TWIST          0x1140

#define     ACK_CURRENCY_COUNT_START    0x0032
#define     ACK_CURRENCY_RESULT         0x0034
#define     ACK_CURRENCY_SENSOR         0x0035
#define     ACK_CURRENCY_JAM            0x003E

#define     ACK_GATE_EMERGENCY          0x0053
#define     ACK_SEAL_COMPLETE           0x0060
#define     ACK_DOOR_OPEN               0x0061
#define     ACK_DOOR_CLOSE              0x0062
#define     ACK_RECEIPT_COMPLETE        0x0063
#define     ACK_BARCODE_DATA            0x0064
#define     ACK_BARCODE_TIMEOUT         0x0065
#define     ACK_FINGER_IDENTIFY         0x0070
#define     ACK_FINGER_TIMEOUT          0x0071
#define     ACK_FINGER_NOTFOUND         0x0072
#define     ACK_FINGER_ENROLL           0x0073

#define     ACK_DEVICE_ERROR            0x0100

#define     NO_CHECK    0
#define     CUR_SEN     1
#define     DOOR_SEN    2
#define     UP_SEN      3

//DEFINE CURRENCY COMMAND
typedef struct
{
    uint16_t    nation;
    uint16_t    num_Denom;
}t_Denom;

typedef struct
{
    uint32_t    nReject;
    uint32_t    nOutStack;
    uint32_t    nData[MAX_NOTE_NUM];
    uint32_t    nPreData[MAX_NOTE_NUM];
}t_NOTE;

typedef struct
{
    uint8_t Error;
//////////////// Sensor status 1
    uint8_t sol_pos;    // Solenoid Position 0: to Rejector
    uint8_t rp1_left;   // 0-> JAM or Front cover opened
    uint8_t rp1_right;  // 0-> JAM or Front cover opened
    uint8_t jp_left;    // 0-> JAM or Front cover opened
    uint8_t jp_right;   // 0-> JAM or Front cover opened
//////////////// Sensor status 2
    uint8_t sp_left;    // 0-> JAM or Rear cover opened
    uint8_t sp_right;   // 0-> JAM or Rear cover opened
    uint8_t stacker;    // 0-> there is note on stacker pocket
    uint8_t reject_f;   // 0-> there is note on reject pocket
    uint8_t front_cover;// 0-> Closed
    uint8_t rear_cover; // 0-> Closed
    uint8_t reject_r;   // 0-> there is note on reject pocket
    uint8_t hopper;     // 0-> no bank naote on hopper

}t_CUR_SENSOR;


typedef struct
{
    uint32_t    Cur_Code;
    uint32_t    Count_Mode;
    uint32_t    fAdd;
    uint32_t    stackCap;
}t_Cur_Set;

#endif // DEFINITION_H
