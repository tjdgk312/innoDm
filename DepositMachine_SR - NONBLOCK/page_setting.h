#ifndef PAGE_SETTING_H
#define PAGE_SETTING_H

#include <QWidget>
#include <QPushButton>

namespace Ui {
class page_setting;
}

class page_setting : public QWidget
{
    Q_OBJECT

public:
    explicit page_setting(QWidget *parent = 0);
    ~page_setting();
    QList <QPushButton*>  buttonList;

    int preSave;
    void set_menupageCountry();
    void lang_change();
    void setLanguageCountry(int index);
public slots:
private slots:
    void on_combo_language_currentIndexChanged(int index);

    void on_btn_saveexit_clicked();

    void on_btn_cancelsetting_clicked();

    void on_editbtn_ip_clicked();

    void on_editbtn_cap_clicked();

    void on_edit_cap_textChanged(const QString &arg1);

private:
    Ui::page_setting *ui;
};

#endif // PAGE_SETTING_H
