#ifndef PAGE_VISITORLOGIN_H
#define PAGE_VISITORLOGIN_H

#include <QWidget>

namespace Ui {
class page_visitorLogin;
}

class page_visitorLogin : public QWidget
{
    Q_OBJECT

public:
    explicit page_visitorLogin(QWidget *parent = 0);
    ~page_visitorLogin();
    void lang_change();
private slots:
    void on_btn_Search_clicked();

private:
    Ui::page_visitorLogin *ui;
};

#endif // PAGE_VISITORLOGIN_H
