#include "dialog.h"
#include "ui_dialog.h"

dialog* apply_dialog=NULL;

dialog::dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dialog)
{
    ui->setupUi(this);
}

dialog::~dialog()
{
    delete ui;
}


void dialog::change_dialog(QString text, QString info){
    ui->dialog_text->setText(text);
    ui->dialog_info->setText(info);
}

void dialog::change_usage(int flag){
    switch(flag){
    case 0:                     // 확인로그
        ui->dialog_yes->hide();
        ui->dialog_no->show();
        ui->dialog_info->hide();
        ui->dialog_no->setText(trUtf8("확인"));
        ui->dialog_no->setGeometry(ui->dialog_no->pos().rx(), 130, ui->dialog_no->width(), ui->dialog_no->height());
        this->resize(480,200);
        break;
    case 1:                      // 예, 아니오
        ui->dialog_yes->show();
        ui->dialog_no->show();
        ui->dialog_info->show();
        ui->dialog_no->setText(trUtf8("아니오"));
        ui->dialog_no->setGeometry(ui->dialog_no->pos().rx(), 170, ui->dialog_no->width(), ui->dialog_no->height());
        this->resize(480,240);
        break;
    case 2:                     // 로딩 및 대기창
        ui->dialog_yes->hide();
        ui->dialog_no->hide();
        ui->dialog_info->hide();
        this->resize(480,200);
//        ui->dialog_no->setText(trUtf8("아니오"));
        break;
    }
}

void dialog::lang_change()
{
    ui->retranslateUi(this);
}
