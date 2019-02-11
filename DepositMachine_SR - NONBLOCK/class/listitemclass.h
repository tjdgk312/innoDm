#ifndef LISTITEMCLASS_H
#define LISTITEMCLASS_H

#include <QListWidget>
#include <QListWidgetItem>

#include <QLabel>
#include <QPushButton>
#include <QGridLayout>

#include <QDate>

typedef struct
{
    QString    eventMale;
    QString    eventFemale;
    QString    eventTime;
    QString    eventHole;
    QString    totalCash;
    QString    giftCash;
    QString    storedCash;
    QString    balanceCash;
    QString    primaryid;           //계약번호
    int        mealCount;
    int        personCount;
}t_SelectEvent, *t_pSelectEvent;

class listItemClass : public QListWidgetItem
{
//    Q_OBJECT
public:
    explicit listItemClass(int flag, QListWidget *parent = 0);

    t_SelectEvent itemStrEvent;

//    QString eventTime;

    QGridLayout* layout;
    QFrame* frm_item;
    QLabel* lbl_event;
    QLabel* lbl_holename;
    QLabel* lbl_clientname;

signals:

public slots:
};

#endif // LISTITEMCLASS_H
