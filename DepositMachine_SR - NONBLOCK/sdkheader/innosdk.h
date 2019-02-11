#ifndef INNOSDK_H
#define INNOSDK_H

#include <QString>

#include <QtSerialPort/QSerialPort>
#include "database/databasemanager.h"
#include "device/statethread.h"
#include "vault.h"
#include "user.h"
#include "usbfinger/finger_class.h"


#include <QObject>

// window test
//#define PATH_USER  "D:/Deposit_Info.db"
//#define PATH_DEPOSIT   "D:/DepositList.db"

// pi test
#define PATH_USER    "/home/pi/Deposit_Info.db"
#define PATH_DEPOSIT "/home/pi/DepositList.db"

// ubuntu test
//#define PATH_USER    "/home/young/Deposit_Info.db"
//#define PATH_DEPOSIT "/home/young/DepositList.db"




class InnoSDK : public QObject
{
    Q_OBJECT

public:
    explicit InnoSDK(QWidget *parent = 0);


    int initFlag;

    int fScan;                // 스캔 플래그
    finger_class *pFinger;

    QString main_dbPath;
    State_Thread *m_stateThread;
    t_NOTE          tNoteData;
    t_CUR_SENSOR    tCurrSen;
    uint32_t        dev_err;
    uint32_t        receipt_comp;
    bool    fOutStack = false;      // Currency in OutStacker;
    bool    fCountStart = false;    // Counting Start
    bool    fGateEmcy  = false;
    bool    fSealComplete = false;
    bool    fDoorOpen  = false;
    bool    fDoorClose = false;
    int init_serial();
    void lang_change();
    bool StateChanged;
    DatabaseManager* pDatabase;

    int errReqCnt;              // 에러 났을 시 재요청 플래그

    int fConnection;            // 지문인식 연결 플래그


    QTimer *pMain_timer;
 //-------------------------------------SDK Interface------------------------------------------


    int currentMode;                        // 현재모드저장
    int preMode;                             // 이전모드저장
    int intermediateFlag;

    int currentStep;                        // 동작모드에 따른 현재 스텝 저장

    int currentState;                       // State Flag
    int errorStateCode;                     // Error code
    int retryCnt;                           // 트랜잭션 재시도 카운트
    int waitCnt;                            // 동작 대기 카운트

    int autoErrClearCnt;                    // 자동 에러 클리어 ( 미입금시만 동작 )

    bool barcodeChanged;
    QString preBagCode;
    QString currentBagCode;
    QByteArray barcodeByte;

    int depositFlag;

    int fingerState;

    const char* currentLogUser;
//    const char* currentLogId;
    int currentUserRole;
    bool loginState;

    int currentVaultID;
    int currentUserId;                                     // The Id of the user that performed the deposit
    int currentVaultDepositId;                     //   Deposit Number - Sequential
    int currentCurrencyId;                             //   CURRENCY_TYPE_DOP  = 38;
    QList<int>currentCountryList;



    VAULTDEPOSIT_INFO depositStr;
    LPVAULTDEPOSIT_INFO deposit_data;
//    LPUSER_INFO          currentUsetInfoStr;
    PFN_INNOSTEC_ON_DEPOSIT_COMPLETE pDepositFunc;
    void*  my_data_ptr;

    VAULT_SETTLEMENT_INFO settleStr;
    LPVAULT_SETTLEMENT_INFO settle_data;
    PFN_INNOSTEC_ON_SETTLEMENT_COMPLETE pSettlementFunc;
    void*  my_stdata_ptr;

    PFN_INNOSTEC_CURRENT_STATE pStatusFunc;

    int transStep;

    void operationStart();
    void setCountryUnitList(int code);

    QMap<int, QList<int>> unitMap;
    void init_countryUnit();

    char *getStatusMessage(int state);

    void depositOperationComplete();
    void init_DepositStr();
    void countCompleteSignal();
private:

    QSerialPort *m_serial;

    QDate m_date;

    QString rxLogfile;
    QString txLogfile;
//    QPlainTextEdit *editor;
//    Ui::MainWindow *ui;
    void depositCompleteSignal();

    void settlementCompleteSignal(int status);
signals:
    void wndSendRxData(QByteArray &tData);

public slots:
    void writeData(const QByteArray &data);

    void rsvMcData(int cmd, void * pdata);

private slots:
    void readData();
    void main_timer_event();
};




#endif // INNOSDK_H
