#include "innosdk.h"
#include "device/statethread.h"
#include "sdk_global.h"

InnoSDK *pMain;

int status = 0;

InnoSDK::InnoSDK(QWidget *parent)
{
    qDebug() << "init SDK class222";
    initFlag = 0;
    // DB 객체 생성 및 init

    deposit_data = &depositStr;
    init_DepositStr();

    settle_data = &settleStr;

    // 기계번호 등록
    // 스크립트 읽어서 기계시리얼 확인 필요 또는 사용자 등록
    currentVaultID = 201801001;
    // 입금번호 등록
    // DB마지막 레코드 읽어서 1번호씩 추가
    currentVaultDepositId = 0;
    intermediateFlag = 0;
    depositFlag = 0;        // 입금여부 확인용

    autoErrClearCnt = 0;


    errReqCnt = -1;


#ifdef SDK_DATABASE         // SDK 데이터베이스사용  - PC와의 통신작업에 있어 번거로움이 있음
    // test path
    pDatabase = new DatabaseManager();
    //---------------------------------------------------------USER INFO DATABASE------------------------------------------------------
    pDatabase->OpenDatabase(PATH_USER);
    qDebug() << "create USER_DB idle";
    if(!pDatabase->db.tables().contains("USER")){             // 유저테이블 확인 후 생성
        qDebug() << "create table user";
        QString query = "USER (clientName VARCHAR(255), userFullName VARCHAR(255), userName VARCHAR(255), password VARCHAR(255),"
                        "roles INT, UserId INT, expiresDateTime INT, creationDateTime INT, lastDepositDateTime INT, Status BOOL)";
        pDatabase->CreateTable(query);
    }
    if(!pDatabase->db.tables().contains("INFO")){             // 유저테이블 확인 후 생성
        qDebug() << "create table user";
        QString query = "INFO (VaultId INT, BagId INT, SettlementId INT, SpareInt1 INT, SpareInt2 INT,"
                        "SdkVersion VARCHAR(255), SpareChar1 VARCHAR(255), SpareChar2 VARCHAR(255), SpareBool1 BOOL, SpareBool2 BOOL)";
        pDatabase->CreateTable(query);

        // 스크립트 읽어오고 기계버젼 찾아서 레코드 생성
    }
    pDatabase->db.close();

    //---------------------------------------------------------DEPOSIT DATABASE------------------------------------------------------
    pDatabase->OpenDatabase(PATH_DEPOSIT);
    qDebug() << "create DEPOSIT_DB idle";
    if(!pDatabase->db.tables().contains("DEPOSITLIST")){             // 유저테이블 확인 후 생성
        qDebug() << "create table user";
        QString query = "DEPOSITLIST (DepositDateTime INT, VaultId INT, UserId INT, VaultDepositId INT,"
                        "CurrencyId INT, DepositTypeId INT, currencyUnit VARCHAR(255), currencyCount VARCHAR(255),"
                        "currencySum VARCHAR(255), etcEnvelope VARCHAR(255), sumCount INT, sumTotal DOUBLE)";
        pDatabase->CreateTable(query);
    }

    if(!pDatabase->db.tables().contains("SETTLELIST")){             // 유저테이블 확인 후 생성
        qDebug() << "create table user";
        QString query = "SETTLELIST (SettlementId INT, BagId INT, VaultId INT, SettlementDateTime INT,"
                        "depositCount INT, transCashAmount DOUBLE, transEtcAmount DOUBLE)";
        pDatabase->CreateTable(query);
    }
#else                       // UI 데이터베이스사용


#endif
    init_countryUnit();
    initFlag = init_serial();

    retryCnt = 0;
    waitCnt = 0;
    loginState = false;

    currentMode = MODE_IDLE;
    currentStep = IDLE_NORMALSTATE;

    currentState = 0;
    errorStateCode = 0;
    m_stateThread->transaction(CMD_IDLE,"");
    m_stateThread->setSensor(CUR_SEN);

    // 바코드 인식에 대한 리턴을 주기 위함
    barcodeChanged = 0;
    preBagCode = 000001;                // 처음에 DB에서 받아와야 함
    currentBagCode = 000001;

    // Finger test
    fingerState = 0;
    fScan = 0;

    //메인 타이머 설정
    pMain_timer = new QTimer(this);
    connect(pMain_timer, SIGNAL(timeout()),this,SLOT(main_timer_event()));
    qDebug() << "SDK 메인타이머 시작";
    pMain_timer->start(200);

    tCurrSen.stacker = 0;
    tCurrSen.Error = 0;
    transStep = 0;


    pFinger = new finger_class();
}
//----------------------------------------------------------------------------------------
//----------------국가별 권종 일람표 등록------------------------------------------------------
void InnoSDK::init_countryUnit(){
    QList<int>tmpList[100];
    tmpList[0] << 20 << 50 << 100 << 200 << 500 << 1000 << 2000;
    unitMap.insert(38,tmpList[0]);      // 도미니카 권종  38  DOP
    tmpList[1] << 1000 << 5000 << 10000 << 50000;
    unitMap.insert(03,tmpList[1]);      // 대한민국 권종  03  KRW
    tmpList[2] << 1 << 2 << 5 << 10 << 20 << 50 << 100;
    unitMap.insert(01,tmpList[2]);      // 미국달러 권종  01  USD
    tmpList[3] << 5 << 10 << 20 << 50 << 100 << 200 << 500;
    unitMap.insert(04,tmpList[3]);      // 유로화 권종   04  EUR
    tmpList[4] << 0.25 << 0.5 << 1 << 5 << 10 << 20;
    unitMap.insert(46,tmpList[4]);      // 쿠웨이트 권종  46  KWD
    tmpList[5] << 5 << 10 << 20 << 50 << 100 << 200 << 500 << 1000;
    unitMap.insert(25,tmpList[5]);      // UAE 권종  25  AED
}

void InnoSDK::init_DepositStr(){
    qDebug() << "initialize SDK Deposit struct";
    deposit_data->sumCount = 0;
    deposit_data->sumTotal = 0;
    deposit_data->curCount = 0;
    deposit_data->curTotal = 0;
    for(int i=0; i<12; i++){
        if(i<5){
            deposit_data->etcEnvelope[i] = 0;
        }
        deposit_data->currencyUnit[i] = 0;
        deposit_data->currencyCount[i] = 0;
        deposit_data->currencySum[i] = 0;
    }
}

//void InnoSDK::getCurrentTime(){

//}

//-----------------------------------------------------------------------------------------------------------------------------------------------
void InnoSDK::countCompleteSignal(){
    int status = VAULT_STATUS_NO_ERROR;
    currentVaultDepositId += 1;         // 입금번호
    depositFlag += 1;

    deposit_data->DepositDateTime = QDateTime::currentDateTime().toTime_t();
    deposit_data->VaultId = currentVaultID;
    deposit_data->UserId = currentUserId;
    deposit_data->VaultDepositId = currentVaultDepositId;
    deposit_data->CurrencyId = currentCurrencyId;
    deposit_data->DepositTypeId = currentMode;

    init_DepositStr();

    for(int i=0; i<12; i++){
        if(i < currentCountryList.count()){
            deposit_data->currencyUnit[i] = currentCountryList.at(i);
        }
        else{
            deposit_data->currencyUnit[i] = 0;
        }
        qDebug() << "deposit array: " << tNoteData.nData[i] << deposit_data->currencyUnit[i];
        qDebug() << "deposit prearray: " << tNoteData.nPreData[i] << deposit_data->currencyUnit[i];

        deposit_data->currencyCount[i] = tNoteData.nData[i];
        deposit_data->preCount[i] =      tNoteData.nPreData[i];

        deposit_data->preSum[i] = deposit_data->currencyUnit[i] * deposit_data->preCount[i];
        deposit_data->currencySum[i] = deposit_data->currencyUnit[i] * deposit_data->currencyCount[i];
        deposit_data->sumCount += deposit_data->currencyCount[i];
        deposit_data->curCount += tNoteData.nPreData[i];
        deposit_data->sumTotal += deposit_data->currencySum[i];
        deposit_data->curTotal += deposit_data->currencyUnit[i] * tNoteData.nPreData[i];
    }

    if(tCurrSen.Error){
        status = VAULT_STATUS_ERROR;
    }
// status 에 계수된 데이터 유무 정보 포함   for test = 1
    pDepositFunc(deposit_data, status, my_data_ptr);                                         // 훅 이벤트 호출

//    else{
//        if(currentMode == MODE_BULK_DEPOSIT){                           // 벌크 입금일 경우
//            pMain->depositOperationComplete();      // DB 저장함수
//        }
//    }
}
//--------------------------------------------------------------------------------------------------------------------------------------
void InnoSDK::settlementCompleteSignal(int status){
    pSettlementFunc(settle_data, status, my_stdata_ptr);
}

//--------------------------------------------------------------------------------------------------------------------------------------
void InnoSDK::depositCompleteSignal()
{
//    for(int i=0; i<12; i++){
//        deposit_data->currencyUnit[i] = 0;
//        deposit_data->currencyCount[i] = 0;
//        deposit_data->currencySum[i] = 0;
//    }

    // DB저장 코드 구현해야 함
}
//---------------------------------------------------------------------------------------------------------------------------------------

void InnoSDK::operationStart(){             // 모든 동작 시작
    m_stateThread->transaction(CMD_CURRENCY_START,"");
}
//--------------------------------------------------------------------------------------------------------------------------------------
void InnoSDK::setCountryUnitList(int code)
{
    // for test
    currentCountryList = unitMap.value(code);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------
void InnoSDK::depositOperationComplete()
{
//    pStatusFunc(STATUS_ENABLEDATABASE);             // 입금 타이밍에 훅이벤트 전달

#ifdef SDK_DATABASE         // SDK 데이터베이스사용  - PC와의 통신작업에 있어 번거로움이 있음
    QString unitArr, countArr, sumArr, etcArr;

    etcArr.append(QString("%1").arg(deposit_data->etcEnvelope[0]));
    unitArr.append(QString("%1").arg(deposit_data->currencyUnit[0]));
    countArr.append(QString("%1").arg(deposit_data->currencyCount[0]));
    sumArr.append(QString("%1").arg(deposit_data->currencySum[0]));
    for(int i=1; i<12; i++){
        if(i<5){
            etcArr.append(QString(",%1").arg(deposit_data->etcEnvelope[i]));
        }
        if(i<12){
            unitArr.append(QString(",%1").arg(deposit_data->currencyUnit[i]));
            countArr.append(QString(",%1").arg(deposit_data->currencyCount[i]));
            sumArr.append(QString(",%1").arg(deposit_data->currencySum[i]));
        }
    }
    pMain->pDatabase->db.setDatabaseName(PATH_DEPOSIT);
    pMain->pDatabase->db.open();
    pMain->pDatabase->InsertRecord("DEPOSITLIST", QString("%1, %2, %3, %4, %5, %6, '%7', '%8', '%9', '%10', %11, %12").arg(deposit_data->DepositDateTime).
                                   arg(deposit_data->VaultId).arg(deposit_data->UserId).arg(deposit_data->VaultDepositId).arg(deposit_data->CurrencyId).
                                   arg(deposit_data->DepositTypeId).arg(unitArr).arg(countArr).arg(sumArr).arg(etcArr).arg(deposit_data->sumCount).arg(deposit_data->sumTotal));
#endif
}

//----------------------------------------------------------------------------------------------------------------------------------------------
void InnoSDK::main_timer_event(){

    if(fConnection == 0){
        if(pFinger->g_bConnectionStatus){       // 통신 연결 여부 확인
            fConnection = 1;
        }
    }


//    qDebug() << "error state" << pMain->tCurrSen.Error;
    //    qDebug() << "door key sen" << pMain->m_stateThread->SenDoorKey;
    //    qDebug() << "door sen" << pMain->m_stateThread->SenDoorState;
    //    qDebug() << "fdoor Unlock" << pMain->m_stateThread->fDoorUnlock;
    //    qDebug() << "fdoor Lock" << pMain->m_stateThread->fDoorLock;

    //-----------------------------에러검사 현재 스탭연계 확인--------------------------
//    if(pMain->tCurrSen.Error) {
//        status = VAULT_STATUS_ERROR;
//    }
//    else{
//        status = 0;
//    }
//    qDebug() << "count complete";
//    pMain->fCountStart = false;
//    pMain->fOutStack = 0;
//    countCompleteSignal(status);
    // call deposit hook function
    //-----------------------------------------------------------------------------
    if(tCurrSen.Error > 0){
        if(status == 0){
//            clearErrorState();
            status = VAULT_STATUS_ERROR;
            errReqCnt = 3;
        }
        else{
            if(errReqCnt < 0){
                errorStateCode = tCurrSen.Error;    // ALARM No.1~7: JAM STATE
                 qDebug() <<"에러코드 발생 " << errorStateCode;
            }
            else{
                errReqCnt -= 1;
            }
        }
    }
    else{
        errorStateCode = tCurrSen.Error;    // ALARM No.1~7: JAM STATE
        status = 0;
    }

//    if(transStep == 1){
//        if(pMain->fDoorOpen) {     // Complete Sealing and door locker unlock
//            transStep = 0;
//            settlementCompleteSignal(0);
//        }
//        if(!m_stateThread->SenDoorState) {     // Complete Sealing and door locker unlock
//            transStep = 0;
//            settlementCompleteSignal(0);
//        }
//    }


//    switch(currentMode){
//    case MODE_IDLE:
//        switch(currentStep){
//        case IDLE_NORMALSTATE:
//            if(tCurrSen.stacker) {
//                // HOOK 이벤트 호출 해야 함
//                errorStateCode = 10;    // ALARM No.1: 대기모드인데 아웃포켓 센서가 감지 됨
//                pStatusFunc(STATUS_ERROROPERATION);
//                setCurrentMode(MODE_ERRORMODE);
//            }
//            else if(tCurrSen.Error){         // JAM 상태파악 확인
//                qDebug() << "mode idle Error state occurred";
//                errorStateCode = tCurrSen.Error;    // ALARM No.1~7: JAM STATE
//                pStatusFunc(STATUS_ERROROPERATION);
//                setCurrentMode(MODE_ERRORMODE);
//            }
//            break;
//        case IDLE_GATECLOSECHK:
//            if(pMain->m_stateThread->mc_stat == MACHINE_READY){
//                currentStep = IDLE_NORMALSTATE;
//                pStatusFunc(STATUS_ACKIDLECOMP);
////                if(pMain->preMode == 1 || pMain->preMode == 2){     // 자동취소 전환 시에는 메인으로
////                }
////                    pMain->pStatusFunc(STATUS_TRANSIDLEMODE);
////                }
//                pMain->init_DepositStr();
//            }
//            else if(m_stateThread->gt_outgate->gateStatus == GATE_FAILED ||
//                    m_stateThread->gt_ingate->gateStatus == GATE_FAILED){

//                qDebug() << "idle gate close: " << m_stateThread->gt_ingate->gateStatus
//                         << m_stateThread->gt_outgate->gateStatus;

//                if(m_stateThread->gt_outgate->gateStatus == GATE_FAILED)  m_stateThread->gt_outgate->gateStatus = GATE_STOP;
//                if(m_stateThread->gt_ingate->gateStatus == GATE_FAILED)  m_stateThread->gt_ingate->gateStatus = GATE_STOP;

//                errorStateCode = 11;    // ALARM No.2: IDLE모드 - 대기모드 상태 전환 커맨드 에러.
//                pStatusFunc(STATUS_ERROROPERATION);
//                setCurrentMode(MODE_ERRORMODE);
//            }
//            else if(m_stateThread->gt_outgate->gateStatus == GATE_ERROR){
//                m_stateThread->gt_outgate->gateStatus = GATE_STOP;
//                errorStateCode = 17;    // ALARM No.17: IDLE모드 - 대기모드 상태 전환중 이물질낌.
//                pStatusFunc(STATUS_ERROROPERATION);
//                currentStep = IDLE_NORMALSTATE;
//                setCurrentMode(MODE_ERRORMODE);
//            }
//            break;
//        }
//        break;
//        //-------------------------------------------------------------------------
//    case MODE_DEPOSIT:
////        qDebug() << "MODE_DEPOSIT"<< currentStep;
//        switch(currentStep){
//        case DEPOSIT_IDLE:                  // 작업대기상태
//            if(tCurrSen.Error){         // JAM 상태파악 확인
//                if(waitCnt < 0){
//                    clearErrorState();
//                    autoErrClearCnt += 1;
//                    waitCnt = 5;
//                    if(autoErrClearCnt >= 5){
//                        autoErrClearCnt = 0;
//                        if(depositFlag==0){
//                            qDebug() << "mode escrow Error state occurred";
//                            errorStateCode = tCurrSen.Error;    // ALARM No.1~7: JAM STATE
//                            pStatusFunc(STATUS_ERROROPERATION);
//                            setCurrentMode(MODE_ERRORMODE);
//                        }
//                    }
//                }
//                else{
//                    waitCnt -= 1;
//                }
//            }
//            break;
//        case DEPOSIT_WAITINPUT:         // 투입대기
//            if(tCurrSen.Error){         // JAM 상태파악 확인
//                if(waitCnt < 0){
//                    clearErrorState();
//                    autoErrClearCnt += 1;
//                    waitCnt = 5;
//                    if(autoErrClearCnt >= 5){
//                        autoErrClearCnt = 0;
//                        if(depositFlag==0){
//                            qDebug() << "mode escrow Error state occurred";
//                            errorStateCode = tCurrSen.Error;    // ALARM No.1~7: JAM STATE
//                            pStatusFunc(STATUS_ERROROPERATION);
//                            setCurrentMode(MODE_ERRORMODE);
//                        }
//                    }
//                }
//                else{
//                    waitCnt -= 1;
//                }
//            }
//            else{
//                if(tCurrSen.hopper){
//                }
//            }
//            break;
//        case DEPOSIT_WAITCOUNT:
//            if(fCountStart){     // 계수대기
//                qDebug() <<"계수는 시작됬다. countStarted";
//                pStatusFunc(STATUS_COUNTSTART);
//                currentStep = DEPOSIT_WAITCOUNTING;
//                if(tCurrSen.Error){         // JAM 상태파악 확인
//                    if(waitCnt < 0){
//                        clearErrorState();
//                        autoErrClearCnt += 1;
//                        waitCnt = 5;
//                        if(autoErrClearCnt >= 5){
//                            autoErrClearCnt = 0;
//                            if(depositFlag==0){
//                                qDebug() << "mode escrow Error state occurred";
//                                errorStateCode = tCurrSen.Error;    // ALARM No.1~7: JAM STATE
//                                pStatusFunc(STATUS_ERROROPERATION);
//                                setCurrentMode(MODE_ERRORMODE);
//                            }
//                        }
//                    }
//                    else{
//                        waitCnt -= 1;
//                    }
//                }
//            }
//            else{
//                if(tCurrSen.Error){         // JAM 상태파악 확인
////                    qDebug() <<"계수는 시작안되고 에러";
//                }
//            }
//            break;
//        case DEPOSIT_WAITCOUNTING:      // 계수완료대기
//            if(fOutStack){
//                qDebug() << "count complete";
//                fCountStart = false;
//                fOutStack = 0;
//                currentStep = DEPOSIT_CHECKREJECT;
//                waitCnt = 5;
//                // call deposit hook function
//            }
//            else if(pMain->tCurrSen.Error) {
//                qDebug() <<"에러가 났지만 데이터는 받아야한다. error occured. but get count data";
//                m_stateThread->transaction(CMD_CURRENCY_CHK_CNT_RELSULT,"");
//            }
//            break;
//        case DEPOSIT_CHECKREJECT:
//            if(chk_rejectPocket()){
//                qDebug() << "check reject!!";
//                pStatusFunc(STATUS_REJECTEXIST);
//                currentStep = DEPOSIT_WAITREJECT;
//            }
//            else{
//                if(waitCnt == 0){
//                    qDebug() << "reject clear!!";
//                    //                    currentStep = DEPOSIT_WAITINPUT;
//                    pStatusFunc(STATUS_REJECTCOMPLETE);
//                    currentStep = DEPOSIT_IDLE;
//                    countCompleteSignal(status);
//                }
//                else{
//                    if(fCountStart){                            // 리젝 대기 타이밍 중에 계수가 시작되면 리젝이 없는것임
//                        //                        currentStep = DEPOSIT_WAITCOUNT;
//                        //                        break;
//                    }
//                    waitCnt  -= 1;                  // 리젝트 인식을 2초 대기
//                }
//            }
//            break;
//        case DEPOSIT_WAITREJECT:
//            if(!chk_rejectPocket()){
//                qDebug() << "reject exist clear!!";
//                //                currentStep = DEPOSIT_WAITINPUT;
//                pStatusFunc(STATUS_REJECTCOMPLETE);
//                currentStep = DEPOSIT_IDLE;
//                countCompleteSignal(status);
//            }
//            break;
//        case DEPOSIT_ADDDEPOSIT:
//            if(m_stateThread->mc_stat == MACHINE_DEPOSITED){
//                pMain->pStatusFunc(STATUS_ADDCOMPLETE);
//                m_stateThread->transaction(CMD_IDLE,"");
//                currentStep = DEPOSIT_ADDCOMPLETE;
//                //                setCurrentMode(0);
//            }
//            else if(m_stateThread->mc_stat == MACHINE_DEPOSITING){
//                // 게이트 열렸는데 처리가 안 될 경우  돈 걸린거임
//                clearErrorState();
//                if(waitCnt == 0){
//                    qDebug() << "추가입금 돈 걸렸다~~~!!";
//                    pMain->pStatusFunc(STATUS_ADDCOMPLETE);
//                    m_stateThread->transaction(CMD_IDLE,"");
//                    currentStep = DEPOSIT_ADDCOMPLETE;
//                }
//                else{
//                    waitCnt -= 1;
//                }
//            }
//            else if(m_stateThread->gt_ingate->gateStatus == GATE_FAILED){
//                qDebug() << "add deposit: " << m_stateThread->gt_ingate->gateStatus;

//                if(m_stateThread->gt_ingate->gateStatus == GATE_OPENED){
//                    pMain->pStatusFunc(STATUS_ADDCOMPLETE);
//                }

//                if(m_stateThread->gt_ingate->gateStatus == GATE_FAILED)  m_stateThread->gt_ingate->gateStatus = GATE_STOP;
//                errorStateCode = 19;    // ALARM No.19: 추가입금 처리 중 커맨드 에러
//                qDebug() << "추가 입금처리 중 완료 커맨드 에러!" << m_stateThread->gt_ingate->gateStatus;
//                pStatusFunc(STATUS_ERROROPERATION);
//                setCurrentMode(MODE_ERRORMODE);
//            }
//            break;
//        case DEPOSIT_ADDCOMPLETE:
//            if(pMain->m_stateThread->mc_stat == MACHINE_READY){
//                currentStep = DEPOSIT_WAITINPUT;
//            }
//            else if(m_stateThread->gt_outgate->gateStatus == GATE_FAILED ||
//                    m_stateThread->gt_ingate->gateStatus == GATE_FAILED){

//                qDebug() << "idle gate close: " << m_stateThread->gt_ingate->gateStatus
//                         << m_stateThread->gt_outgate->gateStatus;

//                if(m_stateThread->gt_outgate->gateStatus == GATE_FAILED)  m_stateThread->gt_outgate->gateStatus = GATE_STOP;
//                if(m_stateThread->gt_ingate->gateStatus == GATE_FAILED)  m_stateThread->gt_ingate->gateStatus = GATE_STOP;

//                errorStateCode = 11;    // ALARM No.2: IDLE모드 - 대기모드 상태 전환 커맨드 에러.
//                pStatusFunc(STATUS_ERROROPERATION);
//                setCurrentMode(MODE_ERRORMODE);
//            }
////            else if(m_stateThread->gt_outgate->gateStatus == GATE_ERROR){
////                m_stateThread->gt_outgate->gateStatus = GATE_STOP;
////                errorStateCode = 17;    // ALARM No.17: IDLE모드 - 대기모드 상태 전환중 이물질낌.
////                pStatusFunc(STATUS_ERROROPERATION);
////                setCurrentMode(MODE_ERRORMODE);
////            }
//            break;
//        }
//        break;
//        //------------------------------------------------------------------------------------------------
//    case MODE_BULK_DEPOSIT:
////        qDebug() << "MODE_BULK_DEPOSIT" << currentStep;
//        switch(currentStep){
//        case DEPOSIT_IDLE:                  // 작업대기상태
//            if(tCurrSen.Error){         // JAM 상태파악 확인
//                qDebug() << "mode bulk Error state occurred";
//                errorStateCode = tCurrSen.Error;    // ALARM No.1~7: JAM STATE
//                pStatusFunc(STATUS_ERROROPERATION);
//                setCurrentMode(MODE_ERRORMODE);
//            }
//            break;
//        case DEPOSIT_WAITINPUT:         // 투입대기
//            if(tCurrSen.Error){         // JAM 상태파악 확인
//                if(waitCnt < 0){
//                    clearErrorState();
//                    autoErrClearCnt += 1;
//                    waitCnt = 5;
//                    if(autoErrClearCnt >= 5){
//                        autoErrClearCnt = 0;
//                        if(depositFlag==0){
//                            qDebug() << "mode escrow Error state occurred";
//                            errorStateCode = tCurrSen.Error;    // ALARM No.1~7: JAM STATE
//                            pStatusFunc(STATUS_ERROROPERATION);
//                            setCurrentMode(MODE_ERRORMODE);
//                        }
//                    }
//                }
//                else{
//                    waitCnt -= 1;
//                }
//            }
//            break;
//        case DEPOSIT_WAITCOUNT:
//            if(fCountStart){     // 계수대기
//                pStatusFunc(STATUS_COUNTSTART);
//                currentStep = DEPOSIT_WAITCOUNTING;
//            }
//            else{
//            }
//            break;
//        case DEPOSIT_WAITCOUNTING:      // 계수완료대기
//            if(fOutStack){
//                qDebug() << "count complete";
//                fCountStart = false;
//                fOutStack = 0;
//                currentStep = DEPOSIT_CHECKREJECT;
//                waitCnt = 5;
//                // call deposit hook function
//            }
//            else if(pMain->tCurrSen.Error) {
//                qDebug() <<"에러가 났지만 데이터는 받아야한다.  bulk mode error occured. but get count data";
//                m_stateThread->transaction(CMD_CURRENCY_CHK_CNT_RELSULT,"");
//            }
//            break;
//        case DEPOSIT_CHECKREJECT:
//            if(chk_rejectPocket()){
//                pStatusFunc(STATUS_REJECTEXIST);
//                currentStep = DEPOSIT_WAITREJECT;
//            }
//            else{
//                if(waitCnt == 0){
//                    //                    currentStep = DEPOSIT_WAITINPUT;
//                    pStatusFunc(STATUS_REJECTCOMPLETE);
//                    currentStep = DEPOSIT_IDLE;
//                    countCompleteSignal(status);
//                }
//                else{
//                    if(fCountStart){                            // 리젝 대기 타이밍 중에 계수가 시작되면 리젝이 없는것임
//                        break;
//                    }
//                    waitCnt  -= 1;                  // 리젝트 인식을 1초 대기
//                }
//            }
//            break;
//        case DEPOSIT_WAITREJECT:
//            if(!chk_rejectPocket()){
//                pStatusFunc(STATUS_REJECTCOMPLETE);
//                currentStep = DEPOSIT_IDLE;
//                countCompleteSignal(status);
//            }
//            break;
//        case DEPOSIT_BLOCKOPENCHK:         // 블록게이트 오픈 체크
//            if(m_stateThread->mc_stat == MACHINE_BULKMODE){
//                currentStep = DEPOSIT_WAITINPUT;
//                pStatusFunc(STATUS_ACKBULKMODE);
//                qDebug() << "DEPOSIT_BLOCKOPENED";
//            }
//            else if(m_stateThread->gt_ingate->gateStatus == GATE_FAILED){

//                qDebug() << "block open check: " << m_stateThread->gt_ingate->gateStatus;

//                if(m_stateThread->gt_ingate->gateStatus == GATE_FAILED)  m_stateThread->gt_ingate->gateStatus = GATE_STOP;

//                errorStateCode = 16;    // ALARM No.16: 벌크모드 진입 실패.
//                pStatusFunc(STATUS_ERROROPERATION);
//                setCurrentMode(MODE_ERRORMODE);
//            }
//            break;
//        }
//        break;
//    case MODE_ETC_DEPOSIT:
//        switch(currentStep){
//        case ETC_IDLE:
//            //            qDebug() << "ETC IDLE";
//            break;
//        case ETC_GATEOPENCHK:          // 게이트 오픈 체크
//            qDebug() << "ETC_GATEOPENCHK";
//            if(m_stateThread->gt_outgate->gateStatus == GATE_OPENED){
//                currentStep = ETC_IDLE;
//                //                pMain->pStatusFunc(STATUS_WAITENVINPUT);
//            }
//            else if(m_stateThread->gt_outgate->gateStatus == GATE_FAILED){

//                qDebug() << "etc gate open: " << m_stateThread->gt_outgate->gateStatus;
//                m_stateThread->gt_outgate->gateStatus = GATE_STOP;
//                errorStateCode = 15;    // ALARM No.15: 봉투입금 동작 중 게이트 열기 에러
//                pStatusFunc(STATUS_ERROROPERATION);
//                setCurrentMode(MODE_ERRORMODE);
//            }
//            break;
//        }
//        break;
//        //----------------------------------------------------------------
//    case MODE_DEPOSIT_CANCEL:             //  취소 동작 시퀀스
//        switch(currentStep){
//        case CANCEL_IDLE:
//            break;
//        case CANCEL_GATEOPENCHK:          // 게이트 오픈 체크
//            if(m_stateThread->gt_outgate->gateStatus == GATE_OPENED){
//                currentStep = CANCEL_WAITRETURN;
//                waitCnt = 6;
//                retryCnt = 50;
//                pMain->pStatusFunc(STATUS_WAITRETURN);
//            }
//            else if(m_stateThread->gt_outgate->gateStatus == GATE_FAILED){
//                qDebug() << "cancel gate open: " << m_stateThread->gt_outgate->gateStatus;
//                m_stateThread->gt_outgate->gateStatus = GATE_STOP;
//                errorStateCode = 12;    // ALARM No.12: 취소 동작 중 게이트 열기 에러
//                pStatusFunc(STATUS_ERROROPERATION);
//                setCurrentMode(MODE_ERRORMODE);
//            }
//            break;
//        case CANCEL_WAITRETURN:           //  금액 회수 대기
//            if(!pMain->tCurrSen.stacker){
//                if(waitCnt == 0){
//                    pMain->m_stateThread->transaction(CMD_OUTGATE_CLOSE,"");
//                    currentStep = CANCEL_CHECKCLOSE;
//                    waitCnt = 4;
//                }
//                else{
//                    waitCnt  -= 1;                  // 회수 완료 후 2초대기
//                }
//            }
////            else{
////                if(retryCnt == 0){          // 안찾아가면 10 초
////                    pMain->m_stateThread->transaction(CMD_OUTGATE_CLOSE,"");
////                    currentStep = CANCEL_CHECKCLOSE;
////                }
////                retryCnt -= 1;
////            }
//            break;
//        case CANCEL_CHECKCLOSE:
//            if(m_stateThread->gt_outgate->gateStatus == GATE_CLOSED){
//                currentStep = CANCEL_IDLE;
//                pMain->pStatusFunc(STATUS_CANCELCOMPLETE);
//                //                setCurrentMode(0);
//            }
//            else if(m_stateThread->gt_outgate->gateStatus == GATE_FAILED){

//                qDebug() << "cancel check close: " << m_stateThread->gt_outgate->gateStatus;
//                m_stateThread->gt_outgate->gateStatus = GATE_STOP;
//                errorStateCode = 13;    // ALARM No.13: 취소 동작 중 게이트 닫기 에러
//                pStatusFunc(STATUS_ERROROPERATION);
//                setCurrentMode(MODE_ERRORMODE);
//            }
//            else if(m_stateThread->gt_outgate->gateStatus == GATE_CHK_STAT){
//                // Emergency ing.....
//            }
//            else if(m_stateThread->gt_outgate->gateStatus == GATE_ERROR){
//                pStatusFunc(OUTGATE_EMERGENCY);
//                // later Emergency Ack 해야함...고민.....
//            }
//            break;
//        }
//        break;
//        //----------------------------------------------------------------
//    case MODE_DEPOSIT_COMPLETE:             //  완료 모드
//        //        qDebug() << "MODE_DEPOSIT_COMPLETE"<< currentStep;
//        switch(currentStep){
//        case COMPLETE_IDLE:
//            break;
//        case COMPLETE_GATEOPENCHK:
//            if(m_stateThread->mc_stat == MACHINE_DEPOSITED){
//                if(preMode == 1){       // 기타입금 또는 벌크모드에서의 완료모드랑 분리해야함
//                    pMain->depositOperationComplete();      // DB 저장함수
//                }
//                currentStep = COMPLETE_IDLE;
//                pMain->pStatusFunc(STATUS_DEPOSITCOMPLETE);
//                //                setCurrentMode(0);
//            }
//            else if(m_stateThread->gt_ingate->gateStatus == GATE_FAILED){

//                qDebug() << "complete block open check: " << m_stateThread->gt_ingate->gateStatus;

//                if(m_stateThread->gt_ingate->gateStatus == GATE_OPENED){
//                    if(preMode == 1){       // 기타입금 또는 벌크모드에서의 완료모드랑 분리해야함
//                        pMain->depositOperationComplete();      // DB 저장함수
//                    }
//                }

//                if(m_stateThread->gt_ingate->gateStatus == GATE_FAILED)  m_stateThread->gt_ingate->gateStatus = GATE_STOP;
//                errorStateCode = 14;    // ALARM No.14: 입금 처리 중 커맨드 에러
//                pStatusFunc(STATUS_ERROROPERATION);
//                setCurrentMode(MODE_ERRORMODE);
//            }
//            else if(m_stateThread->mc_stat == MACHINE_DEPOSITING){
//                // 게이트 열렸는데 처리가 안 될 경우  돈 걸린거임
//                clearErrorState();
//                if(waitCnt == 0){
//                    qDebug() << "idle gate close: " << m_stateThread->gt_ingate->gateStatus
//                             << m_stateThread->gt_outgate->gateStatus;
//                    qDebug() << "돈 걸렸다~~~!!";
//                    if(preMode == 1){       // 기타입금 또는 벌크모드에서의 완료모드랑 분리해야함
//                        pMain->depositOperationComplete();      // DB 저장함수
//                    }
//                    currentStep = COMPLETE_IDLE;
//                    pMain->pStatusFunc(STATUS_DEPOSITCOMPLETE);
//                }
//                else{
//                    waitCnt -= 1;
//                }
//            }
//            break;
//        }
//        break;
//        //---------------------------------------------------CANCEL_WAITRETURN-------------
//    case MODE_TRANSCASH:                    //  현송 모드
//        if(pMain->m_stateThread->mc_stat == MACHINE_ERROR){
//            errorStateCode = 18;    // ALARM No.18: 현송오류
//            pStatusFunc(STATUS_ERROROPERATION);
//            setCurrentMode(MODE_ERRORMODE);
//        }
//        else if(transStep == 1){
//            if(pMain->fDoorOpen) {     // Complete Sealing and door locker unlock
//                transStep = 2;
//                settlementCompleteSignal(0);
//            }
//        }
//        break;
//    case MODE_ERRORMODE:                    //  에러 모드 처리 조건에 따라 분기
//        switch(errorStateCode){
//        case 11:
//        case 17:
//            if(m_stateThread->mc_stat == MACHINE_READY){
//                errorStateCode = 0;
//            }
//            break;
//        case 12:
//        case 15:
//            if(m_stateThread->gt_outgate->gateStatus == GATE_OPENED){
//                errorStateCode = 0;
//            }
//            break;
//        case 13:
//            if(m_stateThread->gt_outgate->gateStatus == GATE_CLOSED){
//                errorStateCode = 0;
//            }
//            break;
//        case 14:
//        case 19:
//            if(m_stateThread->mc_stat == MACHINE_DEPOSITING || m_stateThread->mc_stat == MACHINE_DEPOSITED){
//                errorStateCode = 0;
//            }
//            break;
//        case 16:
//            if(m_stateThread->mc_stat == MACHINE_BULKMODE){
//                errorStateCode = 0;
//            }
//            break;
//        }
////        if(m_stateThread->mc_stat != MACHINE_WAIT){
////            m_stateThread->mc_stat = MACHINE_WAIT;
////        }
//        //        switch(errorStateCode){
//        //        case 0:
//        //            break;
//        //        case 1:
//        //            if(!tCurrSen.stacker) {
//        //                if(pMain->m_stateThread->gt_outgate->gateStatus == GATE_CLOSED){
//        //                    setCurrentMode(0);
//        //                }
//        //            }
//        //            break;
//        //        case 2:
//        //            break;
//        //        case 3:
//        //            if(pMain->m_stateThread->gt_outgate->gateStatus == GATE_OPENED){
//        //                currentMode = MODE_DEPOSIT_CANCEL;          // 이전모드로 돌려놓고
//        //                currentStep = CANCEL_WAITRETURN;
//        //                pMain->pStatusFunc(STATUS_WAITRETURN);
//        //            }
//        //            break;
//        //        case 4:
//        //            if(pMain->m_stateThread->gt_outgate->gateStatus == GATE_CLOSED){
//        //                currentMode = MODE_DEPOSIT_CANCEL;          // 이전모드로 돌려놓고
//        //                setCurrentMode(0);
//        //            }
//        //            break;
//        //        case 5:
//        //            break;
//        //        }

//        // 에러에따른 처리여부 체크
//        break;
//    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------
int InnoSDK::init_serial(){
    m_serial = new QSerialPort(this);
    m_serial->setPortName("ttyAMA0");
    m_serial->setBaudRate(QSerialPort::Baud115200);
    if (m_serial->open(QIODevice::ReadWrite)!= true) {
        qDebug() << "Serial Open Error";
        return 1;
    }
    m_stateThread = new State_Thread();

    // serial ->  read Event
    connect(m_serial, &QSerialPort::readyRead, this, &InnoSDK::readData);
    // read serial data -> thread
    connect(this,&InnoSDK::wndSendRxData,m_stateThread,&State_Thread::rcvRxData);
    // thread ->  sdk
    connect(m_stateThread, &State_Thread::mc2HMI, this,&InnoSDK::rsvMcData);

    // thread -> serial
    connect(m_stateThread, &State_Thread::writeTxData, this, &InnoSDK::writeData);
    m_stateThread->start();
    return 0;
}





//------------------------------------------------------------------serial------------------------------------------------
void InnoSDK::rsvMcData(int cmd, void * pdata)
{
    QByteArray bData;
    switch (cmd)
    {
    case ACK_CURRENCY_RESULT:      // Currency
        qDebug() << "MainWindow - ACK_CURRENCY_RESULT";
        m_stateThread->setSensor(CUR_SEN);
        memcpy(&tNoteData,(t_NOTE *)pdata, sizeof(t_NOTE));
        fCountStart = false;
        fOutStack = true;
        break;

    case ACK_CURRENCY_SENSOR:       // Currency Sensor
//        qDebug() << "get sensor";
        memcpy(&tCurrSen,(t_CUR_SENSOR *)pdata, sizeof(t_CUR_SENSOR));
        break;

    case ACK_CURRENCY_COUNT_START:
        m_stateThread->setSensor(CUR_SEN);
        memcpy(&tCurrSen,(t_CUR_SENSOR *)pdata, sizeof(t_CUR_SENSOR));
        fCountStart = true;
        break;

    case ACK_GATE_EMERGENCY:
        fGateEmcy = true;
        break;

    case ACK_SEAL_COMPLETE:
        qDebug() << "MainWindow - Seal Complete";
        fSealComplete = true;
        break;

    case ACK_DOOR_OPEN:
        qDebug() << "MainWindow - Door Open";
        fDoorOpen = true;
        fDoorClose = false;
        break;

    case ACK_DOOR_CLOSE:
        qDebug() << "MainWindow - Door close";
        fDoorOpen = false;
        fDoorClose = true;
        break;

    case ACK_DEVICE_ERROR:
        qDebug() << "Device - Error";
        memcpy(&dev_err,(uint32_t *)pdata, sizeof(uint32_t));
        break;

    case ACK_RECEIPT_COMPLETE:
        receipt_comp = 1;
        break;

    case ACK_BARCODE_DATA:
        bData.append( *(QByteArray *)pdata);
        qDebug() << "barcode = " << bData << QString::fromUtf8(bData.data()) << QString::fromUtf8(bData.data()).toUtf8();
        barcodeByte = bData;
//        if(currentMode == 3){
            pMain->pStatusFunc(STATUS_ACKBARCODE);          // 상위단에 바코드 인식 HooK
//        }
//        else{
//            currentBagCode = QString::fromUtf8(bData.data());
//        }
        qDebug() << currentBagCode << bData.data() << barcodeByte;
        break;

    case ACK_BARCODE_TIMEOUT:
        // Barcode Time Out
        break;

    case ACK_FINGER_IDENTIFY:
        fingerState = 1;
        break;

    case ACK_FINGER_TIMEOUT:
        fingerState = 3;
        break;

    case ACK_FINGER_NOTFOUND:
        fingerState = 2;
        break;

    case ACK_FINGER_ENROLL:
        fingerState = 4;
        break;
    default:
        break;
    }
}

void InnoSDK::readData()
{
    QByteArray data = m_serial->readAll();

    emit wndSendRxData(data);
}

void InnoSDK::writeData(const QByteArray &data)
{
    m_serial->write(data);
}


char *InnoSDK::getStatusMessage(int state){

    char *msg;
    switch(state){
    case VAULT_STATUS_NO_ERROR:
        msg = "no error";
        break;
    case VAULT_STATUS_NO_COMM:
        msg = "no comm";
        break;
    case VAULT_STATUS_SCROW_GATE_OPEN:
        msg = "no scrowOpen";
        break;
    case VAULT_STATUS_VAULT_GATE_OPEN:
        msg = "no vaultOpen";
        break;
    case VAULT_STATUS_NO_ALARM:
        msg = "no alarm";
        break;
    case VAULT_STATUS_NO_FINGERPRINT:
        msg = "no fingerprint";
        break;
    case VAULT_STATUS_NO_SCROW_DETECTED:
        msg = "no scrow_detected";
        break;
    case VAULT_STATUS_NO_BAG_DETECTED:
        msg = "no bag_detected";
        break;
    }
    return msg;
}
