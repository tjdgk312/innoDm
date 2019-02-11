#include "page_signimage.h"
#include "ui_page_signimage.h"

#include "global.h"

//#include "interfaces.h"
//#include "paintarea.h"

#include <QPainter>
#include <QMouseEvent>

#include <QFileDialog>
#include <QMessageBox>

page_signImage::page_signImage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::page_signImage)
{
    ui->setupUi(this);

    paintArea = new PaintArea();
    ui->scrollArea->setBackgroundRole(QPalette::Dark);
    ui->scrollArea->setWidget(paintArea);
}

page_signImage::~page_signImage()
{
    delete ui;
}

//void page_signImage::on_pushButton_2_clicked()
//{
//    const QString fileName = QFileDialog::getOpenFileName(this,
//                                                          tr("Open File"),
//                                                          QDir::currentPath());
//    if (!fileName.isEmpty()) {
//        if (!paintArea->openImage(fileName)) {
//            QMessageBox::information(this, tr("Plug & Paint"),
//                                     tr("Cannot load %1.").arg(fileName));
//            return;
//        }
//        paintArea->adjustSize();
//    }
//}

void page_signImage::on_btn_complete_clicked()
{
//    QString pathImage = QString(PATH_ROOT+"%1.bmp").arg("test");
//    paintArea->saveImage(pathImage, "bmp");
    p_Main->pLogin->assignInfo = 5;     // 5로 먼저 바꾸고 페이지 전환 해야 함
    p_Main->select_Page(SEARCHPAGE);
//    p_Main->curEventStr->giftCash
//    print_personalReceipt(pathImage);       // 축의금 영수증
//    paintArea->clearImage();
}

void page_signImage::print_personalReceipt(QString imagePath){
    p_Main->printFlag = 0;

    QTextCodec* codec = QTextCodec::codecForName("eucKR");

    InitPrinterStatus();


    // unsigned char changeFont[] = {0x1b, 0x74, 0x00};
    // ControlCommand(changeFont,3);
    //    BmpSaveSpool(p_Main->printerlib,"://image/LOGO.png");
    SetCharCodeTable(255,2);


    InitPageMode(0,0,_2INCH,300);       // 인치 및 길이설정

    Page_SetStandardMode();

//    SetFontSize(0);
    SetTextStyle(false,true,1,1,false);
    SetTextAlignment(1);
    QString tmp = "축의금 영수증";
    QByteArray tmpByte = codec->fromUnicode(tmp);
//    tmp.toU
//    QByteArray tmpByte = codec->toU;
//    QByteArray tmpByte = codec->fromUnicode(tmp);
    TextSaveSpool(tmpByte.data());  Page_Newline();
    Page_DotFeed(15);

//    SetFontSize(1);
//    SetTextAlignment(2);
//    tmp = "입금기록\r\n";
//    SetTextStyle(false,false,1,1,false);
//    TextSaveSpool(codec->fromUnicode(tmp).data());
                                                                          // 출력 방향으로 dots만큼 이동하여 새로운 라인을 만들고 시작 지점에 쓰기 위치를 이동합니다.
    SetTextStyle(false,true,0,0,false);
    SetFontSize(0);
//    Page_SetPosition(147,105);                                                                  // Page mode에서 출력하려는 데이터의 위치를 설정합니다.
    SetTextAlignment(2);
    TextSaveSpool(QString("%1\r\n").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).toUtf8().data());
//    SetFontSize(0);

//    Page_SetPosition(320,5);                                                                     //  Page mode에서 데이터의 시작위치를 설정합니다. dot 위치 지정

    //-----------------------------------------------------------------------------
    // Page_DrawBox( int iXPos, int iYPos, int width, int height, int thickness )   출력 가능한 직선 또는 사각형 이미지 데이터를 작성하고, printing buffer에 저장합니다.
    SetTextAlignment(0);
    Page_DrawBox(0,130,_2INCH,0,2);   Page_Newline();                                                                  //  "----------------------------------------------------------------------------"
    Page_DotFeed(15);
    tmp = "입금액:";
    TextSaveSpool(codec->fromUnicode(tmp).data());

    SetTextAlignment(2);
    tmp = QString("%L1 원").arg(p_Main->pResult->curDeposit.sumTotal,0,'f',0,' ');
    TextSaveSpool(codec->fromUnicode(tmp).data());

//    SetTextAlignment(0);
//    Page_DotFeed(10);
//    Page_DrawBox(0,270,_2INCH,0,2);	Page_Newline();


    //    else if(flag == 1){
    //        Page_DrawBox(0,130,_2INCH,0,2);
    //        Page_DotFeed(5);
    //        TextSaveSpool("     #KOR        Amount\r\n");
    //        TextSaveSpool(QString(trUtf8("     Charge   %1")).arg(ui->edit_total->text()).toUtf8().data());	Page_DotFeed(10);
    //        TextSaveSpool(QString(trUtf8("     Deposit  %1")).arg(ui->edit_count->text()).toUtf8().data());	Page_Newline();
    //        TextSaveSpool(QString(trUtf8("     Balance  %1")).arg(ui->edit_balance->text()).toUtf8().data());	Page_Newline();
    //        Page_DrawBox(0,270,_2INCH,0,2);	Page_Newline();
    //    }
    //    else if(flag == 2){
    //        Page_DrawBox(0,130,_2INCH,0,2);
    //        Page_DotFeed(5);
    //        SetTextStyle(false,true,2,2,false);
    //        TextSaveSpool(trUtf8("  식권\r\n").toLocal8Bit().data()); Page_Newline();
    //        SetFontSize(10);
    //        TextSaveSpool(QString(trUtf8("   %1   ")).arg(number).toUtf8().data());
    //        Page_Newline();
    //    }
    Page_Print_StandardMode();

//    SetFontSize(0);
//    Page_DrawBox(0,310,115,0,2); Page_DrawBox(260,310,124,0,2);
//    Page_Newline();
//    Page_SetPosition(123,300);
//    TextSaveSpool("User Information\r\n");
//    Page_Newline();
//    Page_SetPosition(0,350);

//    SetPageMode();
    SetTextAlignment(1);
    int result = BmpSaveSpool(p_Main->printerlib,"/home/pi/INNOSTEC/test.bmp");  //  이미지를 버퍼에 추가

//    qDebug() << "print sign Image return : " << result;

    Page_Newline();
    SetFontSize(0);

//    Page_DrawBox(0,400,120,0,2);	Page_DrawBox(250,400,134,0,2);Page_DotFeed(15);
    //    TextSaveSpool("Sum Total\r\n");

    //    TextSaveSpool("Total:                RD$$ 220");	Page_Newline();
    //    TextSaveSpool("ETC Total:            RD$$   0");	Page_Newline();

    //    Page_DrawBox(0,490,_2INCH,0,2);	Page_DotFeed(15);
    //    TextSaveSpool("Total:                  $ 9.50");	Page_Newline();
    //    TextSaveSpool("Credit Card:            $ 9.50");	Page_Newline();

//    SetTextAlignment(1);

    Page_SetPosition(145,390);
    OneDimensionBarcodeSaveSpool(CODE128,2,50,TRUE,"20150127123456789");
    Page_LineFeed(1);

    Page_Print_StandardMode();

    SetTextAlignment(1);
    TextSaveSpool("www.innostec.co.kr\r\n\r\n\r\n\r\n\r\n");

    FeedToMark();
    CutPaper(0);

    PrintSpool(p_Main->printerlib,TRUE);

    GetPrinterStatus(p_Main->printerlib, 1000);
}

void page_signImage::lang_change()
{
    ui->retranslateUi(this);
}

void page_signImage::on_btn_clear_clicked()
{
    paintArea->clearImage();
}
