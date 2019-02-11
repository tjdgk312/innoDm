#include "tcpthread.h"
#include "global.h"

#include <QDateTime>
#include <QFile>

#define UDINT unsigned int
#define L_DATA(x,y) (short)(x*y) & 0x0000FFFF
#define H_DATA(x,y) (short)((x*y & 0xFFFF0000) >> 16)


#define READ_REGISTERS      0x03
#define WRITE_REGISTERS     0x10

#define HL_DATA(x,y) ((get_data[x+1] << 16) | (get_data[x] & 0xFFFF) )*y
#define UI_WRITE_DATA(X,Y)  X->setText(Fnum(Y))
#define UI_READ_DATA(X,Y,Z)  pDict[Y] = X->text().toFloat() * Z

int pDict[1600];    //all modbus address data

//#define SEND_INFO      0x31
//#define SEND_CHANGED   0x32

//#define CHECK_DEVICE   0x40             // check state poll
//// 장비ID / 구분 / 금액 / 계약번호 / 식권 / 기타 / 기타2
//#define DEV_DEPOSIT    0x50             // deposit operation
//#define DEV_TRANSFER   0x51


int testSum = 0;

tcpthread::tcpthread(int thread_ID, QObject *parent) :
    QThread(parent)
{
    this->socketDescriptor=thread_ID;
//    init_write_buf();
}

tcpthread::~tcpthread()
{
    qDebug() << "아이고 쓰레드죽네";
    p_Main->initializeFlag = 0;
    emit ack_systemLog("클라이언트와의 연결이 끊어졌습니다.");
    exit(0);
}
void tcpthread::init_write_buf ()
{
}

void tcpthread::run()
{
    infoIndex = 0;
    qDebug()<< socketDescriptor << " - Thread started";
    emit ack_systemLog(QString("%1 클라이언트가 연결되었습니다.").arg(socketDescriptor));

    p_Main->curDoorSen = chk_doorSenState();
    p_Main->curKeySen = chk_KeySenState();
    p_Main->lockFlag = 1;
    p_Main->rollFlag = 1;

    socket = new QTcpSocket();
    socket->flush();
    if(!socket->setSocketDescriptor(this->socketDescriptor))
    {
        emit error(socket->error());
        return;
    }

    connect(socket,SIGNAL(readyRead()),this,SLOT(readyRead()),Qt::DirectConnection);
    connect(socket,SIGNAL(disconnected()),this,SLOT(disconnected()),Qt::DirectConnection);
    exec();

}

void tcpthread::stop()
{
    disconnected();
}
/*******************************************************
 *readMessage : 클라이언트로 부터의 데이터 전달 시 동작
 * 1. QByteArray에 전송 데이터 저장 및 Debug출력
 * 2. 전달된 데이터 그대로 리턴 - Echo Server
 ******************************************************/
void tcpthread::readyRead()
{
//    qDebug() << "server receive";
    char Recvbuf[255]={0};

    //-------------------------------------------파일전송------------------------------------------
    if(p_Main->fingerFileSize > 0){
        qDebug() << socket->readBufferSize();
        QByteArray file;
        while (socket->bytesAvailable() > 0)
        {
            file = socket->readAll();
            p_Main->fingerBufSize += file.size();
            p_Main->curUserFinger->write(file);
            qDebug() << "write file" << p_Main->curUserFinger->size() << p_Main->fingerBufSize << p_Main->fingerFileSize;
            if(p_Main->fingerBufSize >= p_Main->fingerFileSize){
                qDebug() << "다받았다~!!!";
                p_Main->curUserFinger->close();
                p_Main->fingerFileSize = 0;
                file.clear();
                file.append(FINGERCOMP);
                socket->write(file);
            }
        }
        socket->flush();
        return;
    }

    //-------------------------------------------통신프로토콜------------------------------------------
    QByteArray Data=socket->readAll();

    memcpy(Recvbuf,Data,255);
    char funcCode = Recvbuf[0];    //function code
//    qDebug() << "readData print : " << Data << funcCode;

    Data.remove(0,1);       // function code erase

    QByteArray mydata;

    switch(funcCode){
    case S_DEVICEINIT:
    {
        QString infoStr = Data;
        QStringList infoList = infoStr.split(",",QString::SkipEmptyParts);
        p_Main->managerPW = infoList.at(0);
        QString fileSize;
        fileSize = infoList.at(1);
        p_Main->fingerFileSize = fileSize.toInt();
        if(p_Main->fingerFileSize > 0){
            p_Main->curUserFinger->resize(0);
            p_Main->fingerBufSize = 0;
            p_Main->curUserFinger->open(QIODevice::WriteOnly);
        }
        emit save_mangerPW();
        qDebug() << "send device init data";
        emit ack_systemLog(QString("%1 장비데이터를 서버로 전송합니다.").arg(socketDescriptor));
        qDebug() << "send device init data2";
        mydata.append(R_DEVICEINIT);
        mydata.append(p_Main->deviceType+",");         // 장비타입
        mydata.append(p_Main->deviceNum+",");         // 기계시리얼넘버  // later
        mydata.append(p_Main->deviceVer+",");              // 프로그램버젼
        socket->write(mydata);

        // 지문파일 전송 대기--------------------
        break;
    }
    case S_CHECKDEV:
        if(p_Main->initializeFlag == 0){            // 초기화 작업 전이라면 통신완료 후 체크 모드로 들어갈때 완료
            p_Main->initializeFlag = 1;
        }

        if(p_Main->stateFlag == 1){            // 기기상태 변경 플래그
            if(p_Main->stateSwich){
                p_Main->stateSwich = false;
            }
            else{
                qDebug() << "device state process...";
                mydata.append(S_DEVSTATE);
                mydata.append(QString("%1,").arg(p_Main->alarmState));
                QString alText = p_Main->curAlarm[0][2].toString();
                if(alText == ""){
                    alText = "-";
                }
                mydata.append(QString("%1,").arg(alText));
                mydata.append(QString("%1,").arg(p_Main->curDoorSen));
                mydata.append(QString("%1,").arg(p_Main->curKeySen));
                mydata.append(QString("%1,").arg(p_Main->rollFlag));
                mydata.append(QString("%1,").arg(p_Main->bagFlag));
                emit ack_systemLog("기기정보 전송 중");
                socket->write(mydata);
                p_Main->stateSwich = true;
                break;
            }
        }
        else if(p_Main->eventFlag == 1){            // ID 인증요청
            qDebug() << "login assign process...";
            mydata.append(S_REQDBDATA);
            mydata.append(QString("%1,").arg(p_Main->curUserId));
            QString strData = mydata;
            emit ack_systemLog("로그인 인증 요청중...");
            socket->write(mydata);
            qDebug() << "login assign process...4";
            // later 현송정보구현
        }
        else if(p_Main->eventFlag == 2){            // 현송코드 인증요청
            qDebug() << "trans assign process...";
            mydata.append(S_TRANSSIGN);
            mydata.append(QString("%1,").arg(p_Main->curTransCode));
//            mydata.append(QString("%1").arg(p_Main->curUserPW));
            QString strData = mydata;
            emit ack_systemLog("현송 인증 요청중...");
            socket->write(mydata);
            // later 현송정보구현
        }
        else if(p_Main->eventFlag == 3){            // 지문 템플릿 인증요청
            qDebug() << "finger assign process...";
            mydata.append(S_FINGASSIGN);
            mydata.append(QByteArray::fromRawData((const char*)p_Main->templateBuf,384));
            //            mydata.append(p_Main->templateBuf);
            //            mydata.append(QString("%1").arg(p_Main->curUserPW));
            QString strData = mydata;
            emit ack_systemLog("현송 인증 요청중...");
            socket->write(mydata);
            // later 현송정보구현
        }
        else if(p_Main->transactionList.count() > 0){
            if(p_Main->transactionList.values().at(0) == ""){
                p_Main->transactionList.clear();
                return;
            }
            mydata.append(S_TRANSACTION);
            mydata.append(p_Main->transactionList.values().at(0));
            QString strData = mydata;
            qDebug() << "transaction transfer data" << strData;
            emit ack_systemLog(QString("입금정보 전송 %1.").arg(strData));
            socket->write(mydata);
        }
        break;
    case S_SETPW:
    {
        QString infoStr = Data;         // end Event Code;
        p_Main->managerPW = infoStr;
        emit save_mangerPW();
        mydata.append(R_SETPW);
        emit ack_systemLog("관리자 비밀번호 변경 처리 중.. ");
        socket->write(mydata);
        break;
    }
    case R_REQDBDATA:
    {
        QString infoStr = Data;
        QStringList infoList = infoStr.split(",",QString::SkipEmptyParts);
        if(infoList.count() > 0){
            QString tmpStr = infoList.at(0);
            p_Main->assignFlag = tmpStr.toInt();        // 인증 플래그 확인
            emit ack_systemLog(QString("로그인 인증 정보 수신 %1.").arg(infoStr));
            p_Main->eventFlag = 0;
        }
        else{
            // 요청한 계약정보와 수신한 정보가 다름
            emit ack_systemLog(QString("로그인 인증 정보 수신에러 %1.").arg(infoStr));
        }
        break;
    }
    case R_TRANSSIGN:
    {
        QString infoStr = Data;
        QStringList infoList = infoStr.split(",",QString::SkipEmptyParts);
        if(infoList.count() > 0){
            QString tmpStr = infoList.at(0);
            p_Main->assignFlag = tmpStr.toInt();        // 인증 플래그 확인
            emit ack_systemLog(QString("로그인 인증 정보 수신 %1.").arg(infoStr));
            p_Main->eventFlag = 0;
        }
        else{
            // 요청한 계약정보와 수신한 정보가 다름
            emit ack_systemLog(QString("로그인 인증 정보 수신에러 %1.").arg(infoStr));
        }
        break;
    }
    case R_TRANSACTION:
    {
        QString infoStr = Data;
        qDebug() <<"여긴가?" << infoStr;
        for(int i=0; i<p_Main->transactionList.count(); i++){
            qDebug() << p_Main->transactionList.keys().at(i);
        }
        if(p_Main->transactionList.contains(infoStr)){
            bool ret = p_Main->pDatabase->UpdateRecord(RECORD_TABLE,QString("processTime='%1'")
                                                       .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")),QString("time='%1'").arg(infoStr));
            qDebug() << "update transaction data" << ret;
            emit ack_systemLog(QString("트랜잭션 처리가 완료 되었습니다. 목록을 찾아 삭제합니다.").arg(infoStr));
            p_Main->transactionList.remove(infoStr);
        }
        // later 트랜잭션번호 구분
        break;
    }
    case R_FINGASSIGN:
    {
        QString infoStr = Data;
        qDebug() <<"지문인증 리턴드러옴" << infoStr;
        QStringList infoList = infoStr.split(",",QString::SkipEmptyParts);
        if(infoList.count() > 0){
            QString tmpStr = infoList.at(0);
            p_Main->assignFlag = tmpStr.toInt();        // 인증 플래그 확인
            qDebug() <<"지문인증 리턴결과" << tmpStr <<tmpStr.toInt();
            emit ack_systemLog(QString("지문 인증 정보 수신 %1.").arg(infoStr));
            p_Main->eventFlag = 0;
        }
        else{
            qDebug() <<"지문인증 실패결과" << infoStr;
            // 요청한 계약정보와 수신한 정보가 다름
            emit ack_systemLog(QString("지문 인증 정보 수신에러 %1.").arg(infoStr));
        }
        break;
    }
    case R_DEVSTATE:
    {
        QString infoStr = Data;
        emit ack_deviceReturn(infoStr);
        break;
    }
    }
    socket->flush();
}










/*******************************************************
 *Protocol_Handler : Function코드별 구분 함수
 * 1. func_l 값을 바탕으로 구분
 * 2. SendBuf를 만든 후 송신
 ******************************************************/
void  tcpthread::Protocol_Handler(char* pRecvBuf)
{
}
/*******************************************************
 *Read_Registers : 데이터 리드 패키지
 * 1. Start_addr,byte_cnt를 바탕으로 패킷 완성
 * 2. 데이터 자료형은 int으로 지정
 ******************************************************/
char  tcpthread::Read_Registers(char* pRecvBuf,char* pSendBuf,int *length)
{
    int iData;
    quint16 start_addr;
    quint16 byte_cnt;

    start_addr = (pRecvBuf[8] << 8 ) | (pRecvBuf[9] & 0xFF );   //start address
    byte_cnt   = (pRecvBuf[10] << 8 ) | (pRecvBuf[11] & 0xFF );  //Byte Count

    *(pSendBuf+5)   = *length = byte_cnt*2 + 3;  //PDU길이
    *(pSendBuf+6)   = pRecvBuf[6];              //슬레이브 커넥트 라인 및 버스수
    *(pSendBuf+7)   = 0x03;                     //function code
    *(pSendBuf+8)   = byte_cnt*2;               //데이터 총 길이 저장
    pSendBuf +=9;                               //다음 메모리부터는 데이터 입력

    //Data mapping upload
    modbus_var_map();

    for(int rou =0;rou<byte_cnt;rou++){

        iData = (pDict[start_addr+rou]&0xFF) << 8 | pDict[start_addr+rou] >> 8; //memcpy시 반전됨
        memcpy(pSendBuf,(char*)&iData,2); //Char형 2byte로 지정(16bit)
        pSendBuf +=2;   //메모리 주소 증
    }
}
/*******************************************************
 *Write_Registers : 데이터 리드 패키지
 * 1. Start_addr,byte_cnt를 바탕으로 패킷 완성
 ******************************************************/
char tcpthread::Write_Registers(char *pRecvBuf, char *pSendBuf, int *length)
{
    int get_Data[255];
    quint16 start_addr;
    quint16 byte_cnt;

    *(pSendBuf+5)   = *length = 6;  //PDU길이
    *(pSendBuf+6)   = pRecvBuf[6];  //슬레이브 커넥트 라인 및 버스수
    *(pSendBuf+7)   = 0x10;  //function code
    *(pSendBuf+8)   = pRecvBuf[8];  //startaddr_H
    *(pSendBuf+9)   = pRecvBuf[9];  //startaddr_L
    *(pSendBuf+10)  = pRecvBuf[10];  //Quantify of Register_H
    *(pSendBuf+11)  = pRecvBuf[11];  //Quantify of Register_L

    /*******************************************************************
     *write register일 시 write 할 데이터를 get_Data의 배열에 16bit로 저장
     *******************************************************************/
    start_addr = (pRecvBuf[8] << 8 ) | (pRecvBuf[9] & 0xFF );   //start address
    byte_cnt   =  pRecvBuf[12];  //Byte Count

    for(int rou =0;rou<byte_cnt;rou++){ //16bit데이로 묶음
        get_Data[rou/2] = (pRecvBuf[13+rou] << 8 ) | (pRecvBuf[14+rou] & 0xFF );
        rou++;
    }

    modbus_write_map (start_addr,byte_cnt,get_Data);
}
void tcpthread::disconnected()
{
    p_Main->lockFlag = 1;
    //qDebug() <<socketDescriptor<<" Disconnected";
    qDebug() << "아이고 쓰레드 Disconnected";
    p_Main->initializeFlag = 0;
    emit ack_systemLog(QString("%1 클라이언트 연결이 해제되었습니다.").arg(socketDescriptor));
    socket->deleteLater();
    exit(0);
}

void tcpthread::modbus_write_map (int start_addr,int byte_cnt,int* get_data)
{
    float fScale = 0.1;

    /*******************************************************************
     *어드레스 100번 아래데이터는 에디터를 사용하지 않은 Value값으로 데이터를 저장
     *iCtrFlag 배열을 통해서 Value데이터 비교
     *******************************************************************/
    if(start_addr <100){
        for(int rou=0; rou<byte_cnt/2;rou++){   //16bit 데이터 묶음
            if(start_addr+rou >= 100)
                break;
            //iCtrFlag의 배열 번호와 모드버스 주소와 같음
            //각 배열에 16bit 씩 저장되므로 get_data 1개씩(앞 함수에서 8bit+8bit) 매칭
            pDict[start_addr+rou] = get_data[rou];
        }
    }
    else{
        //if(pDict[28]== 0 && pDict[30] == 0 ) //write state flag check
        //{return;}


        for(int rou=0; rou<byte_cnt/2;rou++){   //32bit 데이터 묶음
            /*******************************************************************
             *아래 if조건은 소수점 1자리인 모드버스 주소에 대한 범위임
             *******************************************************************/
//            if(  (start_addr+rou >=194 && start_addr+rou <= 212) ||(start_addr+rou >=234 && start_addr+rou <= 236) ||(start_addr+rou >=258 && start_addr+rou <= 266) || (start_addr+rou >=298 && start_addr+rou <= 316)
//               ||(start_addr+rou >=418 && start_addr+rou <= 420) ||(start_addr+rou >=434 && start_addr+rou <= 452) || (start_addr+rou >=474 && start_addr+rou <= 492)
//               ||(start_addr+rou >=514 && start_addr+rou <= 520) ||(start_addr+rou >=574 && start_addr+rou <= 582) || (start_addr+rou >=634 && start_addr+rou <= 642)
//               ||(start_addr+rou >=654 && start_addr+rou <= 658) ||(start_addr+rou >=698 && start_addr+rou <= 702) || (start_addr+rou >=734 && start_addr+rou <= 738)
//               ||(start_addr+rou >=746 && start_addr+rou <= 750) ||(start_addr+rou >=786 && start_addr+rou <= 790) ){
//               fScale = 0.1;
//            }
//            else
//                fScale = 1;

            if(  (rou >=100 && rou <= 112) || rou == 752 || (rou >=770 && rou <= 772)){
               fScale = 1;
            }
            else
                fScale = 0.1;

            if(qWrite_buf[start_addr+rou] == NULL) //SP 데이터엔 에디터의 메모리가 없기 때문에 0을 입력
                qWrite_buf[start_addr+rou]= 0;
            else{
                //qWrite_buf[start_addr+rou]->setValue(((get_data[rou+1] << 16 ) | (get_data[rou] & 0xFFFF ))*fScale);
                qWrite_buf[start_addr+rou]->setValue(get_data[rou]*fScale);

                //32bit data write
                if(start_addr==112){
                    pDict[112] = get_data[rou];
                    pDict[113] = get_data[rou+1];
//                    t_pProctrl.spin2List.at(0).at(1)->setValue(HL_DATA(112,1));
                }
            }
            rou++;  // 기준 모드버스 어드레스가 2씩 증가
        }

        if(start_addr >=100 && start_addr <= 110){
            /*Set Data *100~799*/
//            p_Main->pLogin->on_bt_setTime_clicked();
        }
    }

    //write complete flag
    pDict[28] = 0;
    pDict[30] = 0;

}

void tcpthread::modbus_var_map ()
{
}
