#ifndef CLASS_GATE_H
#define CLASS_GATE_H

#include <QObject>
#include <QBasicTimer>
#include "definition.h"

#define NO_ERROR        0x00
#define ERROR_OPEN      0x01
#define ERROR_CLOSE     0x02


#include <QElapsedTimer>


class class_gate : public QObject
{
    Q_OBJECT

public:
    class_gate(QString id);


    Gate_TypeDef getStatus()
    {
        return gateStatus;
    }

    uint32_t    setStatus(Gate_TypeDef setSt);

    QString         gateId;
    Gate_TypeDef    gateStatus;
    Gate_TypeDef    reqStatus;
    uint32_t        chkCnt;
    QBasicTimer     chktimer;
    uint32_t        retryCnt;
    uint32_t        errorRetry;

    uint32_t        gateErr;

    QElapsedTimer test;

    QByteArray command;
protected:



signals:
    void gateTxData(const QByteArray & txData);
    void wrData2HMI(int cmd, void * pdata);

public slots:
    void rxGetAck(const QByteArray & rxAckData);


private:
};

#endif // CLASS_GATE_H
