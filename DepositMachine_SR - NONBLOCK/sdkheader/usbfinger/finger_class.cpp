#include "finger_class.h"
#include <QDebug>
#include "include/UF_API.h"
#include "include/SFM_Template.h"

#include "include/SFM_Command.h"
#include "include/SFM_Packet.h"
#include "include/SFM_SysParameter.h"

#include "include/SFM_Serial.h"
#include "include/SFM_Error.h"

#include "../sdk_global.h"

#include <QFile>

#define UF_VALID_TEMPLATE_DB	0x1f2f3f4f

#define MAX_TEMPLATE_NO		10000

void (*s_ScanCallback_S)( BYTE ) = NULL;
void (*s_UserInfoCallback_S)( int, int ) = NULL;


finger_class::finger_class(QObject *parent):
    QObject(parent)
{
    g_nPort = 25;           // 23-AMA0 25-ACM0
    g_nBaudrate = 115200;   // 115200
    g_nProtocol = 0;  //0 : single   1: network
    g_nModuleID = 1;
    g_bConnectionStatus = 0;
    g_bTrace = 0;

//    pMain->fScan = FINGER_IDLE;
    UF_SetAsciiMode(0);

//    connect(&fingerThread,SIGNAL(started()),this,SLOT(run()));
//    connect(&fingerThread,SIGNAL(finished()),this,SLOT(stop()));
    connect(&fingerThread,SIGNAL(finished()),&fingerThread,SLOT(deleteLater()));
    connect(this,SIGNAL(sConnect()),this,SLOT(Connect()));
    connect(this,SIGNAL(sEnroll()),this,SLOT(Enroll()));
    connect(this,SIGNAL(sScan()),this,SLOT(ScanTemplate()));
    connect(this,SIGNAL(sIden()),this,SLOT(Identify()));
    connect(this,SIGNAL(sLoad()),this,SLOT(LoadDB()));
    emit sConnect();
    this->moveToThread(&fingerThread);
    fingerThread.start();
}

void finger_class::run()
{
    qDebug() << "finger Thread run start";
    while(1){

    }
}

void finger_class::set_timeout(){
}

void finger_class::Enroll()
{
    UF_ENROLL_OPTION flag = UF_ENROLL_AUTO_ID;
    int ret = UF_Enroll(0,flag,0,0);
    qDebug() << "Finger Enroll " << ret;

    switch(ret){
    case UF_PROTO_RET_SUCCESS:
    case UF_RET_SUCCESS:
        pMain->fScan = FINGER_ENROllSUCCESS;
        break;
    default:
        pMain->fScan = FINGER_ENROllFAIL;
        break;
    }

//    int result;
//    UINT32 userID;
//    int imageQaulity;
//    int enroll_mode;
//    int index;
//    BYTE packet[SFM_PACKET_LEN];
//    int param = 0;
//    int size = 0;
//    int flag = UF_ENROLL_AUTO_ID;

//    if(g_bConnectionStatus == 0)
//    {
//        printf("The module was not connected. Please, Connect to module first.\n");
////        WaitKey();
//        qDebug() << "g_bConnectionStatus is 0";
//        pMain->fScan = FINGER_DISCONNECT;
//        return;
//    }
//    UF_MakePacket( SFM_COM_ES , param, size, flag, packet );

//    SendPacket(packet);

//    //Wait(500);
//    index = GetSystemParameterIndex("ENROLL_MODE");
//    enroll_mode = sysParameter[index].value;

//    if(enroll_mode == UF_ENROLL_TWO_TEMPLATES1 || enroll_mode == UF_ENROLL_TWO_TIMES1)
//    {
//        qDebug() << "[ Place finger on the sensor ]";
//        ClearBuffer();
//        RecievePacket(packet, SFM_PACKET_LEN, 10000);
//        if(SFM_GetPacketValue(SFM_PACKET_FLAG, packet) == UF_PROTO_RET_SCAN_SUCCESS){
//            qDebug() << "[ Scan Success ]";
//        }

//        //Wait(500);
//        ClearBuffer();
//        RecievePacket(packet, SFM_PACKET_LEN, 10000);
//        if(SFM_GetPacketValue(SFM_PACKET_FLAG, packet) == UF_PROTO_RET_SUCCESS)
//        {
//            qDebug() << "[ Success ]";
//            userID = SFM_GetPacketValue(SFM_PACKET_PARAM, packet);
//            imageQaulity = SFM_GetPacketValue(SFM_PACKET_SIZE, packet);
//            printf("\n[ Enrolled UserID : %d  Score : %d ]\n",userID, imageQaulity);
//        }
//    }
//    if(enroll_mode == UF_ENROLL_TWO_TIMES2 || enroll_mode == UF_ENROLL_TWO_TEMPLATES2)
//    {
//        UF_MakePacket( SFM_COM_ES , param, size, UF_ENROLL_CONTINUE, packet );

//        SendPacket(packet);
//    }
//    qDebug() << "[ Place finger on the sensor ]";
//    ClearBuffer();
//    RecievePacket(packet, SFM_PACKET_LEN, 10000);
//    if(SFM_GetPacketValue(SFM_PACKET_FLAG, packet) == UF_PROTO_RET_SCAN_SUCCESS){
//        printf("\n[ Scan Success ]\n\n");
//    }

//    //Wait(500);
//    ClearBuffer();
//    RecievePacket(packet, SFM_PACKET_LEN, 10000);
//    if(SFM_GetPacketValue(SFM_PACKET_FLAG, packet) == UF_PROTO_RET_SUCCESS)
//    {
//        pMain->fScan = FINGER_ENROllSUCCESS;
//        printf("\n[ Success ]\n\n");
//        userID = SFM_GetPacketValue(SFM_PACKET_PARAM, packet);
//        imageQaulity = SFM_GetPacketValue(SFM_PACKET_SIZE, packet);
//        printf("\n[ Enrolled UserID : %d  Score : %d ]\n",userID, imageQaulity);
//    }
//    else{
//        pMain->fScan = FINGER_ENROllFAIL;
//    }
//    ClearBuffer();
}

void finger_class::ScanTemplate()
{
//    pMain->fScan = FINGER_IDLE;

//    unsigned int templateSize;
//    unsigned int imageQuality;
//    BYTE packet[SFM_PACKET_LEN];

//    int ret = UF_ScanTemplate(templateBuf,&templateSize,&imageQuality);
//    qDebug() << "Finger Scan Template " << ret;

//    switch(ret){
//    case UF_PROTO_RET_SUCCESS:
//    case UF_RET_SUCCESS:
//        pMain->fScan = FINGER_SCANCOMPLETE;
//        break;
//    default:
//        pMain->fScan = FINGER_TIMEOUT;
//        break;
//    }

    pMain->fScan = 99;
    qDebug() << "ScanTemplate Call";

    unsigned int templateSize;
    unsigned int imageQuality;
    BYTE packet[SFM_PACKET_LEN];
    UINT32 param = 0;
    UINT32 size = 0;
    BYTE flag = SFM_ADD_CHECKSUM;
    UINT32 version;
    BYTE buf[5];
    unsigned checksum = 0;
    int i;

    if(g_bConnectionStatus == 0)
    {
        printf("The module was not connected. Please, Connect to module first.\n");
        //        WaitKey();
        qDebug() << "g_bConnectionStatus is 0";
        pMain->fScan = FINGER_DISCONNECT;
        return;
    }

    int result = UF_GetSysParameter( UF_SYS_FIRMWARE_VERSION, &version );

    if( result != UF_RET_SUCCESS )
    {
        printf("[ UF_GetSysParameter ]\n");
        pMain->fScan = FINGER_GETPARFAIL;
        return;
    }

    qDebug() << "Scan Template 2";

    UF_MakePacket( SFM_COM_ST , param, size, flag, packet );

    SendPacket(packet);

    qDebug() << "Scan Template 1";

    qDebug() << "[ Place finger on the sensor ]";
    ClearBuffer();
    int ret = RecievePacket(packet, SFM_PACKET_LEN, 5000);
    if(ret == UF_ERR_WRITE_SERIAL_TIMEOUT){
        pMain->fScan = FINGER_TIMEOUT;
        return;
    }
    if(SFM_GetPacketValue(SFM_PACKET_FLAG, packet) == UF_PROTO_RET_SCAN_SUCCESS){
        qDebug() << "[ Scan Success ]";
    }
    else{
        pMain->fScan = FINGER_FLAGFAIL;
        return;
    }


    qDebug() << "Scan Template 2";

    //Wait(500);
    ClearBuffer();
    ret = RecievePacket(packet, SFM_PACKET_LEN, 5000);
    if(ret == UF_ERR_WRITE_SERIAL_TIMEOUT){
        pMain->fScan = FINGER_TIMEOUT;
        return;
    }

    qDebug() << "Scan Template 3";

    if(SFM_GetPacketValue(SFM_PACKET_FLAG, packet) == UF_PROTO_RET_SUCCESS)
    {
//        qDebug() << "[ Success ]";
        param = SFM_GetPacketValue(SFM_PACKET_PARAM, packet);
        size = SFM_GetPacketValue(SFM_PACKET_SIZE, packet);
        printf("\n[ Scanned param : %d  Score : %d ]\n",param, imageQuality);
    }

    qDebug() << "Scan Template 4";

    templateSize = size;
    imageQuality = param;

    qDebug() << "get template size qual: " << templateSize << imageQuality;
//    BYTE packetTemp[templateSize];

    ret = RecieveExtraPacket(templateBuf, templateSize, 5000);
    if(ret == UF_ERR_WRITE_SERIAL_TIMEOUT){
        pMain->fScan = FINGER_TIMEOUT;
        return;
    }

    qDebug() << "Scan Template 5";

//    QFile tmpB("bufDEV.dat");
//    int openRet = tmpB.open(QFile::WriteOnly);
//    qDebug() <<"create file and open: " <<openRet;
//    tmpB.write(templateData);
//    tmpB.close();

    QByteArray testByte = QByteArray::fromRawData((const char*)templateBuf,384);
    QFile tmpR("getDataTestDEV.dat");
    int openRet = tmpR.open(QFile::WriteOnly);
    qDebug() <<"create file and open: " <<openRet;
    tmpR.write(testByte);
    tmpR.close();

    qDebug() << "Scan Template 6";

//    templateBuf = packetTemp;
    pMain->fScan = FINGER_SCANCOMPLETE;

    qDebug() << "extra packet: " << param << size;
    qDebug() << "Error check" << result << templateData << templateSize << imageQuality;
}

void finger_class::Identify()
{
    UINT32 userId;
    BYTE subId;
    int ret = UF_Identify(&userId,&subId);
    qDebug() << "Finger Identify " << ret;

    switch(ret){
    case UF_PROTO_RET_SUCCESS:
    case UF_RET_SUCCESS:
        pMain->fScan = FINGER_IDENSUCCESS;
        break;
    default:
        pMain->fScan = FINGER_IDENFAIL;
        break;
    }


//    int result;
//    UINT32 userID;
//    UINT32 subID;
//    BYTE packet[SFM_PACKET_LEN];
//    int param = 0;
//    int size = 0;
//    int flag = 0;

//    if(g_bConnectionStatus == 0)
//    {
//        printf("The module was not connected. Please, Connect to module first.\n");
//        pMain->fScan = FINGER_DISCONNECT;
//        return;
//    }

//    UF_MakePacket( SFM_COM_IS , param, size, flag, packet );

//    SendPacket(packet);


//    printf("Place finger on the sensor\n");
//    ClearBuffer();
//    RecievePacket(packet, SFM_PACKET_LEN, 5000);
//    if(SFM_GetPacketValue(SFM_PACKET_FLAG, packet) == UF_PROTO_RET_SCAN_SUCCESS){
//        printf("\n[ Scan Success ]\n\n");
//    }

//    ClearBuffer();
//    RecievePacket(packet, SFM_PACKET_LEN, 5000);
//    if(SFM_GetPacketValue(SFM_PACKET_FLAG, packet) == UF_PROTO_RET_SUCCESS)
//    {
//        printf("\n[ Success ]\n");
//        userID = SFM_GetPacketValue(SFM_PACKET_PARAM, packet);
//        subID = SFM_GetPacketValue(SFM_PACKET_SIZE, packet);
//        printf("\n[ Identified UserID : %d(%d) ]\n",userID, subID);
//        pMain->fScan = FINGER_IDENSUCCESS;
//    }
//    else{
//        pMain->fScan = FINGER_IDENFAIL;
//    }
//    ClearBuffer();
}

void finger_class::LoadDB()
{
    pMain->fScan = FINGER_LOADING;
    QFile test("fingerList.db");
    int ret = UF_LoadDB("fingerList.db");
    qDebug() << "LoadDB :"  << ret << test.exists();

    switch(ret){
    case UF_PROTO_RET_SUCCESS:
    case UF_RET_SUCCESS:
        pMain->fScan = FINGER_IDLE;
        break;
    default:
        pMain->fScan = FINGER_IDLE;
        break;
    }
}

void finger_class::DeleteAll()
{

}

void finger_class::ReadSystemParameterSetting()
{
}

int finger_class::GetSystemParameterIndex(char *paramName)
{
}

int finger_class::Connect()
{
    int ret = UF_InitCommPort(USB_INTERFACE_DEV0,g_nBaudrate,0);
    qDebug() << "Finger Identify " << ret;

    switch(ret){
    case UF_PROTO_RET_SUCCESS:
    case UF_RET_SUCCESS:
        g_bConnectionStatus = 1;
        break;
    default:
        g_bConnectionStatus = 0;
        break;
    }

    return ret;

//    char mode[]={'8','N','1',0};

//    qDebug() <<"connect start - " << g_nPort - 1 << g_nBaudrate << mode;

//    //Connection
//    if(SFM_RS232_OpenComport(USB_INTERFACE_DEV0, g_nBaudrate))
//    {
//        qDebug() << "FINGER Can not open port!";
//        g_bConnectionStatus = 0;
//        g_nPort = 26;           // 23-AMA0 25-ACM0
//        if(SFM_RS232_OpenComport(USB_INTERFACE_DEV0, g_nBaudrate))
//        {
//            qDebug() << "FINGER Can not open port!";
//            g_bConnectionStatus = 0;
//            return 0;
//        }
//    }

//    qDebug() << "FINGER open port!";

//    unsigned char send_buf[SFM_PACKET_LEN]={0x40,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x44,0x0A};
//    unsigned char recv_buf[SFM_PACKET_LEN]="";
//    int flag;
//    int chksum;
//    //port test

//    SendPacket(send_buf);
//    qDebug() <<"connect send packet";

//    //Wait(200);
//    RecievePacket(recv_buf, SFM_PACKET_LEN, 3000);

//    qDebug() <<"end receive packet";

//    flag = SFM_GetPacketValue(SFM_PACKET_FLAG, recv_buf);
//    chksum = SFM_GetPacketValue(SFM_PACKET_CHECKSUM, recv_buf);

//    qDebug() << "connection packet state = " << flag << chksum;

//    if(flag == UF_PROTO_RET_SUCCESS)
//    {
//        g_bConnectionStatus = 1;
//        qDebug() << "connection success!" << g_bConnectionStatus;
////        ReadSystemParameterSetting();

//#ifdef __linux__
//        system("clear");
//#elif _WIN32
//        system("cls");
//#endif
//        qDebug() << QString("[ Port %1 (%1bps)was connected ]").arg(g_nPort).arg(g_nBaudrate);
//    }
//    ClearBuffer();
//    return 1;
}

void finger_class::SendPacket(unsigned char *data)
{
    int writeLen = UF_WriteData(data, SFM_PACKET_LEN, 2000 );

    if( writeLen < 0 ) {
        if (g_ChannelType == UF_SERIAL_CHANNEL)
            return;
    } else if ( writeLen < SFM_PACKET_LEN) {
        if (g_ChannelType == UF_SERIAL_CHANNEL)
            return;
    }

    qDebug() << "send packet!" << data;
}

void finger_class::SendRawPacket(unsigned char *data, int size)
{
//    int pnt = 0;
//    while(pnt < size){
//        if(size - pnt > 254){
//            RS232_SendBuf(g_nPort - 1, data+pnt, 254);
//            pnt += 254;
//        }
//        else{
//            RS232_SendBuf(g_nPort - 1, data+pnt, size - pnt);
//            pnt += (size - pnt);
//        }
//    }

////    BYTE endCode = UF_PACKET_END_CODE;
////    RS232_SendBuf(g_nPort - 1, data+pnt, size - pnt);
////    pnt += (size - pnt);

//    usleep(100);
//    qDebug() << "send raw packet!" << data << pnt;
}

int finger_class::UFLoadDB(const char* fileName)
{
}

void finger_class::BinToHex(unsigned char c, unsigned char *buf)
{
//    TOHEX(c, buf);
}

int finger_class::RecievePacket(unsigned char *buf, int size, int timout)
{
    int i,n,s,pos;
    unsigned char tempbuf[size];
    unsigned char hexbuf[size*3];
    int _cmd,_param,_size,_flag,_chksum;

    memset(tempbuf, 0, size);
    memset(hexbuf, 0, size);

    n = 0;
    pos = 0;

    QString getData;

    while(1)
    {
//        usleep(100000);

        //n = RS232_PollComport(g_nPort - 1, tempbuf+pos, size-(pos-1));
        n = SFM_RS232_PollComport(tempbuf+pos, 1);

//        qDebug() << "send packet count = " << n;
        if(n > 0)
        {
//            qDebug() << "read byate available: " << n;

            tempbuf[n + pos] = '\0';   /* always put a "null" at the end of a string! */
            //hexbuf[(n + pos)*2-1] = 0;

            for(i=pos; i < n+pos; i++)
            {
//                qDebug() << QString::number(tempbuf[i], 16).toUpper();
                getData.append(QString::number(tempbuf[i], 16).toUpper()+" ");
                buf[i] = tempbuf[i];
            }

            //break;
        }

        if(tempbuf[pos]==SFM_PACKET_END_CODE || pos == SFM_PACKET_LEN){
            qDebug() << "send packet endpoint!" << tempbuf[pos] << pos;
            break;
        }

        pos += n;
    }

    qDebug() << "getData: " << getData;

    BinToHexPacket(tempbuf, hexbuf, size);
    /*
    printf("received %i bytes: ", pos);
    for(i=0; i<size; i++)
    {
        printf("%c",(char)tempbuf[i]);
    }
*/
//    printf("%8s : ", "[RECV]");
    for(i=0; i<size*3; i++)
    {
//        printf("%c",(char)hexbuf[i]);
    }
//    printf("\n");

    if(buf[SFM_PACKET_END_CODE_POS]==SFM_PACKET_END_CODE && buf[SFM_PACKET_START_CODE_POS]==SFM_PACKET_START_CODE )
    {
        _cmd = SFM_GetPacketValue(SFM_PACKET_COMMAND, buf);
        _param = SFM_GetPacketValue(SFM_PACKET_PARAM, buf);
        _size = SFM_GetPacketValue(SFM_PACKET_SIZE, buf);
        _flag = SFM_GetPacketValue(SFM_PACKET_FLAG, buf);
        _chksum = SFM_GetPacketValue(SFM_PACKET_CHECKSUM, buf);

//        printf("%8s = 0x%02x(%d)\n","Command", _cmd, _cmd);
//        printf("%8s = 0x%08x(%d)\n","Param", _param, _param);
//        printf("%8s = 0x%08x(%d)\n","Size", _size, _size);
//        printf("%8s = 0x%02x(%d)\n","Flag", _flag, _flag);
//        printf("%8s = 0x%02x(%d)\n","CheckSum", _chksum, _chksum);
    }
    return 0;
}

int finger_class::RecieveExtraPacket(unsigned char *buf, int size, int timout)
{

    int i,n,s,pos;
    unsigned char tempbuf[size+5];
    unsigned char hexbuf[(size+5)*3];
    int _cmd,_param,_size,_flag,_chksum;

    memset(tempbuf, 0, size+5);
    memset(hexbuf, 0, size+5);

    n = 0;
    pos = 0;

    QString getData;
    int idx = 0;
    templateData.clear();

    while(1)
    {
//        usleep(100000);
//        qDebug() << "read packet elapsed: " << finger_timer->elapsed();
        //n = RS232_PollComport(g_nPort - 1, tempbuf+pos, size-(pos-1));
        n = SFM_RS232_PollComport(tempbuf+pos, 1);

//        qDebug() << "send packet count = " << n;
        if(n > 0)
        {
//            qDebug() << "read byate available: " << n;

//            tempbuf[n + pos] = '\0';   /* always put a "null" at the end of a string! */
            //hexbuf[(n + pos)*2-1] = 0;

            for(i=pos; i < n+pos; i++)
            {
                if(idx <= size){
//                    qDebug() << idx << QString::number(tempbuf[i], 16).toUpper();
                    getData.append(QString::number(tempbuf[i], 16).toUpper());
                    templateData.append(tempbuf[i]);
                    buf[idx] = tempbuf[i];
                }
                idx++;
            }

            //break;
        }

        if(idx > 384 && tempbuf[pos]==SFM_PACKET_END_CODE){
            qDebug() << "send packet endpoint!" << tempbuf[pos] << pos;
            break;
        }
        pos += n;
    }

//    templateData.append(buf);
//    QFile tmpR("rawTemp.dat");
//    tmpR.open(QFile::WriteOnly);
//    QByteArray tmpByteR;
//    tmpByteR.append(templateData);
//    tmpR.write(tmpByteR);
//    tmpR.close();

    qDebug() << "getData: " << getData << templateData;
    return 0;
}



void finger_class::ClearBuffer()
{
    int i,n;
    unsigned char tempbuf[SFM_PACKET_LEN];

        while(1)
        {
//            usleep(100000);
//            qDebug() << "Clear buffer read packet elapsed: " << finger_timer->elapsed();
            n = SFM_RS232_PollComport(tempbuf, SFM_PACKET_LEN);

            if(n > 0)
            {
                tempbuf[n] = 0;   /* always put a "null" at the end of a string! */

                for(i=0; i < n; i++)
                {
                    if(tempbuf[i] < 32)  /* replace unreadable control-codes by dots */
                    {
                    //    tempbuf[i] = '.';
                    }
                }
            }
            else{
                break;
            }
        }
}

void finger_class::BinToHexPacket(unsigned char *inbuf, unsigned char *outbuf, int inbuf_size)
{
    int i;

    for(i=0; i<inbuf_size; i++)
    {
        BinToHex(inbuf[i], &outbuf[i*3]);
        outbuf[i*3+2] = 32;
    }
}
