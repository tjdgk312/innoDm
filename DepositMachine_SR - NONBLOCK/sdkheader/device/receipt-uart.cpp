#include "receipt.h"
#include "definition.h"

#include <QDebug>

receipt::receipt(QObject *parent) : QObject(parent)
{
    m_txFormatData.resize(1);

    m_txFormatData[0] = 0x32;

}

////////////////////////////////////////////////////////////////////////////
/// \brief receipt::rxGetAck
/// \param rxAckData - Receive Data from Serial port
/// \brief Parsing Received Serial Data from Receipt Printer
///
////////////////////////////////////////////////////////////////////////////
void receipt::rxGetAck(const QByteArray &rxAckData)
{
//    qDebug()<<"Receipt:"<<rxAckData.toHex();
    if(rxAckData.at(0) & RECEIPT_PAPER_EMPTY) {
        receiptStatus = RECEIPT_PAPER_EMPTY;

    }
    else if (rxAckData.at((0))& RECEIPT_HEAD_EMPTY) {
        receiptStatus = RECEIPT_HEAD_EMPTY;
    }
    else receiptStatus = RECEIPT_OK;
}

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
        m_subData.clear();
//        m_subData.resize(20);
        m_subData.resize(5);
        m_subData[0]=0x1B;
        m_subData[1]='@';         // ESC @ 	Initialize

        m_subData[2]=0x1D;
        m_subData[3]='!';                 // GS !  // Select character size
        m_subData[4]=0x00;

        makeTxFormat(m_subData);

        emit receiptTxData(m_txFormatData);

        receiptStatus = RECEIPT_PRINT1;
        break;

    case RECEIPT_PRINT2:
        m_subData.clear();
        if(m_massCnt < m_massValue) {
            receiptStatus = RECEIPT_PRINT2;
            m_subData.resize(CMD_BUF_SIZE);
            memcpy(m_subData.data(),m_PrintData.mid(m_massCnt*CMD_BUF_SIZE,CMD_BUF_SIZE).data(),CMD_BUF_SIZE);
            makeTxFormat(m_subData);
            emit receiptTxData(m_txFormatData);
            m_massCnt++;
        }
        else {
            if(m_remainder) {
                m_subData.resize(m_remainder);
                memcpy(m_subData.data(),m_PrintData.mid(m_massCnt*CMD_BUF_SIZE,CMD_BUF_SIZE).data(),m_remainder);
                makeTxFormat(m_subData);
                emit receiptTxData(m_txFormatData);
            }
            receiptStatus = RECEIPT_PRINTED;
        }
        break;

    case RECEIPT_CUT:
        m_subData.clear();
        m_subData.resize(6);
        m_subData[0]=0x1B;
        m_subData[1]=0x4A;

        m_subData[2]=200;

        m_subData[3]=0x1D;
        m_subData[4]=0x56;
        m_subData[5]=0;     // Full Cut:0
        makeTxFormat(m_subData);
        emit receiptTxData(m_txFormatData);
        receiptStatus = RECEIPT_CUT;

    }
    return 0;
}
