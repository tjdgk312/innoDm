#ifndef PAGE_USER_H
#define PAGE_USER_H

#include <QWidget>

namespace Ui {
class page_user;
}

class page_user : public QWidget
{
    Q_OBJECT

public:
    explicit page_user(QWidget *parent = 0);
    ~page_user();
    void lang_change();
private slots:
    void on_btn_registerUser_clicked();

    void on_edit_client_selectionChanged();

    void on_edit_lastname_selectionChanged();

    void on_edit_firstname_selectionChanged();

    void on_edit_PW_selectionChanged();

    void on_edit_userID_selectionChanged();

private:
    Ui::page_user *ui;
};

#endif // PAGE_USER_H
