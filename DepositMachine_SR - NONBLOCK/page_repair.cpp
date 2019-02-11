#include "page_repair.h"
#include "ui_page_repair.h"
#include "global.h"

page_repair::page_repair(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::page_repair)
{
    ui->setupUi(this);
    ui->frm_manageMode->hide();

    repair_timer = new QTimer(this);
    connect(repair_timer , SIGNAL(timeout()), this, SLOT(set_timeout()));
    qDebug() << "로딩타이머 시작";
    retryCnt = -1;
    currentErrCode=0;

    pixjam1    = new QPixmap("://image/1-1.png");
    pixjam2     = new QPixmap("://image/1-2.png");
    pixjam3       = new QPixmap("://image/2-1.png");
    pixjam4      = new QPixmap("://image/2-2.png");
}

void page_repair::set_timeout(){

    switch(currentErrCode){
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 9:
        if(vaultGetLastError() == 0){
            cmd_RequestStop();
            if(chk_outPocket()){
                if(get_outGateState() == GATE_CLOSED){
                    ui->lbl_status->setText(trUtf8("포켓을 열어주세요."));
                }
                else{
                    ui->lbl_status->setText(trUtf8("지폐를 꺼내주세요."));
                }
            }
            else if(get_outGateState() == GATE_OPENED){
                ui->lbl_status->setText(trUtf8("포켓을 닫아주세요."));
            }
            else{
                p_Main->pResult->curDeposit.sumTotal = 0;
                if(p_Main->deviceType == "마감정산기"){
                    if(p_Main->pLoading->depositFlag == 0){
                        p_Main->select_Page(LOGINPAGE);
                    }
                    else{
                        p_Main->select_Page(RESULTPAGE);
                    }
//                      p_Main->pResult->preDeposit.sumTotal = 0;
                }
                else{
                    if(p_Main->pResult->preDeposit.sumTotal > 0){          // 이전에 입금된게 있으면.
                        p_Main->pResult->init_curDepositStr();
                        p_Main->select_Page(RESULTPAGE);
                    }
                    else{
                        p_Main->pLoading->depositFlag = 0;
                        p_Main->setAudioMedia(QUrl::fromLocalFile(QString("%1audio/현금투입.mp3").arg(PATH_ROOT)),1,15,true);
                        p_Main->pLoading->init_loadingPage(1);
                        p_Main->select_Page(LOADINGPAGE);
                    }
                }
                this->hide();
                repair_timer->stop();
            }
//            p_Main->alarmState = 0;         // ALARM CLEAR
        }
        else{
            clearErrorState();
            if(ui->lbl_image->pixmap() == pixjam1){
                ui->lbl_image->setPixmap(*pixjam1);
                qDebug() << "1이랑 같음" << retryCnt;
            }
            else if(ui->lbl_image->pixmap() == pixjam2){
                ui->lbl_image->setPixmap(*pixjam2);
                qDebug() << "2랑 같음" << retryCnt;
            }
            else if(ui->lbl_image->pixmap() == pixjam3){
                ui->lbl_image->setPixmap(*pixjam3);
                qDebug() << "3이랑 같음" << retryCnt;
            }
            else if(ui->lbl_image->pixmap() == pixjam4){
                ui->lbl_image->setPixmap(*pixjam4);
                qDebug() << "4랑 같음" << retryCnt;
            }
        }
        break;
    case 11:
        if(get_inGateState() == GATE_CLOSED){
            this->hide();
            p_Main->pMsgDialog->hide();
            repair_timer->stop();
        }
        else if(get_inGateState() == GATE_FAILED){
            ui->lbl_status->setText(trUtf8("에러가 해제되지 않았습니다.\n 다시 확인해 주십시오."));
            p_Main->pMsgDialog->hide();
            repair_timer->stop();
        }
        break;
    case 12:
        if(get_outGateState() == GATE_CLOSED){
            this->hide();
            p_Main->pMsgDialog->hide();
            repair_timer->stop();
        }
        else if(get_outGateState() == GATE_FAILED){
            ui->lbl_status->setText(trUtf8("에러가 해제되지 않았습니다.\n 다시 확인해 주십시오."));
            p_Main->pMsgDialog->hide();
            repair_timer->stop();
        }
        break;
    case 13:
        if(get_outGateState() == GATE_OPENED){
            this->hide();
            p_Main->pMsgDialog->hide();
            repair_timer->stop();
        }
        else if(get_outGateState() == GATE_FAILED){
            ui->lbl_status->setText(trUtf8("에러가 해제되지 않았습니다.\n 다시 확인해 주십시오."));
            p_Main->pMsgDialog->hide();
            repair_timer->stop();
        }
        break;
    case 14:
        if(get_inGateState() == GATE_OPENED){
            this->hide();
            p_Main->pMsgDialog->hide();
            repair_timer->stop();
        }
        else if(get_inGateState() == GATE_FAILED){
            ui->lbl_status->setText(trUtf8("에러가 해제되지 않았습니다.\n 다시 확인해 주십시오."));
            p_Main->pMsgDialog->hide();
            repair_timer->stop();
        }
        break;
    case 15:
        if(chk_doorSenState() == 1){
            this->hide();
            p_Main->pMsgDialog->hide();
            repair_timer->stop();
        }
        else{
            ui->lbl_status->setText(trUtf8("도어닫힘 확인 후 재시도 해 주세요."));
            p_Main->pMsgDialog->hide();
            repair_timer->stop();
        }
        break;
    default:
        if(vaultGetLastError() == 0){
            this->hide();
            repair_timer->stop();
        }
        break;
    }
}

void page_repair::init_repairPage(int alCode){
    p_Main->pLoading->preState_step = p_Main->pLoading->current_step;

    if(this->isVisible()){
        return;
    }
    else{
        this->show();
        this->activateWindow();
    }

    ui->frm_manageMode->hide();

    ui->frm_manageMode->show();

    ui->lbl_status->setText("");
    // later 에러별 정리
    ui->btn_openPocket->hide();
    ui->btn_closePocket->hide();
    ui->btn_clear->hide();
    ui->btn_retry->hide();
    ui->btn_reboot->hide();
    ui->btn_result->hide();
    ui->btn_main->hide();
    currentErrCode = alCode;
    retryCnt = -1;
    switch(alCode){
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 9:
        ui->frm_manageMode->show();
        ui->btn_clear->show();
        ui->btn_openPocket->show();
        ui->btn_closePocket->show();
        ui->lbl_image->setPixmap(*pixjam1);
        ui->lbl_alarm->setText(QString(trUtf8("ALARM No.JAM(%1) - \nJAM에러가 발생하였습니다.")).arg(alCode));
        ui->lbl_alText->setText(trUtf8("관리자 호출이 필요합니다."
                                "1. 기기 상단부를 열고. 화면에 안내에따라 해당 부위에 발생한 JAM을 제거하여 주십시오.\n"
                                "2. JAM 제거 후 \"CLEAR\" 버튼을 클릭하여 장비의 상태를 클리어 시켜 주십시오."));
        repair_timer->start(2000);
        break;
//    case 8:
//        ui->lbl_alarm->setText("ERR 8");
//        qDebug() << "ERROR NUM = 8";
//        break;
    case 10:
        ui->btn_openPocket->show();
        ui->btn_closePocket->show();
        ui->lbl_image->setPixmap(*p_Main->pLoading->pixReturn);
        ui->lbl_alarm->setText(trUtf8("ALARM No.10 - 포켓안에 지폐가 남아있습니다."));
        ui->lbl_alText->setText(trUtf8("1. \"포켓열기\" 버튼을 클릭하여 포켓 도어를 열고, 남아있는 지폐를 꺼냅니다. \n"
                                "2. \"포켓닫기\" 버튼을 클릭하여 포켓 도어를 닫고, \"확인\" 을 클릭합니다."));
        break;
    case 11:
        // 입금이든 취소든 완료 된 상태 현재 트랜잭션 데이터 존재여부 확인 후 입금처리
        ui->lbl_image->setPixmap(*p_Main->pLoading->pixLogo);
        ui->btn_retry->show();
        ui->lbl_alarm->setText(trUtf8("ALARM No.11 - 대기모드 전환실패."));
        ui->lbl_alText->setText(trUtf8("1. \"커맨드 재시도\" 버튼을 클릭하여 재시도 합니다. \n"
                                       "2. 재시도 실패시 기기를 다시 부팅합니다."));
        break;
    case 12:
        ui->lbl_image->setPixmap(*p_Main->pLoading->pixLogo);
        ui->btn_retry->show();
        ui->lbl_alarm->setText(trUtf8("ALARM No.12 - 대기모드 전환실패."));
        ui->lbl_alText->setText(trUtf8("1. \"커맨드 재시도\" 버튼을 클릭하여 재시도 합니다. \n"
                                       "2. 재시도 실패시 기기를 다시 부팅합니다."));
//        ui->lbl_alarm->setText(trUtf8("ALARM No.12 - 통신오류 : 취소동작 중 게이트 열기에 실패하였습니다."));
//        ui->lbl_alText->setText(trUtf8("1. \"포켓열기\" 버튼을 클릭하여 포켓도어 열기를 시도하십시오. \n"
//                                       "2. 재시도 실패시 \"재부팅\" 버튼을 클릭하여 기기를 다시 부팅합니다."));
        break;
    case 13:
        ui->lbl_image->setPixmap(*p_Main->pLoading->pixLogo);
        ui->btn_retry->show();
        ui->lbl_alarm->setText(trUtf8("ALARM No.13 - 게이트 열기실패"));
        ui->lbl_alText->setText(trUtf8("1. \"커맨드 재시도\" 버튼을 클릭하여 재시도 합니다. \n"
                                       "2. 재시도 실패시 기기를 다시 부팅합니다."));
        break;
    case 14:
        ui->lbl_image->setPixmap(*p_Main->pLoading->pixLogo);
        ui->btn_retry->show();
        ui->lbl_alarm->setText(trUtf8("ALARM No.14 - 인게이트 열기실패"));
        ui->lbl_alText->setText(trUtf8("1. \"커맨드 재시도\" 버튼을 클릭하여 재시도 합니다. \n"
                                       "2. 재시도 실패시 기기를 다시 부팅합니다."));
        break;
    case 15:
        ui->lbl_image->setPixmap(*p_Main->pLoading->pixLogo);
        ui->btn_retry->show();
        ui->lbl_alarm->setText(trUtf8("ALARM No.15 - 도어 잠금장치가 점검이 필요합니다."));
        ui->lbl_alText->setText(trUtf8("1. 도어 닫힘 상태를 확인해주세요. \n"
                                       "2. \"커맨드 재시도\" 버튼을 클릭하여 재시도 합니다."));
        break;
    default:
        repair_timer->start(2000);
        ui->btn_clear->show();
        ui->lbl_alarm->setText(QString("ERR DEFAULT %1").arg(alCode));
        ui->lbl_alText->setText(trUtf8("정의되지 않은 에러 코드 입니다. \n"
                                       "버튼을 눌러 기기 상태를 체크하십시오.\n"));
        break;
    }

    if(currentErrCode == 0){
//        this->hide();
        return;
    }

    // Alarm Set
    p_Main->curAlarm[0][0] = currentErrCode;
    p_Main->curAlarm[0][1] = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    if(ui->lbl_alarm->text() == ""){
        ui->lbl_alarm->setText("-");
    }
    p_Main->curAlarm[0][2] = ui->lbl_alarm->text();
    p_Main->curAlarm[0][4] = p_Main->pResult->preDeposit.sumTotal;

    p_Main->pManagerMenu->occurred_alarm();
}

page_repair::~page_repair()
{
    delete ui;
}

void page_repair::on_btn_error_clicked()
{
    p_Main->pMsgDialog->init_MessageDialog(9);
}

void page_repair::set_ManageFrame()
{
    // later 비밀번호 인증 추가
    ui->frm_manageMode->show();
}

void page_repair::lang_change()
{
    ui->retranslateUi(this);
}

void page_repair::on_btn_openPocket_clicked()
{
    vaultOpenGate();
}

void page_repair::on_btn_closePocket_clicked()
{
    if(!chk_outPocket()){
        ui->lbl_status->setText("");

        if(currentErrCode == 10){
            ui->btn_main->show();
            ui->lbl_status->setText(trUtf8("\"확인\" 을 클릭합니다."));
        }
        vaultCloseGate();
    }
    else{
        ui->lbl_status->setText(trUtf8("포켓에 돈이 남아있습니다."));
    }

}

void page_repair::on_btn_main_clicked()
{
    ui->frm_manageMode->hide();
    this->hide();
    repair_timer->stop();
}

void page_repair::on_btn_clear_clicked()
{
    clearErrorState();
    retryCnt = 3;
}

void page_repair::on_btn_result_clicked()
{

}

void page_repair::on_btn_retry_clicked()
{
    switch(currentErrCode){
    case 11:
        vaultCloseAcceptorGate();
        p_Main->pMsgDialog->init_MessageDialog(20);
        repair_timer->start(2000);
        break;
    case 12:
        vaultCloseGate();
        p_Main->pMsgDialog->init_MessageDialog(20);
        repair_timer->start(2000);
        break;
    case 13:
        vaultOpenGate();
        p_Main->pMsgDialog->init_MessageDialog(20);
        repair_timer->start(2000);
        break;
    case 14:
        vaultOpenAcceptorGate();
        p_Main->pMsgDialog->init_MessageDialog(20);
        repair_timer->start(2000);
        break;
    case 15:
        transLockDoorGate();
        p_Main->pMsgDialog->init_MessageDialog(20);
        repair_timer->start(2000);
        break;
    }
}
