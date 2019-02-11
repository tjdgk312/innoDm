#ifndef TCPTHREAD_H
#define TCPTHREAD_H

#include <QThread>
#include <QTcpServer>
#include "tcpserver/tcpserver.h"
#include "QDoubleSpinBox"

class tcpthread : public QThread
{
    Q_OBJECT
public:
    explicit tcpthread(int Thread_ID, QObject *parent = 0);
     ~tcpthread();
    void run();
    void stop();
signals:
    void error(QTcpSocket::SocketError socketerror);

    void ack_systemLog(QString log);
    void ack_deviceReturn(QString state);
    void save_mangerPW();

public slots:
    void readyRead();
    void disconnected();
private:
    QTcpSocket *socket;

    int infoIndex;

    int iCtrFlag[100];  //control_flag data
    int check_addr;
    int socketDescriptor;
    QDoubleSpinBox*   qWrite_buf[800]; /*modbus 100~799*/

    void init_write_buf ();
    void modbus_var_map ();
    void modbus_write_map (int start_addr,int byte_cnt, int* get_Data);
    void Protocol_Handler(char* pRecvBuf);
    char Read_Registers(char* pRecvBuf,char* pSendBuf,int *length);
    char Write_Registers(char* pRecvBuf,char* pSendBuf,int *length);

};

#endif // TCPTHRAED_H
