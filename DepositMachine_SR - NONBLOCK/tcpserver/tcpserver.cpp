#include "tcpserver.h"
#include "global.h"

#define READ_REGISTERS  0x03

QLineEdit*   edit_Monitoring_buf[255];
TcpServer::TcpServer(QObject *parent) :
    QTcpServer(parent)
{
    // TCP Server 설정 modbus port 502
    if(!this->listen(QHostAddress::Any, 4880)){
        qDebug() << "Server could not start";
    }
    else{
        qDebug() << "Server started! Port = 4880";
        }
    ///*MultiThread **/pthread = new MultiThread(1,this);
   // connect(pthread,SIGNAL(finished()),pthread,SLOT(deleteLater()));
    //pthread->start();
}
void TcpServer::incomingConnection(int socketDescriptor)
{
    qDebug() << socketDescriptor << " Connecting.." ;
    tcpthread *pthread = new tcpthread(socketDescriptor,this);
    connect(pthread,SIGNAL(finished()),pthread,SLOT(deleteLater()));
    connect(pthread,  &tcpthread::ack_systemLog, p_Main, &MainWindow::setSystemlogData);
    connect(pthread,  &tcpthread::ack_deviceReturn, p_Main, &MainWindow::setDeviceFlag);
    connect(pthread,  &tcpthread::save_mangerPW, p_Main, &MainWindow::setManagerPw);
    pthread->start();
}


