#include "finger.h"
#include "definition.h"
#include <QDebug>

finger::finger(QObject *parent) : QObject(parent)
{
    m_txFormatData.resize(2);

    m_txFormatData[0] = 0x33;
    m_txFormatData[1] = 0x0D;

//    m_txFormatData[2] = 0x40;
//    m_txFormatData[3] = 0x11;
//    m_txFormatData[4] = 0x00;
//    m_txFormatData[5] = 0x00;
//    m_txFormatData[6] = 0x00;
//    m_txFormatData[7] = 0x00;
//    m_txFormatData[8] = 0x00;
//    m_txFormatData[9] = 0x00;
//    m_txFormatData[10] = 0x00;
//    m_txFormatData[11] = 0x00;
//    m_txFormatData[12] = 0x00;
//    m_txFormatData[13] = 0x51;
//    m_txFormatData[14] = 0x0A;

    char charr[] = {0x40,0x11,0,0,0,0,0,0,0,0,0,0x51,0x0A};
    QString tmp = QString::fromUtf8(charr,13);
    m_txFormatData.append(tmp);
}


////////////////////////////////////////////////////////////////////////////
/// \brief receipt::rxGetAck
/// \param rxAckData - Receive Data from Serial port
/// \brief Parsing Received Serial Data from Receipt Printer
///
////////////////////////////////////////////////////////////////////////////
void finger::rxGetAck(const QByteArray &rxAckData)
{
    qDebug()<<"finger:"<<rxAckData.toHex();

    char fCmd = rxAckData.at(1);
//    uint8_t checksum ; // check sum
    uint32_t IdentifyNum = 0;

    if(rxAckData.at(12) != 0x0A) // End Code Check
    {
        return;
    }

    switch (rxAckData.at(1)) {
    case 0x11:      //Identify

        if(rxAckData.at(10) == 0x6C) //Time Out
        {
            emit wrData2HMI(ACK_FINGER_TIMEOUT,NULL);
        }
        else if(rxAckData.at(10) == 0x62) //Scan Success
        {

        }
        else if(rxAckData.at(10) == 0x61) // Identify Number
        {
            IdentifyNum = tr_Param(rxAckData.at(2),rxAckData.at(3),rxAckData.at(4),rxAckData.at(5));
            emit wrData2HMI(ACK_FINGER_IDENTIFY,(void *)&IdentifyNum);

        }
        else if(rxAckData.at(10) == 0x69) // Not Found
        {
            emit wrData2HMI(ACK_FINGER_NOTFOUND,NULL);
        }
        break;

    case 0x04:

        break;

    case 0x03:

        break;

    case 0x86:

        break;

    default:
        break;
    }
}


void finger::setStatus(int32_t setSt, QByteArray reqData)
{
    char identity_str[20]= {0x33,0x0D,0x40,0x11,0,0,0,0,0,0,0,0,0,0x51,0x0A};
//       char enroll_str[20]  = {0x33,0x0D,0x40,0x05,0,0,0,0,0,0,0,0,0x79,0xBE,0x0A};
    char enroll_str[20]  = {0x33,0x0D,0x40,0x05,0,0,0,0,0,0,0,0,0x79,0x0A,0x0A};        // BE -> 0A
       m_retryNum = reqData.toInt();
       m_retryCnt = 0;
       m_txFormatData.clear();
       switch (setSt)
       {
           case FINGER_IDENTIFY:
               m_txFormatData.append(identity_str,15);
           break;

           case FINGER_ENROLL:
               m_txFormatData.append(enroll_str,15);
           break;
       }

       emit fingerTxData(m_txFormatData);
}



