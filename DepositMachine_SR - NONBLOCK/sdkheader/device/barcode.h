#ifndef BARCODE_H
#define BARCODE_H

#include <QObject>

class barcode : public QObject
{
    Q_OBJECT
public:
    explicit barcode(QObject *parent = nullptr);

    int32_t getStatus()
    {
        return barcodeStatus;
    }

    void        setStatus(int32_t setSt, QByteArray reqData);

signals:
    void barcodeTxData(const QByteArray & txData);

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

#endif // BARCODE_H
