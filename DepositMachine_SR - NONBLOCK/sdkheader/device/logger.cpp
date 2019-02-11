#include "logger.h"

#include <QDateTime>
#include <QDebug>

logger::logger(QObject *parent, QString fileName) : QObject(parent) {
    m_showDate = true;
    if (!fileName.isEmpty()) {
        file = new QFile;
        file->setFileName(fileName+".txt");
        if(file->open(QIODevice::Append | QIODevice::Text) != true){
//            qDebug() << "log file open Fail";
        }
    }
}

void logger::write(QByteArray value) {
//    qDebug() << value.toHex();
    if (m_showDate){
        QByteArray wrtieBuf;
        QString textName = QDateTime::currentDateTime().toString("hh:mm:ss:zzz ");
        wrtieBuf.append(textName);
        wrtieBuf.append(value.toHex());
        wrtieBuf.append("\n");
        file->write(wrtieBuf);
    }
    file->close();
}

void logger::setShowDateTime(bool value) {
    m_showDate = value;
}

logger::~logger() {
    if (file != 0)
    file->close();
}
