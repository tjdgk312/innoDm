#include "barcode.h"
#include "definition.h"
#include <QDebug>


barcode::barcode(QObject *parent) : QObject(parent)
{
    m_txFormatData.resize(3);

    m_txFormatData[0] = 0x34;
    m_txFormatData[1] = 0x02;
    m_txFormatData[2] = 0x54;
}


////////////////////////////////////////////////////////////////////////////
/// \brief receipt::rxGetAck
/// \param rxAckData - Receive Data from Serial port
/// \brief Parsing Received Serial Data from Receipt Printer
///
////////////////////////////////////////////////////////////////////////////
void barcode::rxGetAck(const QByteArray &rxAckData)
{
    char aTimeOut[] = {0x02,0x18};
    qDebug()<<"barcode:"<< m_receiveData << rxAckData.toHex();

    m_receiveData += rxAckData;
    if(m_receiveData.contains(0x0D)){
        if(m_receiveData.contains(0x02) || m_receiveData.contains(0x18)){
            if(m_retryCnt <= m_retryNum) {  // Retry cnt is Over
                m_retryCnt++;
                qDebug()<<"retry:" << m_retryCnt << m_receiveData;
                emit barcodeTxData(m_txFormatData);
            }
            else {
                emit wrData2HMI(ACK_BARCODE_TIMEOUT,NULL);
            }
        }
        else {      // Receive Data
            emit wrData2HMI(ACK_BARCODE_DATA,(void *)&m_receiveData);
        }
        m_receiveData.clear();
    }
}


void barcode::setStatus(int32_t setSt, QByteArray reqData)
{
    switch (setSt)
    {
        case BARCODE_LIGHT_ON:
            m_retryNum = reqData.toInt();
            m_retryCnt = 0;
            qDebug() << "set retry: " << m_retryNum << m_retryCnt;
            emit barcodeTxData(m_txFormatData);
        break;
    }

}



