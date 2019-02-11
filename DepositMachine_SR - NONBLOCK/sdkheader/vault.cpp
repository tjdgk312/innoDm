#include "vault.h"
#include "sdk_global.h"

int innoSdkInit()
{
    pMain = new InnoSDK();
    return pMain->initFlag;
}

int vaultGetStatus()
{
    return pMain->currentState;
}

int vaultGetLastError()
{
    return pMain->errorStateCode;
}

void setDepositCountry(char *countrySel)
{
    pMain->m_stateThread->setSensor(CUR_SEN);
    QString tmpStr = countrySel;
    QString countryStr = tmpStr.mid(0,2);
    pMain->currentCurrencyId = countryStr.toInt();
    QByteArray tmpArr;
    tmpArr.append(countrySel);
    pMain->m_stateThread->transaction(CMD_CURRENCY_STATE_DATA2,tmpArr);
    pMain->setCountryUnitList(countryStr.toInt());
}

void setDepositCount(char *depositCount)
{
    pMain->m_stateThread->setSensor(CUR_SEN);
    QString tmpStr = depositCount;
    QString countryStr = tmpStr.mid(0,2);
    pMain->currentCurrencyId = countryStr.toInt();
    QByteArray tmpArr;
    tmpArr.append(depositCount);
    qDebug()  << tmpArr;
    pMain->m_stateThread->transaction(CMD_CURRENCY_CHANGE,tmpArr);
    pMain->setCountryUnitList(countryStr.toInt());

    pMain->currentStep = DEPOSIT_WAITINPUT;
    pMain->waitCnt = 2;
}

void cmd_repairErrorState(int errCode){
//    switch(errCode){
//    case 11:
//    case 17:
//        pMain->m_stateThread->transaction(CMD_IDLE,"");
//        break;
//    case 14:
//    case 19:
//        pMain->m_stateThread->transaction(CMD_DEPOSIT,"");
//        break;
//    case 16:
//        pMain->m_stateThread->transaction(CMD_BULK_DEPOSIT,"");
//        break;
//    }
}

//--------------------------------------입금관련-----------------------------------------
int setCurrentMode(int mode)
{
}

int getCurrentMode()
{
    return pMain->currentMode;
}

void vaultOpenGate()
{
    pMain->m_stateThread->transaction(CMD_OUTGATE_OPEN,"");
}

void vaultCloseGate()
{
    pMain->m_stateThread->transaction(CMD_OUTGATE_CLOSE,"");
}

void vaultOpenBlock()
{
    pMain->m_stateThread->transaction(CMD_BLOCK_OPEN,"");
}

void vaultCloseBlock()
{
    pMain->m_stateThread->transaction(CMD_BLOCK_CLOSE,"");
}

void clearErrorState()
{
    pMain->m_stateThread->transaction(CMD_CURRENCY_CLEAR_JAM,"");
}

void clearStackerState(){
    pMain->m_stateThread->transaction(CMD_CURRENCY_CLEAR_STACKER,"");
}

void vaultOpenAcceptorGate()
{
    pMain->m_stateThread->transaction(CMD_INGATE_OPEN,"");
}

bool chk_outPocket()
{
    if(pMain->m_stateThread->bChkSen != CUR_SEN){
        pMain->m_stateThread->setSensor(CUR_SEN);
    }
    return pMain->tCurrSen.stacker;
}

void vaultGetScrowInboxInfo(LPVAULT_SCROW_INBOX_INFO scrowInfo)
{
    if(pMain->tCurrSen.stacker) {      // 아웃포켓 상태체크
        scrowInfo->status = pMain->tCurrSen.stacker;
    }
    int sumCount = 0;
    for(int i=0; i<12; i++){
        sumCount += pMain->tNoteData.nData[i];
    }
    scrowInfo->billsCount = sumCount;
}

#ifdef SDK_DATABASE

double vaultGetLastAmountDeposited()
{
    QVariant buf[10];
    pMain->pDatabase->db.setDatabaseName(PATH_DEPOSIT);
    pMain->pDatabase->db.open();
//    SELECT * FROM USER WHERE userName = 'sungha' ORDER BY expiresDateTime DESC LIMIT 1
    int result = pMain->pDatabase->SelectRecordOrder("DEPOSITLIST","sumTotal","ORDER BY DepositDateTime DESC LIMIT 1",buf);
    double resultSum = 0;
    if(result){
        resultSum = buf[0].toDouble();
    }
    return resultSum;
    // DB 에서 가장 마지막 데이터 읽어서 총금액 리턴
}

int vaultGetLastCurrencyID()
{
    QVariant buf[10];
    pMain->pDatabase->db.setDatabaseName(PATH_DEPOSIT);
    pMain->pDatabase->db.open();
//    SELECT * FROM USER WHERE userName = 'sungha' ORDER BY expiresDateTime DESC LIMIT 1
    int result = pMain->pDatabase->SelectRecordOrder("DEPOSITLIST","CurrencyId","ORDER BY DepositDateTime DESC LIMIT 1",buf);
    double resultSum = 0;
    if(result){
        resultSum = buf[0].toInt();
    }
    return resultSum;
}
#endif


bool get_CountState()
{
//    qDebug() << "called countstate - " << pMain->fCountStart;
    return pMain->fCountStart;
}

bool get_CountComplete(){
    return pMain->fOutStack;
}

bool get_InputState()
{
//    qDebug() << "get input state - " << pMain->tCurrSen.hopper;
    return pMain->tCurrSen.hopper;
}

void cmd_CountStart()
{
//    if(pMain->loginState){
//        if(role==13 || role==2047){
//            pMain->m_stateThread->transaction(CMD_CURRENCY_START,"");
//        }
//    }
    pMain->currentStep = DEPOSIT_WAITINPUT;
}

void cmd_CountStartAct()
{
    pMain->autoErrClearCnt = 0;
    pMain->m_stateThread->transaction(CMD_CURRENCY_START,"");
    pMain->currentStep = DEPOSIT_WAITCOUNT;
    pMain->waitCnt = 2;
}

void cmd_RequestStop(){
    pMain->currentStep = 0;         // 어떤모드에서든 Stop을 받았을 때 대기모드로 전환하기 위함
    pMain->m_stateThread->transaction(CMD_CURRENCY_COUNT_STOP,"");
}


LPVAULTDEPOSIT_INFO vaultGetDepositList()
{
    return pMain->deposit_data;
}

void vaultCloseAcceptorGate()
{
    pMain->m_stateThread->transaction(CMD_INGATE_CLOSE,"");
}


LPVAULT_DEPOSIT_INFO vaultGetDepositList(time_t DepositDateTime, int *plen)
{
}

//-----------------------------------------------------------------------------------------------------------------------------------------
//void vaultGetStatusInfo(VAULT_STATUS_INFO *pvault_info)
//{
//    char *msg = pMain->getStatusMessage(pMain->currentState);
//    pvault_info->status = pMain->currentState;
//    memcpy(pvault_info->status_msg, msg, sizeof(msg));
//}



int vaultGetLastTransactionID()
{
    return 0;
}

void transUnlockDoorGate()
{
    pMain->m_stateThread->transaction(CMD_DOOR_UNLOCK,"");
}

void transLockDoorGate()
{
    pMain->m_stateThread->transaction(CMD_DOOR_LOCK,"");
}

bool get_doorOpenState()
{
    return pMain->fDoorOpen;
}

bool get_doorCloseState()
{
    return pMain->fDoorClose;
}

bool chk_KeySenState()
{
    return pMain->m_stateThread->SenDoorKey;
}

bool chk_doorSenState()
{
    return pMain->m_stateThread->SenDoorState;
}

bool chk_bagChangeState(){
    qDebug() << "called barcode change state - " << pMain->preBagCode << pMain->currentBagCode;
    if(pMain->preBagCode != pMain->currentBagCode){
        pMain->preBagCode = pMain->currentBagCode;
        return 1;
    }
    else{
        return 0;
    }
}

int chk_fingerState(){
//    qDebug() << "called Finger state - " << pMain->fingerState ;
//    return pMain->fingerState;
    return pMain->fScan;
}

void cmd_SettlementStart()
{
    pMain->transStep = 1;
    pMain->m_stateThread->transaction(CMD_WS_START,"");
}

void cmd_barcodeScanStart()
{
    uint32_t   sendCmd = CMD_BARCODE_ON;
    pMain->m_stateThread->transaction(sendCmd,"3");    // 3 is retry count.
}
void cmd_fingerScanStart()
{
//    pMain->fScan = FINGER_IDLE;
    emit pMain->pFinger->sScan();
    qDebug() << "scanTemplate command";
//    uint32_t   sendCmd = CMD_FINGER_IDT;
//    pMain->m_stateThread->transaction(sendCmd,"3");    // 3 is retry count.
//    pMain->fingerState = 0;
}

void cmd_fingerEnrollStart()
{
    pMain->fScan = FINGER_IDLE;
    emit pMain->pFinger->sEnroll();
    qDebug() << "enroll command";
//    uint32_t   sendCmd = CMD_FINGER_ENROLL;
//    pMain->m_stateThread->transaction(sendCmd,"3");    // 3 is retry count.
//    pMain->fingerState = 0;
}

void cmd_fingerIdenStart()
{
    pMain->fScan = FINGER_IDLE;
//    pMain->fEnroll = 0;
    emit pMain->pFinger->sIden();
    qDebug() << "finger iden command";
}

void cmd_fingerConnect(){
    pMain->fScan = FINGER_IDLE;
    emit pMain->pFinger->sConnect();
    qDebug() << "finger connect command";
}

void cmd_fingerLoadDB(){
    pMain->fScan = FINGER_IDLE;
    emit pMain->pFinger->sLoad();
    qDebug() << "finger load DB command";
}

int getFingerTemplate(unsigned char *pTemplate)
{
    qDebug() << "get template data";

    if(pMain->fScan == FINGER_SCANCOMPLETE){
        for(int i=0; i<384; i++){
            pTemplate[i] = pMain->pFinger->templateBuf[i];
        }
//        pTemplate = pMain->pFinger->templateBuf;
        return 1;
    }
    else{
        return 0;
    }
}

bool get_fingerConnectState()
{
    return pMain->fConnection;
}

void vaultGetCurrentUser(char *buffUser, int len)
{
    buffUser = const_cast<char*>(pMain->currentLogUser);
    len = sizeof(pMain->currentLogUser);
}


void vaultSetDepositEvents(PFN_INNOSTEC_ON_DEPOSIT_COMPLETE pEventHandler, void *user_data)
{
    pMain->pDepositFunc = pEventHandler;
    pMain->my_data_ptr = user_data;
}

void vaultSetSettlementEvents(PFN_INNOSTEC_ON_SETTLEMENT_COMPLETE pEventHandler, void *user_data)
{
    pMain->pSettlementFunc = pEventHandler;
    pMain->my_stdata_ptr = user_data;
}

bool get_CountingState()
{
    return pMain->fCountStart;
}

void get_LastCountData(){
    pMain->fOutStack = false;
    pMain->countCompleteSignal();
}

bool chk_rejectPocket()
{
    if(pMain->tCurrSen.reject_f || pMain->tCurrSen.reject_r){
        return 1;
    }
    else{
        return 0;
    }
}

void vaultSetStateEvent(PFN_INNOSTEC_CURRENT_STATE pEventHandler)
{
    pMain->pStatusFunc = pEventHandler;
}

void vaultIntermediateDeposit()
{
    pMain->waitCnt = 100;        // 추가 입금동작 완료까지 제한시간 10초
    pMain->intermediateFlag = 1;
    pMain->currentStep = DEPOSIT_ADDDEPOSIT;
    pMain->m_stateThread->transaction(CMD_DEPOSIT,"");
}

int finConnectState()
{
    return pMain->pFinger->g_bConnectionStatus;
}

void get_barcodeData(char *barcodeData, int *len)
{
    pMain->barcodeByte = pMain->barcodeByte.trimmed();
//    memcpy(barcodeData,pMain->barcodeByte.data(), pMain->barcodeByte.size());
    for(int i=0; i<pMain->barcodeByte.count(); i++){
        barcodeData[i] = pMain->barcodeByte.at(i);
    }
//    barcodeData = pMain->barcodeByte.data();
    *len = pMain->barcodeByte.count();
//    qDebug() <<barcodeData << pMain->barcodeByte;
}


void cmd_sealOpen()
{
    pMain->m_stateThread->transaction(CMD_SEAL_OPEN,"");
}

void cmd_sealClose()
{
    pMain->m_stateThread->transaction(CMD_SEAL_CLOSE,"");
}

void cmd_lockOpen()
{
    pMain->m_stateThread->transaction(CMD_LOCK_OPEN,"");
}

void cmd_lockClose()
{
    pMain->m_stateThread->transaction(CMD_LOCK_CLOSE,"");
}

int get_outGateState()
{
    return pMain->m_stateThread->gt_outgate->gateStatus;
}

int get_inGateState()
{
    return pMain->m_stateThread->gt_ingate->gateStatus;
}

int get_blGateState()
{
    return pMain->m_stateThread->gt_block->gateStatus;
}

void cmd_twistSeal(){
    pMain->m_stateThread->transaction(CMD_SEAL_TWIST,"");
}

int get_sealGateState()
{
    return pMain->m_stateThread->gt_seal->gateStatus;
}

int get_fingerState()
{
    return pMain->pFinger->g_bConnectionStatus;
}

void set_SensorDelay(int type, int value){

    pMain->m_stateThread->senType = type;
    pMain->m_stateThread->senValue = value;
    pMain->m_stateThread->transaction(CMD_SENDELAY,"");
}

int cmd_chkDoorSen()
{
    pMain->m_stateThread->transaction(CMD_COMM_UPPERSEN,"");
//    pMain->m_stateThread->transaction(CMD_DOOR_SEN,"");
}


int get_upsStatus(){
//    qDebug() << "get UPS Status";
    return pMain->m_stateThread->curr_stat->getStatus();
    // for test UPS setup
//    return 0;
}

