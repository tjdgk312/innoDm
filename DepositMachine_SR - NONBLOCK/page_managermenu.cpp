#include "page_managermenu.h"
#include "ui_page_managermenu.h"
#include "global.h"

page_managermenu::page_managermenu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::page_managermenu)
{
    ui->setupUi(this);

    ui->frm_alarm->hide();

    ui->table_alarm->setColumnWidth(0,30);
    ui->table_alarm->setColumnWidth(1,200);
    ui->table_alarm->setColumnWidth(2,150);
    ui->table_alarm->setColumnWidth(3,220);
    ui->table_alarm->setColumnWidth(4,160);

    for(int count=0; count < p_Main->alCount; count++){
        ui->table_alarm->insertRow(count);
        ui->table_alarm->setItem(count,0,new QTableWidgetItem(p_Main->Albuf[count][0].toString()));
        ui->table_alarm->setItem(count,1,new QTableWidgetItem(p_Main->Albuf[count][1].toString()));
        ui->table_alarm->setItem(count,2,new QTableWidgetItem(p_Main->Albuf[count][2].toString()));
        ui->table_alarm->setItem(count,3,new QTableWidgetItem(p_Main->Albuf[count][3].toString()));
        ui->table_alarm->setItem(count,4,new QTableWidgetItem(p_Main->Albuf[count][4].toString()));
    }
}

page_managermenu::~page_managermenu()
{
    delete ui;
}

void page_managermenu::lang_change()
{
    ui->retranslateUi(this);
}

void page_managermenu::on_btn_transCash_clicked()
{
    p_Main->currentDepositMode = 0;
    p_Main->pMsgDialog->init_MessageDialog(5);
}

void page_managermenu::on_btn_setting_clicked()
{
    p_Main->select_Page(SETTINGPAGE);
}

void page_managermenu::on_btn_usermanage_clicked()
{
    p_Main->select_Page(USERPAGE);
}

void page_managermenu::on_btn_deposit_clicked()
{
    p_Main->select_Page(LOGINPAGE);
}

void page_managermenu::on_btn_back_clicked()
{
    p_Main->select_Page(LOGINPAGE);
}

void page_managermenu::on_btn_engineer_clicked()
{
    p_Main->pMsgDialog->init_MessageDialog(18);
//    p_Main->select_Page(TESTPAGE);
}


void page_managermenu::on_btn_user_clicked()
{

}

void page_managermenu::on_btn_alarm_clicked()
{
    ui->frm_alarm->show();
}

void page_managermenu::on_btn_alClose_clicked()
{
    ui->frm_alarm->hide();
}


void page_managermenu::occurred_alarm(){

    p_Main->pDatabase->InsertRecord(ALARM_TABLE,QString("'%1' ,'%2', '%3', '%4', '%5'").arg(p_Main->curAlarm[0][0].toString())
            .arg(p_Main->curAlarm[0][1].toString()).arg(p_Main->curAlarm[0][2].toString()).arg(p_Main->curAlarm[0][3].toString())
            .arg(p_Main->curAlarm[0][4].toString()));

    ui->table_alarm->insertRow(0);
    ui->table_alarm->setItem(0,0,new QTableWidgetItem(p_Main->curAlarm[0][0].toString()));
    ui->table_alarm->setItem(0,1,new QTableWidgetItem(p_Main->curAlarm[0][1].toString()));
    ui->table_alarm->setItem(0,2,new QTableWidgetItem(p_Main->curAlarm[0][2].toString()));
    ui->table_alarm->setItem(0,3,new QTableWidgetItem(p_Main->curAlarm[0][3].toString()));
    ui->table_alarm->setItem(0,4,new QTableWidgetItem(p_Main->curAlarm[0][4].toString()));

//    p_Main->curAlarm[0][0] = "";
//    p_Main->curAlarm[0][1] = "";
//    p_Main->curAlarm[0][2] = "";
//    p_Main->curAlarm[0][3] = "";
//    p_Main->curAlarm[0][4] = "";


    if(ui->table_alarm->rowCount() > 30){
        p_Main->pDatabase->DeleteRecord(ALARM_TABLE,QString("time ='%1'").arg(ui->table_alarm->item(30,1)->text()));
        ui->table_alarm->removeRow(30);
    }
}
