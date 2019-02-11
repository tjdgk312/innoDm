#ifndef PAGE_SEARCH_H
#define PAGE_SEARCH_H

#include <QWidget>
#include <QPushButton>

namespace Ui {
class page_search;
}

class page_search : public QWidget
{
    Q_OBJECT

public:
    explicit page_search(QWidget *parent = 0);
    ~page_search();

    QList <QPushButton*>  btnTimeList;

    QList <QPushButton*>  btnHoleList;

    void init_searchPage();
    void lang_change();
    void setCurrentStackPage(int page);
private slots:

    void on_btn_Back_clicked();

    void on_btn_nameSearch_clicked();

    void on_btn_timeSearch_clicked();

    void on_btn_holeSearch_clicked();

    void on_edit_searchname_selectionChanged();

    void on_btn_nameSearchClick_clicked();

    void on_btnTimeList_clicked();
    void on_btn_selectWedding_clicked();

    void on_stackedWidget_currentChanged(int arg1);

    void on_btnHoleList_clicked();
    void on_btn_deposit_clicked();

    void on_btn_Back_2_clicked();

    void on_btn_deposit_2_clicked();

    void on_btn_couponComp_clicked();

    void on_btn_countUp_clicked();

    void on_btn_countDown_clicked();

private:
    Ui::page_search *ui;
};

#endif // PAGE_SEARCH_H
