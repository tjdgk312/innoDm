#include "class_gate.h"

#include <qdebug.h>
#include <qevent.h>

class_gate::class_gate(QString id)
{
    QByteArray tmCmd;
    gateId = id;
    gateStatus = GATE_STOP;
    retryCnt = 0;
    errorRetry = 0;
    gateErr = NO_ERROR;

    test.start();
//    qDebug() << "Create Gate:" << gateId;
}

void class_gate::rxGetAck(const QByteArray &rxAckData)
{
    qDebug() << "실게이트 Ack 들어옴";

//    qDebug() << "cls - rxGetAck :" << rxAckData.toHex();
    if(rxAckData.at(2) == 'S'){    // Ack to action
        qDebug() << "체크 rxAckData.at(2) == 'S'" << gateId;
//        chktimer.start(3000,this);  // Restart timer
        if(rxAckData.at(1) == 'O'){
            gateStatus = GATE_OPEN;
        }
        else if(rxAckData.at(1) == 'C'){
            gateStatus = GATE_CLOSE;
        }
        else if(rxAckData.at(1) == 'S'){
            gateStatus = GATE_TWIST;
        }
    }
    else if(rxAckData.at(1) == 'K'){
        if(rxAckData.at(2) == 'O') {
    //        chktimer.stop();
            qDebug() << "도어오픈됨";
            gateStatus = GATE_DOPENED;
        }
        else if(rxAckData.at(2) == 'C') {
    //        chktimer.stop();
            qDebug() << "도어클로즈됨";
            gateStatus = GATE_DCLOSED;
        }
    }
    else if(rxAckData.at(1) == 'O') {
//        chktimer.stop();
        qDebug() << "게이트오픈됨" << gateId << test.elapsed();
        gateStatus = GATE_OPENED;
    }
    else if(rxAckData.at(1) == 'C') {
//        chktimer.stop();
        qDebug() << "게이트클로즈됨" << gateId << test.elapsed();
        gateStatus = GATE_CLOSED;
    }
    else if(rxAckData.at(1) == 'E') {
        emit wrData2HMI(ACK_GATE_EMERGENCY,NULL);
        gateStatus = GATE_CHK_STAT;
//        errorRetry += 1;
//        if(errorRetry == 5){
//            errorRetry = 0;
//        }
        gateStatus = GATE_ERROR;
    }
    else if(rxAckData.at(1) == 'S') {
        qDebug() << "게이트 TWIST됨" << gateId << test.elapsed();
        gateStatus = GATE_TWISTED;
    }
    else if(rxAckData.at(0) == 'D') {
        if(rxAckData.at(2) == 'K') {    // Ack to action
//            gateStatus = GATE_CHGVAREND;
            qDebug() << "센서값 설정 됨" << gateId << rxAckData.at(1);
        }
        else{
            qDebug() << "센서값 설정 안됨" << gateId << rxAckData.at(1);
//            gateStatus = GATE_CHGVAREND;
        }
    }
}

uint32_t   class_gate::setStatus(Gate_TypeDef setSt)
{
    gateStatus = GATE_STOP;
    uint32_t result = 0;
    QByteArray tdCmd;
//    if(gateStatus == GATE_ERROR) {
//        return gateErr;
//    }

    switch (setSt) {
    case GATE_CLOSE:
        qDebug() << "게이트 CLOSE Set" << gateId;
        if(gateStatus == GATE_CLOSED) {
//            qDebug() << "gate has closed.";
        }
        else if(gateStatus == GATE_CLOSE) {
//            if(retryCnt++ > 20) {
//                gateStatus = GATE_ERROR;
//            }
        }
        else {
            gateStatus = GATE_STOP;
            tdCmd.append(gateId);
            tdCmd.append('C');
            reqStatus = GATE_CLOSED;
            emit gateTxData(tdCmd);
//            retryCnt = 0;
//            chktimer.start(3000,this);
        }
        break;
    case GATE_OPEN:
        qDebug() << "게이트 OPEN Set" << gateId;
        if((gateStatus == GATE_OPENED)) {
            qDebug() << "gate has opened.";
        }
        else if(gateStatus == GATE_OPEN) {
//            if(retryCnt++ > 20) {
//                gateStatus = GATE_ERROR;
//            }
        }
        else {
            qDebug() << "gate open SETUP";
            gateStatus = GATE_STOP;
            tdCmd.append(gateId);
            tdCmd.append('O');
            reqStatus = GATE_OPENED;
            emit gateTxData(tdCmd);
            retryCnt = 0;
//            chktimer.start(3000,this);
        }
        break;
    case GATE_CHK_STAT:
        qDebug() << "게이트 체크 SET" << gateId;
        tdCmd.append('S');
        reqStatus = GATE_CHK_STAT;
        emit gateTxData(tdCmd);
    case GATE_TWIST:
        gateStatus = GATE_STOP;
        tdCmd.append(gateId);
        tdCmd.append('S');
        reqStatus = GATE_TWIST;
        emit gateTxData(tdCmd);
        break;
    case GATE_CHGVAR:
        gateStatus = GATE_STOP;
        reqStatus = GATE_CHGVAR;
        emit gateTxData(command);
        break;
    case GATE_CHKSEN:
        gateStatus = GATE_STOP;
        tdCmd.append(gateId);
        tdCmd.append('K');
        reqStatus = GATE_CHKSEN;
        emit gateTxData(tdCmd);
        break;
    default:
        break;
    }

    return result;
}
