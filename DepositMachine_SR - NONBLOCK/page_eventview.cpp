#include "page_eventview.h"
#include "ui_page_eventview.h"
#include "global.h"

#include <QGridLayout>
#include <QLabel>

page_eventView::page_eventView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::page_eventView)
{
    ui->setupUi(this);

    holeImage.insert(0,new QPixmap("://image/1.png"));
    holeImage.insert(1,new QPixmap("://image/2.png"));
    holeImage.insert(2,new QPixmap("://image/3.png"));


    for(int i=0; i<p_Main->timeList.count(); i++){
        ui->list_timeEvent->addItem(p_Main->timeList.at(i));
        ui->list_timeEvent->item(i)->setSizeHint(QSize(ui->list_timeEvent->item(i)->sizeHint().width(), 80));
    }
    // later 현재시간과 가장 가까운 이벤트 시간으로 현재화면 설정
    ui->list_timeEvent->setCurrentRow(0);
}

void page_eventView::init_eventList(){
    // later 시간대별로 세팅되도록 조정해야 함

    qDebug() << "EVENT LIST REFRESH";

    ui->list_holeEvent->clear();

    for(int i=0; i<p_Main->eventMap.count(); i++){
        listItemClass* tmpItem = new listItemClass(0);
        ui->list_holeEvent->addItem(tmpItem);
        ui->list_holeEvent->setItemWidget(tmpItem,tmpItem->frm_item);
        tmpItem->lbl_event->setPixmap(*holeImage.value(qrand()%2));

        QString name = p_Main->eventMap.values().at(i).at(4);
        QStringList nameList = name.split("-");     // ex) ksh-cyj 이렇게 되어있음
        if(nameList.count() < 2){
            // 신랑신부이름 인덱스 카운트 에러
            qDebug() << "bride name index error!!";
            continue;
        }
        tmpItem->itemStrEvent.totalCash = p_Main->eventMap.values().at(i).at(5);
        tmpItem->itemStrEvent.giftCash = p_Main->eventMap.values().at(i).at(6);
        tmpItem->itemStrEvent.storedCash = p_Main->eventMap.values().at(i).at(7);
        tmpItem->itemStrEvent.balanceCash = p_Main->eventMap.values().at(i).at(8);
        tmpItem->itemStrEvent.eventMale = nameList.at(0);
        tmpItem->itemStrEvent.eventFemale = nameList.at(1);
        tmpItem->itemStrEvent.eventTime = p_Main->eventMap.values().at(i).at(3);
        tmpItem->itemStrEvent.eventHole = p_Main->eventMap.values().at(i).at(2);
        tmpItem->itemStrEvent.primaryid = p_Main->eventMap.values().at(i).at(0);

        tmpItem->lbl_holename->setText(tmpItem->itemStrEvent.eventHole);
        tmpItem->lbl_clientname->setText(QString("신랑 %1\n신부 %2").arg(tmpItem->itemStrEvent.eventMale).arg(tmpItem->itemStrEvent.eventFemale));
        // for test
    }

}

void page_eventView::lang_change()
{
    ui->retranslateUi(this);
}



page_eventView::~page_eventView()
{
    delete ui;
}

void page_eventView::on_list_holeEvent_itemClicked(QListWidgetItem *item)
{
    qDebug() << "holeEvent Clicked";
    listItemClass* tmpWidgetItem = (listItemClass*)item;
    p_Main->curEventStr = &tmpWidgetItem->itemStrEvent;

    if(p_Main->pLogin->assignInfo >= 4){            // 축의금납부
        p_Main->select_Page(SEARCHPAGE);
    }
    else{
        // later 서버로 부터 납입정보 받아서 처리
        p_Main->eventFlag = 1;                      // Request DB data
        p_Main->pLoading->init_loadingPage(23);
        p_Main->select_Page(LOADINGPAGE);
    }

    ui->list_holeEvent->setCurrentRow(-1);
//    ui->list_holeEvent->s
}

void page_eventView::on_list_timeEvent_currentRowChanged(int currentRow)
{
    // later 신랑신부 정보 텍스트 읽어서 변경 해주어야 함
}

void page_eventView::on_btn_back_clicked()
{
   p_Main->select_Page(MENUPAGE);
}
