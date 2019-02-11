/*
 * keypad.cpp
 *
 *  Created on: 2014. 2. 17.
 *      Author: Woojin Plaimm Research Institute
 */
#include "keypad.h"
#include "ui_keypad.h"
#include <QDebug>

#include <qtextcodec.h>

keypad* key_pad=NULL;
#define EDIT_MARGIN 5

keypad::keypad(QWidget *parent) :
    QDialog(parent,Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint),
    KEYPAD(new Ui::keypad)
{
    hangulMata =new automata();

    cursorpos=0;
    VALUE=new QString("");
    if (parent!=0){
        KEYPAD->setupUi(parent);
    }
    else{
        KEYPAD->setupUi(this);
    }
    //set operation event button
    QObject::connect(KEYPAD->PB_x0,SIGNAL(clicked()),this,SLOT(etc_append()));
    QObject::connect(KEYPAD->PB_x1,SIGNAL(clicked()),this,SLOT(etc_append()));
    QObject::connect(KEYPAD->PB_x2,SIGNAL(clicked()),this,SLOT(etc_append()));
    QObject::connect(KEYPAD->PB_x3,SIGNAL(clicked()),this,SLOT(etc_append()));
    QObject::connect(KEYPAD->PB_x4,SIGNAL(clicked()),this,SLOT(etc_append()));

    QObject::connect(KEYPAD->PB_0,SIGNAL(clicked()),this,SLOT(etc_append()));
    QObject::connect(KEYPAD->PB_1,SIGNAL(clicked()),this,SLOT(etc_append()));
    QObject::connect(KEYPAD->PB_2,SIGNAL(clicked()),this,SLOT(etc_append()));
    QObject::connect(KEYPAD->PB_3,SIGNAL(clicked()),this,SLOT(etc_append()));
    QObject::connect(KEYPAD->PB_4,SIGNAL(clicked()),this,SLOT(etc_append()));
    QObject::connect(KEYPAD->PB_5,SIGNAL(clicked()),this,SLOT(etc_append()));
    QObject::connect(KEYPAD->PB_6,SIGNAL(clicked()),this,SLOT(etc_append()));
    QObject::connect(KEYPAD->PB_7,SIGNAL(clicked()),this,SLOT(etc_append()));
    QObject::connect(KEYPAD->PB_8,SIGNAL(clicked()),this,SLOT(etc_append()));
    QObject::connect(KEYPAD->PB_9,SIGNAL(clicked()),this,SLOT(etc_append()));

    QObject::connect(KEYPAD->PB_q,SIGNAL(clicked()),this,SLOT(append()));
    QObject::connect(KEYPAD->PB_w,SIGNAL(clicked()),this,SLOT(append()));
    QObject::connect(KEYPAD->PB_e,SIGNAL(clicked()),this,SLOT(append()));
    QObject::connect(KEYPAD->PB_r,SIGNAL(clicked()),this,SLOT(append()));
    QObject::connect(KEYPAD->PB_t,SIGNAL(clicked()),this,SLOT(append()));
    QObject::connect(KEYPAD->PB_y,SIGNAL(clicked()),this,SLOT(append()));
    QObject::connect(KEYPAD->PB_u,SIGNAL(clicked()),this,SLOT(append()));
    QObject::connect(KEYPAD->PB_i,SIGNAL(clicked()),this,SLOT(append()));
    QObject::connect(KEYPAD->PB_o,SIGNAL(clicked()),this,SLOT(append()));
    QObject::connect(KEYPAD->PB_p,SIGNAL(clicked()),this,SLOT(append()));

    QObject::connect(KEYPAD->PB_a,SIGNAL(clicked()),this,SLOT(append()));
    QObject::connect(KEYPAD->PB_s,SIGNAL(clicked()),this,SLOT(append()));
    QObject::connect(KEYPAD->PB_d,SIGNAL(clicked()),this,SLOT(append()));
    QObject::connect(KEYPAD->PB_f,SIGNAL(clicked()),this,SLOT(append()));
    QObject::connect(KEYPAD->PB_g,SIGNAL(clicked()),this,SLOT(append()));
    QObject::connect(KEYPAD->PB_h,SIGNAL(clicked()),this,SLOT(append()));
    QObject::connect(KEYPAD->PB_j,SIGNAL(clicked()),this,SLOT(append()));
    QObject::connect(KEYPAD->PB_k,SIGNAL(clicked()),this,SLOT(append()));
    QObject::connect(KEYPAD->PB_l,SIGNAL(clicked()),this,SLOT(append()));

    QObject::connect(KEYPAD->PB_z,SIGNAL(clicked()),this,SLOT(append()));
    QObject::connect(KEYPAD->PB_x,SIGNAL(clicked()),this,SLOT(append()));
    QObject::connect(KEYPAD->PB_c,SIGNAL(clicked()),this,SLOT(append()));
    QObject::connect(KEYPAD->PB_v,SIGNAL(clicked()),this,SLOT(append()));
    QObject::connect(KEYPAD->PB_b,SIGNAL(clicked()),this,SLOT(append()));
    QObject::connect(KEYPAD->PB_n,SIGNAL(clicked()),this,SLOT(append()));
    QObject::connect(KEYPAD->PB_m,SIGNAL(clicked()),this,SLOT(append()));

    QObject::connect(KEYPAD->PB_enter,SIGNAL(clicked()),this,SLOT(push_enter()));
    QObject::connect(KEYPAD->PB_back,SIGNAL(clicked()),this,SLOT(push_backspace()));
    QObject::connect(KEYPAD->PB_copy,SIGNAL(clicked()),this,SLOT(push_copy()));
    QObject::connect(KEYPAD->PB_paste,SIGNAL(clicked()),this,SLOT(push_paste()));
    QObject::connect(KEYPAD->PB_clear,SIGNAL(clicked()),this,SLOT(push_clear()));


    //set palette for background color
    palette = new QPalette();
    copy_data.clear();
    //keypad mouse position check timer set
    key_pad_timer = new QTimer(this);
    connect(key_pad_timer , SIGNAL(timeout()), this, SLOT(check_key_pad_timer()));
    old_data.clear();
}

//get text data and display edit
void keypad::append(){
    qDebug() << "append";
    QPushButton* push = qobject_cast<QPushButton*>(sender());

    QString sendStr;
    if(KEYPAD->PB_tr->isChecked()){                 // 한글 입력이면 오토마타 사용
        if(KEYPAD->PB_shift->isChecked()){
            sendStr = push->statusTip();
            hangulMata->SetKeyCode(sendStr.toInt());
            qDebug() << "append text:  " << sendStr;
        }
        else{
            sendStr = push->toolTip();
            hangulMata->SetKeyCode(push->toolTip().toInt());
            qDebug() << "append text:  " << sendStr;
        }
//        const char * str = hangulMata->completeText.toStdString().c_str();
//        qDebug() << QString::fromUtf8(str);
//        QString strText = QString::fromUtf8(str);
//        unsigned int test =0xAC00;
//        qDebug() << QString::fromUcs4(&test, 1);

//        QTextCodec * codec = QTextCodec::codecForName("eucKR");
//        QString strText = codec->toUnicode(hangulMata->completeText);
//         const char * str = hangulMata->completeText.toStdString().c_str();
//        QString strText   = QString::fromUtf8("0AC",sizeof(str));
//        qDebug() << strText;

         if(hangulMata->m_completeWord != NULL)
         {
             qDebug() << "compword not null";

             QString strText = QString::number(hangulMata->m_completeWord.toInt(),16).toUpper().prepend("0x");
             unsigned int test = strText.toUInt(0,16);
//             qDebug() << test;
             if(test == 0){
                 VALUE->append(hangulMata->m_completeWord);
             }
             else{
                 VALUE->append(QString::fromUcs4(&test,1));
             }
         }
         if(hangulMata->ingWord != NULL)
         {
             QString strText = QString::number(hangulMata->ingWord.toInt(),16).toUpper().prepend("0x");
             unsigned int test = strText.toUInt(0,16);
//             qDebug() << test;
             if(test == 0){
                 KEYPAD->lineEdit->setText(*VALUE+hangulMata->ingWord);
             }
             else{
                 KEYPAD->lineEdit->setText(*VALUE+QString::fromUcs4(&test,1));
             }
         }
         else{
             KEYPAD->lineEdit->setText(*VALUE);
         }
    }
    else{
        closecnt = 0;
        //get current event button text

        if(push->text() =="&&")
        {
            VALUE->append("&");
        }
        else{
            VALUE->insert(cursorpos , push->text());
        }
        KEYPAD->lineEdit->setText(*VALUE);
        KEYPAD->lineEdit->setCursorPosition(VALUE->length());
    }
}

void keypad::etc_append(){

    QPushButton* push = qobject_cast<QPushButton*>(sender());
    closecnt = 0;
    //get current event button text

    if(push->text() =="&&")
    {
        VALUE->append("&");
    }
    else{
        VALUE->insert(cursorpos , push->text());
    }
    KEYPAD->lineEdit->setText(*VALUE);
    KEYPAD->lineEdit->setCursorPosition(VALUE->length());
}
void keypad::push_enter(){
    closecnt = 0;
    edit_value->setText(KEYPAD->lineEdit->text());
//    edit_value->setStyleSheet("background-color: white; color: rgb(0, 0, 0); selection-background-color: white; selection-color: black;");
    edit_value->setStatusTip("not used"); // 중복실행방지
    key_pad_timer->stop();
    KEYPAD->lineEdit->setText("");
    KEYPAD->lineEdit->setEchoMode(QLineEdit::Normal);

    this->hide();
}

void keypad::push_backspace()
{
    closecnt = 0;
    if(KEYPAD->PB_tr->isChecked()){                 // 한글 입력이면 오토마타 사용
        hangulMata->completeText = *VALUE;
        hangulMata->SetKeyCode(-3);
        *VALUE = hangulMata->completeText;
    }
    else{
        VALUE->remove(cursorpos-1,1);
        int savecursor= cursorpos-1;
        KEYPAD->lineEdit->setText(*VALUE);
        KEYPAD->lineEdit->setCursorPosition(savecursor);
    }
}

void keypad::push_copy()
{
    closecnt = 0;
    copy_data = *VALUE;
}

void keypad::push_paste()
{
    closecnt = 0;
    VALUE->insert(cursorpos , copy_data);
    KEYPAD->lineEdit->setText(*VALUE);
}

void keypad::push_clear()
{
    closecnt = 0;
    hangulMata->Clear();
    VALUE->clear();
    KEYPAD->lineEdit->setText(*VALUE);
}

void keypad::push_esc()
{
    QPoint p = QCursor::pos();
    closecnt = 0;
    KEYPAD->lineEdit->setText("");
    edit_value->setStyleSheet("background-color: white; color: rgb(0, 0, 0); selection-background-color: white; selection-color: black;");
    edit_value->setStatusTip("not used");  // 중복실행방지
    key_pad_timer->stop();
    KEYPAD->lineEdit->setEchoMode(QLineEdit::Normal);
    this->close();
}
void keypad::change_text(QLineEdit *pEdit,int m_width,int m_height)
{
//    if(pEdit->echoMode() == QLineEdit::Password){   // 비번입력 에딧트에 대한 예외처리
//        KEYPAD->lineEdit->setEchoMode(QLineEdit::Password);
//    }
    KEYPAD->lineEdit->setEnabled(false);
    VALUE->clear();
    old_data = pEdit->text();
    KEYPAD->lineEdit->setText(old_data);
    VALUE->append(old_data);

    //    //set keypad dialog position
    QPoint init(0,0);
    pEdit_pos = pEdit->mapToGlobal(init);
    pEdit_rect=pEdit->contentsRect();
    pad_rect=rect();

//    pad_x = pEdit_pos.rx() + pEdit_rect.width();
//    pad_y = pEdit_pos.ry() + pEdit_rect.height();
//    if(pad_x + pad_rect.width() > m_width)   pad_x = pad_x-(pad_x+pad_rect.width()-m_width);
//    if( pad_y + pad_rect.height()> m_height)  pad_y =  pad_y-(pEdit_rect.height()+pad_rect.height());

//    setGeometry( pad_x, pad_y,pad_rect.width(),pad_rect.height());
    edit_value = pEdit;
    edit_value->setStyleSheet("background-color: green; color: rgb(0, 0, 0); selection-background-color: green; selection-color: black;");
    closecnt = 0;
//    key_pad_timer->start(200);  //mouse pos check timer
    edit_value->setStatusTip("used");   // SelectionChanged 중복실행방지
    KEYPAD->lineEdit->setEnabled(true);
    exec();
}

void keypad::open_relativePad(QLineEdit *pEdit,QWidget *pMain)
{
    if(pEdit->echoMode() == QLineEdit::Password){   // 비번입력 에딧트에 대한 예외처리
        KEYPAD->lineEdit->setEchoMode(QLineEdit::Password);
    }
    KEYPAD->lineEdit->setEnabled(false);
    VALUE->clear();
    old_data = pEdit->text();
    KEYPAD->lineEdit->setText(old_data);
    VALUE->append(old_data);

//    qDebug() << pMain->cursor().pos();

    //    //set keypad dialog position
    QPoint init(0,0);
    pEdit_pos = pEdit->mapToGlobal(init);
    pEdit_rect=pEdit->contentsRect();
    pad_rect=rect();
    pad_x = pEdit_pos.rx() + pEdit_rect.width();
    pad_y = pEdit_pos.ry() + pEdit_rect.height();


//    setGeometry( 0, 100,pad_rect.width(),pad_rect.height());


    edit_value = pEdit;
//    edit_value->setStyleSheet("background-color: green; color: rgb(0, 0, 0); selection-background-color: green; selection-color: black;");
    closecnt = 0;
//    key_pad_timer->start(500);  //mouse pos check timer
    edit_value->setStatusTip("used");   // SelectionChanged 중복실행방지
    KEYPAD->lineEdit->setEnabled(true);
//    this->setModal(true);
//    this->activateWindow();
//    this->raise();
    this->setParent(pMain);
    exec();
}

void keypad::open_pad(QLineEdit *pEdit, QPushButton *pBtn ,int m_width,int m_height)
{
    KEYPAD->lineEdit->setEnabled(false);
    VALUE->clear();
    old_data = pEdit->text();
    KEYPAD->lineEdit->setText(old_data);
    VALUE->append(old_data);
    //    //set keypad dialog position
    QPoint init(0,0);
    pEdit_pos = pBtn->mapToGlobal(init);
    pEdit_rect = pBtn->contentsRect();
    pad_rect=rect();

    pad_x = pEdit_pos.rx() + pEdit_rect.width();
    pad_y = pEdit_pos.ry() + pEdit_rect.height();
    if(pad_x + pad_rect.width() > m_width)   pad_x = pad_x-(pad_x+pad_rect.width()-m_width);
    if( pad_y + pad_rect.height()> m_height)  pad_y =  pad_y-(pEdit_rect.height()+pad_rect.height());

    setGeometry( pad_x, pad_y,pad_rect.width(),pad_rect.height());
    edit_value = pEdit;
    edit_value->setStyleSheet("background-color: green; color: rgb(0, 0, 0); selection-background-color: green; selection-color: black;");
    closecnt = 0;
//    key_pad_timer->start(200);  //mouse pos check timer
    edit_value->setStatusTip("used");   // SelectionChanged 중복실행방지
    KEYPAD->lineEdit->setEnabled(true);
    exec();
}

keypad::~keypad()
{
    delete KEYPAD;
}

void keypad::on_PB_shift_clicked(bool checked)
{
    if(checked){
        if(KEYPAD->PB_tr->isChecked()){
            on_changeKeyboardText(KOREAN_S);
        }
        else{
            on_changeKeyboardText(ENGLISH_S);
        }
    }
    else{
        if(KEYPAD->PB_tr->isChecked()){
            on_changeKeyboardText(KOREAN);
        }
        else{
            on_changeKeyboardText(ENGLISH);
        }
    }

    switch(checked)
    {
    case 0 :
        break;
    case 1:

        break;
    }
}
// numpad display ui show & hide motion check
void keypad::check_key_pad_timer()
{
    closecnt++;
    QPoint p = QCursor::pos();
    QCursor a;
//    qDebug() << p << rect();
    if(closecnt == 50){
        a.setPos(0,0);
    }
    if( ( p.rx() > 10 && p.rx() < 10+pad_rect.width())
         &&  ( p.ry() > 10 && p.ry() < 10+pad_rect.height()) ){  //if numpad touch
    }
    else{
        if(p.rx()<0||p.rx()>800||p.ry()<0||p.ry()>480){//마우스 팅김 예외처리
            return;
        }
//        push_esc();
    }
//    if(p.rx() >
//            ){
//        push_esc();
//    }
}


void keypad::on_lineEdit_cursorPositionChanged(int arg1, int arg2)
{
    cursorpos=arg2;
}

void keypad::on_PB_tr_clicked(bool checked)
{
    if(checked){
        if(KEYPAD->PB_shift->isChecked()){
            on_changeKeyboardText(KOREAN_S);
        }
        else{
            on_changeKeyboardText(KOREAN);
        }
    }
    else{
        if(KEYPAD->PB_shift->isChecked()){
            on_changeKeyboardText(ENGLISH_S);
        }
        else{
            on_changeKeyboardText(ENGLISH);
        }
    }
}

void keypad::on_changeKeyboardText(int type)
{
    switch(type){
    case ENGLISH:
        KEYPAD->PB_q->setText("q");
        KEYPAD->PB_w->setText("w");
        KEYPAD->PB_e->setText("e");
        KEYPAD->PB_r->setText("r");
        KEYPAD->PB_t->setText("t");
        KEYPAD->PB_y->setText("y");
        KEYPAD->PB_u->setText("u");
        KEYPAD->PB_i->setText("i");
        KEYPAD->PB_o->setText("o");
        KEYPAD->PB_p->setText("p");

        KEYPAD->PB_a->setText("a");
        KEYPAD->PB_s->setText("s");
        KEYPAD->PB_d->setText("d");
        KEYPAD->PB_f->setText("f");
        KEYPAD->PB_g->setText("g");
        KEYPAD->PB_h->setText("h");
        KEYPAD->PB_j->setText("j");
        KEYPAD->PB_k->setText("k");
        KEYPAD->PB_l->setText("l");

        KEYPAD->PB_z->setText("z");
        KEYPAD->PB_x->setText("x");
        KEYPAD->PB_c->setText("c");
        KEYPAD->PB_v->setText("v");
        KEYPAD->PB_b->setText("b");
        KEYPAD->PB_n->setText("n");
        KEYPAD->PB_m->setText("m");
        break;
    case ENGLISH_S:
        KEYPAD->PB_q->setText("Q");
        KEYPAD->PB_w->setText("W");
        KEYPAD->PB_e->setText("E");
        KEYPAD->PB_r->setText("R");
        KEYPAD->PB_t->setText("T");
        KEYPAD->PB_y->setText("Y");
        KEYPAD->PB_u->setText("U");
        KEYPAD->PB_i->setText("I");
        KEYPAD->PB_o->setText("O");
        KEYPAD->PB_p->setText("P");

        KEYPAD->PB_a->setText("A");
        KEYPAD->PB_s->setText("S");
        KEYPAD->PB_d->setText("D");
        KEYPAD->PB_f->setText("F");
        KEYPAD->PB_g->setText("G");
        KEYPAD->PB_h->setText("H");
        KEYPAD->PB_j->setText("J");
        KEYPAD->PB_k->setText("K");
        KEYPAD->PB_l->setText("L");

        KEYPAD->PB_z->setText("Z");
        KEYPAD->PB_x->setText("X");
        KEYPAD->PB_c->setText("C");
        KEYPAD->PB_v->setText("V");
        KEYPAD->PB_b->setText("B");
        KEYPAD->PB_n->setText("N");
        KEYPAD->PB_m->setText("M");
        break;
    case KOREAN:
        KEYPAD->PB_q->setText("ㅂ");
        KEYPAD->PB_w->setText("ㅈ");
        KEYPAD->PB_e->setText("ㄷ");
        KEYPAD->PB_r->setText("ㄱ");
        KEYPAD->PB_t->setText("ㅅ");
        KEYPAD->PB_y->setText("ㅛ");
        KEYPAD->PB_u->setText("ㅕ");
        KEYPAD->PB_i->setText("ㅑ");
        KEYPAD->PB_o->setText("ㅐ");
        KEYPAD->PB_p->setText("ㅔ");

        KEYPAD->PB_a->setText("ㅁ");
        KEYPAD->PB_s->setText("ㄴ");
        KEYPAD->PB_d->setText("ㅇ");
        KEYPAD->PB_f->setText("ㄹ");
        KEYPAD->PB_g->setText("ㅎ");
        KEYPAD->PB_h->setText("ㅗ");
        KEYPAD->PB_j->setText("ㅓ");
        KEYPAD->PB_k->setText("ㅏ");
        KEYPAD->PB_l->setText("ㅣ");

        KEYPAD->PB_z->setText("ㅋ");
        KEYPAD->PB_x->setText("ㅌ");
        KEYPAD->PB_c->setText("ㅊ");
        KEYPAD->PB_v->setText("ㅍ");
        KEYPAD->PB_b->setText("ㅠ");
        KEYPAD->PB_n->setText("ㅜ");
        KEYPAD->PB_m->setText("ㅡ");
        break;
    case KOREAN_S:
        KEYPAD->PB_q->setText("ㅃ");
        KEYPAD->PB_w->setText("ㅉ");
        KEYPAD->PB_e->setText("ㄸ");
        KEYPAD->PB_r->setText("ㄲ");
        KEYPAD->PB_t->setText("ㅆ");
        KEYPAD->PB_y->setText("ㅛ");
        KEYPAD->PB_u->setText("ㅕ");
        KEYPAD->PB_i->setText("ㅑ");
        KEYPAD->PB_o->setText("ㅒ");
        KEYPAD->PB_p->setText("ㅖ");

        KEYPAD->PB_a->setText("ㅁ");
        KEYPAD->PB_s->setText("ㄴ");
        KEYPAD->PB_d->setText("ㅇ");
        KEYPAD->PB_f->setText("ㄹ");
        KEYPAD->PB_g->setText("ㅎ");
        KEYPAD->PB_h->setText("ㅗ");
        KEYPAD->PB_j->setText("ㅓ");
        KEYPAD->PB_k->setText("ㅏ");
        KEYPAD->PB_l->setText("ㅣ");

        KEYPAD->PB_z->setText("ㅋ");
        KEYPAD->PB_x->setText("ㅌ");
        KEYPAD->PB_c->setText("ㅊ");
        KEYPAD->PB_v->setText("ㅍ");
        KEYPAD->PB_b->setText("ㅠ");
        KEYPAD->PB_n->setText("ㅜ");
        KEYPAD->PB_m->setText("ㅡ");
        break;
    }
}
