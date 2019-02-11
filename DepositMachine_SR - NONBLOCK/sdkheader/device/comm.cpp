#include "comm.h"
#include "definition.h"
#include <QDebug>

comm::comm(QObject *parent) : QObject(parent)
{
    m_txFormatData.resize(1);
    m_txFormatData[0] = 0x35;
}

////////////////////////////////////////////////////////////////////////////
/// \brief receipt::rxGetAck
/// \param rxAckData - Receive Data from Serial port
/// \brief Parsing Received Serial Data from Receipt Printer
///
////////////////////////////////////////////////////////////////////////////
void comm::rxGetAck(const QByteArray &rxAckData)
{
    char aTimeOut[] = {0x02,0x18};
    qDebug()<<"comm:"<< m_receiveData << rxAckData.toHex();

//    m_receiveData += rxAckData;
    if(rxAckData.at(0) == 'K'){    // Ack to action
        qDebug() << "체크 rxAckData.at(2) == 'K'";
//        chktimer.start(3000,this);  // Restart timer
        if(rxAckData.at(1) == 'O'){
            commStatus = GATE_OPEN;
        }
        else if(rxAckData.at(1) == 'C'){
            commStatus = GATE_CLOSE;
        }
    }

//    if(m_receiveData.contains(0x0D)){
//        if(m_receiveData.contains(0x02) || m_receiveData.contains(0x18)){
//            if(m_retryCnt <= m_retryNum) {  // Retry cnt is Over
//                m_retryCnt++;
//                qDebug()<<"retry:" << m_retryCnt << m_receiveData;
//                emit commTxData(m_txFormatData);
//            }
//            else {
//                emit wrData2HMI(ACK_BARCODE_TIMEOUT,NULL);
//            }
//        }
//        else {      // Receive Data
//            emit wrData2HMI(ACK_BARCODE_DATA,(void *)&m_receiveData);
//        }
//        m_receiveData.clear();
//    }
}


void comm::setStatus(int32_t setSt, QByteArray reqData)
{
    switch (setSt)
    {
        case COMM_UPPERDOOR:
            m_retryNum = reqData.toInt();
            m_retryCnt = 0;
            m_txFormatData.append('K');
            qDebug() << "set retry: " << m_retryNum << m_retryCnt;
            emit commTxData(m_txFormatData);
        break;
    }

}
