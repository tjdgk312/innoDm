#ifndef COMM_H
#define COMM_H

#include <QObject>

class comm : public QObject
{
    Q_OBJECT
public:
    explicit comm(QObject *parent = nullptr);

    int32_t getStatus()
    {
        return commStatus;
    }

    void        setStatus(int32_t setSt, QByteArray reqData);

signals:
    void commTxData(const QByteArray & txData);

    void wrData2HMI(int cmd, void * pdata);

public slots:
    void rxGetAck(const QByteArray & rxAckData);

private:
    int32_t commStatus;

    QByteArray      m_txFormatData;
    QByteArray      m_subData;
    QByteArray      m_receiveData;
    int32_t         m_retryCnt;
    int32_t         m_retryNum;
};

#endif // COMM_H
