#include "page_setting.h"
#include "ui_page_setting.h"
#include <QGridLayout>
#include <QFile>
#include "global.h"

int countryCount;
QStringList countryList;

page_setting::page_setting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::page_setting)
{
    ui->setupUi(this);

    countryCount = 0;


    ui->edit_cap->setText(p_Main->capNum);
    ui->edit_IP->setText(p_Main->deviceIp);
    ui->lbl_ip->setText(p_Main->deviceIp);

    ui->combo_language->blockSignals(true);
    ui->combo_language->setCurrentIndex(p_Main->langCountry);
    ui->combo_language->blockSignals(false);


    buttonList << ui->btn_country_1 << ui->btn_country_2 <<  ui->btn_country_3 <<  ui->btn_country_4 <<  ui->btn_country_5 << ui->btn_country_6 <<
                  ui->btn_country_7 << ui->btn_country_8 <<  ui->btn_country_9 <<  ui->btn_country_10 <<  ui->btn_country_11 << ui->btn_country_12 <<
                  ui->btn_country_13 << ui->btn_country_14 <<  ui->btn_country_15 <<  ui->btn_country_16 <<  ui->btn_country_17 << ui->btn_country_18 <<
                  ui->btn_country_19 << ui->btn_country_20 <<  ui->btn_country_21 <<  ui->btn_country_22 <<  ui->btn_country_23 << ui->btn_country_24 << ui->btn_country_25;

    for(int i=0; i<buttonList.count(); i++){
        QListWidgetItem* lwi = new QListWidgetItem();
        lwi->setSizeHint(buttonList.at(i)->sizeHint());
        ui->listWidget->addItem(lwi);
        ui->listWidget->setItemWidget(lwi,buttonList.at(i));
//        connect( buttonList.at(i),SIGNAL(toggled(bool)) ,this ,SLOT(on_countryBtn_toggled(bool)) );
    }

    //-------------------------------------국가 설정 정보저장 텍스트파일------------------------------------
    QFile test(QString("%1SetFiles/countrySet_S.ini").arg(PATH_ROOT));
    if(!test.open(QIODevice::ReadWrite)){
        qDebug() << "countrySet File Open Fail";
        countryCount = 0;
    }
    else{
        // 다국어 설정 정보 부터 읽어옴
        QString countryData = test.readLine();
        preSave = countryData.remove("\n").toInt();
        qDebug() << countryData;

        countryData = test.readLine();
        countryList = countryData.split(",",QString::SkipEmptyParts);
        countryCount = countryList.count();
//        qDebug() << countryList;

        QByteArray tmpArr;
        if(countryCount==1){
            QString strRow = countryList.at(0);
            tmpArr.append(buttonList.at(strRow.toInt())->statusTip());       // 이 텍스트 값은 국가명 코드 이므로 serial로 국가 설정
            //SDK - Set Country
        }
        else if(countryCount == 0){
            QByteArray tmpArr;
            tmpArr.append(DEFAULTCOUNTRY);
            // Default 값은 한국 화폐이므로 한국 화폐로 계수기 값이 설정 되도록 작업
            //SDK - Set korea
        }
        set_menupageCountry();
    }
    test.close();

    ui->lbl_serial->setText(p_Main->deviceNum);
    ui->lbl_type->setText(p_Main->deviceType);
    ui->lbl_ip->setText(p_Main->deviceIp);

    ui->combo_language->setItemDelegate(itemDelegate);
    ui->combo_finger->setItemDelegate(itemDelegate);

}

void page_setting::set_menupageCountry()
{
    for(int i=0; i<p_Main->pMenu->btnGroup->buttons().count(); i++){
        if(i < countryList.count()){
            p_Main->pMenu->btnGroup->button(i)->setEnabled(true);
            QString strRow = countryList.at(i);
            p_Main->pMenu->btnGroup->button(i)->setStatusTip(buttonList.at(strRow.toInt())->statusTip()); // statustip = 국가코드
            buttonList.at(strRow.toInt())->blockSignals(true);
            buttonList.at(strRow.toInt())->setChecked(true);
            buttonList.at(strRow.toInt())->blockSignals(false);
            p_Main->pMenu->btnGroup->button(i)->setIcon(buttonList.at(strRow.toInt())->icon());
        }
        else{
            p_Main->pMenu->btnGroup->button(i)->setStatusTip("");
//            p_Main->pMenu->btnGroup->button(i)->setEnabled(false);
        }
    }
}

void page_setting::lang_change()
{
    ui->retranslateUi(this);
}

page_setting::~page_setting()
{
    delete ui;
}

void page_setting::on_combo_language_currentIndexChanged(int index)
{
    QString currentPath = QString("%1SetFiles").arg(PATH_ROOT);

    switch(index){
    case 0:                 // 한국어
        qDebug() << currentPath+"/tt2_kr_device.qm";
        translator->load(currentPath+"/tt2_kr_device.qm");
        break;
    case 1:                 // 영어
        qDebug() << currentPath+"/tt2_en_device.qm";
        translator->load(currentPath+"/tt2_en_device.qm");
        break;
    case 2:                 // 스페인어
//        qDebug() << currentPath+"/tt2_es_device.qm";
//        translator->load(currentPath+"/tt2_es_device.qm");
        break;
    }

    p_Main->langCountry = index;
    p_Main->pDatabase->UpdateRecord(INFO2_TABLE, QString("col3 = '%1'").arg(p_Main->langCountry)
                                    ,"recName = 'deviceInfo2'");


    ui->combo_language->blockSignals(true);
    p_Main->lang_change();
    p_Main->pMsgDialog->lang_change();
    p_Main->pDevSetup->lang_change();
    p_Main->pEngineer->lang_change();
    p_Main->pEtc->lang_change();
    p_Main->pEventView->lang_change();
    p_Main->pLoading->lang_change();
    p_Main->pLogin->lang_change();
    p_Main->pManagerMenu->lang_change();
    p_Main->pMenu->lang_change();
    p_Main->pRepair->lang_change();
    p_Main->pResult->lang_change();
    p_Main->pSearch->lang_change();
    p_Main->pSign->lang_change();
    p_Main->pTransCash->lang_change();
    p_Main->pUser->lang_change();
    p_Main->pVisitor->lang_change();
    ui->retranslateUi(this);
    ui->combo_language->setCurrentIndex(index);
    ui->combo_language->blockSignals(false);
}

void page_setting::on_btn_saveexit_clicked()
{
    countryList.clear();
    for(int i=0; i<buttonList.count(); i++){
        if(buttonList.at(i)->isChecked()){
            countryList.append(QString("%1").arg(i));
        }
    }
    countryCount = countryList.count();
    set_menupageCountry();

    QFile test(QString("%1SetFiles/countrySet_S.ini").arg(PATH_ROOT));
    if(!test.open(QIODevice::ReadWrite)){
        qDebug() << "countrySet File Save Open Fail";
    }
    else{
        test.resize(0);
        QByteArray arrCountry;
        arrCountry.append(QString("%1\n").arg(p_Main->langCountry));             // 국가설정 정보 저장 시 언어설정 먼저 저장
        QString tmp = countryList.join(",");
        arrCountry.append(tmp);
        //            qDebug() << countryList << arrCountry << tmp << tmp.split(",").count();
        test.write(arrCountry);
        test.close();
    }

    if(p_Main->deviceIp != ui->edit_IP->text()){

        bool res = false;
        qDebug() << ui->edit_IP->text();
        res = p_Main->pDatabase->UpdateRecord(INFO_TABLE,QString("col3='%1'")
                                              .arg(ui->edit_IP->text()),"recName='deviceInfo'");
        if(res){
            p_Main->deviceIp = ui->edit_IP->text();
            QString cmd = QString("sudo ifconfig eth0 %1 up").arg(p_Main->deviceIp);

            QProcess *qProc = new QProcess(this);
            connect(qProc, SIGNAL(finished(int)),qProc,SLOT(deleteLater()));
            qProc->start(cmd);
            qProc->waitForFinished();
        }
    }

    ui->lbl_type->setText(p_Main->deviceType);
    ui->lbl_ip->setText(p_Main->deviceIp);

    p_Main->select_Page(LOGINPAGE);
}

void page_setting::on_btn_cancelsetting_clicked()
{
    for(int i=0; i<buttonList.count(); i++){
        buttonList.at(i)->setChecked(false);
    }
    for(int i=0; i<countryList.count(); i++){
        QString idx = countryList.at(i);
        buttonList.at(idx.toInt())->setChecked(true);
    }

//    ui->combo_language->setCurrentIndex(preSave);

    p_Main->select_Page(LOGINPAGE);
}

void page_setting::setLanguageCountry(int index){
    ui->combo_language->setCurrentIndex(index);
}

void page_setting::on_editbtn_ip_clicked()
{
    key_pad->open_relativePad(ui->edit_IP,p_Main);           //에디트 컨트롤의 정보를 가지고 text 설정 함수 호출
}

void page_setting::on_editbtn_cap_clicked()
{
    key_pad->open_relativePad(ui->edit_cap,p_Main);
}

void page_setting::on_edit_cap_textChanged(const QString &arg1)
{
    p_Main->capNum = arg1;
    p_Main->pDatabase->UpdateRecord(INFO2_TABLE, QString("col1 = '%1'").arg(p_Main->capNum)
                                    ,"recName = 'deviceInfo2'");

}
