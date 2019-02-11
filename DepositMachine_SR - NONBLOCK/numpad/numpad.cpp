/*
 * numpad.cpp
 *  -numpad operation
 */

#include <QString>
#include "numpad.h"
#include "ui_numpad.h"
#include "qdebug.h"
#include "QDebug"

#include <QMessageBox>

numpad* num_pad=NULL;
#define EDIT_MARGIN 5


numpad::numpad(QWidget *parent) :
    QWidget(parent,Qt::FramelessWindowHint),
    NUMPAD(new Ui::numpad)
{

    VALUE=new QString("");
    if (parent!=0){
        NUMPAD->setupUi(this);
    }
    else{
        NUMPAD->setupUi(this);
    }
    dotPoint=0;
    dotCnt=0;

    neventFlag=0;
    nclickFlag=0;
    //set operation event button
    QObject::connect(NUMPAD->QPB_0,SIGNAL(clicked()),this,SLOT(append()));
    QObject::connect(NUMPAD->QPB_1,SIGNAL(clicked()),this,SLOT(append()));
    QObject::connect(NUMPAD->QPB_2,SIGNAL(clicked()),this,SLOT(append()));
    QObject::connect(NUMPAD->QPB_3,SIGNAL(clicked()),this,SLOT(append()));
    QObject::connect(NUMPAD->QPB_4,SIGNAL(clicked()),this,SLOT(append()));
    QObject::connect(NUMPAD->QPB_5,SIGNAL(clicked()),this,SLOT(append()));
    QObject::connect(NUMPAD->QPB_6,SIGNAL(clicked()),this,SLOT(append()));
    QObject::connect(NUMPAD->QPB_7,SIGNAL(clicked()),this,SLOT(append()));
    QObject::connect(NUMPAD->QPB_8,SIGNAL(clicked()),this,SLOT(append()));
    QObject::connect(NUMPAD->QPB_9,SIGNAL(clicked()),this,SLOT(append()));
    QObject::connect(NUMPAD->QPB_ENTER,SIGNAL(clicked()),this,SLOT(append_enter()));
    QObject::connect(NUMPAD->QPB_POINT,SIGNAL(clicked()),this,SLOT(append()));
    QObject::connect(NUMPAD->QPB_PLUS_MINUS,SIGNAL(clicked()),this,SLOT(toggle_plus_minus()));
    QObject::connect(NUMPAD->QPB_BACK,SIGNAL(clicked()),this,SLOT(back_value()));
    QObject::connect(NUMPAD->QPB_CLEAR,SIGNAL(clicked()),this,SLOT(clear_value()));
    QObject::connect(NUMPAD->QPB_ESC,SIGNAL(clicked()),this,SLOT(esc_value()));

    //set palette for background color
    palette = new QPalette();

    //Step check timer set
    pad_timer = new QTimer(this);
    connect(pad_timer , SIGNAL(timeout()), this, SLOT(check_pad_timer()));
    edit_value =new QLineEdit();

    old_data.clear();

    installEventFilter(this);
}

numpad::~numpad()
{
    delete NUMPAD;
}

void numpad::append(){
    closecnt=0;
    insert=1;
    QPushButton* push = qobject_cast<QPushButton*>(sender());
//    if(dotPoint){                            // 소수점 입력 후에는 소수점 갯수 카운트
//        if(edit_value->decimals() == dotCnt){   // 소수점 갯수와 같을경우 리턴
//            return;}
//        dotCnt++;
//    }
//    if(push->text() == "."){      // 소수점 입력시작
//        dotPoint =1;
//        NUMPAD->QPB_POINT->setDisabled(dotPoint);
//    }
    QString lastStr = *VALUE;
    VALUE->append(push->text());
    qDebug() << lastStr << *VALUE;
//    if(VALUE->count() >= 10 && VALUE->toUInt() == 0 ){      // 4 바이트 데이터 제한 초과
//        *VALUE = lastStr;
//        qDebug() << lastStr << *VALUE;
//        QMessageBox reply(QMessageBox::Warning,
//                          "",
//                          "금액 제한을 초과하였습니다.\n"
//                          "4294967295 를 초과 할 수 없습니다.",
//                          QMessageBox::Ok,this, Qt::FramelessWindowHint);
//        reply.setStyleSheet("QMessageBox{border: 2px solid rgb(0, 0, 0); font: 13pt;} QPushButton { min-height: 31px; min-width: 61px; }");
//        if(reply.exec() == QMessageBox::Warning){
//        }
//    }
//    edit_btn->setText(QString("%L2").arg(VALUE->toInt()));
//    edit_btn->setText(QString("%1").arg(VALUE->toInt()));
    edit_btn->setText(QString("%L1").arg(VALUE->toDouble() ,0,'f',0,' '));
//    NUMPAD->TextEdit->setText(*VALUE);
}

//event operation function
void numpad::append_enter()
{
    closecnt=0;
    dotPoint=0;
    dotCnt=0;
    NUMPAD->QPB_POINT->setDisabled(false);

    double dbl_val = VALUE->toDouble();
    int int_val = dbl_val;
    if(VALUE->contains(".")){
        if(VALUE->right(VALUE->size() - (VALUE->indexOf(".")+1)).isEmpty()){
            VALUE->remove(".");
        }
    }
//    if(insert == 0){
//        edit_value->setValue(old_data.toDouble());
//    }
//    else{
//        if(edit_value->minimum() > dbl_val){
//            edit_value->setValue(edit_value->minimum());
//        }
//        else if(edit_value->maximum() < dbl_val){
//            edit_value->setValue(edit_value->maximum());
//        }
//        else{
//            edit_value->setValue(dbl_val);
//        }
//    }
    old_data = *VALUE;
    VALUE->clear();
    removeEventFilter(this);
    //edit_value->setEnabled(true);
    edit_value->setStyleSheet("background-color: white; selection-background-color: white; selection-color: black;");
//    pad_timer->stop();
    edit_value->setStatusTip("not used"); // 중복실행방지
//    close();
}

void numpad::toggle_plus_minus(){
    closecnt=0;
    if (VALUE->left(1)=="-"){
        *VALUE=VALUE->right(VALUE->size()-1);
        NUMPAD->TextEdit->setText(*VALUE);
    }
    else{
        VALUE->prepend("-");
        NUMPAD->TextEdit->setText(*VALUE);
    }
    emit value_changed(get_Value());
}
float numpad::get_Value(){
    return VALUE->toDouble();
}

void numpad::clear_value(){
    closecnt=0;
    VALUE->clear();
    dotPoint=0;
    dotCnt=0;
    NUMPAD->QPB_POINT->setDisabled(false);
//    NUMPAD->TextEdit->setText(*VALUE);


    edit_btn->setText(*VALUE);      // 카운터용

    emit value_changed(get_Value());
}
void numpad::back_value(){
    closecnt=0;
    if(VALUE->right(1)== "."){
        dotPoint =0;
        NUMPAD->QPB_POINT->setDisabled(dotPoint);
    }
    if(dotPoint){                            // 소수점 입력 후에는 소수점 갯수 - 카운트
        dotCnt--;
    }
    *VALUE=VALUE->left(VALUE->size()-1);
//    NUMPAD->TextEdit->setText(*VALUE);

    edit_btn->setText(*VALUE);              // 카운터용


    emit value_changed(get_Value());
}

void numpad::esc_value()
{
    closecnt=0;
    dotPoint=0;
    dotCnt=0;
    NUMPAD->QPB_POINT->setDisabled(false);

    VALUE->clear();
    //    edit_value->setEnabled(true);
    edit_value->setStyleSheet("background-color: white; selection-background-color: white; selection-color: black;");
    edit_value->setStatusTip("not used");  // 중복실행방지
    pad_timer->stop();
    close();
}

// numpad display ui show & hide motion check
void numpad::check_pad_timer()
{
    closecnt++;
    QPoint p = QCursor::pos();
    if(closecnt == 50){
        QCursor a;
        a.setPos(0,0);
    }
    if( (
                ( p.rx() > pad_x && p.rx() < pad_x+pad_rect.width())
                &&  ( p.ry() > pad_y && p.ry() < pad_y+pad_rect.height())   //if numpad touch
                )
            ||
            (
                ( p.rx() > pEdit_pos.rx() - EDIT_MARGIN && p.rx() < pEdit_pos.rx() + EDIT_MARGIN + pEdit_rect.width())
                &&  ( p.ry() > pEdit_pos.ry() - EDIT_MARGIN && p.ry() < pEdit_pos.ry() + EDIT_MARGIN + pEdit_rect.height()) //if lineedit touch
                )
            ){
        //null
    }
    else{
        if(p.rx()<0||p.rx()>764||p.ry()<0||p.ry()>1024){ //마우스 팅김 예외처리
            return;
        }
        esc_value();
    }
}

//init numpad data
void numpad::change_text(QLineEdit *pEdit,int m_with,int m_height)
{
    if(edit_value!=pEdit){
        qDebug() << "escvalue";
        this->esc_value();
    }
    NUMPAD->TextEdit->setObjectName(pEdit->objectName());
    insert = 0;

    show();

    old_data = pEdit->text();
//	save_data = pEdit->value();
//    NUMPAD->TextEdit->setText(QString("%1").arg(save_data));
    QPoint init(0,0);
    pEdit_pos = pEdit->mapToGlobal(init);   // pEdit for Global Position from init(0,0)
    pEdit_rect=pEdit->contentsRect();
    pad_rect=rect();

    pad_x = pEdit_pos.rx() ;   // Frame Location Margin
    pad_y = pEdit_pos.ry() + pEdit_rect.height();
    if(pad_x + pad_rect.width() > m_with)   pad_x = pad_x-(pad_x+pad_rect.width()-m_with);
    if( pad_y + pad_rect.height()> m_height)  pad_y =  pad_y-(pEdit_rect.height()+pad_rect.height());

    setGeometry( pad_x, pad_y,pad_rect.width(),pad_rect.height());

    // init numpad data
    edit_value = pEdit;

    VALUE->append(edit_value->text());
    NUMPAD->TextEdit->setText(*VALUE);
    VALUE->clear();

    edit_value->setStatusTip("used");   // SelectionChanged 중복실행방지
    QString help = edit_value->whatsThis();
    NUMPAD->label->setText(help);
    edit_value->setStyleSheet("background-color: red;  color:black; selection-background-color: red; selection-color: black;");
    // edit_value->setEnabled(false);
    closecnt=0;
//    pad_timer->start(200);  //mouse pos check timer
}
