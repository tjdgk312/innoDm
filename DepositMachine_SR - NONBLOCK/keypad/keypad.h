#ifndef KEYPAD_H
#define KEYPAD_H

#include <QDialog>
#include "qlineedit.h"
#include "qtimer.h"
#include "automata/automata.h"

#define ENGLISH  0
#define ENGLISH_S  1
#define KOREAN  2
#define KOREAN_S  3

namespace Ui {
class keypad;
}

class keypad : public QDialog
{
    Q_OBJECT
    
public:
    explicit keypad(QWidget *parent = 0);
    ~keypad();
    
protected:

public:
    automata  *hangulMata;

    QString     *VALUE;
    QLineEdit   *edit_value;
    QPalette    *palette;

    QRect pad_rect;
    int pad_x;
    int pad_y;

    QPoint pEdit_pos;
    QRect  pEdit_rect;

    QPoint pMain_pos;
    QRect  pMain_rect;

    QString copy_data;
    QString old_data;
    int cursorpos;
    int closecnt;
public:
    void change_text(QLineEdit *pEdit, int m_width, int m_height);

    Ui::keypad *KEYPAD;
    QTimer  *key_pad_timer;

    void open_pad(QLineEdit *pEdit, QPushButton *pBtn, int m_width, int m_height);

    void open_relativePad(QLineEdit *pEdit, QWidget *pMain);
public slots:
    void push_esc();

private slots:
    void append();
    void etc_append();
    void push_enter();
    void push_backspace();
    void push_copy();
    void push_paste();
    void push_clear();
    void check_key_pad_timer();

    void on_lineEdit_cursorPositionChanged(int arg1, int arg2);
    void on_PB_shift_clicked(bool checked);
    void on_PB_tr_clicked(bool checked);
    void on_changeKeyboardText(int type);
};

#endif // KEYPAD_H
