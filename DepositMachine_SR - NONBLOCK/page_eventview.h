#ifndef PAGE_EVENTVIEW_H
#define PAGE_EVENTVIEW_H

#include <QWidget>
#include <QListWidget>

namespace Ui {
class page_eventView;
}

class page_eventView : public QWidget
{
    Q_OBJECT

public:
    explicit page_eventView(QWidget *parent = 0);
    ~page_eventView();

    QMap<int,QPixmap*> holeImage;

    void init_eventList();
    void lang_change();
private slots:
    void on_list_holeEvent_itemClicked(QListWidgetItem *item);

    void on_list_timeEvent_currentRowChanged(int currentRow);

    void on_btn_back_clicked();

private:
    Ui::page_eventView *ui;
};

#endif // PAGE_EVENTVIEW_H
