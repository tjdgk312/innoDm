#include "statethread.h"
#include <qdebug.h>
#include <QtSerialPort/QSerialPort>
#include "class_gate.h"
#include "sdkheader/sdk_global.h"

State_Thread::State_Thread(QObject *parent):QThread(parent)
{

    qDebug()<<"Thread :"<<QThread::currentThreadId();
    curCnt  = 5;
    doorCnt  = 1;

    SenDoorKey = 1;
    SenDoorState = 1;

    bActive = true;
    m_rxState = 0;

    preStep = MACHINE_INIT;
    mc_stat = MACHINE_INIT;

    inPreStep = 0;
    inStep = 0;

    m_gatherData.clear();

//// Create Gate machine Object ////
    curr_stat   = new cls_currency();
    gt_outgate  = new class_gate(ID_OUTGATE);
    gt_ingate   = new class_gate(ID_INGATE);
    gt_block    = new class_gate(ID_BLOCK);
    gt_seal     = new class_gate(ID_SEAL);
    gt_lock     = new class_gate(ID_LOCK);
//    pReceipt    = new receipt();
    pBarcode    = new barcode();
    pFinger     = new finger();
    pComm       = new comm();

// device -> thread
    connect(curr_stat,  &cls_currency::currTxData, this, &State_Thread::TransTxData);
    connect(gt_outgate, &class_gate::gateTxData, this, &State_Thread::TransTxData);
    connect(gt_ingate,  &class_gate::gateTxData, this, &State_Thread::TransTxData);
    connect(gt_block,   &class_gate::gateTxData, this, &State_Thread::TransTxData);
    connect(gt_seal,    &class_gate::gateTxData, this, &State_Thread::TransTxData);
    connect(gt_lock,    &class_gate::gateTxData, this, &State_Thread::TransTxData);
    connect(pBarcode,   &barcode::barcodeTxData, this, &State_Thread::TransTxData);
    connect(pFinger,   &finger::fingerTxData, this, &State_Thread::TransTxData);
    connect(pComm,   &comm::commTxData, this, &State_Thread::TransTxData);

// device -> sdk
    connect(curr_stat,  &cls_currency::wrData2HMI, this,&State_Thread::TransHMIData);
    connect(gt_outgate, &class_gate::wrData2HMI,   this,&State_Thread::TransHMIData);
    connect(pBarcode,   &barcode::wrData2HMI, this, &State_Thread::TransHMIData);
    connect(pFinger,   &finger::wrData2HMI, this, &State_Thread::TransHMIData);
    connect(pComm,   &comm::wrData2HMI, this, &State_Thread::TransHMIData);
//RX connect
    connect(this, &State_Thread::AckCurrency,curr_stat, &cls_currency::rxGetAck);
    connect(this, &State_Thread::AckOutgate,gt_outgate, &class_gate::rxGetAck);
    connect(this, &State_Thread::AckIngate, gt_ingate,  &class_gate::rxGetAck);
    connect(this, &State_Thread::AckBlock,  gt_block,   &class_gate::rxGetAck);
    connect(this, &State_Thread::AckSeal,   gt_seal,    &class_gate::rxGetAck);
    connect(this, &State_Thread::AckLock,   gt_lock,    &class_gate::rxGetAck);
    connect(this, &State_Thread::AckBarcode,pBarcode,   &barcode::rxGetAck);
    connect(this, &State_Thread::AckFinger, pFinger,    &finger::rxGetAck);
    connect(this, &State_Thread::AckComm, pComm,    &comm::rxGetAck);
//    connect(this, &State_Thread::AckReceipt,pReceipt,   &receipt::rxGetAck);
}

////////////////////////////////////////////////////////////////////////////
/// \brief State_Thread::TransTxData
/// \param data
/// \brief Make protocol for TX Serial data to Board..
////////////////////////////////////////////////////////////////////////////
void State_Thread::TransTxData(const QByteArray & data)
{
    int cmdLength = data.length();
    m_Txdata.clear();
    m_Txdata.resize(2);
    m_Txdata[0] = 0xFD;
    m_Txdata[1] = cmdLength;

    //m_Txdata.append(m_request);
    m_Txdata.append(data);
    m_Txdata.resize(3+cmdLength);
    m_Txdata[2+cmdLength] = 0xFE;

    emit writeTxData(m_Txdata);
}

////////////////////////////////////////////////////////////////////////////
/// \brief State_Thread::TransHMIData
/// \param cmd, pdata
/// \brief Transmit data from Units to HMI
////////////////////////////////////////////////////////////////////////////
void State_Thread::TransHMIData(int cmd, void *pdata)
{
// Check Thread Status.. Routine..
//    QString id;
//    switch (cmd) {TransHMIData

//    case ACK_CURRENCY_JAM:
//        setSensor(CUR_SEN);
//        break;

//    case ACK_DEVICE_ERROR:

//        break;

//    }

    emit mc2HMI(cmd, pdata);
}


////////////////////////////////////////////////////////////////////////////
/// \brief State_Thread::rcvRxData
/// \param rxdata
/// \abstract Check RX data Protocol.. Divide Object..(etc..GATE, Currency)
void State_Thread::rcvRxData(const QByteArray & rxdata)
{
//    qDebug() << "rcvRx :";// << rxdata.toHex();
    int remainSize = 0;
    int rxSize = rxdata.size();

    for(int i = 0; i < rxSize; i++) {
        switch (m_rxState)
        {
        case 0:
            if(rxdata.at(i ) == 0xFD) {
                m_rxState++;
                rxBcc = 0;
            }
            break;

        case 1:
            rxLength = rxdata.at(i);
            if(rxLength < 200)  m_rxState++;
            else                m_rxState = 0;
            break;

        case 2:
            m_rxID = rxdata.at(i);
            m_rxState++;
            break;

        case 3:
            remainSize = rxSize - i;
            if((remainSize) > rxLength) { // Include ETX
                if(rxdata.at(i + rxLength) == 0xFE) {
                    QByteArray tRxData(rxdata.mid(i,rxLength));
                    i += rxLength;
                    parsingRxData(rxLength, m_rxID, tRxData);
                    m_rxState = 0;
                }
                else {
                    // Error
                    qDebug() << "Thread case 3 Error";
                }
            }
            else {
                m_rxState++;
                m_gatherData += rxdata.mid(i,remainSize);
                i += remainSize;
            }
            break;

        case 4:     // Seperate Rx Sentence...
            remainSize = rxLength - m_gatherData.size();
            m_gatherData += rxdata.mid(0,remainSize);
            i += remainSize;
            if(remainSize < rxSize) {
                if(rxdata.at(remainSize) == 0xFE) {
                    parsingRxData(rxLength, m_rxID, m_gatherData);
                    m_gatherData.clear();
                    m_rxState = 0;
                }
                else {
                    // Error
//                    qDebug() << "Error Thread case 4 ";
                    m_gatherData.clear();       // Test01
                    m_rxState = 0;
                }
            }
            else {
//                qDebug() << "Thread case 4 small rxSize";
            }
            break;

        default:

            break;

        }
    }
}

uint32_t State_Thread::parsingRxData(uint8_t Length, uint8_t ID, const QByteArray & data)
{
    switch (ID)
    {
    case ID_MOT1_BD:    // 0x41

        if(Length == 4) {
            switch (data.at(0)) {
            case 'S':
                if(data.at(1) == 3)         emit AckIngate("SOC0");
                else if(data.at(1) == 6)    emit AckIngate("SCC0");
                else if(data.at(1) == 10)   emit AckIngate("SEE0");

                if(data.at(2) == 3)         emit AckOutgate("SOO0");
                else if(data.at(2) == 6)    emit AckOutgate("SCC0");
                else if(data.at(2) == 10)   emit AckOutgate("SEE0");

                break;

            case 'I':
                qDebug() << "Ack InGate";
                emit AckIngate(data);
                break;

            case 'O':
                qDebug() << "Ack OutGate";
                emit AckOutgate(data);
                break;

            case 'D':   // Ack Adjust Delay

                break;

            case 'V':   // Ack Version Information

                break;

            default:

                break;
            }
        }

        break;

    case ID_MOT2_BD:
        if(Length == 4) {
            switch (data.at(0)) {

            case 'S':
                if(data.at(1) == 3)         emit AckSeal("SOC0");
                else if(data.at(1) == 6)    emit AckSeal("SHC0");
                else if(data.at(1) == 'e')  emit AckSeal("SHE0");

                if(data.at(2) == 'o')       emit AckLock("SLO0");
                else if(data.at(2) == 'c')  emit AckLock("SLC0");
                else if(data.at(2) == 'e')  emit AckLock("SLE0");

                if(data.at(3) == 'o')       emit AckBlock("SBO0");
                else if(data.at(3) == 'c')  emit AckBlock("SBC0");
                else if(data.at(3) == 'e')  emit AckBlock("SBE0");

                break;

            case 'H':
                qDebug() << "Ack Seal";
                emit AckSeal(data);
                break;

            case 'L':
                qDebug() << "Ack Lock";
                emit AckLock(data);
                break;

            case 'B':
                qDebug() << "Ack Block";
                emit AckBlock(data);
                break;

            case 'D':       // Adjust Delay

                break;

            case 'V':       // Check Version

                break;

            default:

                break;
            }
        }
        break;

    case ID_DOOR_BD:
        if(bChkSen == CUR_SEN){
            doorCnt = 0;
        }
        switch(data.at(0)) {
        case 'S':
            SenDoorState = data.at(1);    // 센서감지가 0으로 들어옴  감지=언락된상태(0) 미감지=락된상태(1)
            SenDoorKey = data.at(2);
            break;

        case 'U':
            fDoorLock = false;
            fDoorUnlock = true;
            break;

        case 'L':
            fDoorLock = true;
            fDoorUnlock = false;
            break;

        case 'E':

            break;

        }


        break;

    case ID_CURR_BD:
        if(data.at(0)!= 0xF0 || data.at(1)!=0xF1) {
            qDebug() << "Currency STX Error";
            break;
        }
        else {
            emit AckCurrency(data);
        }
        break;

    case ID_BARCODE:
        emit AckBarcode(data);
        break;

    case ID_FINGER:
        emit AckFinger(data);
        break;
    case ID_COMM:
        emit AckComm(data);
        break;

//    case ID_RECEIPT:
////        emit AckReceipt(data);
//        break;
    default:

        break;

    }
    return 0;
}

// Execute Command from HMI
void State_Thread::transaction(const uint32_t command, const QByteArray request)
{
    QByteArray tdCmd;
    QByteArray tmpData;
    if(gt_outgate->gateStatus == GATE_FAILED)  gt_outgate->gateStatus = GATE_STOP;
    if(gt_ingate->gateStatus == GATE_FAILED)  gt_ingate->gateStatus = GATE_STOP;
    m_retryCnt = 0;
    //------------------------------- raw command  - gateStatus 로 체크하는 부분
    switch (command) {
    case CMD_OUTGATE_OPEN:
        gt_outgate->setStatus(GATE_OPEN);
        mc_stat = MACHINE_OUTOPEN;
        break;

    case CMD_OUTGATE_CLOSE:
        gt_outgate->setStatus(GATE_CLOSE);
        mc_stat = MACHINE_OUTCLOSE;
        break;

    case CMD_INGATE_OPEN:
        gt_ingate->setStatus(GATE_OPEN);
        mc_stat = MACHINE_INOPEN;
        break;

    case CMD_INGATE_CLOSE:
        gt_ingate->setStatus(GATE_CLOSE);
        mc_stat = MACHINE_INCLOSE;
        break;

    case CMD_BLOCK_OPEN:
        gt_block->setStatus(GATE_OPEN);
        mc_stat = MACHINE_BLOCKOPEN;
        break;

    case CMD_BLOCK_CLOSE:
        gt_block->setStatus(GATE_CLOSE);
        mc_stat = MACHINE_BLOCKCLOSE;
        break;

// 사용자 미사용 커맨드 - 현재는....
    case CMD_SEAL_OPEN:
        gt_seal->setStatus(GATE_OPEN);
        mc_stat = MACHINE_WAIT;
        break;

    case CMD_SEAL_CLOSE:
        gt_seal->setStatus(GATE_CLOSE);
        mc_stat = MACHINE_WAIT;
        break;

    case CMD_LOCK_OPEN:
        gt_lock->setStatus(GATE_OPEN);
        mc_stat = MACHINE_WAIT;
        break;

    case CMD_LOCK_CLOSE:
        gt_lock->setStatus(GATE_CLOSE);
        mc_stat = MACHINE_WAIT;
        break;

  //------------------------------------------------------------------------------

    case CMD_CURRENCY_SET_LIVE:
        curr_stat->setStatus(CURRENCY_SET_LIVE,request);
        break;

    case CMD_BULK_DEPOSIT:
        gt_block->setStatus(GATE_OPEN);
        gt_ingate->setStatus(GATE_OPEN);
        mc_stat = MACHINE_OPENBLOCK;
        bAutoDeposit = true;
        break;

    case CMD_DEPOSIT:
        // Currency stop deposit
        m_retryCnt = 0;
        qDebug() << "CMD_DEPOSIT sencheck" << bChkSen;
        curr_stat->setStatus(CURRENCY_REQ_STOP,request);
        gt_block->setStatus(GATE_OPEN);
        mc_stat = MACHINE_DEPOSIT;
        break;

    case CMD_IDLE:      // Disable Currency, All Gate Closed.
        setSensor(CUR_SEN);
        curr_stat->setStatus(CURRENCY_REQ_STOP,request);
        gt_block->setStatus(GATE_CLOSE);
        gt_ingate->setStatus(GATE_CLOSE);
        gt_outgate->setStatus(GATE_CLOSE);
        mc_stat = MACHINE_IDLE;
        break;

    case CMD_READY:
        gt_outgate->setStatus((GATE_CLOSE));
        gt_ingate->setStatus(GATE_CLOSE);
        gt_block->setStatus(GATE_CLOSE);
        curr_stat->setStatus(CURRENCY_READY,"");
        mc_stat = MACHINE_READY;
        break;

    case CMD_RETURN:
        curr_stat->setStatus(CURRENCY_REQ_STOP,request);
        if(gt_outgate->getStatus() != GATE_OPENED) {
            gt_outgate->setStatus(GATE_OPEN);
        }
        m_retryCnt = 0;
        mc_stat = MACHINE_OUTOPEN;
        break;

    case CMD_CURRENCY_STOP:
        curr_stat->setStatus(CURRENCY_REQ_STOP,request);
        mc_stat = MACHINE_READY;
        break;

    case CMD_CURRENCY_START:
        curr_stat->setStatus(CURRENCY_REQ_START, request);
        mc_stat = MACHINE_WAIT_MONEY;
        setSensor(CUR_SEN);
        break;

    case CMD_CURRENCY_DEVICE_SETUP2:
        curr_stat->setStatus(CURRENCY_DEV_SETUP2, request);
        break;

    case CMD_CURRENCY_STATE_DATA2:      // Currency Set
        curr_stat->setStatus(CURRENCY_STATE_DATA2, request);
        break;
    case CMD_CURRENCY_COUNT_STOP:      // Currency Set
        curr_stat->setStatus(CURRENCY_COUNT_STOP, request);
        break;

    case CMD_CURRENCY_CHANGE:
        curr_stat->setStatus(CURRENCY_CHANGE, request);
        break;

    case CMD_CURRENCY_CHK_DEV_STAT2:
        tmpData.clear();
        tmpData.resize(1);
        tmpData[0] = 0x12;
        curr_stat->setStatus(CURRENCY_RESULT_REQ,tmpData);
        break;

    case CMD_CURRENCY_CHK_CNT_RELSULT:
        tmpData.clear();
        tmpData.resize(1);
        tmpData[0] = 0x02;
        curr_stat->setStatus(CURRENCY_RESULT_REQ,tmpData);
        break;

    case CMD_CURRENCY_CLEAR_JAM:
        tmpData.clear();
        tmpData.resize(1);
        tmpData[0] = 0x20;
        curr_stat->setStatus(CURRENCY_RESULT_CLEAR, tmpData);
        break;
    case CMD_CURRENCY_CLEAR_STACKER:
        tmpData.clear();
        tmpData.resize(1);
        tmpData[0] = 0x02;
        curr_stat->setStatus(CURRENCY_RESULT_CLEAR, tmpData);
        break;

    case CMD_DOOR_UNLOCK:
        tdCmd.append(ID_DOOR_UNLOCK);
        TransTxData(tdCmd);
        setSensor(DOOR_SEN);

        mc_stat = MACHINE_CHK_DOOR_UNLOCK;
        break;

    case CMD_DOOR_LOCK:
        tdCmd.append(ID_DOOR_LOCK);
        TransTxData(tdCmd);
        setSensor(DOOR_SEN);

        mc_stat = MACHINE_CHK_DOOR_LOCK;
        break;

    case CMD_WS_START:
        gt_outgate->setStatus(GATE_CLOSE);
        gt_block->setStatus(GATE_OPEN);
        mc_stat = MACHINE_WS_DEPOSIT;
        m_retryCnt = 0;
        setSensor(CUR_SEN);
        break;

    case CMD_CURRENCY_CHK_SENSOR:
        tmpData.resize(1);
        tmpData[0] = 0x10;
        curr_stat->setStatus(CURRENCY_RESULT_REQ,tmpData);
        break;

    case CMD_BARCODE_ON:
        pBarcode->setStatus(BARCODE_LIGHT_ON, request);
        break;

    case CMD_FINGER_IDT:
        pFinger->setStatus(FINGER_IDENTIFY, request);     
        break;
    case CMD_FINGER_ENROLL:
        pFinger->setStatus(FINGER_ENROLL, request);
        break;
    case CMD_SEAL_TWIST:
        gt_seal->setStatus(GATE_TWIST);
        mc_stat = MACHINE_WAIT;
        break;
    case CMD_DOOR_SEN:
        gt_seal->setStatus(GATE_CHKSEN);
        mc_stat = MACHINE_WAIT;
        break;

    case CMD_COMM_UPPERSEN:
        pComm->setStatus(COMM_UPPERDOOR, request);
        break;

    case CMD_SENDELAY:
    {
        QString tmpVal = QString::number(senValue);
        if(tmpVal.count() < 2){
            tmpVal.prepend("0");
        }
        QByteArray tmpArr;
        class_gate *   pGate;
        switch(senType){
        case 0:
            pGate = gt_outgate;
            tmpArr.append(ID_OUTGATE);
            tmpArr.append('D');
            tmpArr.append(0x31);
            break;
        case 1:
            pGate = gt_outgate;
            tmpArr.append(ID_OUTGATE);
            tmpArr.append('D');
            tmpArr.append(0x32);
            break;
        case 2:
            pGate = gt_ingate;
            tmpArr.append(ID_INGATE);
            tmpArr.append('D');
            tmpArr.append(0x33);
            break;
        case 3:
            pGate = gt_ingate;
            tmpArr.append(ID_INGATE);
            tmpArr.append('D');
            tmpArr.append(0x34);
            break;
        case 4:
            pGate = gt_lock;
            tmpArr.append(ID_LOCK);
            tmpArr.append('D');
            tmpArr.append(0x31);
            break;
        case 5:
            pGate = gt_lock;
            tmpArr.append(ID_LOCK);
            tmpArr.append('D');
            tmpArr.append(0x32);
            break;
        case 6:
            pGate = gt_seal;
            tmpArr.append(ID_SEAL);
            tmpArr.append('D');
            tmpArr.append(0x33);
            break;
        case 7:
            pGate = gt_seal;
            tmpArr.append(ID_SEAL);
            tmpArr.append('D');
            tmpArr.append(0x34);
            break;
        case 8:
            pGate = gt_block;
            tmpArr.append(ID_BLOCK);
            tmpArr.append('D');
            tmpArr.append(0x35);
            break;
        case 9:
            pGate = gt_block;
            tmpArr.append(ID_BLOCK);
            tmpArr.append('D');
            tmpArr.append(0x36);
            break;
        }

        tmpArr.append(tmpVal);
        qDebug() <<"set sensor data : " << tmpArr;
        pGate->command = tmpArr;
        pGate->setStatus(GATE_CHGVAR);
        break;
    }
    }
}

void State_Thread::run()
{
    Gate_TypeDef bl_stat,out_stat,in_stat,seal_stat,lock_stat;
    static uint32_t    waitcnt = 0;
    QByteArray tmpData,tdCmd;
    static bool bToggle = false;
    while(bActive)
    {
        msleep(320);
        bl_stat     = gt_block->getStatus();
        out_stat    = gt_outgate->getStatus();
        in_stat     = gt_ingate->getStatus();
        seal_stat   = gt_seal->getStatus();
        lock_stat   = gt_lock->getStatus();

//        qDebug() << "gate Status : " << bl_stat << out_stat << in_stat << seal_stat << lock_stat;
//    qDebug() << "Machine mode: " << mc_stat;
        if(!bToggle) {
            bToggle = true;

            if(preStep != mc_stat){
                qDebug() << "M retry count 초기화: " << mc_stat << m_retryCnt;
                m_retryCnt = 0;
                preStep = mc_stat;
                inStep = 0;
                inPreStep = 0;
            }
            else{
                if(inStep != inPreStep){
                    qDebug() << "내부스텝에서 M retry count 초기화: " << mc_stat << m_retryCnt;
                    m_retryCnt = 0;
                    inPreStep = inStep;
                }
            }


            switch (mc_stat) {
            qDebug() << "command retry count check: " << mc_stat << m_retryCnt;
            case MACHINE_WAIT:
                mc_stat = MACHINE_INIT;
                break;
            case MACHINE_OUTOPEN:
                if(out_stat != GATE_OPENED){
                    if(waitcnt++>= 3) {
                        waitcnt = 0;
                        if(m_retryCnt++ < 3) {
                            gt_outgate->setStatus(GATE_OPEN);
                        }
                        else{
                            mc_stat = MACHINE_WAIT;
                            gt_outgate->gateStatus = GATE_FAILED;           // 커맨드 실패
                        }
                    }
                }
                else{
                    mc_stat = MACHINE_OUTOPENED;
                }
                break;
            case MACHINE_OUTOPENED:
                break;

            case MACHINE_OUTCLOSE:
                if(out_stat != GATE_CLOSED){
                    if(waitcnt++>= 5) {
                        waitcnt = 0;
                        if(m_retryCnt++ < 3) {
                            gt_outgate->setStatus(GATE_CLOSE);
                        }
                        else{
                            mc_stat = MACHINE_WAIT;
                            if(gt_outgate->gateStatus != GATE_ERROR){            // GATE_ERROR = EMERGENCY RETRY FAILED
                                gt_outgate->gateStatus = GATE_FAILED;           // 커맨드 실패
                            }
                            else{

                            }
                        }
                    }
                }
                else{
                    mc_stat = MACHINE_OUTCLOSED;
                }
                break;
            case MACHINE_OUTCLOSED:
                break;
            case MACHINE_INOPEN:
                if(in_stat != GATE_OPENED){
                    if(waitcnt++>= 5) {
                        waitcnt = 0;
                        if(m_retryCnt++ < 3) {
                            gt_ingate->setStatus(GATE_OPEN);
                        }
                        else{
                            gt_ingate->gateStatus = GATE_FAILED;           // 커맨드 실패
                            mc_stat = MACHINE_WAIT;
                        }
                    }
                }
                else{
                    mc_stat = MACHINE_INOPENED;
                }
                break;
            case MACHINE_INOPENED:
                break;
            case MACHINE_INCLOSE:
                if(in_stat != GATE_CLOSED){
                    if(waitcnt++>= 5) {
                        waitcnt = 0;
                        if(m_retryCnt++ < 3) {
                            gt_ingate->setStatus(GATE_CLOSE);
                        }
                        else{
                            gt_ingate->gateStatus = GATE_FAILED;           // 커맨드 실패
                            mc_stat = MACHINE_WAIT;
                        }
                    }
                }
                else{
                    mc_stat = MACHINE_INCLOSED;
                }
                break;
            case MACHINE_INCLOSED:
                break;

            case MACHINE_BLOCKOPEN:
                if(bl_stat != GATE_OPENED){
                    if(waitcnt++>= 5) {
                        waitcnt = 0;
                        if(m_retryCnt++ < 3) {
                            gt_block->setStatus(GATE_OPEN);
                        }
                        else{
                            gt_block->gateStatus = GATE_FAILED;           // 커맨드 실패
                            mc_stat = MACHINE_WAIT;
                        }
                    }
                }
                else{
                    mc_stat = MACHINE_BLOCKOPENED;
                }
                break;
            case MACHINE_BLOCKOPENED:
                break;
            case MACHINE_BLOCKCLOSE:
                if(bl_stat != GATE_CLOSED){
                    if(waitcnt++>= 5) {
                        waitcnt = 0;
                        if(m_retryCnt++ < 3) {
                            gt_block->setStatus(GATE_CLOSE);
                        }
                        else{
                            gt_block->gateStatus = GATE_FAILED;           // 커맨드 실패
                            mc_stat = MACHINE_WAIT;
                        }
                    }
                }
                else{
                    mc_stat = MACHINE_BLOCKCLOSED;
                }
                break;
            case MACHINE_BLOCKCLOSED:
                break;

            case MACHINE_INIT:
                //                mc_stat = MACHINE_IDLE;
                m_retryCnt = 0;
                break;
            case MACHINE_IDLE:
//                if(in_stat == GATE_CLOSED && out_stat == GATE_CLOSED && bl_stat == GATE_CLOSED) {
//                    mc_stat = MACHINE_READY;
//                }
//                else if(out_stat != GATE_CLOSED){
//                    inStep = 1;
//                    if(waitcnt++>= 5) {
//                        waitcnt = 0;
//                        if(m_retryCnt++ < 6) {
//                            qDebug() << "RETRY OUTGATE CLOSE" << m_retryCnt;
//                            gt_outgate->setStatus(GATE_CLOSE);
//                        }
//                        else{
//                            mc_stat = MACHINE_WAIT;
//                            if(gt_outgate->gateStatus != GATE_ERROR){            // GATE_ERROR = EMERGENCY RETRY FAILED
//                                gt_outgate->gateStatus = GATE_FAILED;           // 커맨드 실패
//                            }
//                        }
//                    }
//                }
//                else if(in_stat != GATE_CLOSED){
//                    inStep = 2;
//                    if(waitcnt++>= 5) {
//                        waitcnt = 0;
//                        if(m_retryCnt++ < 6) {
//                            qDebug() << "RETRY INGATE CLOSE" << m_retryCnt;
//                            gt_ingate->setStatus(GATE_CLOSE);
//                        }
//                        else{
//                            mc_stat = MACHINE_WAIT;
//                            gt_ingate->gateStatus = GATE_FAILED;           // 커맨드 실패
//                        }
//                    }
//                }
//                else if(bl_stat != GATE_CLOSED){
//                    inStep = 3;
//                    if(waitcnt++>= 5) {
//                        waitcnt = 0;
//                        if(m_retryCnt++ < 6) {
//                            qDebug() << "RETRY BLOCKGATE CLOSE" << m_retryCnt;
//                            gt_block->setStatus(GATE_CLOSE);
//                        }
//                        else{
//                            mc_stat = MACHINE_WAIT;
//                            gt_block->gateStatus = GATE_FAILED;           // 커맨드 실패
//                        }
//                    }
//                }

                if(in_stat == GATE_CLOSED && out_stat == GATE_CLOSED) {
                    mc_stat = MACHINE_READY;
                }
                else if(out_stat != GATE_CLOSED){
                    inStep = 1;
                    if(waitcnt++>= 5) {
                        waitcnt = 0;
                        if(m_retryCnt++ < 6) {
                            qDebug() << "RETRY OUTGATE CLOSE" << m_retryCnt;
                            gt_outgate->setStatus(GATE_CLOSE);
                        }
                        else{
                            mc_stat = MACHINE_WAIT;
                            if(gt_outgate->gateStatus != GATE_ERROR){            // GATE_ERROR = EMERGENCY RETRY FAILED
                                gt_outgate->gateStatus = GATE_FAILED;           // 커맨드 실패
                            }
                            else{
                                pMain->pStatusFunc(OUTGATE_EMERGENCY);          // 상위단에 바코드 인식 HooK
                            }
                        }
                    }
                }
                else if(in_stat != GATE_CLOSED){
                    inStep = 2;
                    if(waitcnt++>= 5) {
                        waitcnt = 0;
                        if(m_retryCnt++ < 6) {
                            qDebug() << "RETRY INGATE CLOSE" << m_retryCnt;
                            gt_ingate->setStatus(GATE_CLOSE);
                        }
                        else{
                            mc_stat = MACHINE_WAIT;
                            gt_ingate->gateStatus = GATE_FAILED;           // 커맨드 실패
                        }
                    }
                }
                break;

            case MACHINE_READY:     // Ready to Deposit
                //                setSensor(CUR_SEN);
                if(bAutoDeposit){
                    bAutoDeposit = false;
                }
                break;
            case MACHINE_OPENBLOCK:
//                if(in_stat == GATE_OPENED && bl_stat == GATE_OPENED) {
//                    mc_stat = MACHINE_BULKMODE;
//                    //                    err_id = 0;
//                }
//                else if(bl_stat == GATE_OPENED || in_stat != GATE_OPENED) {
//                    inStep = 1;
//                    if(waitcnt++>= 5) {
//                        waitcnt = 0;
//                        if(m_retryCnt++ < 5) {
//                            gt_ingate->setStatus(GATE_OPEN);
//                        }
//                        else{
//                            gt_ingate->gateStatus = GATE_FAILED;           // 커맨드 실패
//                            mc_stat = MACHINE_WAIT;
//                        }
//                    }
//                }
//                else if(bl_stat != GATE_OPENED) {
//                    inStep = 2;
//                    if(waitcnt++>= 5) {
//                        waitcnt = 0;
//                        if(m_retryCnt++ < 5) {
//                            gt_block->setStatus(GATE_OPEN);
//                        }
//                        else{
//                            gt_block->gateStatus = GATE_FAILED;           // 커맨드 실패
//                            mc_stat = MACHINE_WAIT;
//                        }
//                    }
//                }
                if(in_stat == GATE_OPENED) {
                    mc_stat = MACHINE_BULKMODE;
                    //                    err_id = 0;
                }
                else{
                    inStep = 1;
                    if(waitcnt++>= 5) {
                        waitcnt = 0;
                        if(m_retryCnt++ < 5) {
                            gt_ingate->setStatus(GATE_OPEN);
                        }
                        else{
                            gt_ingate->gateStatus = GATE_FAILED;           // 커맨드 실패
                            mc_stat = MACHINE_WAIT;
                        }
                    }
                }
                break;
            case MACHINE_BULKMODE:
                // bulk mode
                break;
            case MACHINE_WAIT_MONEY:
//                setSensor(CUR_SEN);
//                if(bAutoDeposit) {
//                    if(bl_stat == GATE_OPENED || in_stat != GATE_OPENED) {
//                        inStep = 1;
//                        if(waitcnt++>= 5) {
//                            waitcnt = 0;
//                            if(m_retryCnt++ < 5) {
//                                gt_ingate->setStatus(GATE_OPEN);
//                            }
//                            else{
//                                gt_ingate->gateStatus = GATE_FAILED;           // 커맨드 실패
//                                mc_stat = MACHINE_WAIT;
//                            }
//                        }
//                    }
//                    else if(bl_stat != GATE_OPENED) {
//                        inStep = 2;
//                        if(waitcnt++>= 5) {
//                            waitcnt = 0;
//                            if(m_retryCnt++ < 5) {
//                                gt_block->setStatus(GATE_OPEN);
//                            }
//                            else{
//                                gt_block->gateStatus = GATE_FAILED;           // 커맨드 실패
//                                mc_stat = MACHINE_WAIT;
//                            }
//                        }
//                    }
//                }
//                if(bAutoDeposit) {
//                    if(in_stat != GATE_OPENED) {
//                        inStep = 1;
//                        if(waitcnt++>= 5) {
//                            waitcnt = 0;
//                            if(m_retryCnt++ < 5) {
//                                gt_ingate->setStatus(GATE_OPEN);
//                            }
//                            else{
//                                gt_ingate->gateStatus = GATE_FAILED;           // 커맨드 실패
//                                mc_stat = MACHINE_WAIT;
//                            }
//                        }
//                    }
//                }
                break;

            case MACHINE_DEPOSIT:
//                if(in_stat == GATE_OPENED && bl_stat == GATE_OPENED) {
//                    mc_stat = MACHINE_DEPOSITING;
//                }
//                else if(bl_stat != GATE_OPENED) {
//                    inStep = 1;
//                    if(waitcnt++>= 5) {
//                        waitcnt = 0;
//                        if(m_retryCnt++ < 9) {
//                            qDebug() << "MACHINE DEPOSIT blockgate OPEN: " << m_retryCnt;
//                            gt_block->setStatus(GATE_OPEN);
//                        }
//                        else{
//                            gt_block->gateStatus = GATE_FAILED;           // 커맨드 실패
//                            mc_stat = MACHINE_WAIT;
//                        }
//                    }
//                }
//                if(bl_stat == GATE_OPENED || in_stat != GATE_OPENED) {
//                    inStep = 2;
//                    if(waitcnt++>= 5) {
//                        waitcnt = 0;
//                        if(m_retryCnt++ < 18) {
//                            qDebug() << "MACHINE DEPOSIT ingate OPEN: " << m_retryCnt;
//                            gt_ingate->setStatus(GATE_OPEN);
//                        }
//                        else{
//                            gt_ingate->gateStatus = GATE_FAILED;           // 커맨드 실패
//                            mc_stat = MACHINE_WAIT;
//                        }
//                    }
//                }
                if(in_stat == GATE_OPENED){
                    mc_stat = MACHINE_DEPOSITING;
                }
                else{
                    if(waitcnt++>= 5) {
                        waitcnt = 0;
                        if(m_retryCnt++ < 18) {
                            qDebug() << "MACHINE DEPOSIT ingate OPEN: " << m_retryCnt;
                            gt_ingate->setStatus(GATE_OPEN);
                        }
                        else{
                            gt_ingate->gateStatus = GATE_FAILED;           // 커맨드 실패
                            mc_stat = MACHINE_WAIT;
                        }
                    }
                }
                break;
            case MACHINE_DEPOSITING:
                if(bChkSen != CUR_SEN){
                    setSensor(CUR_SEN);
                }
                if(!curr_stat->curSensor.stacker) {
                    mc_stat = MACHINE_DEPOSITED;
                    qDebug() << "MC_Deposited";
                }
                // Wait deposit complete
                break;
            case MACHINE_DEPOSITED:
                // Check Money Data
                break;

            case MACHINE_RETURN:
                break;
                ////////////////////////WORKING SHEET /////////////////////////////

            case MACHINE_WS_DEPOSIT:
//                if(in_stat == GATE_OPENED && bl_stat == GATE_OPENED) {
//                    mc_stat = MACHINE_WS_DEPOSITED;
//                    qDebug() << "WorkingSheet Deposit ";
//                }
//                else if(bl_stat == GATE_OPENED || in_stat != GATE_OPENED){
//                    inStep = 1;
//                    if(in_stat != GATE_OPENED){
//                        if(waitcnt++>= 5) {
//                            waitcnt = 0;
//                            if(m_retryCnt++ < 5) {
//                                gt_ingate->setStatus(GATE_OPEN);
//                            }
//                            else{
//                                gt_ingate->gateStatus = GATE_FAILED;           // 커맨드 실패
//                                mc_stat = MACHINE_WAIT;
//                            }
//                        }
//                    }
//                }
//                else if(bl_stat != GATE_OPENED){
//                    inStep = 2;
//                    if(waitcnt++>= 5) {
//                        waitcnt = 0;
//                        if(m_retryCnt++ < 5) {
//                            gt_block->setStatus(GATE_OPEN);
//                        }
//                        else{
//                            gt_block->gateStatus = GATE_FAILED;           // 커맨드 실패
//                            mc_stat = MACHINE_WAIT;
//                        }
//                    }
//                }

                if(in_stat == GATE_OPENED) {
                    mc_stat = MACHINE_WS_DEPOSITED;
                    qDebug() << "WorkingSheet Deposit ";
                }
                else{
                    inStep = 1;
                    if(waitcnt++>= 5) {
                        waitcnt = 0;
                        if(m_retryCnt++ < 5) {
                            gt_ingate->setStatus(GATE_OPEN);
                        }
                        else{
                            gt_ingate->gateStatus = GATE_FAILED;           // 커맨드 실패
                            mc_stat = MACHINE_WAIT;
                        }
                    }
                }
                break;
            case MACHINE_WS_DEPOSITED:
                if(!curr_stat->curSensor.stacker) {
                    qDebug() << "WorkingSheet Deposit OK";
                    gt_seal->setStatus(GATE_CLOSE);
                    mc_stat = MACHINE_SEAL;
                }
                break;

            case MACHINE_SEAL:
                if(gt_seal->getStatus() == GATE_OPENED) {
                    gt_seal->setStatus(GATE_CLOSE);
                }
                if(gt_seal->getStatus() == GATE_CLOSED) {
                    gt_lock->setStatus(GATE_OPEN);
                    gt_ingate->setStatus(GATE_CLOSE);
                    mc_stat = MACHINE_CHK_UNLOCK;
                }
                break;

            case MACHINE_CHK_UNLOCK :
                if((lock_stat == GATE_OPENED) ) {
                    waitcnt = 0;
                    m_retryCnt = 0;
                    mc_stat = MACHINE_WAIT_SEAL;
                    setSensor(DOOR_SEN);
                }
                break;

            case MACHINE_WAIT_SEAL:
                if(waitcnt++ > 5) {
                    gt_seal->setStatus(GATE_OPEN);
                    mc_stat = MACHINE_CHK_SEAL;
                }
                break;

            case MACHINE_CHK_SEAL :
                if(gt_seal->getStatus() == GATE_OPENED) {
                    gt_lock->setStatus(GATE_CLOSE);
                    TransHMIData(ACK_SEAL_COMPLETE,NULL);
                    mc_stat = MACHINE_SEALED;
                }
                else{
                    waitcnt = 0;
                    if(m_retryCnt++ > 5){
                        mc_stat = MACHINE_ERROR;
                    }
                    else{
                        mc_stat = MACHINE_WAIT_SEAL;
                    }
                }
                break;

            case MACHINE_SEALED :
                if(lock_stat == GATE_OPENED) {
                    gt_lock->setStatus(GATE_CLOSE);
                }
                if(gt_lock->getStatus() == GATE_CLOSED) {
                    qDebug() << "Sealed gate lock status CLOSED";
                    tmpData.clear();
                    tmpData.append(ID_DOOR_UNLOCK);
                    TransTxData(tmpData);
                    setSensor(DOOR_SEN);
                    waitcnt = 0;
                    mc_stat = MACHINE_CHK_DOOR_UNLOCK;
                }
                break;

            case MACHINE_CHK_DOOR_UNLOCK:
                if(!SenDoorState){                           // 0 = 언락
                    TransHMIData(ACK_DOOR_OPEN,NULL);
                    mc_stat =MACHINE_DOOR_UNLOCKED;
                }
                else{
                    if(waitcnt++ > 5) {
                        mc_stat = MACHINE_CHK_DOOR_UNLOCK;
                    }
                }
                break;

            case MACHINE_DOOR_UNLOCKED:
//                setSensor(DOOR_SEN);
//                if(SenDoorState){
                setSensor(CUR_SEN);
                mc_stat = MACHINE_WAIT;
//                }
                break;

            case MACHINE_CHK_DOOR_LOCK:
                qDebug() << "일로는 안들어오는건가??";
                if(SenDoorState==1){                        // 1 = 락
//                    tdCmd.append(ID_DOOR_LOCK);
//                    TransTxData(tdCmd);
//                    TransHMIData(ACK_DOOR_CLOSE,NULL);
                    mc_stat =MACHINE_DOOR_LOCKED;
                }
                else{
                }
                break;

            case MACHINE_DOOR_LOCKED:
                setSensor(CUR_SEN);
                mc_stat = MACHINE_WAIT;
                break;
            case MACHINE_ERROR:
                break;

            default:
                qDebug() << "error state!!!";
                break;
            } //switch (mc_stat)

        }
        else{
            bToggle = false;
            tmpData.clear();
            switch (bChkSen)
            {
            case CUR_SEN:
                tmpData.resize(1);
                tmpData[0] = 0x10;
                curr_stat->setStatus(CURRENCY_RESULT_REQ,tmpData);
                curCnt -= 1;
                if(curCnt <= 0){
                    curCnt = 5;
                    bChkSen = DOOR_SEN;
                }
                break;
            case DOOR_SEN:
                tmpData.append(ID_DOOR_SEN);
                TransTxData(tmpData);
                doorCnt -= 1;
                if(doorCnt <= 0){
                    doorCnt = 1;
                    bChkSen = CUR_SEN;
                }
                break;

            case UP_SEN:

                break;

            case NO_CHECK:

                break;
            default:
            break;
            }
        }

    } //while(bActive)

}
