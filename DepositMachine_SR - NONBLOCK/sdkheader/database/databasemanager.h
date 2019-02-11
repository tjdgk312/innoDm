#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H
//==============================================================================
//	@file    databasemanager.cpp
//	@author  ksh
//	@date    2017.06.15
//	@brief   database file management functions
//
//==============================================================================

#include <QWidget>
#include "QtSql"
#include "QDoubleSpinBox"
#include "qpushbutton.h"
#include "QLabel"
//#include "Typedef.h"

#define MINVAL 0
#define MAXVAL 1
#define SUMVAL 2
#define COUNT 3

class DatabaseManager : public QWidget
{
    Q_OBJECT

public:
    DatabaseManager();
    ~DatabaseManager();

private:

public:
    void OpenDatabase(QString DBname);
    void CreateTable(QString table);
    bool Droptable(QString table);

    int SelectAllData(QString table,QVariant buf[][5]);
    void DeleteAllData(QString table);

    int SelectRecord(QString table, QString column, QString record, QVariantList *buf);
    bool InsertRecord(QString table, QString values);
    bool UpdateRecord(QString table, QString SetSql, QString term);
    void DeleteRecord(QString table, QString column);

    int SelectMinMax(QString column, QString table, int type);

    void SignalAllData(QString table, int SIGbuf[][100]);
    QSqlDatabase	db;	//get Sql class

    QStringList tables;
    int SelectAllTable(QString table, QString column, QVariant buf[]);
    int SelectRecordOrder(QString table, QString column, QString record, QVariant buf[]);
    int SelectColumnAll(QString table, QString column, QList<QVariantList> *buf);
    int SelectExist(QString term);
};

#endif // DATABASEMANAGER_H
