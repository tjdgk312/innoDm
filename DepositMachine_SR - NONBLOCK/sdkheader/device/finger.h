#ifndef FINGER_H
#define FINGER_H
#include <QObject>

class finger : public QObject
{
    Q_OBJECT
public:
    explicit finger(QObject *parent = nullptr);

    int32_t getStatus()
    {
        return barcodeStatus;
    }

    void        setStatus(int32_t setSt, QByteArray reqData);

    uint8_t tr_Param(char Value1, char Value2,char Value3, char Value4)
    {
        uint8_t rtnVal = Value1;
        rtnVal += (uint8_t)(Value2 << 8);
        rtnVal += (uint8_t)(Value3 << 16);
        rtnVal += (uint8_t)(Value4 << 24);

        return rtnVal;
    }

signals:
    void fingerTxData(const QByteArray & txData);

    void wrData2HMI(int cmd, void * pdata);

public slots:
    void rxGetAck(const QByteArray & rxAckData);


private:
    int32_t barcodeStatus;

    QByteArray      m_txFormatData;
    QByteArray      m_subData;
    QByteArray      m_receiveData;
    int32_t         m_retryCnt;
    int32_t         m_retryNum;
};

#endif // FINGER_H
