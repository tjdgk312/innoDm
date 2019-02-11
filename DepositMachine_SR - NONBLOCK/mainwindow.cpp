#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "global.h"
#include "printer/woosimprinter.h"
#include <QProcess>

MainWindow *p_Main;
#define     MAX_BUF_SIZE    1024
#define SCARD_CTL_CODE(code) (0x42000000 + (code))

QStyledItemDelegate* itemDelegate;          // 콤보박스  Style 적용을 위한 전역

char  sample_buf[MAX_BUF_SIZE];

void OnDepositCompleteEvent(LPVAULTDEPOSIT_INFO settlement_info_ptr, int status, void* userData);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

//    managerPW = "inn123456";

    ui->setupUi(this);
    p_Main=this;

//    this->setFocus();
    countryCount = 0;

    compareCount = 0;

    authenKey = JkHlJk;
    masterCode = MASTERCODE;
//    corpCode = CORPCODE;
    deviceVer = PROGRAM_VERSION;          //  프로그램 버젼
//    deviceVer = DEVICE_VER;          //  프로그램 버젼

    currentCountry = DEFAULTCOUNTRY;
    curCountryText = TEXTCOUNTRY;

    assignRfcard = 0;

    printConnect = 0;

    //콤보박스 갭 생성을 위한  공백 Icon 객체생성 및 초기화작업
    itemDelegate = new QStyledItemDelegate();

    connect(this,SIGNAL(sSysMessage(QString)),this,SLOT(setSysMessage(QString)));

    curDoorSen = 1;
    curKeySen = 1;

    rollState = 0;
    rollLeft = 9999999;
    rollFlag = 0;
    rollReqCnt = 0;

    bagFlag = 0;
    bagState = 0;


    prePage = LOGINPAGE;
    curPage = LOGINPAGE;

    cardOpType = 0;

    upsOpFlag = 0;

    // Database initialize
    startupSetupFlag = databaseInit();

    finCmdType = 0;
    fingerFileSize = 0;
    curUserFinger = new QFile("fingerList.db");


    key_pad = new keypad();
    key_pad->hide();

    pMsgDialog = new MessageDialog();

//    pMsgDialog->setGeometry(100,130,pMsgDialog->width(),pMsgDialog->height());
    pMsgDialog->hide();

    if(startupSetupFlag==2){
        deviceNormalInit();
    }
    else{

        pDevSetup    = new page_deviceSetup(this);      // DEVICE 정보 설정
        ui->list_systemlog->hide();
        pDevSetup->init_deviceInterface(startupSetupFlag);
    }
//    pDevSetup->init_deviceInterface(startupSetupFlag);
}

int MainWindow::databaseInit(){

    pDatabase = new DatabaseManager();
    //---------------------------------------------------------INFO DATABASE------------------------------------------------------
    pDatabase->OpenDatabase(PATH_DB);

    int retFlag = 0;

    if(!pDatabase->db.tables().contains(INFO_TABLE)){
        qDebug() << "info table not exist";
        pDatabase->CreateTable(QString("%1(recName TEXT, col1 TEXT, col2 TEXT, col3 TEXT, col4 TEXT, col5 TEXT, totalCount TEXT, totalPrint TEXT, rollState TEXT, rollLeft TEXT)").arg(INFO_TABLE));
        // later 정보테이블 없으면 정보입력모드로 진입
        turnNum  = QDate::currentDate().toString("yyyyMM")+"001";
        qDebug() << "set Turn number" << turnNum;
        pDatabase->InsertRecord(INFO_TABLE, QString("'deviceInfo','NOTSET','NOTSET','255.255.255.255','%1', '0', '0', '0', '0', '99999'").arg(turnNum));

        deviceIp = "255.255.255.255";
        retFlag = 0;
        rollState = 0;
        rollLeft = 9999999;
    }
    else{
        // 장치정보 받아오기-------------------------------------
        QVariantList buf;
        QSqlQuery query;
        int index = 0;
        query.exec(QString("select col1,col2,col3,col4,col5,totalCount,totalPrint,rollState,rollLeft from %1 where recName='deviceInfo'").arg(INFO_TABLE));
        while (query.next())
        {
            while(query.value(index).isValid()){   // 검색 열 갯수검사
                buf.append(query.value(index));
                index++;
            }
        }
        if(buf.count() > 8){
            if(buf.at(0).toString() == "NOTSET"){          //     장치명 설정
                retFlag = 0;
            }
            else if(buf.at(1).toString() == trUtf8("선택해주세요")){          //     장치구분 설정
                retFlag = 0;
            }
            else if(buf.at(2).toString() == "255.255.255.255"){          // IP 설정이 안되어있다면.

                // for test
                deviceIp = buf.at(2).toString();
                retFlag = 2;
////                deviceIp = buf.at(2).toString();
//                QString cmd = QString("sudo ifconfig eth0 %1 up").arg(deviceIp);
//                QProcess *qProc = new QProcess(this);
//                connect(qProc, SIGNAL(finished(int)),qProc,SLOT(deleteLater()));
//                qProc->start(cmd);
//                qProc->waitForFinished();
                // for develop test

                // 정상동작
//                retFlag = 1;
            }
            else{
                qDebug() << buf.at(0).toString() << buf.at(1).toString();

                //for develop test

//                deviceIp = "192.168.20.87"

                deviceIp = buf.at(2).toString();

                QString cmd = QString("sudo ifconfig eth0 %1 up").arg(deviceIp);
                QProcess *qProc = new QProcess(this);
                connect(qProc, SIGNAL(finished(int)),qProc,SLOT(deleteLater()));
                qProc->start(cmd);
                qProc->waitForFinished();
                retFlag = 2;
            }

            deviceNum =  buf.at(0).toString();       // 기계시리얼넘버
            deviceType = buf.at(1).toString();         // 기종구분
//            deviceIp = buf.at(2).toString();
            turnNum = buf.at(3).toString();         // 회차 턴넘버
            curBagBarcode = buf.at(4).toString();         // 백 바코드
            totalBagCount = buf.at(5).toInt();

            compareCount = (totalBagCount/TWISTPERIOD + 1) * TWISTPERIOD;

            totalPrintCount = buf.at(6).toInt();
            rollState = buf.at(7).toInt();
            rollLeft = buf.at(8).toInt();

            if(p_Main->rollState == 1){
                p_Main->rollFlag = 1;
                emit p_Main->sSysMessage(trUtf8("프린터 용지를 교체해야 합니다."));
            }
        }
        else{
            // 초기 데이터 재입력
            retFlag = 0;
        }
    }

    if(!pDatabase->db.tables().contains(INFO2_TABLE)){
        qDebug() << "info table not exist";
        pDatabase->CreateTable(QString("%1(recName TEXT, col1 TEXT, col2 TEXT, col3 TEXT, col4 TEXT, col5 TEXT, col6 TEXT, col7 TEXT, col8 TEXT, col9 TEXT)").arg(INFO2_TABLE));
        // later 정보테이블 없으면 정보입력모드로 진입
        pDatabase->InsertRecord(INFO2_TABLE, QString("'deviceInfo2','3000','inn123456','0','0', '0', '0', '0', '0', '0'"));
        capNum = "3000";
        managerPW = "inn123456";
        langCountry = 0;
    }
    else{
        // 장치정보 받아오기-------------------------------------
        QVariantList buf;
        QSqlQuery query;
        int index = 0;
        query.exec(QString("select col1,col2,col3,col4,col5,col6,col7,col8,col9 from %1 where recName='deviceInfo2'").arg(INFO2_TABLE));
        while (query.next())
        {
            while(query.value(index).isValid()){   // 검색 열 갯수검사
                buf.append(query.value(index));
                index++;
            }
        }
        capNum =  buf.at(0).toString();       // 기계시리얼넘버
        managerPW = buf.at(1).toString();       // 매니저 PW 저장
        langCountry = buf.at(2).toInt();     // 언어국가선택

        switch(langCountry){
        case 0:                 // 한국어
            qDebug() << "/tt2_kr_device.qm";
            translator->load(QString("%1SetFiles/tt2_kr_device.qm").arg(PATH_ROOT));
            break;
        case 1:                 // 영어
            qDebug() << "/tt2_en_device.qm";
            translator->load(QString("%1SetFiles/tt2_en_device.qm").arg(PATH_ROOT));
            break;
        }
        ui->retranslateUi(this);
    }

    if(capNum.toInt()-500 < totalBagCount){
        bagState = 1;
    }

    //---------------------------------------------------------DEPOSIT DATABASE------------------------------------------------------
    // 장치 동작 기록 테이블 구성
    if(!pDatabase->tables.contains(RECORD_TABLE)){
        qDebug() << "record table not exist";                   // 14 개 컬럼
        pDatabase->CreateTable(QString("%1(time TEXT, deviceid TEXT, turnNum INT, depositNum INT, division TEXT, country TEXT, totalAmount TEXT, "
                                       "totalCount TEXT, etcAmount TEXT, primaryid TEXT, processTime TEXT, log TEXT, tmp1 TEXT, tmp2 TEXT)").arg(RECORD_TABLE));
        p_Main->depositNum = 1;      // 1회차 1번 트랜잭션번호
    }
    else{
        QVariant buf;
        int idx=0;
        QSqlQuery query;
        query.exec(QString("select max(depositNum) from %1").arg(RECORD_TABLE));
        while (query.next())
        {
            if(query.value(idx).isValid()){   // 검색 열 갯수검사
                buf = query.value(idx);
            }
        }
        qDebug() << "deposit num max" << buf;

        p_Main->depositNum = buf.toInt();
        qDebug() << "READ DEPOSIT NUMBER = " << p_Main->depositNum << p_Main->turnNum;
        if(p_Main->depositNum <= 0){
            p_Main->depositNum = 1;
        }
//        else if(p_Main->depositNum < compareNum){       // 저장 된 번호가 현재 월 입금번호 보다 낮을 때
//            p_Main->depositNum = compareNum;      // 저장회차 1번 트랜잭션번호 현송하고 껐을 때
//        }
        else{
            p_Main->depositNum += 1;
        }
    }

    // 알람 테이블 구성----------------------------------------
    if(!pDatabase->db.tables().contains(ALARM_TABLE)){
        qDebug() << "alarm table not exist";                   // 14 개 컬럼
        p_Main->pDatabase->CreateTable(QString("%1 (alnum TEXT,"
                                           "time TEXT, altext TEXT, state TEXT,"
                                           "data1 TEXT)").arg(ALARM_TABLE));
        alCount = 0;
    }
    else{
        alCount = pDatabase->SelectAllData(QString("%1 order by time desc LIMIT 30").arg(ALARM_TABLE),Albuf);
    }

    return retFlag;
}

void MainWindow::deviceNormalInit(){
    // printer port open
    QString cmd("sudo chmod 0666 /dev/usb/lp0");
    QProcess *qProc = new QProcess(this);
    connect(qProc, SIGNAL(finished(int)),qProc,SLOT(deleteLater()));
    qDebug() << "장치초기화 시작";
    qProc->start(cmd);
    qDebug() << "장치초기화 끝";
    qProc->waitForFinished();
    // printer init

    // SDK initialize
    int retSdk  = innoSdkInit();

    printerInit();

    // set struct init
//    current_user = &strUser;

    // later get time list from pc server
    holeList << "BRONZE" << "SILVER" << "GOLD";
    timeList << "11:30"  << "12:00" << "12:30" << "13:00" << "13:30" << "14:00";

    couponCount = 0;            // 식권개수 초기화

    initializeFlag = 0;
    deviceStatusFlag = 0;

    eventFlag = 0;

    reserveEventRefresh = 0;         // 이벤트목록 갱신 예약플래그
    btn_keyTest = 0;

    pMenu       = new page_menu(this);
    pSetting    = new page_setting(this);            // 설정 불러와야 하기 때문에 가장먼저.

    pRepair      = new page_repair();
    pRepair->hide();

    pLogin       = new page_login(this);
    pLoading     = new page_loading(this);
    pResult      = new page_result(this);
    pManagerMenu = new page_managermenu(this);
    pTransCash   = new page_transcash(this);
    pUser        = new page_user(this);
    pSearch      = new page_search(this);
    pEngineer    = new page_engineer(this);
    pEventView   = new page_eventView(this);
    pSign        = new page_signImage(this);
    pVisitor     = new page_visitorLogin(this);
    pDevSetup    = new page_deviceSetup(this);      // DEVICE 정보 설정
    pEtc         = new page_etc(this);

    pServer            = new TcpServer();            //port 초기화를 위하여 로그인화면 이후 객체 생성


//    pSetting->setLanguageCountry(pSetting->preSave);
//    ui->retranslateUi(this);
//    pMenu->lang_change();

    ui->stackedWidget->addWidget(pLogin);
    ui->stackedWidget->addWidget(pMenu);
    ui->stackedWidget->addWidget(pLoading);
    ui->stackedWidget->addWidget(pResult);
    ui->stackedWidget->addWidget(pSetting);
    ui->stackedWidget->addWidget(pManagerMenu);
    ui->stackedWidget->addWidget(pTransCash);
    ui->stackedWidget->addWidget(pUser);
    ui->stackedWidget->addWidget(pEngineer);
    ui->stackedWidget->addWidget(pSearch);
    ui->stackedWidget->addWidget(pEventView);
//    ui->stackedWidget->addWidget(pRepair);
    ui->stackedWidget->addWidget(pSign);
    ui->stackedWidget->addWidget(pVisitor);
    ui->stackedWidget->addWidget(pDevSetup);
    ui->stackedWidget->addWidget(pEtc);

    if(retSdk){
        pRepair->show();

        p_Main->curAlarm[0][3] = "초기화";
        p_Main->pRepair->init_repairPage(20);
//        ui->stackedWidget->setCurrentWidget(pRepair);
    }
    else{
        setAudioMedia(QUrl::fromLocalFile(QString("%1audio/m1.mp3").arg(PATH_ROOT)),1,15,true);
//        ui->stackedWidget->setCurrentWidget(pEngineer);
        ui->stackedWidget->setCurrentWidget(pLogin);
        curPage = LOGINPAGE;
        setSystemlogData("장치 초기화가 완료되었습니다. 클라이언트 연결을 기다립니다....");
        setSystemlogData("장시간 다음동작이 진행되지 않을 경우 통신 상태를 점검하십시오.");
    }
}

void MainWindow::set_currentTime(){
    ui->lbl_time->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
}

void MainWindow::lang_change()
{
    ui->retranslateUi(this);
}

void MainWindow::get_NotProcessedData(){
    QSqlQuery query;
    int idx = 0;
    query.exec(QString("select * from %1 where processTime='0'").arg(RECORD_TABLE));
    while (query.next())
    {
        QString depTime;
        QString depList;
        idx=0;
        while(query.value(idx).isValid()){   // 검색 열 갯수검사
            if(idx==0){
                depTime = query.value(idx).toString();
            }
            depList.append(QString("%1,").arg(query.value(idx).toString()));
            qDebug() << query.value(idx);
            idx++;
        }
        setSystemlogData(QString("전송되지 못한 목록을 트랜잭션 리스트에 추가합니다. %1.").arg(depList));
        p_Main->transactionList.insert(depTime,depList);
    }
}


void MainWindow::set_visible_systemlog(bool checked){
//    ui->list_systemlog->setVisible(checked);
}

//------------------------------------printer library-----------------------------------------------------

void MainWindow::printerInit()
{
    //-------------------------------Printer library initialize-------------------------------------
    poll_init();

//    printerlib  = woosimlib_open("/dev/tty3", 115200, 8, 'n', 1);
    printerlib  = woosimlib_open("/dev/usb/lp0", 57600, 8, 'n', 1);
    if ( NULL == printerlib || printerlib == 0)
    {
        printConnect = 0;
        return;
//        printf( "error = %d\n", error_no);
//        qDebug()<<"error = %d\n"<< error_no;
    }
    printConnect = 1;

    printerlib->on_error = on_error;
    printerlib->on_read  = on_read;

//    InitPrinterStatus();
////    InitPageMode(0,0,0,0);       // 인치 및 길이설정
//    Page_Print_StandardMode();
//    FeedToMark();
//    PrintSpool(p_Main->printerlib,TRUE);
//    InitPrinterStatus();
    GetPrinterStatus(p_Main->printerlib, 1000);
//    int sz_read;
//    qDebug() << "init_on_read";
//    sz_read = woosimlib_read( printerlib, sample_buf, MAX_BUF_SIZE);
//    int iPaperSensor = sample_buf[0] & 0x01;

//    if(p_Main->rollState != iPaperSensor){
//        if(iPaperSensor == 0){          // 롤 교체 됨
//            p_Main->rollLeft = 9999999;
//        }
//        else{                           // 롤 교체필요
//            p_Main->rollLeft = 0;
//            emit p_Main->sSysMessage(trUtf8("프린터 용지를 교체해야 합니다."));
//        }
//        p_Main->rollState = iPaperSensor;
//        p_Main->pDatabase->UpdateRecord(INFO_TABLE,QString("rollState ='%1', rollLeft='%2'")
//                                        .arg(p_Main->rollState).arg(p_Main->rollLeft),"recName='deviceInfo'");

//        p_Main->rollFlag = 1;
//    }
    //----------------------------------------------------------------------------------------
}

void MainWindow::on_error( woosimlib *sender, int reopen_ok)
{
    if ( 0 == reopen_ok)
    {
        printf( "port error\n");
        printf( "%s reopen port.\n", sender->devname);
    }
    else
    {
        printf( "port error close program\n");
    }
}

void MainWindow::on_read(woosimlib *sender)
{
    int sz_read;

    printf( "on_read\n");


    sz_read = woosimlib_read( sender, sample_buf, MAX_BUF_SIZE);

    int iPaperSensor = sample_buf[0] & 0x01;

    if ( 0 < sz_read)
    {
        printf( "->Receive data : %s cnt : %d\n", sample_buf,sz_read);
    }
    else
    {
        printf( "read error\n");
    }

    if(p_Main->rollState != iPaperSensor){
        if(iPaperSensor == 0){          // 롤 교체 됨
            qDebug() << "ROLL CHANGED";
            p_Main->rollLeft = 9999999;
            emit p_Main->sSysMessage("");
        }
        else{                           // 롤 교체필요
            p_Main->rollLeft = ROLLLEFT;
            emit p_Main->sSysMessage(trUtf8("프린터 용지를 교체해야 합니다."));
            qDebug() << "ROLL NEED CHANGED";
        }
        p_Main->rollState = iPaperSensor;
        p_Main->pDatabase->UpdateRecord(INFO_TABLE,QString("rollState ='%1', rollLeft='%2'")
                                        .arg(p_Main->rollState).arg(p_Main->rollLeft),"recName='deviceInfo'");
    }

    if(iPaperSensor == 1){      // 용지 잔여량 차감
        p_Main->rollLeft -= 1;
        p_Main->pDatabase->UpdateRecord(INFO_TABLE,QString("rollLeft='%1'")
                                        .arg(p_Main->rollLeft),"recName='deviceInfo'");
        emit p_Main->sSysMessage(trUtf8("프린터 용지를 교체해야 합니다."));
    }

    qDebug() << "print Event" << p_Main->printFlag;

    p_Main->totalPrintCount += 1;
    p_Main->pDatabase->UpdateRecord(INFO_TABLE,QString("totalPrint='%1'").arg(p_Main->totalPrintCount),"recName='deviceInfo'");

    if(p_Main->printFlag == 2){     // 식권출력 이벤트 이면
        p_Main->printCount += 1;
//        p_Main->curEventStr->mealCount += 1;
        if(p_Main->printCount < p_Main->couponCount){
            qDebug() << "Coupon Print retry: " << p_Main->printCount;
            p_Main->pResult->receipt_Setup(2,p_Main->printCount);
        }
    }
}

//------------------------------------printer library-----------------------------------------------------

void MainWindow::setSystemlogData(QString logText){
//    ui->lbl_info->setText(logText);
//    ui->list_systemlog->addItem(logText);
    //    ui->list_systemlog->scrollToBottom();
}

void MainWindow::setManagerPw()
{
    p_Main->pDatabase->UpdateRecord(INFO2_TABLE,QString("col2='%1'")
                                              .arg(managerPW),"recName='deviceInfo2'");
}


void OnDepositCompleteEvent(LPVAULTDEPOSIT_INFO settlement_info_ptr, int status, void* userData){
}

void MainWindow::select_Page(int pageNum){

    prePage = curPage;

    switch(pageNum){
    case LOGINPAGE:
        setAudioMedia(QUrl::fromLocalFile(QString("%1audio/m1.mp3").arg(PATH_ROOT)),1,15,true);
        ui->stackedWidget->setCurrentWidget(pLogin);
        pLogin->loginPage_init();
        break;
    case MENUPAGE:
        pMenu->init_menuPage();
        setAudioMedia(QUrl::fromLocalFile(QString("%1audio/m2.mp3").arg(PATH_ROOT)),1,15,true);
        ui->stackedWidget->setCurrentWidget(pMenu);
        break;
    case LOADINGPAGE:
        ui->stackedWidget->setCurrentWidget(pLoading);
        break;
    case RESULTPAGE:
//        setAudioMedia(QUrl::fromLocalFile(QString("%1audio/m4.mp3").arg(PATH_ROOT)),1,15,true);
        ui->stackedWidget->setCurrentWidget(pResult);
        pResult->resultPage_init();
        break;
    case SETTINGPAGE:
        ui->stackedWidget->setCurrentWidget(pSetting);
//        pResult->resultPage_init();
        break;
    case MANAGEPAGE:
        ui->stackedWidget->setCurrentWidget(pManagerMenu);
//        pResult->resultPage_init();
        break;
    case TRANSPAGE:
        ui->stackedWidget->setCurrentWidget(pTransCash);
//        pResult->resultPage_init();
        break;
    case USERPAGE:
        ui->stackedWidget->setCurrentWidget(pUser);
//        pResult->resultPage_init();
        break;
    case TESTPAGE:
        ui->stackedWidget->setCurrentWidget(pEngineer);
//        pResult->resultPage_init();
        break;
    case SEARCHPAGE:
        ui->stackedWidget->setCurrentWidget(pSearch);
//        pSearch->init_searchPage();
        break;
    case EVENTVIEWPAGE:
        ui->stackedWidget->setCurrentWidget(pEventView);
        break;
    case REPAIRPAGE:
        ui->stackedWidget->setCurrentWidget(pRepair);
        break;
    case SIGNPAGE:
        ui->stackedWidget->setCurrentWidget(pSign);
        break;
    case VISITORPAGE:
        ui->stackedWidget->setCurrentWidget(pVisitor);
        break;
    case MANAGERMAIN:
        ui->stackedWidget->setCurrentWidget(pDevSetup);
        break;
    case ETCPAGE:
        ui->stackedWidget->setCurrentWidget(pEtc);
        pEtc->init_etcPage();
        break;
    }

    curPage = pageNum;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btn_Test_clicked()
{
    p_Main->select_Page(TESTPAGE);
}

void MainWindow::on_btn_testClose_clicked()
{
    this->close();
}

void MainWindow::on_btn_testClose_pressed()
{
    this->close();
}


void MainWindow::on_btn_setupMode_pressed()
{
    p_Main->btn_keyTest = 1;
}

void MainWindow::on_btn_setupMode_released()
{
    p_Main->btn_keyTest = 0;
    p_Main->btn_keyTimer = 0;
}

void MainWindow::setSysMessage(QString sysText)
{
    ui->lbl_info->setText(sysText);

}


int MainWindow::on_rfid_connect(int rtCnt){
    LONG ret;
    DWORD dwRecvLength;
    SCARDCONTEXT context = 0;
    SCARD_IO_REQUEST pioSendPci;
    BYTE pbRecvBuffer[255];
    BYTE cmd1[255];

    unsigned int i;

    ret = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &context);
    qDebug() << "SCardEstablishContext" << ret;

    if (ret != SCARD_S_SUCCESS) {
        qDebug() << "SCardEstablishContext: " << pcsc_stringify_error(ret);
        return --rtCnt;
    }

    LPTSTR allReaderNames = nullptr;
    DWORD readerCount = SCARD_AUTOALLOCATE;

    ret = SCardListReaders(context, nullptr, reinterpret_cast<LPTSTR>(&allReaderNames), &readerCount);

    qDebug() << allReaderNames << readerCount;

    if (ret != SCARD_S_SUCCESS) {
        qDebug() << "SCardListReaders: " << pcsc_stringify_error(ret);
        return --rtCnt;
    }
    QString readerName = allReaderNames;
    DWORD activeProtocol = 0;
    SCARDHANDLE card = 0;

    ret = SCardConnect(context, readerName.toStdString().c_str(), SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, &card, &activeProtocol);
    if (ret != SCARD_S_SUCCESS) {
        qDebug() << "SCardConnect" << pcsc_stringify_error(ret);
        return --rtCnt;
    }

    switch(activeProtocol)
    {
    case SCARD_PROTOCOL_T0:
        pioSendPci = *SCARD_PCI_T0;
        break;
    case SCARD_PROTOCOL_T1:
        pioSendPci = *SCARD_PCI_T0;
        break;
    case SCARD_PROTOCOL_RAW:
        pioSendPci = *SCARD_PCI_RAW;
        break;
    }

    //            QVector<uint8_t> outputBuffer{ 0xE0, 0x0, 0x0, 0xDA, 0x04, 0x01, 0x02, 0x03, 0x04 };         // Write Serial
    //            QVector<uint8_t> outputBuffer{ 0xE0, 0x0, 0x0, 0x33, 0x0 };         // Read Serial
    //                QVector<uint8_t> outputBuffer{ 0xE0, 0x0, 0x0, 0x28, 0x1, 0x01 };       // Buzzer
    //            QVector<uint8_t> outputBuffer{ 0xE0, 0x0, 0x0, 0x21, 0x1, 0x7F };       // LEDBuzzerSetup 7F:Green BF:Red
    //                QVector<uint8_t> inputBuffer(128, 0);
    //                DWORD bytesReturned = 0;
    //                ret = SCardControl(card, SCARD_CTL_CODE(3500), outputBuffer.data(),
    //                                   outputBuffer.size(), inputBuffer.data(), inputBuffer.size(), &bytesReturned);
    //                qDebug() << "SCardControl" << ret;

    //                qDebug() << "response: "  << bytesReturned;
    //                for(i=0; i<bytesReturned; i++){
    //                    qDebug() << inputBuffer[i] << i;
    //                }

    QVector<uint8_t> outputBuffer{ 0xE0, 0x0, 0x0, 0x21, 0x1, 0x77 };       // LEDBuzzerSetup 7F:Green BF:Red
    QVector<uint8_t> inputBuffer(128, 0);   // A7
    DWORD bytesReturned = 0;
    ret = SCardControl(card, SCARD_CTL_CODE(3500), outputBuffer.data(),
                       outputBuffer.size(), inputBuffer.data(), inputBuffer.size(), &bytesReturned);
    qDebug() << "SCardControl" << ret;

    qDebug() << "response: "  << bytesReturned;
    for(i=0; i<bytesReturned; i++){
        qDebug() << inputBuffer[i] << i;
    }

    dwRecvLength = 255;
    memcpy(cmd1, "\xFF\xCA\x00\x00\x00", 5);
    ret = SCardTransmit(card, SCARD_PCI_RAW, cmd1, 5,NULL, pbRecvBuffer, &dwRecvLength);

    if (ret != SCARD_S_SUCCESS) {
        qDebug() << "SCardTransmit - Get Serial" << pcsc_stringify_error(ret);
        return --rtCnt;
    }

    qDebug() << "response: "  << dwRecvLength;
    for(i=0; i<dwRecvLength; i++){
        qDebug() << pbRecvBuffer[i] << i;
    }

    if(dwRecvLength > 2){
        if(pbRecvBuffer[dwRecvLength-2] == 0x90){
            qDebug() << pbRecvBuffer[-2];
            if(pbRecvBuffer[dwRecvLength-1] == 0x00){
                qDebug() << "Command Success" << pbRecvBuffer[-1];
            }
        }
        else{
            return --rtCnt;
        }
    }


    dwRecvLength = 255;
    memcpy(cmd1, "\xFF\x82\x00\x00\x06\x4A\x6B\x48\x6C\x4A\x6B", 11);
    ret = SCardTransmit(card, SCARD_PCI_RAW, cmd1, 11,
                        NULL, pbRecvBuffer, &dwRecvLength);

    if (ret != SCARD_S_SUCCESS) {
        qDebug() << "SCardTransmit - get key code" << pcsc_stringify_error(ret);
        return --rtCnt;
    }

    qDebug() << "get key code - response: "  << dwRecvLength;
    for(i=0; i<dwRecvLength; i++){
        qDebug() << pbRecvBuffer[i] << i;
    }


    dwRecvLength = 255;
    // General Authenticate - Read the documents mentioned in the README :)
    memcpy(cmd1, "\xFF\x86\x00\x00\x05\x01\x00\x01\x60\x00", 10);
    ret = SCardTransmit(card, SCARD_PCI_RAW, cmd1, 10,
                        NULL, pbRecvBuffer, &dwRecvLength);

    if (ret != SCARD_S_SUCCESS) {
        qDebug() << "SCardTransmit - General Authenticate" << pcsc_stringify_error(ret);
        return --rtCnt;
    }

    qDebug() << "General Authenticate - response: "  << dwRecvLength;
    for(i=0; i<dwRecvLength; i++){
        qDebug() << pbRecvBuffer[i] << i;
    }

    // Update Binary
    //    memcpy(cmd1, "\xFF\xD6\x00\x01\x10\x02\x00\x01\x08\x00\x04\x02\x03\x01\x01\x01\x09\x09\x09\x09\x09", 21);
    //    ret = SCardTransmit(card, SCARD_PCI_RAW, cmd1, 21,
    //                        NULL, pbRecvBuffer, &dwRecvLength);
    //    qDebug() << "SCardTransmit - Update Binary" << ret;

    //    qDebug() << "update binary: "  << dwRecvLength;
    //    for(i=0; i<dwRecvLength; i++){
    //        qDebug() << pbRecvBuffer[i] << i;
    //    }


    dwRecvLength = 255;
    // Read Binary
    memcpy(cmd1, "\xFF\xB0\x00\x01\x10", 5);
    ret = SCardTransmit(card, SCARD_PCI_RAW, cmd1, 5,
                        NULL, pbRecvBuffer, &dwRecvLength);
    if (ret != SCARD_S_SUCCESS) {
        qDebug() << "SCardTransmit - Read Binary" << pcsc_stringify_error(ret);
        return --rtCnt;
    }
    QByteArray tmpArr;
    qDebug() << "Read Binary - Block 01: "  << dwRecvLength;
    for(i=0; i<dwRecvLength; i++){
        if(i < 16){
            tmpArr.append(pbRecvBuffer[i]);
        }
        qDebug() << pbRecvBuffer[i] << i;
    }

    QVector<uint8_t> outputBuffer1{ 0xE0, 0x0, 0x0, 0x28, 0x1, 0x10 };       // Buzzer
    QVector<uint8_t> inputBuffer1(6, 0);   // A7
    DWORD bytesReturned1 = 0;
    ret = SCardControl(card, SCARD_CTL_CODE(3500), outputBuffer1.data(),
                       outputBuffer1.size(), inputBuffer1.data(), inputBuffer1.size(), &bytesReturned1);
    if (ret != SCARD_S_SUCCESS) {
        qDebug() << "SCardControl - Buzzer";
        return --rtCnt;
    }

    qDebug() << "response: "  << bytesReturned1;
    for(i=0; i<bytesReturned1; i++){
        qDebug() << inputBuffer1[i] << i;
    }

    ret = analyizeCardCode(tmpArr);
    if(ret == 0){
        pLoading->current_step = IDLE_DEPOSIT;
        cardRetryCount = 0;
        assignRfcard = 0;
//        if(cardOpType == 0){                 // for test
//            pEngineer->setCardText();
//        }
        if(cardOpType == 0){                 // 투입모드

            qDebug() << "card assign request";

            p_Main->assignFlag = 0;     // 인증 플래그 0으로
            p_Main->eventFlag = 1;                      // Request DB data
            p_Main->pMsgDialog->currentFlag = 12;

            p_Main->pLoading->init_loadingPage(23);                 // 로딩 페이지 타이머 대기 스탭 진입
            p_Main->select_Page(LOADINGPAGE);
            qDebug() << "SET COMP WAIT ASSIGN - " << p_Main->pLoading->timingCount;
        }
        else if(cardOpType == 1){            // Write 모드
//            pManagerMenu->setButtonEnabled(1);
            pMsgDialog->init_MessageDialog(21);
        }
        else if(cardOpType == 2){            // 회수모드
            //--------------------------------회수기록저장--------------------------------------------
            if(!p_Main->pDatabase->db.isOpen()){
                p_Main->pDatabase->db.open();
            }
            QString values;
            QString depositTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
            values.append(QString("'%1',").arg(depositTime));
            values.append("'RETURN',");

            values.append(QString("'%1',").arg("-"));

            values.append(QString("'%1',").arg("-"));

            values.append(QString("'%1',").arg("-"));

            values.append("'-',");                                    //tmp 2
            values.append("'-'");          // process time

            p_Main->currentTransaction = values;

            qDebug() << "COMPLETE RETURN INSERT - " << p_Main->currentTransaction;
            p_Main->pDatabase->InsertRecord(RECORD_TABLE,p_Main->currentTransaction);

            qDebug() << "db op check: " << p_Main->pDatabase->db.isOpen();
            p_Main->currentTransaction = "";
            //----------------------------------------------------------------------------------

            pLoading->init_loadingPage(6);
            select_Page(LOADINGPAGE);
        }
        else if(cardOpType == 98){
            pMsgDialog->hide();
            p_Main->pRepair->set_ManageFrame();
            this->setEnabled(true);
            cardOpType = 0;
        }
        else if(cardOpType == 99){
            pMsgDialog->hide();
            select_Page(MANAGEPAGE);
            this->setEnabled(true);
        }
        else if(cardOpType == 999){
            pMsgDialog->hide();
            select_Page(TESTPAGE);
            this->setEnabled(true);
        }
    }
    else{
        cardRetryCount = 0;
        assignRfcard = 0;
        if(cardOpType == 0){                 // 투입모드
            pMsgDialog->init_MessageDialog(12);
        }
        else if(cardOpType == 1){            // Write 모드
        }
        else if(cardOpType == 2){            // 회수모드
            pMsgDialog->init_MessageDialog(12);
        }
        else if(cardOpType == 99 || cardOpType == 98){
            pMsgDialog->setInfoText(trUtf8("잘못된 카드 정보입니다."));
            cardRetryCount = -1;
            assignRfcard = 1;
        }
        else if(cardOpType == 999){
            assignRfcard = 1;
            pMsgDialog->setInfoText(trUtf8("잘못된 카드 정보입니다."));
            cardRetryCount = -1;
        }
    }

    qDebug() << "block 1 data: " << tmpArr.toHex() << tmpArr.toShort();

    ret = SCardDisconnect(card, SCARD_LEAVE_CARD);
    qDebug() << "SCardDisconnect" << ret;

    ret = SCardFreeMemory(context, allReaderNames);
    qDebug() << "SCardFreeMemory" << ret;
    if (ret != SCARD_S_SUCCESS) {
        qDebug() << "SCardFreeMemory: " << pcsc_stringify_error(ret);
        return --rtCnt;
    }
    ret = SCardReleaseContext(context);
    qDebug() << "SCardReleaseContext" << ret;
    if (ret != SCARD_S_SUCCESS) {
        qDebug() << "SCardReleaseContext: " << pcsc_stringify_error(ret);
        return --rtCnt;
    }
}

bool MainWindow::analyizeCardCode(QByteArray rfCode){
    pLoading->current_step = IDLE_DEPOSIT;
    QString cardSaveCode = QByteArray::fromHex(rfCode.toHex());     // 계약번호 8자리만 받아옴
    QString compareData = rfCode;
    qDebug() <<"direct save buffer: " << compareData;
    int byteInt = 0;
    for(int i=0; i<6; i++){
        byteInt = rfCode.at(i);
        compareData.append(QString("%1").arg(byteInt));
    }
    qDebug() << "get Card primaryCode: "  << cardSaveCode << rfCode.toHex().toUInt(0,10)
             << rfCode << compareData;

    if(cardOpType == 0){
        int idx = 0;
        while(compareData.mid(idx,1) == "0"){
            idx += 1;
        }

        if(idx >= compareData.count()){
            return 1;
            // 예외처리
        }
        else{
            p_Main->curUserId = compareData.mid(idx,compareData.count()-idx);
            qDebug() << "p_Main->curUserId : " << p_Main->curUserId;
            if(p_Main->curUserId== ""){
                p_Main->curUserId = "nullUser";
                return 1;
            }
            return 0;
        }
    }
    if(cardOpType == 99 || cardOpType == 98){
        if(masterCode == compareData.mid(0,masterCode.count())){            // 고유 ID와 같으면
            return 0;
        }
        else{
            return 1;
        }
    }
    else if(cardOpType == 999){
        if(masterCode == compareData.mid(0,masterCode.count())){            // 고유 ID와 같으면
            return 0;
        }
        else{
            return 1;
        }
    }

//    if(cardWriteData == compareData.mid(0,cardWriteData.count())){            // 고유 ID와 같으면
//        //        int typeCode = rfCode.at(8);
//        qDebug() << "exist primaryCode: " << cardSaveCode;
//        // later
//        return 0;
//    }
//    else{
//        qDebug() << "not exist primaryCode: " << cardSaveCode;
//        return 1;
//    }
}

void MainWindow::setAudioMedia(QUrl media, int repCnt, int delayCnt, bool stopFlag)
{
    // file://audio/예식정보확인.mp3
    preMedia = setMedia;
    setMedia = media;
    mDelayCount = delayCnt;
    mRepeatCount = repCnt;
    mTimingCount = delayCnt;
//    player->setMedia(setMedia);
//    player->play();
//    tmp_pthread->stop();
    MusicThread *t_pthread = new MusicThread(this);
    threadMedia = setMedia;
    connect( t_pthread, SIGNAL(finished()), t_pthread,SLOT(deleteLater()));
//    connect( t_pthread, SIGNAL(finished()), this,SLOT(repeatAudioMedia()));
    qDebug()<< "MAKE THREAD - " << threadMedia;
//    t_pthread->start();
}

void MainWindow::on_pushButton_clicked()
{
    int ret = get_fingerState();
    qDebug() << "RECONNECT FINGER" << ret;

    if(!ret){
        cmd_fingerConnect();
    }
//    cmd_twistSeal();
}

void MainWindow::setDeviceFlag(QString state){
    QStringList infoList = state.split(",",QString::SkipEmptyParts);
    int i = 0;
    QString tmp;
    if(infoList.count() > 5){
        tmp = infoList.at(0);
        if(tmp.toInt() != alarmState){
            i = 1;
        }
        tmp = infoList.at(2);
        if(tmp.toInt() != curDoorSen){
            i = 1;
        }
        tmp = infoList.at(3);
        if(tmp.toInt() != curKeySen){
            i = 1;
        }
        tmp = infoList.at(4);
        if(tmp.toInt() != rollFlag){
            i = 1;
        }
    }
    stateFlag = i;
    qDebug() << "deviceState Result: " << stateFlag;
}

