#ifndef PAGE_TRANSCASH_H
#define PAGE_TRANSCASH_H

#include <QWidget>
#include "sdkheader/vault.h"

namespace Ui {
class page_transcash;
}

class page_transcash : public QWidget
{
    Q_OBJECT

public:
    explicit page_transcash(QWidget *parent = 0);
    ~page_transcash();

    static void OnSettlementCompleteEvent(LPVAULT_SETTLEMENT_INFO settlement_info_ptr, int status, void *user_data);
    void init_transpage(int flag);

    void lang_change();
private slots:

    void on_btn_ReturnComp_clicked();

    void on_btn_ChangeComp_clicked();

    void on_btn_Barcode_clicked();

private:
    Ui::page_transcash *ui;
};

#endif // PAGE_TRANSCASH_H
