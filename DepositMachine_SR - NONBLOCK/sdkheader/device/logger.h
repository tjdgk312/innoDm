#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <qfile.h>
#include <QtWidgets/QPlainTextEdit>
#include <QTextStream>

class logger : public QObject
{
    Q_OBJECT
public:
    explicit logger(QObject *parent, QString fileName);
    ~logger();
    void setShowDateTime(bool value);

private:
    QFile *file;
    bool m_showDate;

signals:

public slots:
    void write(QByteArray value);

};

#endif // LOGGER_H




