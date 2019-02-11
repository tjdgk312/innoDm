#ifndef PAGE_DEVICESETUP_H
#define PAGE_DEVICESETUP_H

#include <QWidget>

namespace Ui {
class page_deviceSetup;
}

class page_deviceSetup : public QWidget
{
    Q_OBJECT

public:
    explicit page_deviceSetup(QWidget *parent = 0);
    ~page_deviceSetup();

    void init_deviceInterface(int flag);
    void lang_change();
private slots:
    void on_comboBox_currentTextChanged(const QString &arg1);

    void on_btn_login_clicked();

    void on_btn_ipSave_clicked();

    void on_editbtn_id_clicked();

    void on_editbtn_ip_clicked();

private:
    Ui::page_deviceSetup *ui;
};

#endif // PAGE_DEVICESETUP_H
