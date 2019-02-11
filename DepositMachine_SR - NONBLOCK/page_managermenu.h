#ifndef PAGE_MANAGERMENU_H
#define PAGE_MANAGERMENU_H

#include <QWidget>

namespace Ui {
class page_managermenu;
}

class page_managermenu : public QWidget
{
    Q_OBJECT

public:
    explicit page_managermenu(QWidget *parent = 0);
    ~page_managermenu();
    void lang_change();
    void occurred_alarm();
private slots:
    void on_btn_transCash_clicked();

    void on_btn_setting_clicked();

    void on_btn_usermanage_clicked();

    void on_btn_deposit_clicked();

    void on_btn_back_clicked();

    void on_btn_engineer_clicked();


    void on_btn_user_clicked();

    void on_btn_alarm_clicked();

    void on_btn_alClose_clicked();

private:
    Ui::page_managermenu *ui;
};

#endif // PAGE_MANAGERMENU_H
