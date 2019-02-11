#ifndef STATETHREAD_H
#define STATETHREAD_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QtSerialPort/QSerialPort>
#include <QBasicTimer>

#include "definition.h"
#include "class_gate.h"
#include "currencycls.h"
#include "barcode.h"
#include "finger.h"
#include "comm.h"


class State_Thread : public QThread
{
    Q_OBJECT
public:
    explicit State_Thread(QObject *parent = 0);
    void run();

    uint32_t parsingRxData(uint8_t Length, uint8_t ID, const QByteArray & data);

    int preStep;
    int inStep;         // 내부 조건문 현재 스텝
    int inPreStep;         // 내부 조건만 이전 스텝

    int curCnt;
    int doorCnt;
    void setSensor(int32_t SenType)
    {
        switch (SenType)
        {
        case CUR_SEN:
            bChkSen = CUR_SEN;
            curCnt = 15;
            doorCnt = 0;
            break;
        case DOOR_SEN:
            bChkSen = DOOR_SEN;
            curCnt = 0;
            doorCnt = 15;
            break;
        case UP_SEN:
            bChkSen = UP_SEN;
            break;
        case NO_CHECK:
            bChkSen = NO_CHECK;
            break;
        }
    }

    uint32_t    SenDoorKey;         // 0: key unlock   1: key lock
    uint32_t    SenDoorState;       // 0: door closed  1: door opened

    uint32_t    fDoorLock = false;
    uint32_t    fDoorUnlock = false;


    bool bActive;
    int bChkSen  = NO_CHECK;

    bool bAutoDeposit = false;

    int senType;            // 현재 센서 설정타입
    int senValue;           // 현재 센서 설정값


    int16_t m_rxState;      // Receive Data Parsing state
    int     rxBcc;
    uint8_t rxLength;
    uint8_t m_rxID;

    uint32_t m_retryCnt;
    uint32_t err_id;

    QByteArray m_gatherData;    //Receiving Data

    QByteArray m_Txdata;
    QByteArray m_request;

    Machine_StatDef mc_stat;
    cls_currency *  curr_stat;      //  sb9
    class_gate *    gt_outgate;     //  escrow
    class_gate *    gt_ingate;      //
    class_gate *    gt_block;
    class_gate *    gt_seal;
    class_gate *    gt_lock;
//    receipt    *    pReceipt;
    barcode    *    pBarcode;
    finger     *    pFinger;

    comm    *    pComm;
private:


signals:
    void writeTxData(const QByteArray & txData);
    void AckCurrency(const QByteArray & rxAckData);
    void AckOutgate (const QByteArray & rxAckData);
    void AckIngate  (const QByteArray & rxAckData);
    void AckBlock   (const QByteArray & rxAckData);
    void AckSeal    (const QByteArray & rxAckData);
    void AckLock    (const QByteArray & rxAckData);
//    void AckReceipt (const QByteArray & rxAckData);
    void AckBarcode (const QByteArray & rxAckData);
    void AckFinger (const QByteArray & rxAckData);
    void AckComm (const QByteArray & rxAckData);

    void mc2HMI  (int   cmd, void * pdata);     // to HMI


public slots:
    void transaction(const uint32_t command,const QByteArray request); // Commands From HMI
    void rcvRxData(const QByteArray & rxdata);                      // UART Data From MainWindows
    void TransTxData(const QByteArray & data);                      // UART Data From each Unit( Currency, Gate, etc..)
    void TransHMIData(int cmd, void * pdata);                  // Transmit data to HMI from Unit.

};

#endif // STATETHREAD_H
