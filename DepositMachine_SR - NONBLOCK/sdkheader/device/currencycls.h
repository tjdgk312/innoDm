#ifndef CURRENCY_H
#define CURRENCY_H

#include <QObject>
#include <QBasicTimer>
#include "definition.h"

#define BIT_JR_RIGHT        0x08
#define BIT_JR_LEFT         0x04

#define BIT_RP1_RIGHT       0x08
#define BIT_RP1_LEFT        0x04
#define BIT_SOL_POSITION    0x02

#define BIT_HOPPER          0x08
#define BIT_REJECT_R        0x04
#define BIT_REAR_COVER      0x02
#define BIT_FRONT_COVER     0x01
#define BIT_REJECT_F        0x08
#define BIT_STACKER         0x04
#define BIT_SP_RIGHT        0x02
#define BIT_SP_LEFT         0x01


class cls_currency : public QObject
{
    Q_OBJECT
public:
    cls_currency();

    Curr_TypeDef getStatus()
    {
        return currStatus;
    }

    uint32_t    setStatus(Curr_TypeDef setSt, QByteArray reqData);

    uint32_t    makeTxFormat(uint8_t len,QByteArray subCmd, QByteArray subData);

    uint8_t tr_BCD(char baValueH, char baValueL)
    {
        uint8_t rtnVal = baValueL;
        rtnVal += (uint8_t)(baValueH << 4);

        return rtnVal;
    }
    uint16_t tr_wBCD(char Value4, char Value3,char Value2, char Value1)
    {
        uint16_t rtnVal = Value1;
        rtnVal += (uint16_t)(Value2 << 4);
        rtnVal += (uint16_t)(Value3 << 8);
        rtnVal += (uint16_t)(Value4 << 12);

        return rtnVal;
    }
    t_NOTE bankNote;
    t_CUR_SENSOR curSensor;
    t_Cur_Set   curSet;



signals:
    void currTxData(const QByteArray & txData);

    void wrData2HMI(int cmd, void * pdata);

public slots:
    void rxGetAck(const QByteArray & rxAckData);

private:
// Communication Variable
    QByteArray      m_txFormatData;
    int32_t         m_txSize;

    QByteArray      m_subCmd;
    QByteArray      m_subData;

// Status Variable
    Curr_TypeDef    currStatus;
    Curr_TypeDef    reqStatus;
    uint32_t        chkCnt;
    QBasicTimer     chktimer;
    uint32_t        retryCnt;
    uint32_t        fConnect;       // Check Communication Connect




};

#endif // CURRENCY_H
