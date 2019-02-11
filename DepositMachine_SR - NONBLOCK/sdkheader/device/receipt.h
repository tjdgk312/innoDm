#ifndef RECEIPT_H
#define RECEIPT_H

#include <QObject>

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QtSerialPort/QSerialPort>
#include <QBasicTimer>

//#include <stdio.h>
//#include <stdlib.h>
//#include <unistd.h>
//#include <string.h>
//#include <sys/time.h>
//#include <sys/stat.h>

//#include <linux/lp.h>

#include "device/woosimprinter.h"

#define   MAX_BUF_SIZE    1024


class receipt : public QObject
{
    Q_OBJECT
public:
    explicit receipt(QObject *parent = nullptr);

    int32_t getStatus()
    {
        return receiptStatus;
    }

    uint32_t    setStatus(int32_t setSt, QByteArray reqData);
    uint32_t    makeTxFormat(QByteArray subData);

    woosimlib	*printerlib;

    static void on_read( woosimlib *sender);
    static void on_error( woosimlib *sender, int reopen_ok);


signals:
    void receiptTxData(const QByteArray & txData);

    void wrData2HMI(int cmd, void * pdata);



public slots:
//    void rxGetAck(const QByteArray & rxAckData);

private:
    int32_t receiptStatus;

    QByteArray      m_txFormatData;
    QByteArray      m_subData;
    QByteArray      m_PrintData;
    int32_t         m_txSize;
    int32_t         m_totalSize;
    int32_t         m_massCnt;
    int32_t         m_massValue;
    int32_t         m_remainder;


};

#endif // RECEIPT_H
