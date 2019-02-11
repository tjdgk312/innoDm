#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>


#define YES 1
#define NO_OR_OK 0

namespace Ui {
class dialog;
}

class dialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit dialog(QWidget *parent = 0);
    ~dialog();
    
    void change_dialog(QString text, QString info);
    void change_usage(int flag);
    void lang_change();
private:
    Ui::dialog *ui;
};

#endif // DIALOG_H
