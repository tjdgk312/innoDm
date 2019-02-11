#include "page_etc.h"
#include "ui_page_etc.h"
#include "global.h"

page_etc::page_etc(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::page_etc)
{
    ui->setupUi(this);

    etcList << ui->btn_etc << ui->btn_etc_2 << ui->btn_etc_3 << ui->btn_etc_4 << ui->btn_etc_5;
    for(int i=0; i<etcList.count(); i++){
        connect((QPushButton*)etcList.at(i),SIGNAL(pressed()),this,SLOT(on_etcEdit_pressed()));
    }
    num_pad = new numpad(ui->widget);
    num_pad->edit_btn = ui->btn_etc;
    ui->btn_etc->setStyleSheet("border-image: url(:/image/base7.png);"
                                 "color: rgb(238, 238, 236);");
//    num_pad->hide();
}

void page_etc::init_etcPage(){

    ui->lbl_type->setText(p_Main->curCountryText);
    ui->lbl_type_2->setText(p_Main->curCountryText);
    ui->lbl_type_3->setText(p_Main->curCountryText);
    ui->lbl_type_4->setText(p_Main->curCountryText);
    ui->lbl_type_5->setText(p_Main->curCountryText);

    for(int i=0; i<etcList.count(); i++){
        etcList.at(i)->setText("");
    }
    ui->stackedWidget->setCurrentIndex(0);
}

void page_etc::lang_change()
{
    ui->retranslateUi(this);
}

void page_etc::on_etcEdit_pressed(){
    num_pad->VALUE->clear();
    QPushButton* tmpCheck = qobject_cast<QPushButton*>(sender());
    num_pad->edit_btn = tmpCheck;
    for(int i=0; i<etcList.count(); i++){
        etcList.at(i)->setStyleSheet("border-image: url(:/image/base6.png);");
    }
    tmpCheck->setStyleSheet("border-image: url(:/image/base7.png);"
                                 "color: rgb(238, 238, 236);");
}

page_etc::~page_etc()
{
    delete ui;
}

void page_etc::on_btn_etcDeposit_clicked()
{
    p_Main->setAudioMedia(QUrl::fromLocalFile(QString("%1audio/o2.mp3").arg(PATH_ROOT)),1,15,true);
    etcStep = 0;
    ui->lbl_info_2->setStyleSheet("border-image: url(:/image/barcode.jpg);");
    ui->lbl_info->setText(trUtf8("봉투 바코드를 인식시켜 주세요."));
    ui->btn_etcComplete->setText(trUtf8("재시도"));
    cmd_barcodeScanStart();
    ui->stackedWidget->setCurrentIndex(1);
}

void page_etc::get_barcodeAck(){
    ui->lbl_info_2->setStyleSheet("border-image: url(:/image/input.jpg);");
    ui->lbl_info->setText(trUtf8("봉투를 투입하신 후 \"투입완료\" 버튼을 눌러주세요"));
    ui->btn_etcComplete->setText(trUtf8("투입완료"));
    etcStep = 1;
}

void page_etc::on_btn_etcComplete_clicked()
{
    if(etcStep == 0){
        cmd_barcodeScanStart();
        return;
    }
    p_Main->pLoading->init_loadingPage(21);      // 처리대기 중
    p_Main->select_Page(LOADINGPAGE);               // 완료모드 진입이랑 순서바꿈
    p_Main->pLoading->current_step = ETC_OUTCLOSE;
    vaultCloseGate();
}

void page_etc::complete_etc(){
    double sumTotal = 0;
    for(int i=0; i<etcList.count(); i++){
        p_Main->pResult->curDeposit.etcEnvelope[i] = etcList.at(i)->text().remove(",").remove(" ").toDouble();
        qDebug() <<"ETC complete - "<< i << etcList.at(i)->text() << p_Main->pResult->curDeposit.etcEnvelope[i];
        sumTotal += p_Main->pResult->curDeposit.etcEnvelope[i];
        etcList.at(i)->text().clear();
        etcList.at(i)->setStyleSheet("border-image: url(:/image/base6.png);");
    }
    p_Main->pResult->curDeposit.sumTotal = sumTotal;

    num_pad->edit_btn->text().clear();
    num_pad->VALUE->clear();

    p_Main->deviceStatusFlag = 0;
    p_Main->select_Page(RESULTPAGE);
}

void page_etc::on_btn_etcCancel_clicked()
{
    p_Main->pLoading->init_loadingPage(21);      // 처리대기 중
    p_Main->select_Page(LOADINGPAGE);               // 완료모드 진입이랑 순서바꿈
    p_Main->pLoading->current_step = ETC_CANCELCLOSE;
    vaultCloseGate();
}
