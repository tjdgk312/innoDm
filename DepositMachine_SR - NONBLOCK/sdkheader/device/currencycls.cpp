#include "currencycls.h"

#include <QDebug>

cls_currency::cls_currency()
{
    m_subCmd.resize(2);
    m_txFormatData.resize(16);

    m_txFormatData[0] = 0x31;
    m_txFormatData[1] = 0xF0;
    m_txFormatData[2] = 0xF1;
    m_txFormatData[4] = 0x19;
    m_txFormatData[5] = 0x02;
    m_txFormatData[6] = 0x00;
    m_txFormatData[7] = 0x00;
    m_txFormatData[8] = 0x00;
    m_txFormatData[9] = 0x00;

// Database is connected.


    fConnect = false;       // No check Ack signal

}

////////////////////////////////////////////////////////////////////////////
/// \brief cls_currency::makeTxFormat
/// \param len
/// \param subCmd
/// \param subData
/// \returns m_txFormatData (include currency class)
/// \brief make TX data Format for Currency (SB-9)
////////////////////////////////////////////////////////////////////////////
uint32_t    cls_currency::makeTxFormat(uint8_t len,QByteArray subCmd, QByteArray subData)
{
    int CheckSum = 0x1B;
    if(m_txSize != len) {
        m_txSize = len;
        m_txFormatData.resize(m_txSize + 5);
        m_txFormatData[3] = m_txSize;
    }
    m_txFormatData.replace(10,2,subCmd);
    m_txFormatData.replace(12,(m_txSize-9),subData);

    CheckSum += m_txSize;
    for(int i = 10; i < (m_txSize + 3); i++) {
        CheckSum += m_txFormatData[i];
    }
    m_txFormatData[m_txSize+3]= ((CheckSum & 0xF0)>>4);
    m_txFormatData[m_txSize+4]= (CheckSum & 0x0F);
    return 0;
}

////////////////////////////////////////////////////////////////////////////
/// \brief cls_currency::rxGetAck
/// \param rxAckData - Receive Data from Serial port
/// \brief Parsing Received Serial Data from Currency
///
////////////////////////////////////////////////////////////////////////////
void cls_currency::rxGetAck(const QByteArray &rxAckData)
{
//    qDebug() << "currency - rxGetAck :" << rxAckData.toHex();
    char cmd0 = rxAckData.at(9);
    char cmd1 = rxAckData.at(10);

    uint8_t chkSum = 0;
    uint32_t nNote;
    int len = rxAckData.at(2);
    for(int i = 2; i < (len + 2); i++ ) {
        chkSum += rxAckData.at(i);
    }

    switch (cmd0) {

    case 0x03:
        switch(cmd1) {

        case 0x00:      // 30(h) : DEVICE_SET LIVE
            if(rxAckData.at(12) == 0x01) {      // KNOCK
                m_subCmd[0] = 0x03;
                m_subCmd[1] = 0x00;
                m_subData.resize(2);
                m_subData[0] = 0x00;
                m_subData[1] = 0x02;
                makeTxFormat(0x0B,m_subCmd,m_subData);
                emit currTxData(m_txFormatData);    // ACK
            }
            else if(rxAckData.at(12) == 0x02) {
                currStatus = CURRENCY_READY;
//                fConnect = true;
            }
                fConnect = true;
            break;

        case 0x02:      // 32(h) : COUNT_START
//            qDebug() << "Curr: COUNT_START";

            emit wrData2HMI(ACK_CURRENCY_COUNT_START,&curSensor);
            break;

        case 0x03:      // 33(h) : CD_ERROR
            if(rxAckData.at((12) & 0x01)) {
                // JAM Error
            }
            else if(rxAckData.at((12) & 0x02)) {
                // Feeding Error
            }
            break;

        case 0x04:      // 34(h) : RESULT_DATA
            nNote = (len - 13) / 6;
            qDebug() << rxAckData.count() << len << nNote;

            bankNote.nReject = tr_BCD(rxAckData.at(11), rxAckData.at(12));
            bankNote.nOutStack = tr_BCD(rxAckData.at(13), rxAckData.at(14));
            for( uint32_t i = 0; i < 12; i++){
                bankNote.nData[i] = 0;
                bankNote.nPreData[i] = 0;
                if(i >= nNote){
                    bankNote.nData[i] = 0;
                    bankNote.nPreData[i] = 0;
                }
                else{
                    bankNote.nData[i] = tr_BCD(rxAckData.at(15+2*i), rxAckData.at(16+2*i));
                    if(rxAckData.count() > nNote*6+15){
                        bankNote.nPreData[i] = tr_wBCD(rxAckData.at(2*nNote+(i*4)+15), rxAckData.at(2*nNote+(i*4)+16),
                                                       rxAckData.at(2*nNote+(i*4)+17), rxAckData.at(2*nNote+(i*4)+18));
                    }
                }
            }
            emit wrData2HMI(ACK_CURRENCY_RESULT,&bankNote);
            break;

        case 0x08:      // DEVICE_SETUP2

            break;

        case 0x0B:      // SETUP_NO_ACCEPT
//            qDebug() << "Cur-Setup No Accept";

            break;

        case 0x0D:      // COUNT_STOP
//            qDebug() << "Count Stop";
            break;

        case 0x0E:      // SENSOR_STATUS
            if(rxAckData.at(11) & BIT_JR_RIGHT) curSensor.jp_right = 1;
            else curSensor.jp_right = 0;
            if(rxAckData.at(11) & BIT_JR_LEFT)  curSensor.jp_left = 1;
            else curSensor.jp_left = 0;
            if(rxAckData.at(12) & BIT_RP1_RIGHT)curSensor.rp1_right = 1;
            else curSensor.rp1_right = 0;
            if(rxAckData.at(12) & BIT_RP1_LEFT) curSensor.rp1_left = 1;
            else curSensor.rp1_left = 0;
            if(rxAckData.at(12) & BIT_SOL_POSITION)curSensor.sol_pos = 1;
            else curSensor.sol_pos = 0;

            if(rxAckData.at(13) & BIT_HOPPER)   curSensor.hopper = 1;
            else curSensor.hopper = 0;
            if(rxAckData.at(13) & BIT_REJECT_R) curSensor.reject_r = 1;
            else curSensor.reject_r = 0;
            if(rxAckData.at(13) & BIT_REAR_COVER) curSensor.rear_cover = 1;
            else curSensor.rear_cover = 0;
            if(rxAckData.at(13) & BIT_FRONT_COVER)curSensor.front_cover = 1;
            else curSensor.front_cover = 0;
            if(rxAckData.at(14) & BIT_REJECT_F) curSensor.reject_f = 1;
            else curSensor.reject_f = 0;
            if(rxAckData.at(14) & BIT_STACKER) curSensor.stacker = 1;
            else curSensor.stacker = 0;
            if(rxAckData.at(14) & BIT_SP_RIGHT) curSensor.sp_right = 1;
            else curSensor.sp_right = 0;
            if(rxAckData.at(14) & BIT_SP_LEFT) curSensor.sp_left = 1;
            else curSensor.sp_left = 0;

//            bankNote.nOutStack =  tr_BCD(rxAckData.at(15),rxAckData.at(16));
//            bankNote.nReject = tr_BCD(rxAckData.at(15),rxAckData.at(16));

            curSensor.Error = rxAckData.at(20);

            emit wrData2HMI(ACK_CURRENCY_SENSOR,&curSensor);
            break;

        default:

            break;
        }

        break;      // END - 0x03

    case 0x04:
        switch(cmd1) {
        case 0x01:      // SETUP_STATE_DATA2
            curSet.Cur_Code = tr_BCD(rxAckData.at(11),rxAckData.at(12));
            curSet.Count_Mode = tr_BCD(rxAckData.at(13),rxAckData.at(14));
//            curSet.bStatus = tr_BCD(rxAckData.at(15),rxAckData.at(16));
//            qDebug() << "SETUP STATE DATA2 - GET: " << curSet.bStatus;
            break;
        case 0x05:      // MC_STATUS
//            curSet.mc_state = tr_BCD(rxAckData.at(11),rxAckData.at(12));
//            qDebug() << "MC_STATUS - GET: " << curSet.mc_state;
            break;
        }

        break;      // END - 0x04

    }

}


uint32_t   cls_currency::setStatus(Curr_TypeDef setSt,QByteArray reqData)
{
    uint32_t result = 0;
    QByteArray tdCmd = QByteArray::fromHex(reqData);
    QString tmpStr;

    if(!fConnect) {
        m_subCmd.resize(2);
        m_subCmd[0] = 0x03;
        m_subCmd[1] = 0x00;
        m_subData.resize(2);
        m_subData[0] = 0x00;
        m_subData[1] = 0x01;
        makeTxFormat(0x0B,m_subCmd,m_subData);
        emit currTxData(m_txFormatData);    // ACK
        return 1;
    }

    if(currStatus == CURRENCY_ERROR) {
        return curSensor.Error;
    }

    switch (setSt) {
    case CURRENCY_IDLE:
        if((currStatus !=CURRENCY_IDLE)) {


        }
        else {
//            qDebug() << "Curr is IDLE";
        }

        break;
    case CURRENCY_STACK:
        if((currStatus != CURRENCY_STACK)) {

        }
        else {
//            qDebug() << "Curr is Stack";
        }

        break;
    case CURRENCY_SET_LIVE:
        if(currStatus != CURRENCY_SET_LIVE) {
            m_subCmd[0] = 0x03;
            m_subCmd[1] = 0x00;
            m_subData.resize(2);
            m_subData[0] = 0x00;
            m_subData[1] = 0x01;
            makeTxFormat(0x0B,m_subCmd,m_subData);
            emit currTxData(m_txFormatData);
        }

        break;

    case CURRENCY_REQ_STOP:
        if(currStatus != CURRENCY_REQ_STOPED) {
            m_subCmd[0] = 0x03;
            m_subCmd[1] = 0x07;
            m_subData.resize(2);
            m_subData[0] = 0x00;
            m_subData[1] = 0x02;
            makeTxFormat(0x0B,m_subCmd,m_subData);
            emit currTxData(m_txFormatData);
        }
        break;

    case CURRENCY_REQ_START:
        if(currStatus != CURRENCY_REQ_STOPED) {
            m_subCmd[0] = 0x03;
            m_subCmd[1] = 0x07;
            m_subData.resize(2);
            m_subData[0] = 0x00;
            m_subData[1] = 0x01;
            makeTxFormat(0x0B,m_subCmd,m_subData);
            emit currTxData(m_txFormatData);
        }
        break;

    case CURRENCY_RESULT_REQ:       // 0x35 RESULT_REQUEST bit 4 Sensor

        m_subCmd[0] = 0x03;
        m_subCmd[1] = 0x05;
        m_subData.resize(2);

        m_subData[0] = reqData.at(0)>>4;
        m_subData[1] = (reqData.at(0)&0x0F);
        makeTxFormat(0x0B,m_subCmd,m_subData);
        emit currTxData(m_txFormatData);

        break;

    case CURRENCY_RESULT_CLEAR:       // 0x36 JAM Clear bit 5

        m_subCmd[0] = 0x03;
        m_subCmd[1] = 0x06;
        m_subData.resize(2);

        m_subData[0] = reqData.at(0)>>4;
        m_subData[1] = (reqData.at(0)&0x0F);
        makeTxFormat(0x0B,m_subCmd,m_subData);
        emit currTxData(m_txFormatData);

        break;

    case CURRENCY_SETUP:

        break;

    case CURRENCY_DEV_SETUP2:
        m_subCmd[0] = 0x03;
        m_subCmd[1] = 0x08;
        m_subData.resize(2);
        if(reqData.at(0) == '1') {
            m_subData[1] = 0x01;
        }
        else m_subData[1] = 0x00;
        m_subData[0] = 0x00;
        makeTxFormat(0x0B,m_subCmd,m_subData);
        emit currTxData(m_txFormatData);

        break;
    case CURRENCY_COUNT_STOP:
            m_subCmd[0] = 0x03;
            m_subCmd[1] = 0x0D;
            m_subData.resize(2);
            m_subData[0] = 0x00;
            m_subData[1] = 0x01;
            makeTxFormat(0x0B,m_subCmd,m_subData);
            emit currTxData(m_txFormatData);
        break;
    case CURRENCY_STATE_DATA2:

//        qDebug() << "STATEDATA2" << tdCmd.at(1);

        m_subCmd[0] = 0x04;
        m_subCmd[1] = 0x01;
        m_subData.clear();
        m_subData.resize(22);

        m_subData[0] = (tdCmd.at(0) >> 4) & 0x0F;
        m_subData[1] = (tdCmd.at(0) & 0x0F);

        m_subData[2] = 0x00;
        m_subData[3] = 0x01;    //0 count  1 Mixed  2 single

        m_subData[4] = 0x0E;                      // E = add off   F = add on
//        m_subData[5] = 0x04;    //Basic Status    // Batch on = 5   Batch off = 4   Auto = C
        m_subData[5] = 0x04;    //Basic Status    // Batch on = 5   Batch off = 4   Auto = C

        m_subData[6] = 0x00;
        m_subData[7] = 0x00;    // Data[4]    mode lock off

        m_subData[8] = 0x00;
        m_subData[9] = 0x00;    // Data[5];   motor speed 1000rpm

        m_subData[10] = 0x01;
        m_subData[11] = 0x0E;   // Count Mode       1111 0  ALL mode

        m_subData[12] = 0x00;                      // 0000 30 PCS
        m_subData[13] = (tdCmd.at(1) & 0x0F);   // Pocket Cap.     // 0000 200 PCS   0001 150PCS 0010 100PCS

        m_subData[14] = 0x00;
        m_subData[15] = 100;    // Batch Num.
        m_subData[16] = 0x00;
        m_subData[17] = 3;
        m_subData[18] = 0x00;
        m_subData[19] = 3;
        m_subData[20] = 0x00;
        m_subData[21] = 3;

        makeTxFormat(0x1F,m_subCmd,m_subData);
        emit currTxData(m_txFormatData);
        break;

    case CURRENCY_CHANGE:

        m_subCmd[0] = 0x04;
        m_subCmd[1] = 0x01;
        m_subData.clear();
        m_subData.resize(22);

        m_subData[0] = (tdCmd.at(0) >> 4) & 0x0F;
        m_subData[1] = (tdCmd.at(0) & 0x0F);

        m_subData[2] = (tdCmd.at(1) >> 4) & 0x0F;
        m_subData[3] = (tdCmd.at(1) & 0x0F);

//        m_subData[3] = (reqData.at(2)& 0x0F);    //Mixed     Data[2]

        m_subData[4] = 0x0E;    // batch bit on
        m_subData[5] = 0x05;    //Basic Status      Data[3]

        m_subData[6] = 0x00;
        m_subData[7] = 0x00;    // Data[4]

        m_subData[8] = 0x00;
        m_subData[9] = 0x00;    // Data[5];

        m_subData[10] = (tdCmd.at(2) >> 4) & 0x0F;
        m_subData[11] = (tdCmd.at(2) & 0x0F);
//        m_subData[10] = (reqData.at(5) & 0x0F);
//        m_subData[11] = (reqData.at(6) & 0x0F);     //   Data[6]

        m_subData[12] = 0x00;   // Data[7]
        m_subData[13] = 0x02;   // Pocket Cap. 200장

        m_subData[14] = (tdCmd.at(3) >> 4) & 0x0F;
        m_subData[15] = (tdCmd.at(3) & 0x0F);

//        m_subData[15] = (reqData.at(4) & 0x0F);
//        m_subData[14] = 0x00;
//        m_subData[15] = 8;

        m_subData[16] = 0x00;
        m_subData[17] = 3;
        m_subData[18] = 0x00;
        m_subData[19] = 3;
        m_subData[20] = 0x00;
        m_subData[21] = 3;

        makeTxFormat(0x1F,m_subCmd,m_subData);
        emit currTxData(m_txFormatData);
        break;
    }

    return result;
}

