#ifndef PAGE_ENGINEER_H
#define PAGE_ENGINEER_H

#include <QWidget>

namespace Ui {
class page_engineer;
}

class page_engineer : public QWidget
{
    Q_OBJECT

public:
    explicit page_engineer(QWidget *parent = 0);
    ~page_engineer();

    QTimer  *engineer_timer;
    void lang_change();
    void setCardText();
private slots:
    void set_timeout();

    void on_btn_test_clicked();

    void on_Btn_Enable_clicked();

    void on_Btn_Disable_clicked();

    void on_Btn_Door_Lock_clicked();

    void on_Btn_Door_Unlock_clicked();

    void on_btn_SetCurPage_clicked();

    void on_Btn_sealSeq_clicked();

    void on_Btn_seal_open_clicked();

    void on_Btn_Lock_close_clicked();

    void on_Btn_Barcode_clicked();

    void on_Btn_Print_clicked();

    void on_Btn_FingerEnroll_clicked();

    void on_Btn_seal_close_clicked();

    void on_Btn_Lock_open_clicked();

    void on_Btn_CardRead_clicked();

    void on_Btn_outClose_clicked();

    void on_Btn_inOpen_clicked();

    void on_Btn_inClose_clicked();

    void on_Btn_blOpen_clicked();

    void on_Btn_blClose_clicked();

    void on_Btn_outOpen_clicked();

    void on_btn_SensorDelay_clicked();

    void on_btn_value_clicked();

    void on_Btn_DoorChk_clicked();

    void on_Btn_upsState_clicked();

    void on_Btn_upsOff_clicked();

private:
    Ui::page_engineer *ui;
};

#endif // PAGE_ENGINEER_H
