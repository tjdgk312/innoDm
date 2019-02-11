#ifndef PAGE_ETC_H
#define PAGE_ETC_H

#include <QWidget>
#include <QPushButton>

namespace Ui {
class page_etc;
}

class page_etc : public QWidget
{
    Q_OBJECT

public:
    explicit page_etc(QWidget *parent = 0);
    ~page_etc();

    QList <QPushButton*> etcList;

    void init_etcPage();
    void lang_change();
    void get_barcodeAck();
    int etcStep;
    void complete_etc();
public slots:
    void on_etcEdit_pressed();
private slots:
    void on_btn_etcDeposit_clicked();

    void on_btn_etcComplete_clicked();

    void on_btn_etcCancel_clicked();

private:
    Ui::page_etc *ui;
};

#endif // PAGE_ETC_H
