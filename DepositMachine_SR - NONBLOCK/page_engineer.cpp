#include "page_engineer.h"
#include "ui_page_engineer.h"
#include "global.h"


page_engineer::page_engineer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::page_engineer)
{
    ui->setupUi(this);

    engineer_timer = new QTimer(this);
    connect(engineer_timer , SIGNAL(timeout()), this, SLOT(set_timeout()));
    engineer_timer->start(1000);
    ui->combo_sen->setItemDelegate(itemDelegate);
}

page_engineer::~page_engineer()
{
    delete ui;
}

void page_engineer::lang_change()
{
    ui->retranslateUi(this);
}

void page_engineer::set_timeout()
{
    if(get_sealGateState() == GATE_DOPENED){
        ui->lbl_doorsen->setText("도어열림");
    }
    else if(get_sealGateState() == GATE_DCLOSED){
        ui->lbl_doorsen->setText("도어닫힘");
    }
}


void page_engineer::on_btn_test_clicked()
{
    p_Main->select_Page(LOGINPAGE);
}

void page_engineer::on_Btn_Enable_clicked()
{
    cmd_CountStartAct();
}

void page_engineer::on_Btn_Disable_clicked()
{
//    uint32_t   sendCmd =CMD_CURRENCY_STOP;
//    QByteArray sendData("");

//    p_Main->m_stateThread->transaction(sendCmd,sendData);
}

void page_engineer::on_Btn_Door_Lock_clicked()
{
    transLockDoorGate();
}

void page_engineer::on_Btn_Door_Unlock_clicked()
{
    transUnlockDoorGate();
}

void page_engineer::on_btn_SetCurPage_clicked()
{
    p_Main->select_Page(SETTINGPAGE);
//    p_Main->p_certifyUser->set_certNumpadPosition();
}

void page_engineer::on_Btn_sealSeq_clicked()
{
    cmd_SettlementStart();
}

void page_engineer::on_Btn_seal_open_clicked()
{
    cmd_sealOpen();
}
void page_engineer::on_Btn_seal_close_clicked()
{
    cmd_sealClose();
}

void page_engineer::on_Btn_Lock_open_clicked()
{
    cmd_lockOpen();
}
void page_engineer::on_Btn_Lock_close_clicked()
{
    cmd_lockClose();
}

void page_engineer::on_Btn_Barcode_clicked()
{
    cmd_barcodeScanStart();
}

void page_engineer::on_Btn_Print_clicked()
{
    QTextCodec* codec = QTextCodec::codecForName("eucKR");
    QString tmp = "TEST PRINT";
    Page_Newline();
    Page_Newline();
    Page_Newline();
    TextSaveSpool(codec->fromUnicode(tmp).data()); Page_Newline();
    Page_Print_StandardMode();

    SetTextAlignment(1);
    TextSaveSpool("www.innostec.co.kr\r\n\r\n\r\n\r\n\r\n");

    Page_Newline();
    Page_Newline();
    Page_Newline();

    FeedToMark();
    CutPaper(0);

    PrintSpool(p_Main->printerlib,TRUE);
//    GetPrinterStatus(p_Main->printerlib, 1000);
//    p_Main->pResult->receipt_Setup(0,0);
}

void page_engineer::on_Btn_FingerEnroll_clicked()
{
    cmd_fingerEnrollStart();
}



void page_engineer::setCardText(){
    ui->cardData->setText(p_Main->curUserId);
}

void page_engineer::on_Btn_CardRead_clicked()
{
    p_Main->cardOpType = 0;
    p_Main->pLoading->init_loadingPage(27);         // 로딩스텝만 전환하고 화면은 서치페이지에서 표시
}

void page_engineer::on_Btn_outClose_clicked()
{
    vaultCloseGate();
}

void page_engineer::on_Btn_inOpen_clicked()
{
    vaultOpenAcceptorGate();
}

void page_engineer::on_Btn_inClose_clicked()
{
    vaultCloseAcceptorGate();
}

void page_engineer::on_Btn_blOpen_clicked()
{
    vaultOpenBlock();
}

void page_engineer::on_Btn_blClose_clicked()
{
    vaultCloseBlock();
}

void page_engineer::on_Btn_outOpen_clicked()
{
    vaultOpenGate();
}

void page_engineer::on_btn_SensorDelay_clicked()
{
    set_SensorDelay(ui->combo_sen->currentIndex(), ui->edit_value->text().toInt());
}

void page_engineer::on_btn_value_clicked()
{
    key_pad->open_relativePad(ui->edit_value,p_Main);           //에디트 컨트롤의 정보를 가지고 text 설정 함수 호출
}

void page_engineer::on_Btn_DoorChk_clicked()
{
    cmd_chkDoorSen();
}

void page_engineer::on_Btn_upsState_clicked()
{

}

void page_engineer::on_Btn_upsOff_clicked()
{

}
