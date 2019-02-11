#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>

#include <QTcpServer>   // Qt ServerTest
#include <QTcpSocket>
#include "tcpthread.h"
class tcpthread;

class TcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit TcpServer(QObject *parent = 0);

    char Read_Registers(char* pRecvBuf,char* pSendBuf,int *len);

//    tcpthread *pthread;
protected:
    void incomingConnection(int socketDescriptor);

private:
    //QTcpServer *server;
    //QTcpSocket *socket;

signals:

public slots:

};

#endif // TCPSERVER_H
