#ifndef PAGE_LOADING_H
#define PAGE_LOADING_H

#include <QWidget>

namespace Ui {
class page_loading;
}

class page_loading : public QWidget
{
    Q_OBJECT

public:
    explicit page_loading(QWidget *parent = 0);
    ~page_loading();

    QTimer  *loading_timer;
    int pageMode;
    int timingCount;
    int inputWaitCount;

    int retryCount;
    void init_loadingPage(int mode);

    int current_step;
    int preState_step;

    bool depositFlag;
    void depositFrameVisible(bool checked);


    static void OnStatusEvent(int status);

    QPixmap *pixDoorClose;
    QPixmap *pixDoorOpen;
    QPixmap *pixFinger;
    QPixmap *pixKeyLock;
    QPixmap *pixKeyUnlock;
    QPixmap *pixReject;
    QPixmap *pixReturn;
    QPixmap *pixInput;
    QPixmap *pixBarcode;
    QPixmap *pixPrintCounpon;


    QPixmap *pixLogo;


    void lang_change();
    int deviceStateCheck();
public slots:

private slots:
    void set_timeout();


    void on_btn_accuTotal_clicked();

private:
    Ui::page_loading *ui;
};

#endif // PAGE_LOADING_H
