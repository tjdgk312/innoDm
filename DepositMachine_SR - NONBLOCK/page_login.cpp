#include "page_login.h"
#include "ui_page_login.h"
#include "global.h"

page_login::page_login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::page_login)
{
    ui->setupUi(this);
}

page_login::~page_login()
{
    delete ui;
}

/* http://notebook2share.blogspot.kr/2011/09/qt-convert-qstring-to-const-char-and.html */

void page_login::loginPage_init(){
    assignInfo = 0;
}

void page_login::lang_change()
{
    ui->retranslateUi(this);
}

//---------------------------------------------------------------------------------------------

void page_login::on_btn_IDAssign_clicked()
{
    if(check_idleStep() == 0){
        return;
    }

    assignInfo = 2;
    p_Main->pMsgDialog->init_MessageDialog(6);
}

void page_login::on_btn_fingerAssign_clicked()
{
//    cmd_fingerLoadDB();

    if(!finConnectState()){
        cmd_fingerConnect();
        p_Main->pMsgDialog->init_MessageDialog(11);
        return;
    }

    if(check_idleStep() == 0){
        return;
    }

    assignInfo = 1;

    if(FINGERTYPE == 0){
        p_Main->setAudioMedia(QUrl::fromLocalFile(QString("%1audio/f1.mp3").arg(PATH_ROOT)),1,15,true);
        p_Main->finCmdType = 1;         // 재시도 커맨드 구분을 위한 타입구분
        p_Main->select_Page(LOADINGPAGE);
        p_Main->pLoading->init_loadingPage(12);
        cmd_fingerIdenStart();
    }
    else{
        p_Main->setAudioMedia(QUrl::fromLocalFile(QString("%1audio/f1.mp3").arg(PATH_ROOT)),1,15,true);
        p_Main->finCmdType = 2;
        p_Main->select_Page(LOADINGPAGE);
        p_Main->pLoading->init_loadingPage(12);
        p_Main->pLoading->timingCount = 10;
        cmd_fingerScanStart();
    }
}

void page_login::on_btn_back_clicked()
{
    assignInfo = 0;
    p_Main->select_Page(EVENTVIEWPAGE);
}

void page_login::on_btn_CardAssign_clicked()
{
    if(check_idleStep() == 0){
        return;
    }

    p_Main->setAudioMedia(QUrl::fromLocalFile(QString("%1audio/r1.mp3").arg(PATH_ROOT)),1,15,true);
    p_Main->cardOpType = 0;
    p_Main->pSearch->setCurrentStackPage(1);
    p_Main->select_Page(SEARCHPAGE);

    p_Main->pLoading->init_loadingPage(27);         // 로딩스텝만 전환하고 화면은 서치페이지에서 표시

    assignInfo = 3;
}

int page_login::check_idleStep(){

//    if(get_fingerState() == 10){
//        p_Main->pMsgDialog->init_MessageDialog(25);
//    }

    p_Main->cardOpType = 0;
//    qDebug() << "도어센서값 출력: " << chk_doorSenState() << chk_KeySenState();
//    transLockDoorGate();
    clearErrorState();

    p_Main->pResult->init_curDepositStr();
    p_Main->pResult->init_DepositStr();

    if(get_upsStatus() == 1){
//        p_Main->pMsgDialog->init_MessageDialog(23);
//        return 0;
    }

    if(p_Main->printConnect == 0){
        if ( NULL == p_Main->printerlib || p_Main->printerlib == 0)
        {
        }
        else{
            woosimlib_close(p_Main->printerlib);
        }
        p_Main->printerInit();
        p_Main->pMsgDialog->init_MessageDialog(21);
        return 0;
    }

    if(chk_doorSenState() == 0){            // 도어 잠금장치 열림
        transLockDoorGate();
        p_Main->pLoading->preState_step = p_Main->pLoading->current_step;
        p_Main->pLoading->current_step = AUTO_DOORLOCK;
        p_Main->pLoading->timingCount = 8;
        p_Main->pMsgDialog->init_MessageDialog(20);
        return 0;
    }

    if(get_InputState()){            // 지폐가 남아있거나 센서체크오류
        p_Main->pMsgDialog->init_MessageDialog(22);
        return 0;
    }

    if(chk_outPocket()){            // 지폐가 남아있거나 센서체크오류
        p_Main->curAlarm[0][3] = "이벤트선택";
        p_Main->pRepair->init_repairPage(10);
        return 0;
    }


    if(get_inGateState() == GATE_OPENED){
        p_Main->pLoading->preState_step = p_Main->pLoading->current_step;
        p_Main->pLoading->current_step = WAIT_INCLOSE;
        vaultCloseAcceptorGate();
        p_Main->pMsgDialog->init_MessageDialog(20);
        return 0;
    }
    if(get_outGateState() == GATE_OPENED){
        p_Main->pLoading->preState_step = p_Main->pLoading->current_step;
        p_Main->pLoading->current_step = WAIT_OUTCLOSE;
        vaultCloseGate();
        p_Main->pMsgDialog->init_MessageDialog(20);
        return 0;
    }
    if(vaultGetLastError() != 0){
        p_Main->pLoading->preState_step = p_Main->pLoading->current_step;
        p_Main->pLoading->current_step = WAIT_CLEARJAM;
        p_Main->pLoading->timingCount = 6;
        clearErrorState();
        p_Main->pMsgDialog->init_MessageDialog(20);
        return 0;
    }

    if(p_Main->rollLeft < ROLLLEFT){
        GetPrinterStatus(p_Main->printerlib, 1000);
        p_Main->pMsgDialog->init_MessageDialog(19);
        return 0;
    }


    if(p_Main->capNum.toInt()-500 < p_Main->totalBagCount){
        p_Main->bagState = 1;
        p_Main->pMsgDialog->init_MessageDialog(16);
        return 0;
    }

    if(p_Main->initializeFlag == 0){        // 통신 끊김
        p_Main->pMsgDialog->init_MessageDialog(2);
        return 0;
    }
    if(p_Main->initializeFlag == 1){        // 통신 끊김
        p_Main->pMsgDialog->init_MessageDialog(4);
        return 0;
    }

    return 1;
}

void page_login::on_pushButton_clicked()
{
//    cmd_fingerEnrollStart();
}
