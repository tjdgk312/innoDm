#ifndef PAGE_MENU_H
#define PAGE_MENU_H

#include <QWidget>
#include <QPushButton>
#include <QButtonGroup>

namespace Ui {
class page_menu;
}

class page_menu : public QWidget
{
    Q_OBJECT

public:
    explicit page_menu(QWidget *parent = 0);
    ~page_menu();

    QList <QPushButton*>  buttonList;
    QButtonGroup *btnGroup;

    QString pocketCnt;
    void lang_change();
    void init_menuPage();
public slots:
    void on_menuBtn_pressed();
private slots:

    void on_btn_deposit_clicked();

    void on_btn_deposit_2_clicked();

    void on_btn_back_clicked();


    void on_btn_test_clicked();

    void on_btn_home_clicked();

private:
    Ui::page_menu *ui;
};

#endif // PAGE_MENU_H
