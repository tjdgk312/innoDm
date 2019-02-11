#ifndef MACHINE_H
#define MACHINE_H

#include <QObject>

class machine : public QObject
{
    Q_OBJECT
public:
    explicit machine(QObject *parent = 0);

signals:

public slots:
};

#endif // MACHINE_H
