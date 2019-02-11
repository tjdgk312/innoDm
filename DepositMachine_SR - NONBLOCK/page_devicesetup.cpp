#include "page_devicesetup.h"
#include "ui_page_devicesetup.h"
#include "global.h"

page_deviceSetup::page_deviceSetup(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::page_deviceSetup)
{
    ui->setupUi(this);
}

page_deviceSetup::~page_deviceSetup()
{
    delete ui;
}

void page_deviceSetup::init_deviceInterface(int flag){
    ui->stackedWidget->setCurrentIndex(flag);
}

void page_deviceSetup::lang_change()
{
    ui->comboBox->blockSignals(true);
    ui->comboBox->setStatusTip(QString("%1").arg(ui->comboBox->currentIndex()));
    ui->retranslateUi(this);
    ui->comboBox->setCurrentIndex(ui->comboBox->statusTip().toInt());
    ui->comboBox->blockSignals(false);
}

void page_deviceSetup::on_comboBox_currentTextChanged(const QString &arg1)
{
    p_Main->deviceType = arg1;
}

void page_deviceSetup::on_btn_login_clicked()
{
    bool res = false;

    if(ui->stackedWidget->currentIndex() == 0){
//        if(p_Main->deviceType == "" && p_Main->deviceNum == ""){
            res = p_Main->pDatabase->UpdateRecord(INFO_TABLE,QString("col1='%1',col2='%2'")
                                                  .arg(ui->edit_id->text()).arg(ui->comboBox->currentText()),"recName='deviceInfo'");
//        }
    }

    if(res){
        p_Main->deviceNum =  ui->edit_id->text();       // 기계시리얼넘버
        p_Main->deviceType = ui->comboBox->currentText();         // 기종구분
//        qDebug() << "init data save";
//        QProcess *qProc = new QProcess(this);
//        QString cmd;
//        cmd = "sudo reboot";
//        qProc->start(cmd);
//        qProc->waitForFinished();
        ui->stackedWidget->setCurrentIndex(1);
    }
    else{
        p_Main->pMsgDialog->init_MessageDialog(3);
        qDebug() << "init data save failed";
    }
}

void page_deviceSetup::on_btn_ipSave_clicked()
{
    bool res = false;
    qDebug() << ui->edit_ip->text();
    res = p_Main->pDatabase->UpdateRecord(INFO_TABLE,QString("col3='%1'")
                                          .arg(ui->edit_ip->text()),"recName='deviceInfo'");
    if(res){
        QString cmd = QString("sudo ifconfig eth0 %1 up").arg(p_Main->deviceIp);
        QProcess *qProc = new QProcess(this);
        connect(qProc, SIGNAL(finished(int)),qProc,SLOT(deleteLater()));
        qProc->start(cmd);
        qProc->waitForFinished();

        p_Main->deviceIp = ui->edit_ip->text();

        p_Main->deviceNormalInit();
        this->hide();
    }
    else{
        p_Main->pMsgDialog->init_MessageDialog(3);
        qDebug() << "init ip save failed";
    }
}

void page_deviceSetup::on_editbtn_id_clicked()
{
    key_pad->open_relativePad(ui->edit_id,p_Main);           //에디트 컨트롤의 정보를 가지고 text 설정 함수 호출
}

void page_deviceSetup::on_editbtn_ip_clicked()
{
    key_pad->open_relativePad(ui->edit_ip,p_Main);           //에디트 컨트롤의 정보를 가지고 text 설정 함수 호출
}
