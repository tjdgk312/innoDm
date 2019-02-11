#ifndef MESSAGEDIALOG_H
#define MESSAGEDIALOG_H

#include <QDialog>

namespace Ui {
class MessageDialog;
}

class MessageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MessageDialog(QDialog *parent = 0);
    ~MessageDialog();

    int select;
    int currentFlag;
    int init_MessageDialog(int msgFlag);
    void setAssignResult();
    void lang_change();
    void setInfoText(QString text);
private slots:
    void on_btn_yes_clicked();

    void on_btn_no_clicked();

    void on_edit_input_selectionChanged();

    void on_pushButton_clicked();

private:
    Ui::MessageDialog *ui;
};

#endif // MESSAGEDIALOG_H
