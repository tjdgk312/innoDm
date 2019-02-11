#ifndef PAGE_LOGIN_H
#define PAGE_LOGIN_H

#include <QWidget>


namespace Ui {
class page_login;
}

class page_login : public QWidget
{
    Q_OBJECT

public:
    explicit page_login(QWidget *parent = 0);
    ~page_login();

    void loginPage_init();

    int assignInfo;  // 이전 검색 및 인증 정보를 저장하여 뒤로가기 시 사용하기위함
    void lang_change();
    int check_idleStep();
protected:

public slots:
    void on_btn_fingerAssign_clicked();
    void on_btn_CardAssign_clicked();
    void on_btn_IDAssign_clicked();
private slots:



    void on_btn_back_clicked();


    void on_pushButton_clicked();

private:
    Ui::page_login *ui;
};

#endif // PAGE_LOGIN_H
