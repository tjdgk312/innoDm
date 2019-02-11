#include "receipt.h"
#include "definition.h"
#include "device/woosimprinter.h"

#include <QDebug>
#include <QBasicTimer>

receipt::receipt(QObject *parent) : QObject(parent)
{
//    m_txFormatData.resize(1);

//    m_txFormatData[0] = 0x32;



    poll_init();

//    printerlib  = woosimlib_open("/dev/ttyS0", 57600, 8, 'n', 1);
    printerlib  = woosimlib_open("/dev/usb/lp0", 57600, 8, 'n', 1);
    if ( NULL == printerlib)
    {
        printf( "error = %d\n", error_no);
    }
    printerlib->on_error = on_error;
    printerlib->on_read  = on_read;
}

/*
//sensor check
int iPaperSensor;
int iCoverSensor;
int iMarkSensor;
int iCutterStatus;
int iFrontSensor;
*/

void receipt::on_error( woosimlib *sender, int reopen_ok)
{
    if ( 0 == reopen_ok)
    {
        printf( "port error\n");
        printf( "%s reopen port.\n", sender->devname);
    }
    else
    {
        printf( "port error close program\n");
    }
}

void receipt::on_read(woosimlib *sender)
{
    int     sz_read;

    printf( "on_read\n");

    //usec unit
//    usleep(1000 * 100);

    /*
    //sensor check
    iPaperSensor = 0;
    iCoverSensor = 0;ttyS0
    iMarkSensor  =  0;
    iCutterStatus  = 0;
    iFrontSensor  = 0;
    */

    sz_read = woosimlib_read( sender, sample_buf, MAX_BUF_SIZE);
    if ( 0 < sz_read)
    {

        // iPaperSensor = sample_buf[0] & 0x01;
        // iCoverSensor =  (sample_buf[0] >> 1) & 0x01;
        // iMarkSensor  =  (sample_buf[0] >> 2) & 0x01;
        // iCutterStatus  =  (sample_buf[0] >> 3) & 0x01;
        // iFrontSensor  =  (sample_buf[0]>> 6) & 0x01;
        // printf( "iPaperSensor : %02x \niCoverSensor : %02x \niMarkSensor : %02x \niCutterStatus : %02x \niFrontSensor : %02x \n", iPaperSensor,iCoverSensor,iMarkSensor,iCutterStatus,iFrontSensor);


        printf( "->Receive data : %s cnt : %d\n", sample_buf,sz_read);
    }
    else
    {
        printf( "read error\n");
    }
}




////////////////////////////////////////////////////////////////////////////
/// \brief receipt::rxGetAck
/// \param rxAckData - Receive Data from Serial port
/// \brief Parsing Received Serial Data from Receipt Printer
///
////////////////////////////////////////////////////////////////////////////
//void receipt::rxGetAck(const QByteArray &rxAckData)
//{
////    qDebug()<<"Receipt:"<<rxAckData.toHex();
//    if(rxAckData.at(0) & RECEIPT_PAPER_EMPTY) {
//        receiptStatus = RECEIPT_PAPER_EMPTY;

//    }
//    else if (rxAckData.at((0))& RECEIPT_HEAD_EMPTY) {
//        receiptStatus = RECEIPT_HEAD_EMPTY;
//    }
//    else receiptStatus = RECEIPT_OK;
//}

uint32_t    receipt::makeTxFormat(QByteArray subData)
{
    m_txFormatData.clear();
    m_txFormatData.append('2');
    m_txFormatData.append(subData);

    return 0;
}


uint32_t   receipt::setStatus(int32_t setSt,QByteArray reqData)
{
    switch (setSt)
    {
    case RECEIPT_CHK_SEN:
        m_PrintData.clear();
        m_PrintData.append(reqData);

        m_totalSize = m_PrintData.size();
        m_massValue = m_totalSize / CMD_BUF_SIZE;
        m_remainder = m_totalSize % CMD_BUF_SIZE;
        m_massCnt = 0;

        m_subData.clear();
        m_subData.resize(3);
        m_subData[0] = 0x10;
        m_subData[1] = 0x04;
        m_subData[2] = 0x04;

        makeTxFormat(m_subData);
        emit receiptTxData(m_txFormatData);

        break;

    case RECEIPT_PRINT1:

        //////////////////////////////////////////////////////////////////////////
        // sample 2.
        // use library API
        //
        InitPrinterStatus();
        // unsigned char changeFont[] = {0x1b, 0x74, 0x00};
        // ControlCommand(changeFont,3);
        BmpSaveSpool(printerlib,"woosim.bmp");

        InitPageMode(0,0,_2INCH,800);
        SetTextStyle(false,true,1,1,false);
        TextSaveSpool("Woosim Cafe\r\n");

        SetTextStyle(false,false,0,0,false);
        TextSaveSpool("Coffee, tea and bread\r\n");
        Page_DotFeed(15);
        SetFontSize(1);
        Page_SetPosition(277,105);
        TextSaveSpool("2015-01-27\r\n");
        SetFontSize(0);

        Page_SetPosition(290,5);
        QRCodeSaveSpool(0,'H',2,"www.woosim.com/english");

        Page_DrawBox(0,130,_2INCH,0,2);
        Page_DotFeed(5);
        TextSaveSpool(" #      Item      Num    Charge\r\n");
        TextSaveSpool(" 1      Latte      1     $ 3.35");	Page_Newline();
        TextSaveSpool(" 2  House Coffee   2     $ 3.98");	Page_Newline();
        TextSaveSpool(" 3       Tea	       1     $ 1.95");	Page_DotFeed(10);
        Page_DrawBox(0,270,_2INCH,0,2);	Page_Newline();

        Page_DrawBox(0,310,115,0,2); Page_DrawBox(260,310,124,0,2); Page_DotFeed(15);
        Page_SetPosition(123,300);
        TextSaveSpool("Credit Card\r\n");

        TextSaveSpool("Number:         ");
        SetFontSize(1);
        TextSaveSpool("6688-9988-6644-3311");	Page_Newline();
        SetFontSize(0);
        TextSaveSpool("Payment:                $ 9.50 ");	Page_Newline();

        Page_DrawBox(0,400,120,0,2);	Page_DrawBox(250,400,134,0,2);Page_DotFeed(15);
        Page_SetPosition(145,390);
        TextSaveSpool("Payment\r\n");

        TextSaveSpool("Price:                  $ 9.28");	Page_Newline();
        TextSaveSpool("Tax:                    $ 0.22");	Page_Newline();

        Page_DrawBox(0,490,_2INCH,0,2);	Page_DotFeed(15);
        TextSaveSpool("Total:                  $ 9.50");	Page_Newline();
        TextSaveSpool("Credit Card:            $ 9.50");	Page_Newline();

        Page_SetPosition(40,580);
        OneDimensionBarcodeSaveSpool(CODE128,2,50,TRUE,"20150127123456789");
        Page_LineFeed(2);

        Page_Print_StandardMode();

        TextSaveSpool("\r\n\r\n\r\n\r\n\r\n");

        FeedToMark();
        CutPaper(0);

        PrintSpool(printerlib,TRUE);

        GetPrinterStatus(printerlib, 1000);

        break;

    case RECEIPT_PRINT2:

    break;
        
    }
    return 0;
}
