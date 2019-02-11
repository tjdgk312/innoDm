#ifndef FINGER_CLASS_H
#define FINGER_CLASS_H

#include "include/SFM_Error.h"
#include "include/SFM_Command.h"
#include "include/SFM_Enroll.h"
#include "include/SFM_Packet.h"
#include "include/SFM_SysParameter.h"
#include "include/UF_API.h"
#include "include/SFM_Serial.h"
#include "include/SFM_RS232.h"


#include <QElapsedTimer>
#include <QThread>
#include <QObject>

// common
#define FINGER_IDLE             0
#define FINGER_DISCONNECT       1

// scan temp
#define FINGER_GETPARFAIL       2
#define FINGER_TIMEOUT          3
#define FINGER_SCANCOMPLETE     4

// enroll
#define FINGER_ENROllSUCCESS    5
#define FINGER_ENROllFAIL       6

// identify
#define FINGER_IDENSUCCESS      7
#define FINGER_IDENFAIL         8

// verify
#define FINGER_FLAGFAIL         9

// LoadDB
#define FINGER_LOADING          10
class finger_class : public QObject
{
    Q_OBJECT
public:
    explicit finger_class(QObject *parent = 0);

    QThread fingerThread;

    void Intro();
    void MainMenu();
    int StartProgram();
    void CommunicationSetup();
    void Verify();
    void SystemParameterSetting();
    void ReadSystemParameterSetting();
    int GetSystemParameterIndex(char* paramName);
    void ChangeSystemParameterSetting();
    void Exit();

    //Communication Setup
    void SetPort();
    void SetBaudrate();
    void SetProtocol();
    void SetModuleID();

    int Disconnect();
    int Reconnect();
    int SearchModule();
    int ChangeCommunicationSetup();

    //Communicaiton Setup submenu
    void CurrentCummunicationSetup();
    void CommunicationSetupMenu();
    void ChangeCommunicationSetupMenu();

    void WaitKey();
    void Wait(int n);

    void SendPacket(unsigned char* data);


    void BinToHex(unsigned char c ,unsigned char *buf);
    void BinToHexPacket(unsigned char* inbuf, unsigned char *outbuf, int inbuf_size);

    void printline();

    //commander

    void Commander();
    int GetCommandParameterIndex(char* paramName);
    int GetStrToInt(char* str);

    SysParameter *sysParameter;
    //global variable

    int g_nPort;
    int g_nBaudrate;
    int g_nProtocol;  //0 : single   1: network
    int g_nModuleID;
    int g_bConnectionStatus;
    int g_bTrace;

    QByteArray templateData;
    unsigned char templateBuf[384];

    int RecieveExtraPacket(unsigned char *buf, int size, int timout);
    void SendRawPacket(unsigned char *data, int size);

    int UFLoadDB(const char *fileName);
signals:
    int sConnect();
    void sEnroll();
    void sScan();
    void sIden();
    void sLoad();
public slots:
    int Connect();

private slots:
    void Enroll();
    void ScanTemplate();
    void Identify();
    void LoadDB();
    void DeleteAll();


    int RecievePacket(unsigned char *buf, int size, int timout);
    void ClearBuffer();
    void set_timeout();
    void run();
};

#endif // FINGER_CLASS_H
