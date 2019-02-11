/*
 *   Copyright 20012 Maxime Haselbauer <maxime.haselbauer@googlemail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */


#ifndef NUMPAD_H
#define NUMPAD_H

#include <QWidget>
#include "qlineedit.h"
#include "qspinbox.h"
#include "qtimer.h"

#include <QPushButton>
namespace Ui {
    class numpad;
}

class numpad : public QWidget
{
    Q_OBJECT

public:
    explicit numpad(QWidget *parent = 0);
    ~numpad();
    float get_Value();
    int insert;
    int dotPoint;
    int dotCnt;

    int neventFlag;
    int nclickFlag;
    int closecnt;

protected:
     //bool eventFilter(QObject *obj, QEvent *ev);

public:

  signals:
    void value_changed(double);

public:
    Ui::numpad  *NUMPAD;
    QString     *VALUE;
    QLineEdit   *edit_value;

    QLineEdit *edit_counter;

    QPushButton *edit_btn;          // selection changed 이벤트 죨라안먹힘

    //QSpinBox    *edit_value2;
    QPalette    *palette;

    QIntValidator* qval;

    QRect pad_rect;
    int pad_x;
    int pad_y;

    QPoint pEdit_pos;
    QRect  pEdit_rect;
    QTimer  *pad_timer;
    QString old_data;
    double save_data;
public slots:
    void esc_value();

private slots:
    void append();
    void append_enter();
    void toggle_plus_minus();
    void clear_value();
    void back_value();

    void check_pad_timer();

  public:
//    void change_text(QLineEdit* pEdit,int m_with,int m_height);
    void change_text(QLineEdit *pEdit, int m_with, int m_height);
    //void change_text2(QSpinBox* pEdit,int m_with,int m_height);
};

#endif // NUMPAD_H
