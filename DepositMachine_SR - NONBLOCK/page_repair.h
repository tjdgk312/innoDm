#ifndef PAGE_REPAIR_H
#define PAGE_REPAIR_H

#include <QWidget>

namespace Ui {
class page_repair;
}

class page_repair : public QWidget
{
    Q_OBJECT

public:
    explicit page_repair(QWidget *parent = 0);
    ~page_repair();

    QTimer  *repair_timer;
    int currentErrCode;

    void init_repairPage(int alCode);
    QPixmap *pixjam1;
    QPixmap *pixjam2;
    QPixmap *pixjam3;
    QPixmap *pixjam4;

    int retryCnt;
    void set_ManageFrame();
    void lang_change();
private slots:
    void on_btn_error_clicked();

    void on_btn_openPocket_clicked();

    void on_btn_closePocket_clicked();

    void on_btn_main_clicked();

    void on_btn_clear_clicked();

    void on_btn_result_clicked();

    void set_timeout();
    void on_btn_retry_clicked();

private:
    Ui::page_repair *ui;
};

#endif // PAGE_REPAIR_H
