#include "page_visitorlogin.h"
#include "ui_page_visitorlogin.h"
#include "global.h"

page_visitorLogin::page_visitorLogin(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::page_visitorLogin)
{
    ui->setupUi(this);
}

page_visitorLogin::~page_visitorLogin()
{
    delete ui;
}

void page_visitorLogin::lang_change()
{
    ui->retranslateUi(this);
}

void page_visitorLogin::on_btn_Search_clicked()
{
    p_Main->select_Page(EVENTVIEWPAGE);
}
