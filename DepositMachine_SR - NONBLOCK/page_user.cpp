#include "page_user.h"
#include "ui_page_user.h"
#include "global.h"


page_user::page_user(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::page_user)
{
    ui->setupUi(this);
}

page_user::~page_user()
{
    delete ui;
}

void page_user::lang_change()
{
    ui->retranslateUi(this);
}

void page_user::on_btn_registerUser_clicked()
{
//    const char *clientName;
//    const char *userFullName;
//    const char *userName;
//    const char *pw;

    QString strFullname = ui->edit_firstname->text() + ui->edit_lastname->text();
//    clientName = ui->edit_client->text().toStdString().c_str();
//    userFullName = strFullname.toStdString().c_str();
//    userName = ui->edit_firstname->text().toStdString().c_str();
//    pw   = ui->edit_PW->text().toStdString().c_str();

//     str.toLocal8Bit().constData()
    int result;
//    result = usrCreateUser(ui->edit_client->text().toStdString().c_str(),
//                  strFullname.toStdString().c_str(), ui->edit_firstname->text().toStdString().c_str(),
//                  ui->edit_PW->text().toStdString().c_str(), 2047, ui->edit_userID->text().toInt());

    QMessageBox reply(QMessageBox::Warning,
                      "",
                      "",
                      QMessageBox::Ok,this, Qt::FramelessWindowHint);
    reply.setStyleSheet("QMessageBox{border: 2px solid rgb(0, 0, 0); font: 13pt;} QPushButton { min-height: 31px; min-width: 61px; }");
    switch(result){
    case 0:
         reply.setText("사용자 등록에 성공하였습니다.");
        break;
    case 1:
        reply.setText("동일한 ID를 가진 사용자가 이미 존재합니다.");
        break;
    case 2:
        reply.setText("사용자 등록에 대한 권한이 없습니다.");
        break;
    case 3:
        reply.setText("사용자 등록 수 제한을 초과하였습니다.");
        break;
    case 4:
        reply.setText("사용자 레코드 생성에 실패하였습니다.");
        break;
    }
    if(reply.exec() == QMessageBox::Warning){}

    ui->edit_firstname->blockSignals(true);
    ui->edit_client->blockSignals(true);
    ui->edit_lastname->blockSignals(true);
    ui->edit_PW->blockSignals(true);
    ui->edit_userID->blockSignals(true);

    ui->edit_firstname->clear();
    ui->edit_client->clear();
    ui->edit_lastname->clear();
    ui->edit_PW->clear();
    ui->edit_userID->clear();

    ui->edit_firstname->blockSignals(false);
    ui->edit_client->blockSignals(false);
    ui->edit_lastname->blockSignals(false);
    ui->edit_PW->blockSignals(false);
    ui->edit_userID->blockSignals(false);
}


// 이벤트 발생 잘 안될시에 이벤트필터로 변경 될 수 있음.
void page_user::on_edit_client_selectionChanged()
{
//    key_pad->open_relativePad(ui->edit_client,this);           //에디트 컨트롤의 정보를 가지고 text 설정 함수 호출
}

void page_user::on_edit_lastname_selectionChanged()
{
//    key_pad->open_relativePad(ui->edit_lastname,this);           //에디트 컨트롤의 정보를 가지고 text 설정 함수 호출
}

void page_user::on_edit_firstname_selectionChanged()
{
//    key_pad->open_relativePad(ui->edit_firstname,this);           //에디트 컨트롤의 정보를 가지고 text 설정 함수 호출
}

void page_user::on_edit_PW_selectionChanged()
{
//    key_pad->open_relativePad(ui->edit_PW,this);           //에디트 컨트롤의 정보를 가지고 text 설정 함수 호출
}

void page_user::on_edit_userID_selectionChanged()
{
//    key_pad->open_relativePad(ui->edit_userID,this);           //에디트 컨트롤의 정보를 가지고 text 설정 함수 호출
//    num_pad->change_text(ui->edit_userID,800,480);
}
