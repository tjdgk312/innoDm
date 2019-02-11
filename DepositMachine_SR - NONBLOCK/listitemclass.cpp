#include "class/listitemclass.h"
#include "global.h"


listItemClass::listItemClass(int flag, QListWidget *parent) :
    QListWidgetItem(parent)
{
    QFont font;
    font.setPixelSize(20);
    font.setBold(true);

    frm_item =  new QFrame();
    frm_item->setMaximumSize(150,600);
    layout = new QGridLayout();
    layout->setContentsMargins(30,30,30,30);
    layout->setSpacing(10);
    layout->setSizeConstraint( QLayout::SetFixedSize );
    lbl_event = new QLabel();
    lbl_event->setMaximumSize(150,150);
    lbl_event->setScaledContents(true);

    // later holelist input
    lbl_holename = new QLabel();
    lbl_holename->setMaximumSize(150,80);
    lbl_holename->setAlignment(Qt::AlignHCenter);
    lbl_holename->setFont(font);

    // later clientname input
    lbl_clientname = new QLabel();
    lbl_clientname->setMaximumSize(150,100);
    lbl_clientname->setAlignment(Qt::AlignHCenter);
    lbl_clientname->setFont(font);

    layout->addWidget(lbl_holename,0,0);
    layout->addWidget(lbl_event,1,0);
    layout->addWidget(lbl_clientname,2,0);
    frm_item->setLayout(layout);
    qDebug() << layout->sizeHint();
    this->setSizeHint( QSize(170,700) );

//    itemStrEvent.eventHole = lbl_holename->text();
//    itemStrEvent.eventString = lbl_clientname->text();
//    itemStrEvent.eventTime = eventTime;
}
