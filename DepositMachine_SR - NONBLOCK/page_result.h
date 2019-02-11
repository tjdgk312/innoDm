#ifndef PAGE_RESULT_H
#define PAGE_RESULT_H

#include <QWidget>

#include "sdkheader/vault.h"

namespace Ui {
class page_result;
}

class page_result : public QWidget
{
    Q_OBJECT

public:
    explicit page_result(QWidget *parent = 0);
    ~page_result();
    QTimer  *result_timer;
    void resultPage_init();

    VAULTDEPOSIT_INFO preDeposit;
    VAULTDEPOSIT_INFO curDeposit;
    static void OnDepositCompleteEvent(LPVAULTDEPOSIT_INFO deposit_info_ptr, int status, void *user_data);

    QStringList etcList;

    int additionalFlag;
    int additionalState;

    void init_DepositStr();
    void receipt_Setup(int flag, int number);
    void print_calculateReceipt();

    void resultPage_waitTimer(int timeCnt);
    void lang_change();
    void set_resultButton(bool checked);
    int btnState;
    void resultPage_State(QString text);
    void init_curDepositStr();
public slots:
    void on_btn_confirmDeposit_clicked();

private slots:
    void on_btn_cancelDeposit_clicked();

    void set_timeout();

    void on_btn_receipt_clicked();

    void on_btn_additionalDeposit_clicked();

private:
    Ui::page_result *ui;
};

#endif // PAGE_RESULT_H
