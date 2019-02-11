#include "page_loading.h"
#include "ui_page_loading.h"
#include <QTimer>
#include "global.h"

page_loading::page_loading(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::page_loading)
{
    ui->setupUi(this);

    p_Main->alarmState = 0;

    depositFlag = 0;
    loading_timer = new QTimer(this);
    connect(loading_timer , SIGNAL(timeout()), this, SLOT(set_timeout()));

    qDebug() << "로딩타이머 시작";
    loading_timer->start(500);          // for test

    current_step = IDLE_DEPOSIT;
    preState_step = IDLE_DEPOSIT;

    // set status hook event
    vaultSetStateEvent(&OnStatusEvent);

    pixDoorClose    = new QPixmap("://image/IMAGE_DOORCLOSE.PNG");
    pixDoorOpen     = new QPixmap("://image/IMAGE_DOOROPEN.PNG");
    pixFinger       = new QPixmap("://image/IMAGE_FINGER.PNG");
    pixKeyLock      = new QPixmap("://image/IMAGE_KEYCLOSE.PNG");
    pixKeyUnlock    = new QPixmap("://image/IMAGE_KEYOPEN.PNG");
    pixReject       = new QPixmap("://image/IMAGE_REJECT.PNG");
    pixReturn       = new QPixmap("://image/IMAGE_RETURN.PNG");
    pixInput        = new QPixmap("://image/IMAGE_INPUT.PNG");
    pixBarcode      = new QPixmap("://image/IMAGE_BARCODE.PNG");
    pixPrintCounpon = new QPixmap("://image/LOGO.png");

    pixLogo         = new QPixmap("://image/로고.png");
}

page_loading::~page_loading()
{
    delete ui;
}

void page_loading::init_loadingPage(int mode){
    depositFrameVisible(false);
//    depositFlag = 0;
    QPixmap *tmpPix = pixLogo;

    ui->btn_accuTotal->hide();

    preState_step = current_step;

    pageMode = mode;
    switch(pageMode){
    case 0:     // 수동입금
    case 1:
        ui->btn_accuTotal->show();
        p_Main->setAudioMedia(QUrl::fromLocalFile(QString("%1audio/m3.mp3").arg(PATH_ROOT)),1,15,true);
        tmpPix = pixInput;
        ui->lbl_info->setText(trUtf8("투입구에 현금을 투입하세요."));
        //        cmd_OperationStart();                                               // SDK - CMD_BULK_DEPOSIT
        timingCount = 0;
        inputWaitCount = 30;
        current_step = WAIT_INPUT;
        break;
    case 2:
//        ui->lbl_info->setText("기타입금 안내 이미지");
        break;
    case 3:       // 취소 시 금액회수 및 완료 확인
        tmpPix = pixReturn;
        p_Main->setAudioMedia(QUrl::fromLocalFile(QString("%1audio/es1.mp3").arg(PATH_ROOT)),1,15,true);
        ui->lbl_info->setText(trUtf8("반환된 현금을 포켓에서 회수해 주세요."));
        current_step = CANCEL_WAITRETURN;
        timingCount = 4;
        break;
    case 4:
//        p_Main->setAudioMedia(QUrl::fromLocalFile(PATH_ROOT+"audio/m5.mp3"),1,15,true);
        ui->lbl_info->setText(trUtf8("처리가 완료 되었습니다. 이용해 주셔서 감사합니다."));
        current_step = DISPLAY_COMPLETE;
        timingCount = 1;
        break;
    case 5:
        ui->lbl_info->setText(trUtf8("현송 준비 중 입니다. 잠시만 기다려 주십시오."));
        current_step = WAIT_SETTLECOMPLETE;
        timingCount = 0;
        break;
    case 6:
        ui->lbl_info->setText(trUtf8("현송 준비가 완료되었습니다. 도어를 열어주세요."));
        current_step = WAIT_TRDOOROPEN;
        timingCount = 10;
        break;
    case 7:
        tmpPix = pixDoorClose;
        ui->lbl_info->setText(trUtf8("도어를 완전히 닫아주세요. 5초 후 도어잠금장치를 동작합니다."));
        current_step = WAIT_TRDOORCLOSE;
        timingCount = 10;
        break;
    case 8:
        ui->lbl_info->setText(trUtf8("현송작업이 완료 되었습니다. 감사합니다."));
        current_step = DISPLAY_TRCOMPLETE;
        timingCount = 2;
        break;
    case 9:
        // 계수중입니다 이미지
        ui->lbl_info->setText(trUtf8("계수중입니다."));
        current_step = WAIT_COUNTING;
        timingCount = 40;
        break;
    case 10:
        p_Main->setAudioMedia(QUrl::fromLocalFile(QString("%1audio/m6.mp3").arg(PATH_ROOT)),1,15,true);
        tmpPix = pixReject;
        ui->lbl_info->setText(trUtf8("인식되지 않은 지폐를 포켓에서 제거해 주세요."));
        current_step = WAIT_REJECT;
        timingCount = 6;
        break;
    case 11:
        // 에러발생 이미지
        ui->lbl_info->setText(trUtf8("에러발생 이미지"));
        current_step = ERROR_CHECK;
//        timingCount = 6;
        break;
    case 12:
        // 지문인식 대기 이미지
        tmpPix = pixFinger;
        ui->lbl_info->setText(trUtf8("우측에 부착된 지문인식기 센서에 지문을 인식시켜 주세요."));
        current_step = WAIT_FINGERPRINT;
        timingCount = 30;
        break;
    case 13:
        tmpPix = pixKeyUnlock;
        ui->lbl_info->setText(trUtf8("보조키 잠금을 해제 해 주세요."));
        // 키 오픈 해주세요 이미지
        break;
    case 14:
        tmpPix = pixDoorOpen;
        ui->lbl_info->setText(trUtf8("다이얼을 돌려 도어를 열어주세요."));
        // 다이얼을 돌려 문을 여세요 이미지
        break;
    case 15:
        tmpPix = pixBarcode;
        ui->lbl_info->setText(trUtf8("우측 하단에 부착된 바코드 리더기에 새 봉투의 바코드를 인식시켜주세요."));
        current_step = WAIT_CHANGEBAGBARCODE;
        timingCount = 6;
        // 바코드 인식 대기 이미지
        break;
    case 16:
        tmpPix = pixDoorClose;
        ui->lbl_info->setText(trUtf8("도어를 완전히 닫아주세요."));
        timingCount = 6;
        // 도어 닫아주세요
        break;
    case 17:
        tmpPix = pixKeyLock;
        ui->lbl_info->setText(trUtf8("키를 빼주세요"));
        // 보조키를 돌려 잠가주세요 이미지
        break;
    case 18:
        ui->lbl_info->setText(trUtf8("도어 잠금장치 동작을 체크합니다."));
        timingCount = 4;
        current_step = CHECK_TRDOORLOCK;
        //
        break;
    case 19:
        ui->lbl_info->setText(trUtf8("작업이 취소 되었습니다. 이용해 주셔서 감사합니다."));
        current_step = DISPLAY_CANCELCOMPLETE;
        timingCount = 4;
        //
        break;
    case 20:
        tmpPix = pixPrintCounpon;
        ui->lbl_info->setText(trUtf8("식권 출력 중 입니다. 잠시만 기다려 주세요."));
        current_step = WAIT_COUPONPRINT;
        timingCount = 6;
        //
        break;
    case 21:
        ui->lbl_info->setText(trUtf8("처리 중 입니다 잠시만 기다려주세요."));
//        current_step = WAIT_COMPLETE;
        //
        break;
    case 22:
        ui->lbl_info->setText(trUtf8("모든 수납이 완료 되었습니다. 초기화면으로 전환됩니다."));
        current_step = DISPLAY_COMPLETE;
        timingCount = 1;
        //
        break;
    case 23:
        ui->lbl_info->setText(trUtf8("정보 인증을 요청중입니다...."));
        p_Main->assignTimer = 20;
        qDebug() << "SET WAIT ASSIGN - " << timingCount;
        current_step = WAIT_DBRECEIVE;
        if(p_Main->pLogin->assignInfo == 1){
            p_Main->assignTimer = 20;
        }
        break;
    case 24:
        ui->lbl_info->setText(trUtf8("중간 정산을 시작합니다."));
        current_step = MID_INOPEN;
        break;
    case 25:
        ui->lbl_info->setText(trUtf8("중간 정산이 완료 되었습니다."));
        current_step = WAIT_ADDCOMPLETE;
        break;
    case 27:
        ui->lbl_info->setText(trUtf8("카드를 인식부에 대 주세요."));
        p_Main->assignRfcard = 1;
        p_Main->cardRetryCount = 10;
        timingCount = 3;
        break;
    }
    if(ui->lbl_info_2->pixmap() != tmpPix){
        qDebug() << "image is different!! Changed!";
        ui->lbl_info_2->setPixmap(*tmpPix);
    }
    if(!loading_timer->isActive())  loading_timer->start(500);
}

int page_loading::deviceStateCheck(){
    int i = p_Main->stateFlag;
    if(p_Main->curDoorSen != chk_doorSenState() || p_Main->curKeySen != chk_KeySenState()){
        qDebug() << "Door Sen State changed: " << chk_doorSenState();
        qDebug() << "Key Sen State changed: " << chk_KeySenState();
        p_Main->curDoorSen = chk_doorSenState();
        p_Main->curKeySen = chk_KeySenState();
        i = 1;
    }
    // 알람프레임 Visible 여부로 알람 해제 및 발생 상태 확인가능
    if(p_Main->alarmState != p_Main->pRepair->isVisible()){       // Show 상태일 때
        p_Main->alarmState = p_Main->pRepair->isVisible();
        i = 1;
    }
    if(p_Main->rollState != p_Main->rollFlag){
        p_Main->rollFlag = p_Main->rollState;
        i = 1;
    }
    if(p_Main->bagState != p_Main->bagFlag){
        p_Main->bagFlag = p_Main->bagState;
        i = 1;
    }
    return i;
}


void page_loading::set_timeout(){

    if(p_Main->initializeFlag == 1){            //CHKDEV 모드로 들어간 후 - 초기구동 또는 재연결
        if(getCurrentMode() == 0){              // 모드 IDLE인 상태에서만
            QString tmpStr= p_Main->currentCountry;
            tmpStr.append("00");
            setDepositCountry(tmpStr.toUtf8().data());

            p_Main->get_NotProcessedData();             // 미처리 목록 받아오기 , 재연결 또는 재구동 시 호출
            p_Main->set_visible_systemlog(0);
            p_Main->initializeFlag = 2;

            if(FINGERTYPE == 0){
                if(finConnectState()){
                    qDebug() << "지문 로드하자~!!!";
                    cmd_fingerLoadDB();
                }
            }
        }
    }

    if(p_Main->btn_keyTest == 1){
        p_Main->btn_keyTimer++;
        if(p_Main->btn_keyTimer == 3){
            p_Main->btn_keyTimer = 0;
            p_Main->btn_keyTest = 0;
            p_Main->clearFocus();
            p_Main->setAudioMedia(QUrl::fromLocalFile(QString("%1audio/c1.mp3").arg(PATH_ROOT)),1,15,true);
            p_Main->pMsgDialog->show();
            p_Main->pMsgDialog->init_MessageDialog(7);
        }
    }

    if(p_Main->pMsgDialog->isHidden()){
        if(!p_Main->isEnabled()){
            p_Main->setEnabled(true);
        }
    }

    // 기기상태체크
    p_Main->stateFlag = deviceStateCheck();


    p_Main->set_currentTime();              // 시간표시

    if(get_InputState()){
        if(ui->btn_accuTotal->isEnabled()){
            ui->btn_accuTotal->setEnabled(false);
        }
        if(p_Main->pResult->btnState){
            p_Main->pResult->set_resultButton(0);
        }
    }
    else{
        if(!ui->btn_accuTotal->isEnabled()){
            ui->btn_accuTotal->setEnabled(true);
        }
        if(p_Main->pResult->btnState == 0){
            p_Main->pResult->set_resultButton(1);
        }
    }


    if(p_Main->rollState > 0){
        if(p_Main->rollReqCnt < 0){
            GetPrinterStatus(p_Main->printerlib, 1000);
            p_Main->rollReqCnt = 6;
        }
        else{
            p_Main->rollReqCnt -= 1;
        }
    }


    if(vaultGetLastError() > 0){
        qDebug() << "에러 체크: " <<vaultGetLastError();
        p_Main->curAlarm[0][3] = QString(trUtf8("동작중 잼에러 체크 : %1")).arg(current_step);
        p_Main->pRepair->init_repairPage(vaultGetLastError());
    }

    // RFID ASSIGN TIMER-------------------------------------------------------------
    if(p_Main->assignRfcard == 1){
        if(timingCount <= 0){
            p_Main->cardRetryCount = p_Main->on_rfid_connect(p_Main->cardRetryCount);
            qDebug() << p_Main->cardRetryCount;
            if(p_Main->cardRetryCount == 0){
                p_Main->assignRfcard = 0;
                qDebug() << "retry rf end.";
                current_step = IDLE_DEPOSIT;
                p_Main->select_Page(LOGINPAGE);
//                p_Main->pMsgDialog->init_MessageDialog(13);
            }
            timingCount = 2;
        }
        else{
            timingCount  -= 1;
        }
    }


    // UPS 메시지 출력
    if(p_Main->upsOpFlag != get_upsStatus()){
        if(get_upsStatus() == 1){
            emit p_Main->setSysMessage(trUtf8("시스템 전원이 불안정합니다."));
            p_Main->upsOpFlag = get_upsStatus();
        }
        else if(get_upsStatus() == 0){
            emit p_Main->setSysMessage("");
            p_Main->upsOpFlag = get_upsStatus();
        }
    }


    //    qDebug() << "coupon debug" << p_Main->printCount << p_Main->couponCount;
    //    current_step = get_currentOperationStep();                                      // SDK에서 현재 스텝을 읽어 옴
    //    qDebug() << preState_step << current_step;
    //    switch(pageMode){
    ////------------------------------------------------split-----------------------------------------------------------
    //    case 0:
//    qDebug() << "현재 스탭 보기" << current_step;
    switch (current_step){
    case IDLE_DEPOSIT:
        if(!p_Main->pResult->btnState){
            p_Main->pResult->set_resultButton(1);
        }
        break;
    case WAIT_INPUT:
        if(get_InputState()){
            if(timingCount  < 0){
                cmd_CountStartAct();
                current_step = WAIT_STARTCOUNT;
                timingCount = 5;
            }
            else{
                timingCount  -= 1;
            }
        }
        break;
    case WAIT_STARTCOUNT:
        if(get_CountState()){
            p_Main->setAudioMedia(QUrl(""),1,15,true);
            p_Main->pLoading->init_loadingPage(9);      // GO WAIT_COUNTING
            p_Main->select_Page(LOADINGPAGE);
        }
        else{
            if(timingCount < 0){
                current_step = WAIT_INPUT;
            }
            else{
                timingCount -= 1;
            }
        }
        break;
    case WAIT_COUNTING:
        if(get_CountComplete()){
            qDebug() << "COUNT COMPLETE!!";
            timingCount = 3;
            current_step = CHECK_REJECT;
        }
        else{
            if(timingCount < 0){   // 계수 완료가 한참지나도 안올라오면  Error Check 해봄
                current_step = WAIT_INPUT;
            }
            else{
                timingCount -= 1;
            }
        }
        // 계수완료 이벤트 받아서 스탭처리
        break;
    case CHECK_REJECT:
        if(chk_rejectPocket()){
            qDebug() << "check reject!!";
            p_Main->pLoading->init_loadingPage(10);
            p_Main->select_Page(LOADINGPAGE);
        }
        else{
            if(timingCount < 0){
                qDebug() << "reject clear!!";
                current_step = IDLE_DEPOSIT;
                get_LastCountData();
            }
            else{
                timingCount  -= 1;                  // 리젝트 인식을 2초 대기
            }
        }
        break;
    case WAIT_REJECT:
        if(!chk_rejectPocket()){
            qDebug() << "reject exist clear!!";
            current_step = IDLE_DEPOSIT;
            get_LastCountData();
        }
        else{
            // 한참지나도 안바뀌면 버튼 넣어야함
        }
        break;
    case DISPLAY_COMPLETE:
        if(timingCount == 0){
            p_Main->select_Page(LOGINPAGE);
            if(p_Main->totalBagCount > p_Main->compareCount){
                current_step = BAG_TWIST;
                p_Main->compareCount = (p_Main->totalBagCount/TWISTPERIOD + 1) * TWISTPERIOD;
                cmd_twistSeal();
                timingCount = 10;
            }
            else{
                preState_step = current_step;
                current_step = IDLE_DEPOSIT;
            }
        }
        timingCount  -= 1;
        break;
    case WAIT_SETTLECOMPLETE:
        if(!chk_doorSenState()){
            init_loadingPage(6);
        }
        // settlement event 에서 스텝 변경 해줘야 함.
        break;
    case WAIT_TRDOOROPEN:
        if(chk_KeySenState()){
            init_loadingPage(13);
        }
        else{
            p_Main->setAudioMedia(QUrl::fromLocalFile(QString("%1audio/c3.mp3").arg(PATH_ROOT)),1,15,true);
            p_Main->pTransCash->init_transpage(0);      // flag = 0  봉투를 회수해 주십시오
            p_Main->select_Page(TRANSPAGE);
            preState_step = current_step;
            current_step = IDLE_DEPOSIT;
        }
        break;
    case CHECK_TRDOOROPEN:
        break;
    case WAIT_TRDOORCLOSE:
        if(timingCount == 0){
            init_loadingPage(18);
            transLockDoorGate();
        }
        else{
            timingCount  -= 1;
        }
        break;
    case CHECK_TRDOORLOCK:
        if(timingCount == 0){
            if(chk_doorSenState()) {     // door sensor  1 = 도어 락 상태
                if(!chk_KeySenState()){
                    init_loadingPage(17);
                }
                else{

                    init_loadingPage(8);
                }
            }
            else{
                init_loadingPage(16);
                current_step = WAIT_TRDOORCLOSE;
                // 도어 다시 열었거나, 동작에러
            }
        }
        else{
            timingCount  -= 1;
        }
        break;
    case DISPLAY_TRCOMPLETE:
        if(timingCount == 0){
            p_Main->select_Page(LOGINPAGE);
            preState_step = current_step;
            current_step = IDLE_DEPOSIT;
        }
        timingCount  -= 1;
        break;
    case DISPLAY_CANCELCOMPLETE:
        if(timingCount == 0){
            p_Main->select_Page(LOGINPAGE);
            preState_step = current_step;
            current_step = IDLE_DEPOSIT;
        }
        timingCount  -= 1;
        break;
    case ERROR_CHECK:
        break;
    case WAIT_FINGERPRINT:
        qDebug() << "check finger state: " << chk_fingerState();
        switch(chk_fingerState()) {
        case 0:
            if(timingCount < 0){
                timingCount = 10;
                cmd_fingerScanStart();
            }
            else {
                timingCount -= 1;
            }
            break;
        case 1:
        case 2:     // Same
        case 3:
        case 6:     // Enroll Failed
        case 8:     // Iden Failed
        case 9:     // flag Failed
            current_step = IDLE_DEPOSIT;
            p_Main->pMsgDialog->init_MessageDialog(0);
            break;
        case 4:
            current_step = IDLE_DEPOSIT;
            p_Main->pMsgDialog->init_MessageDialog(8);
            break;
        case 5:
            current_step = IDLE_DEPOSIT;
            p_Main->pMsgDialog->init_MessageDialog(10);
            break;
        case 7:
            p_Main->pLoading->preState_step = p_Main->pLoading->current_step;
            current_step = IDLE_DEPOSIT;
            p_Main->select_Page(MENUPAGE);
            break;
        case 99:
            if(timingCount < 0){
                timingCount = 50;
            }
            break;
        }
        break;
    case WAIT_COUPONPRINT:
        if(p_Main->printCount >= p_Main->couponCount){          // 식권 출력 완료 시
            qDebug() << "coupon print complete!!";
            current_step = COUPONPRINT_COMPLETE;
            preState_step = current_step;
        }
        break;
    case COUPONPRINT_COMPLETE:
        break;
    case WAIT_COMPLETE:
        break;
    case WAIT_DBRECEIVE:
        if(p_Main->assignFlag != 0){
            qDebug() << "GET ASSIGN - " << timingCount;
            current_step = IDLE_DEPOSIT;
            p_Main->pMsgDialog->setAssignResult();
            p_Main->setEnabled(true);
        }
        else{
            qDebug() << "WAIT ASSIGN - " << timingCount;
            if(p_Main->assignTimer < 0){
                p_Main->pMsgDialog->init_MessageDialog(17);
                p_Main->cardRetryCount = 0;
                p_Main->pLogin->assignInfo = 0;     // 인증 플래그 0으로
                p_Main->eventFlag = 0;                      // Request DB data
                current_step = IDLE_DEPOSIT;
                p_Main->setEnabled(true);
            }
            else{
                p_Main->assignTimer -= 1;
            }
        }
        // 로그인 인증을 기다립니다.
        break;
    case WAIT_ADDDEPOSIT:
        break;
    case WAIT_ADDCOMPLETE:
        if(get_CountState()){
            qDebug() << "계수중입니다!!";
            p_Main->pLoading->init_loadingPage(9);      // GO WAIT_COUNTING
            p_Main->select_Page(LOADINGPAGE);
        }
        else if(get_CountComplete()){
            timingCount = 2;
            cmd_RequestStop();
            current_step = CHECK_REJECT;
        }
        else{
            p_Main->select_Page(RESULTPAGE);
        }
        break;
    case WAIT_RFIDASSIGN:
//        if(timingCount <= 0){
//            p_Main->cardRetryCount = p_Main->on_rfid_connect(p_Main->cardRetryCount);
//            qDebug() << p_Main->cardRetryCount;
//            if(p_Main->cardRetryCount == 0){
//                qDebug() << "retry rf end.";
//                p_Main->pMsgDialog->init_MessageDialog(12);
//                current_step = IDLE_DEPOSIT;
//                p_Main->select_Page(LOGINPAGE);
//            }
//            timingCount = 6;
//        }
//        else{
//            timingCount  -= 1;
//        }
        break;
        // 동작에 대한 체크 스탭
    case ETC_OUTOPEN:
        if(get_outGateState() == GATE_OPENED){
            p_Main->pLoading->preState_step = p_Main->pLoading->current_step;
            current_step = IDLE_DEPOSIT;
            p_Main->pEtc->get_barcodeAck();
        }
        else if(get_outGateState() == GATE_FAILED){
            p_Main->pRepair->show();
        }
        break;
    case ETC_OUTCLOSE:
        if(get_outGateState() == GATE_CLOSED){
            p_Main->pLoading->preState_step = p_Main->pLoading->current_step;
            current_step = IDLE_DEPOSIT;
            p_Main->pEtc->complete_etc();
        }
        else if(get_outGateState() == GATE_FAILED){
            p_Main->pRepair->show();
        }
        break;
    case ETC_CANCELCLOSE:
        if(get_outGateState() == GATE_CLOSED){
            p_Main->pLoading->preState_step = p_Main->pLoading->current_step;
            current_step = IDLE_DEPOSIT;
            p_Main->pEtc->init_etcPage();
            p_Main->select_Page(LOGINPAGE);
        }
        else if(get_outGateState() == GATE_FAILED){
            p_Main->pRepair->show();
        }
        break;


    case CANCEL_OUTOPEN:
        if(get_outGateState() == GATE_OPENED){
            p_Main->pLoading->init_loadingPage(3);      // 처리대기 중
            p_Main->select_Page(LOADINGPAGE);               // 완료모드 진입이랑 순서바꿈
        }
        else if(get_outGateState() == GATE_FAILED){
            p_Main->curAlarm[0][3] = "취소 게이트열기 : CANCEL_OUTOPEN";
            p_Main->pRepair->init_repairPage(13);
        }
        break;
    case CANCEL_WAITRETURN:           //  금액 회수 대기
        if(!chk_outPocket()){
            if(timingCount < 0){
                p_Main->pLoading->init_loadingPage(21);      // 처리대기 중
                p_Main->select_Page(LOADINGPAGE);               // 완료모드 진입이랑 순서바꿈
                p_Main->pLoading->current_step = CANCEL_OUTCLOSE;
                vaultCloseGate();
//                p_Main->setAudioMedia(QUrl::fromLocalFile(PATH_ROOT+"audio/처리중.mp3"),1,15,true);
            }
            else{
                timingCount  -= 1;                  // 회수 완료 후 2초대기
            }
        }
        else{
        }
        break;
    case CANCEL_OUTCLOSE:
        if(get_outGateState() == GATE_CLOSED){
//            p_Main->setAudioMedia(QUrl::fromLocalFile(PATH_ROOT+"audio/작업취소.mp3"),1,15,true);
            p_Main->pLoading->init_loadingPage(19);
            p_Main->select_Page(LOADINGPAGE);
        }
        else if(get_outGateState() == GATE_FAILED){
            p_Main->curAlarm[0][3] = "취소 게이트닫기 : CANCEL_OUTCLOSE";
            p_Main->pRepair->init_repairPage(12);
        }
        break;
        //----------------------------입금완료-------------------------------
//    case COMP_BLOPEN:
//        if(get_blGateState() == GATE_OPENED){
//            vaultOpenAcceptorGate();
//            p_Main->pLoading->current_step = COMP_INOPEN;
//        }
//        else if(get_blGateState() == GATE_FAILED){
//            p_Main->pRepair->show();
//        }
//        break;
    case COMP_INOPEN:
        if(get_inGateState() == GATE_OPENED){
            OnStatusEvent(STATUS_ENABLEDATABASE);           // DB 저장을함
            current_step = COMP_CHKPOC;
            timingCount = 3;
        }
        else if(get_inGateState() == GATE_FAILED){
            p_Main->curAlarm[0][3] = "완료 인게이트열기 : COMP_INOPEN";
            p_Main->pRepair->init_repairPage(14);
        }
        break;
    case COMP_CHKPOC:
        if(!chk_outPocket()){
            if(timingCount < 0){
                current_step = COMP_INCLOSE;
                vaultCloseAcceptorGate();
            }
            else{
                timingCount  -= 1;                  // 회수 완료 후 2초대기
            }
        }
        else{
        }
        break;
    case COMP_INCLOSE:
        if(get_inGateState() == GATE_CLOSED){
            // BLOCK STATE
//            p_Main->pLoading->current_step = COMP_BLCLOSE;
//            vaultCloseBlock();
            p_Main->pLoading->init_loadingPage(4);      // 처리대기 중
            p_Main->select_Page(LOADINGPAGE);               // 완료모드 진입이랑 순서바꿈
        }
        else if(get_inGateState() == GATE_FAILED){
            p_Main->curAlarm[0][3] = "완료 인게이트닫기 : COMP_INCLOSE";
            p_Main->pRepair->init_repairPage(11);
        }
        break;
    case BAG_TWIST:
        if(get_sealGateState() == GATE_TWISTED){
            current_step = IDLE_DEPOSIT;
        }
        else{
            if(timingCount < 0){
//                cmd_twistSeal();
                current_step = IDLE_DEPOSIT;
            }
            else{
                timingCount  -= 1;                  // 회수 완료 후 2초대기
            }
        }
        break;

//    case COMP_BLCLOSE:
//        if(get_blGateState() == GATE_CLOSED){
//            p_Main->pLoading->init_loadingPage(4);      // 처리대기 중
//            p_Main->select_Page(LOADINGPAGE);               // 완료모드 진입이랑 순서바꿈
//        }
//        else if(get_blGateState() == GATE_FAILED){
//            p_Main->pRepair->show();
//        }
//        break;
        //----------------------------중간정산-------------------------------
//    case MID_BLOPEN:
//        if(get_blGateState() == GATE_OPENED){
//            vaultOpenAcceptorGate();
//            p_Main->pLoading->current_step = MID_INOPEN;
//        }
//        else if(get_blGateState() == GATE_FAILED){
//            p_Main->pRepair->show();
//        }
//        break;
    case MID_INOPEN:
        if(get_inGateState() == GATE_OPENED){
            OnStatusEvent(STATUS_ADDCOMPLETE);           // DB 저장을함
            p_Main->pLoading->current_step = MID_CHKPOC;
            timingCount = 3;
        }
        else if(get_inGateState() == GATE_FAILED){
            p_Main->curAlarm[0][3] = "중간 인게이트열기 : MID_INOPEN";
            p_Main->pRepair->init_repairPage(14);
        }
        break;
    case MID_CHKPOC:
        if(!chk_outPocket()){
            if(timingCount < 0){
                p_Main->pLoading->current_step = MID_INCLOSE;
                vaultCloseAcceptorGate();
            }
            else{
                timingCount  -= 1;                  // 회수 완료 후 2초대기
            }
        }
        else{
        }
        break;
    case MID_INCLOSE:
        if(get_inGateState() == GATE_CLOSED){
            // BLOCK STATE
//            p_Main->pLoading->current_step = MID_BLCLOSE;
//            vaultCloseBlock();
            if(p_Main->currentDepositMode==2){
                p_Main->pResult->set_resultButton(1);
                p_Main->pLoading->timingCount = 0;
                p_Main->pLoading->current_step = WAIT_INPUT;
            }
            else{
                p_Main->pLoading->init_loadingPage(25);
            }
//            p_Main->pLoading->init_loadingPage(0);      // 처리대기 중
//            p_Main->select_Page(RESULTPAGE);               // 완료모드 진입이랑 순서바꿈
        }
        else if(get_inGateState() == GATE_FAILED){
            p_Main->curAlarm[0][3] = "완료 인게이트닫기 : MID_INCLOSE";
            p_Main->pRepair->init_repairPage(11);
        }
        break;
//    case MID_BLCLOSE:
//        if(get_blGateState() == GATE_CLOSED){
//            p_Main->pLoading->init_loadingPage(0);      // 처리대기 중
//            p_Main->select_Page(RESULTPAGE);               // 완료모드 진입이랑 순서바꿈
//        }
//        else if(get_blGateState() == GATE_FAILED){
//            p_Main->pRepair->show();
//        }
//        break;

//-----------------------벌크모드 정산준비-------------------------
//    case CAL_BLOPEN:
//        if(get_blGateState() == GATE_OPENED){
//            vaultOpenAcceptorGate();
//            p_Main->pLoading->current_step = CAL_INOPEN;
//        }
//        else if(get_blGateState() == GATE_FAILED){
//            p_Main->pRepair->show();
//        }
//        break;
    case CAL_INOPEN:
        if(get_inGateState() == GATE_OPENED){
            p_Main->pLoading->init_loadingPage(0);      // 처리대기 중
            p_Main->select_Page(LOADINGPAGE);           // 완료모드 진입이랑 순서바꿈
        }
        else if(get_inGateState() == GATE_FAILED){
            p_Main->curAlarm[0][3] = "정산 인게이트열기 : CAL_INOPEN";
            p_Main->pRepair->init_repairPage(14);
        }
        break;
    case CAL_CHKPOC:
        if(!chk_outPocket()){
            if(timingCount < 0){
                p_Main->pLoading->current_step = CAL_INCLOSE;
                vaultCloseAcceptorGate();
            }
            else{
                timingCount  -= 1;                  // 회수 완료 후 2초대기
            }
        }
        else{
        }
        break;
    case CAL_INCLOSE:
        if(get_inGateState() == GATE_CLOSED){
            // BLOCK STATE
//            p_Main->pLoading->current_step = CAL_BLCLOSE;
//            vaultCloseBlock();
            p_Main->pLoading->init_loadingPage(22);      // 처리대기 중
            p_Main->select_Page(LOADINGPAGE);               // 완료모드 진입이랑 순서바꿈
        }
        else if(get_inGateState() == GATE_FAILED){
            p_Main->curAlarm[0][3] = "정산 인게이트닫기 : CAL_INCLOSE";
            p_Main->pRepair->init_repairPage(11);
        }
        break;
//    case CAL_BLCLOSE:
//        if(get_blGateState() == GATE_CLOSED){
//            p_Main->pLoading->init_loadingPage(22);      // 처리대기 중
//            p_Main->select_Page(LOADINGPAGE);               // 완료모드 진입이랑 순서바꿈
//        }
//        else if(get_blGateState() == GATE_FAILED){
//            p_Main->pRepair->show();
//        }
//        break;
// 대기 상태 스탭
    case WAIT_INCLOSE:
        if(get_inGateState() == GATE_CLOSED){
            current_step = preState_step;
            p_Main->pMsgDialog->hide();
        }
        else if(get_inGateState() == GATE_OPENED){
            current_step = preState_step;
            p_Main->pMsgDialog->hide();
            p_Main->curAlarm[0][3] = "대기 인게이트닫기 : WAIT_INCLOSE";
            p_Main->pRepair->init_repairPage(11);
        }
        break;
    case WAIT_OUTCLOSE:
        if(get_outGateState() == GATE_CLOSED){
            current_step = preState_step;
            p_Main->pMsgDialog->hide();
        }
        else if(get_outGateState() == GATE_OPENED){
            current_step = preState_step;
            p_Main->pMsgDialog->hide();
            p_Main->curAlarm[0][3] = "대기 게이트닫기 : WAIT_OUTCLOSE";
            p_Main->pRepair->init_repairPage(12);
        }
        break;
    case WAIT_CLEARJAM:
        if(vaultGetLastError() == 0){
            current_step = preState_step;
            p_Main->pMsgDialog->hide();
        }
        else{
            if(timingCount < 0){
                current_step = preState_step;
                p_Main->pMsgDialog->hide();
                p_Main->curAlarm[0][3] = "대기 잼클리어 : WAIT_CLEARJAM";
                p_Main->pRepair->init_repairPage(vaultGetLastError());
            }
            timingCount  -= 1;                  // 회수 완료 후 2초대기
        }
        break;
    case AUTO_DOORLOCK:
        if(chk_doorSenState() == 1){
            current_step = preState_step;
            p_Main->pMsgDialog->hide();
        }
        else{
            if(timingCount < 0){
                current_step = preState_step;
                p_Main->pMsgDialog->hide();
                p_Main->curAlarm[0][3] = "초기동작시작";
                p_Main->pRepair->init_repairPage(15);
            }
            timingCount  -= 1;                  // 회수 완료 후 2초대기
        }
        break;
    default:
        break;
    }

//        break;
//------------------------------------------------split-----------------------------------------------------------
//    case 1:
//        if(timingCount == 0){
//            // SDK - 감지 센서에 값 확인 없으면 결과페이지로
//            p_Main->select_Page(RESULTPAGE);
//        }
//        break;
////------------------------------------------------split-----------------------------------------------------------
//    case 2:
//        if(timingCount == 0){
//            // SDK - 감지 센서에 값 확인 없으면 결과페이지로
//            p_Main->select_Page(RESULTPAGE);
//        }
//        break;
////------------------------------------------------split-----------------------------------------------------------
//    case 3:
//        break;
//    case 4:
//        break;
//    case 5:
//        break;
//    }
}

void page_loading::depositFrameVisible(bool checked){
}


void page_loading::OnStatusEvent(int status)
{
    qDebug() << "HOOK" << status;
    switch(status){
    case STATUS_ENABLEDATABASE:
    {
        if(!p_Main->pDatabase->db.isOpen()){
            p_Main->pDatabase->db.open();
        }

        QString curTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

        QString values;
        values.append(QString("'%1',").arg(curTime));  // later 시간비교할수도있음,일단은 장비ID랑 입금번호 로 구별
        values.append(QString("'%1',").arg(p_Main->deviceNum));
        values.append(QString("'%1',").arg(p_Main->turnNum));
        values.append(QString("'%1',").arg(p_Main->depositNum));
        values.append("'escrow',");
        values.append(QString("'%1',").arg(p_Main->curCountryText));
        values.append(QString("'%1',").arg(p_Main->pResult->curDeposit.sumTotal,0,'f',0));
        values.append(QString("'%1',").arg(p_Main->pResult->curDeposit.sumCount,0,'f',0));
        QString etcSum= "null";
        values.append(QString("'%1',").arg(etcSum));        // 기타입금
        values.append(QString("'%1',").arg(p_Main->curUserId));
        values.append("'0',");                                                         // process time 0 으로저장
        QString currencySum;
        for(int i=0; i<12; i++){
            currencySum.append(QString("%1-").arg(p_Main->pResult->curDeposit.currencySum[i],0,'f',0));
        }
        values.append(QString("'%1',").arg(currencySum));        // 입금 세부 기록
        values.append("'-',");                                    //tmp 1
        values.append("'-'");                                    //tmp 2

        p_Main->currentTransaction = values;


//        int exist = p_Main->pDatabase->SelectExist(QString("select primaryid from %1 where depositNum='%2'")
//                                                           .arg(RECORD_TABLE).arg(p_Main->depositNum));
//        if(exist == 1){
//            QString depositTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

//            qDebug() << buf.at(0) << "escrow add deposit data DB update: " << p_Main->pDatabase->UpdateRecord
//                        (RECORD_TABLE,QString("time='%1', totalAmount='%2', etcAmount='%3', log='%4'").arg(depositTime)
//                         .arg(p_Main->pResult->curDeposit.sumTotal+p_Main->pResult->preDeposit.sumTotal)
//                         .arg(etcSum).arg(currencySum),QString("depositNum='%1'").arg(p_Main->depositNum));
//        }
//        else{
            qDebug() << "escrow add deposit data DB insert: "
                     << p_Main->pDatabase->InsertRecord(RECORD_TABLE,p_Main->currentTransaction);
//        }

        p_Main->totalBagCount += p_Main->pResult->curDeposit.sumCount;
        p_Main->pDatabase->UpdateRecord(INFO_TABLE, QString("totalCount = '%1'").arg(p_Main->totalBagCount)
                                        ,"recName = 'deviceInfo'");


        if(p_Main->currentDepositMode != 3){
            p_Main->transactionList.insert(QString("%1").arg(curTime),p_Main->currentTransaction.remove("'"));
        }

        p_Main->pResult->init_DepositStr();
        p_Main->pResult->init_curDepositStr();
        p_Main->currentTransaction = "";
        p_Main->depositNum++;
        break;
    }
    case STATUS_ADDCOMPLETE:
    {
        if(!p_Main->pDatabase->db.isOpen()){
            p_Main->pDatabase->db.open();
        }

        QString curTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

        QString values;
        values.append(QString("'%1',").arg(curTime));  // later 시간비교할수도있음,일단은 장비ID랑 입금번호 로 구별
        values.append(QString("'%1',").arg(p_Main->deviceNum));
        values.append(QString("'%1',").arg(p_Main->turnNum));
        values.append(QString("'%1',").arg(p_Main->depositNum));
        values.append("'escrow',");
        values.append(QString("'%1',").arg(p_Main->curCountryText));
        values.append(QString("'%1',").arg(p_Main->pResult->curDeposit.sumTotal,0,'f',0));
        values.append(QString("'%1',").arg(p_Main->pResult->curDeposit.sumCount,0,'f',0));
        QString etcSum= "null";
        values.append(QString("'%1',").arg(etcSum));        // 기타입금
        values.append(QString("'%1',").arg(p_Main->curUserId));         // userid is primary id
        values.append("'0',");                                                         // process time 0 으로저장

        QString currencySum;
        for(int i=0; i<12; i++){
            currencySum.append(QString("%1-").arg(p_Main->pResult->curDeposit.currencySum[i],0,'f',0));
        }
        values.append(QString("'%1',").arg(currencySum));        // 입금 세부 기록

        values.append("'-',");                                    //tmp 1
        values.append("'-'");                                    //tmp 2

        p_Main->currentTransaction = values;

//        QVariantList buf;
//        if(p_Main->pDatabase->SelectRecord(RECORD_TABLE,"primaryid",QString("depositNum ='%1'").arg(p_Main->depositNum),&buf)==0){
//            // 레코드 존재확인
            qDebug() << "escrow add deposit data DB insert: " << p_Main->pDatabase->InsertRecord(RECORD_TABLE,p_Main->currentTransaction);
//        }
//        else{
//            QString depositTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

//            qDebug() << buf.at(0) << "escrow add deposit data DB update: " << p_Main->pDatabase->UpdateRecord
//                        (RECORD_TABLE,QString("time='%1', totalAmount='%2', etcAmount='%3', log='%4'").arg(depositTime)
//                         .arg(p_Main->pResult->curDeposit.sumTotal+p_Main->pResult->curDeposit.curTotal).arg(etcSum).arg(currencySum),QString("depositNum='%1'").arg(p_Main->depositNum));
//        }

        p_Main->totalBagCount += p_Main->pResult->curDeposit.sumCount;
        p_Main->pDatabase->UpdateRecord(INFO_TABLE, QString("totalCount = '%1'").arg(p_Main->totalBagCount)
                                        ,"recName = 'deviceInfo'");


        p_Main->transactionList.insert(QString("%1").arg(curTime),p_Main->currentTransaction.remove("'"));

        // preDeposit 누적
        p_Main->pResult->preDeposit.sumCount += p_Main->pResult->curDeposit.sumCount;
        p_Main->pResult->preDeposit.sumTotal += p_Main->pResult->curDeposit.sumTotal;

        for(int i=0; i<12; i++){
            p_Main->pResult->preDeposit.currencyCount[i] += p_Main->pResult->curDeposit.currencyCount[i];
            p_Main->pResult->preDeposit.currencySum[i] += p_Main->pResult->curDeposit.currencySum[i];
        }
        p_Main->pResult->init_curDepositStr();
        break;
    }
    case STATUS_ACKBARCODE:
    {
        char test[30];
        int len=0;
        get_barcodeData(test, &len);
        if(p_Main->currentDepositMode == ETCDEPOSIT){
            p_Main->curBarCode = QString::fromLocal8Bit(test,len);
//            qDebug() << tmpStr;
            p_Main->pLoading->current_step = ETC_OUTOPEN;
        }
        else{
//            p_Main->setAudioMedia(QUrl::fromLocalFile(QString("%1audio/c4.mp3").arg(PATH_ROOT)),1,15,true);
            p_Main->curBagBarcode = QString::fromLocal8Bit(test,len);
            p_Main->pTransCash->init_transpage(2);      // flag = 0  봉투를 회수해 주십시오
            p_Main->select_Page(TRANSPAGE);
        }
        break;
    }
    case OUTGATE_EMERGENCY:
//        p_Main->pLoading->current_step = WAIT_OUTRETRY;
        break;
    }
}

void page_loading::lang_change()
{
    ui->retranslateUi(this);
}


void page_loading::on_btn_accuTotal_clicked()
{
    cmd_RequestStop();
    p_Main->pLoading->init_loadingPage(21);
    p_Main->select_Page(LOADINGPAGE);
    p_Main->pLoading->current_step = COMP_INCLOSE;
    vaultCloseAcceptorGate();
}
