#include "page_transcash.h"
#include "ui_page_transcash.h"
#include "global.h"


page_transcash::page_transcash(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::page_transcash)
{
    ui->setupUi(this);
    ui->frm_receiptInput->hide();

    // set settlement hook event
    void* my_data_ptr;
    vaultSetSettlementEvents(&page_transcash::OnSettlementCompleteEvent,my_data_ptr);

    ui->lbl_info_2->setPixmap(*p_Main->pLoading->pixDoorOpen);
    ui->lbl_info->setPixmap(*p_Main->pLoading->pixDoorClose);
    ui->lbl_info_3->setPixmap(*p_Main->pLoading->pixBarcode);
}

void page_transcash::init_transpage(int flag){
    ui->stackedWidget->setCurrentIndex(flag);
}

void page_transcash::lang_change()
{
    ui->retranslateUi(this);
}

void page_transcash::OnSettlementCompleteEvent(LPVAULT_SETTLEMENT_INFO settlement_info_ptr, int status, void* user_data){
    qDebug() << "Called Settlement Complete Event" << status;

//    // 키를 돌려주세요 스탭시작
//    int etst = status;
//    if(status==VAULT_STATUS_ERROR){      // 에러발생
//        p_Main->pLoading->init_loadingPage(11);
//        p_Main->select_Page(LOADINGPAGE);
//    }
//    else{
//        p_Main->pLoading->current_step = WAIT_TRDOOROPEN;
//    }
}

page_transcash::~page_transcash()
{
    delete ui;
}

void page_transcash::on_btn_ReturnComp_clicked()
{
    p_Main->bagState = 0;

    QString curTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

    QString values;
    values.append(QString("'%1',").arg(curTime));  // later 시간비교할수도있음,일단은 장비ID랑 입금번호 로 구별
    values.append(QString("'%1',").arg(p_Main->deviceNum));
    values.append(QString("'%1',").arg(p_Main->turnNum));
    values.append("'TR',");           // 현송 입금번호
    values.append("'CIT',");            // 현송
    values.append("'9999',");
    values.append(QString("'%1',").arg(p_Main->curBagAmount));
    values.append(QString("'%1',").arg(p_Main->curBagCount));
    values.append("'null',");
    values.append("'null',");
    values.append("'null',");
    values.append("'null',");
    values.append("'null',");
    values.append("'null',");

    p_Main->currentTransaction = values;
//    qDebug() << "etc deposit data DB save: " << p_Main->pDatabase->InsertRecord(RECORD_TABLE,p_Main->currentTransaction);
    p_Main->transactionList.insert(QString("%1").arg(curTime),p_Main->currentTransaction.remove("'"));
    p_Main->currentTransaction = "";

    p_Main->pDatabase->DeleteRecord(RECORD_TABLE,"division!='CIT'");

    // 봉투회수가 되었으면 새 봉투의 바코드를 인식 시킨다. 로딩에서 커맨드 재시도
    cmd_barcodeScanStart();
    p_Main->setAudioMedia(QUrl::fromLocalFile(QString("%1audio/c4.mp3").arg(PATH_ROOT)),1,15,true);
    init_transpage(1);
}

void page_transcash::on_btn_ChangeComp_clicked()
{
    // 봉투 교체가 다 되었으면 문닫는 시퀀스 시작
    p_Main->setAudioMedia(QUrl::fromLocalFile(QString("%1audio/c5.mp3").arg(PATH_ROOT)),1,15,true);
    QString curTurn = QString("%1").arg(QDate::currentDate().toString("yyyyMM"));
    if(curTurn == p_Main->turnNum.left(6)){
        qDebug() << "TRANS SAME MONTH " << curTurn << p_Main->turnNum << p_Main->turnNum.toInt()+1;
        p_Main->turnNum = QString("%1").arg(p_Main->turnNum.toInt()+1);
        qDebug() << "TRANS MONTH AFTER" << p_Main->turnNum;
    }
    else{
        qDebug() << "TRANS ANOTHER MONTH " << curTurn << p_Main->turnNum;
        p_Main->turnNum = curTurn+"001";
    }

    bool res = p_Main->pDatabase->UpdateRecord(INFO_TABLE,QString("totalCount='0', col4='%1'")
                                          .arg(p_Main->turnNum),"recName='deviceInfo'");

    p_Main->totalBagCount = 0;
    p_Main->compareCount = TWISTPERIOD;

    if(!res){
        qDebug() << "SAVE TURNNUM FAILED";
    }
    else{
        p_Main->eventFlag = 3;                      // 현송정보 보내기
        p_Main->pLoading->init_loadingPage(7);
        p_Main->select_Page(LOADINGPAGE);
    }

}

void page_transcash::on_btn_Barcode_clicked()
{
    cmd_barcodeScanStart();
}
