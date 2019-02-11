#ifndef PAGE_SIGNIMAGE_H
#define PAGE_SIGNIMAGE_H

#include <QWidget>

#include <QColor>
#include <QImage>
#include <QPainterPath>
#include <QWidget>
#include "class/paintarea.h"

class paintArea;

namespace Ui {
class page_signImage;
}

class page_signImage : public QWidget
{
    Q_OBJECT

public:
    explicit page_signImage(QWidget *parent = 0);
    ~page_signImage();

    void print_personalReceipt(QString imagePath);
    void lang_change();
protected:

private slots:
//    void on_pushButton_2_clicked();

    void on_btn_complete_clicked();

    void on_btn_clear_clicked();

private:
    PaintArea *paintArea;

private:
    Ui::page_signImage *ui;
};

#endif // PAGE_SIGNIMAGE_H
