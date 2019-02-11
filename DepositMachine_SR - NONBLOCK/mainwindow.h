#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "page_login.h"
#include "page_menu.h"
#include "page_loading.h"
#include "page_result.h"
#include "page_setting.h"
#include "page_transcash.h"
#include "page_managermenu.h"
#include "page_user.h"
#include "page_engineer.h"
#include "page_search.h"
#include "page_eventview.h"
#include "page_repair.h"
#include "page_signimage.h"
#include "page_visitorlogin.h"
#include "page_devicesetup.h"
#include "page_etc.h"

#include "class/listitemclass.h"

#include "messagedialog.h"

#include "printer/woosimprinter.h"

#include "PCSC/winscard.h"
#include "PCSC/wintypes.h"

// for test sdk include project
#include "sdkheader/vault.h"
#include "sdkheader/user.h"

#include "tcpserver/tcpserver.h"

#include "musicthread.h"
// for PC DATABASE
#include "sdkheader/database/databasemanager.h"

#define DEVICE_VER  "V1.00_W"
#define JkHlJk "4A6B486C4A6B"
#define MASTERCODE   "JHYSWSJWMJSHMIB"
#define CORPCODE    "LAON180621"

//#define PREMAIN     0           // 프린터 잔여량 설정 (센서감지이후)

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


    QUrl setMedia;
    QUrl preMedia;
    QUrl threadMedia;
    int mDelayCount;
    int mTimingCount;
    int mRepeatCount;
    bool mInitMedia;
    // 각 페이지 클래스---------------------------------
    page_login *pLogin;
    page_menu *pMenu;
    page_loading *pLoading;
    page_result *pResult;
    page_setting *pSetting;
    page_managermenu *pManagerMenu;
    page_transcash *pTransCash;
    page_user      *pUser;
    page_engineer *pEngineer;
    page_search   *pSearch;
    page_eventView *pEventView;
    page_repair    *pRepair;
    page_signImage *pSign;
    page_visitorLogin *pVisitor;
    page_deviceSetup  *pDevSetup;
    page_etc         *pEtc;

    MessageDialog *pMsgDialog;              // 메시지 다이얼로그
    TcpServer *pServer;                     // 통신스레드 클래스


    // 알람 저장 페이지 관련 -----------------------------------
    QVariant Albuf[30][5];
    QVariant curAlarm[1][5];
    int alCount;
    int alarmState;
    int stateFlag;                    // 알람발생 플래그
    bool stateSwich;

    int upsOpFlag;

    int currentDepositMode;
    int currentUserId;
    int currentUserRoles;

    int initializeFlag;             // 장비 초기화 작업 플래그
    int startupSetupFlag;           // 초기설정모드 플래그
    int deviceStatusFlag;           // 1= Jam State   (잼 에러 발생 시 해당 모드 스텝 종료 후 에러 화면으로 전환한다)

    DatabaseManager* pDatabase;

    QString currentCountry;
    QString curCountryText;

    QString authenKey;

    // 장비속성 관련----------------------------------
    QString deviceType;
    QString deviceNum;
    QString deviceIp;
    QString deviceVer;
    QString turnNum;                  // 입금회차기록 str
    QString capNum;                   // 현재 매수

    int depositNum;                 // 입금번호
    int eventFlag;                  // 입금, 현송 등 이벤트 구분

    int btn_keyTest;
    int btn_keyTimer;
    QString signImageName;


    QString managerPW;
    int langCountry; 

    int curDoorSen;                  // 도어 및 키 상태 변경 체크를 위한 저장변수
    int curKeySen;
    int lockFlag;                    // 잠금장치 변경 플래그

    // 인증관련--------------------------------------
    QString masterCode;
    int assignRfcard;
    int assignFlag;
    int assignTimer;
    unsigned char templateBuf[384];



    QString curTransCode;
    QString curUserId;
    QString curUserPW;
    // 지문인증?
    int fingerFileSize;
    int fingerBufSize;                // 파일 버퍼 사이즈
    int finCmdType;
    QFile* curUserFinger;

    // 현송관련
    QString curBagBarcode;      // 현재 백 바코드
    QString curBagAmount;        // 현재 백 총금액
    QString curBagCount;        // 현재 백 수량

    // 장치정보
    int totalBagCount;
    int totalPrintCount;
    int bagFlag;
    int bagState;

    int compareCount;

    // 롤 상태정보
    int rollState;
    int rollLeft;
    int rollFlag;                    // 프린트상태 변경 플래그
    int rollReqCnt;                  // 프린트상태 요청 딜레이 카운트


    //바코드
    QString curBarCode;

    int prePage;        // 이전페이지
    int curPage;        // 현재페이지

    // 웨딩관련 플래그 및 리스트 변수------------------------------------
    QMap<QString,QStringList> eventMap;     // 당일 이벤트 맵
    t_pSelectEvent curEventStr;
    QList <QString> holeList;
    QList <QString> timeList;

    int reserveEventRefresh;            // 동작중이지 않을 때 이벤트목록 갱신예약을 함
    int couponCount;
    int currentBalance;     // 현재 잔액
    QString currentTransaction;      // 현재 트랜잭션 리스트

    // 프린터 관련 선언-----------------------------------
    woosimlib	*printerlib;
    void printerInit();
    static void on_read( woosimlib *sender);
    static void on_error( woosimlib *sender, int reopen_ok);
    int printCount;
    int printFlag;
    int printConnect;


    int databaseInit();
    QMap <QString,QString> transactionList;

//    USER_INFO strUser;
//    LPUSER_INFO current_user;

    void select_Page(int pageNum);
    void set_visible_systemlog(bool checked);
    void deviceNormalInit();
    void get_NotProcessedData();
    void set_currentTime();

    void lang_change();

    // RF 카드관련
    int on_rfid_connect(int rtCnt);
    int cardRetryCount;
    int cardOpType;
    bool analyizeCardCode(QByteArray rfCode);

    void setAudioMedia(QUrl media, int repCnt, int delayCnt, bool stopFlag);
signals:
    void sSysMessage(QString sysText);

public slots:
    void setSysMessage(QString sysText);

    void setSystemlogData(QString logText);

    void setManagerPw();

    void on_btn_Test_clicked();

    void on_btn_testClose_clicked();

    void on_btn_testClose_pressed();

    void on_btn_setupMode_pressed();

    void on_btn_setupMode_released();

    void setDeviceFlag(QString state);
private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
