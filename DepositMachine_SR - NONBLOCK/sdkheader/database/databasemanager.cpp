//==============================================================================
//	@file    databasemanager.cpp
//	@author  ksh
//	@date    2017.06.15
//	@brief   database file management functions
//
//==============================================================================
#include "databasemanager.h"
#include <QtWidgets/QMessageBox>
#include "qdebug.h"
#include  "QtWidgets/qlineedit.h"


DatabaseManager::DatabaseManager()
{
    //database file
    db= QSqlDatabase::addDatabase("QSQLITE");	//sqlite drive use
    db.connectOptions();
}

/*******************************************************************
 *Description: DB Open Function
 *[IN] name	: DB Name
 *[OUT] result : read Name and OpenDB
 *******************************************************************/
void DatabaseManager::OpenDatabase(QString DBname)
{
    db.setDatabaseName(DBname);	//param file use(all parameter
    if(!db.open()){
        QMessageBox reply(QMessageBox::Warning,
                          "error",
                          "Unable to open database",
                          QMessageBox::Ok,this, Qt::FramelessWindowHint);
        reply.setStyleSheet("QMessageBox{border: 2px solid rgb(0, 0, 0); font: 13pt;} QPushButton { min-height: 31px; min-width: 61px; }");
        if(reply.exec() == QMessageBox::Warning){}
    }
    tables = db.tables();	//get database table
}


void DatabaseManager::CreateTable(QString table)
{
    QSqlQuery query;
    qDebug() <<  query.exec(QString("CREATE TABLE %1") .arg(table));
}


bool DatabaseManager::Droptable(QString table)
{
    QSqlQuery query;
    query.exec(QString("DROP TABLE '%1'") .arg(table));

    return true;
}


/***************************************************************
 *SelectAllData : 데이터베이스의 테이블 별 모든 데이터 Read
 * table:각 페이지 별 데이터베이스 테이블명
 * buf: 저장 배열
 ****************************************************************/
int DatabaseManager::SelectAllData(QString table,QVariant buf[][5])
{
    QSqlQuery query;
    int idx=0;
    int columnCnt = 15;
    query.exec(QString("select *from %1").arg(table));
    while (query.next())
    {
        for(int i=0; i<columnCnt; i++){
            if(!query.value(i).isValid()) columnCnt = i;
            buf[idx][i] = query.value(i);
        }
        idx++;
    }
    return idx;
}

void DatabaseManager::DeleteAllData(QString table)
{
    QSqlQuery query;
    query.exec(QString("delete from %1").arg(table));
}


int DatabaseManager::SelectExist(QString term)
{
    QSqlQuery query;
    int ret=0;
    query.exec(QString("SELECT EXISTS (%1)").arg(term));
    while (query.next())
    {
        while(query.value(ret).isValid()){   // 검색 열 갯수검사
            ret = query.value(ret).toInt();
            break;
        }
        break;
    }
    return ret;
}


int DatabaseManager::SelectRecordOrder(QString table, QString column, QString record, QVariant buf[])
{
    QSqlQuery query;
    int idx=0;
    query.exec(QString("select %1 from %2").arg(column).arg(table).arg(record));
    while (query.next())
    {
        while(query.value(idx).isValid()){   // 검색 열 갯수검사
            buf[idx] = query.value(idx);
            idx++;
            //qDebug() << buf[idx];
        }
    }
    return idx;
}

int DatabaseManager::SelectRecord(QString table, QString column, QString record, QVariantList *buf)
{
    QSqlQuery query;
    int idx=0;
    QString str = QString("select %1 from %2 WHERE %3").arg(column).arg(table).arg(record);
    qDebug() << str;
    query.exec(str);
    while (query.next())
    {
        while(query.value(idx).isValid()){   // 검색 열 갯수검사
            buf->append(query.value(idx));
            idx++;
            //qDebug() << buf[idx];
        }
    }
    return idx;
}

int DatabaseManager::SelectAllTable(QString table, QString column, QVariant buf[])
{
    QSqlQuery query;
    int idx=0;
    query.exec(QString("select %1 from %2").arg(column).arg(table));
    while (query.next())
    {
        while(query.value(idx).isValid()){   // 검색 열 갯수검사
            buf[idx] = query.value(idx);
            idx++;
            //qDebug() << buf[idx];
        }
    }
    return idx;
}

bool DatabaseManager::InsertRecord(QString table, QString values)
{
    QSqlQuery query;
    QString queryStr = QString("insert into %1 values(%2)").arg(table).arg(values);
    qDebug() << "InsertRecord Start" << queryStr;
    bool result = query.exec(queryStr);
    qDebug() << "InsertRecord End : " << result;
    return result;
}

bool DatabaseManager::UpdateRecord(QString table, QString SetSql, QString term)
{
    QSqlQuery query;
    bool result = query.exec(QString("UPDATE %1 SET %2 WHERE %3")
               .arg(table).arg(SetSql).arg(term));
    return result;
}

void DatabaseManager::DeleteRecord(QString table, QString column)
{
    QSqlQuery query;
    query.exec(QString("delete from %1 where %2").arg(table).arg(column));
}
/*******************************************************************
 *SelectMinMax : 해당 열의 값 중 최대 또는 최소인 값 검색
 * column: 검색할 열 이름
 * table: 검색 테이블 명
 * DEFINE type:  MINVAL=0 MAXVAL=1
 *******************************************************************/
int DatabaseManager::SelectMinMax(QString column, QString table, int type)
{
    QString query_name;
    QSqlQuery query;
    switch(type){
    case MINVAL:
        query_name = QString("SELECT min(%1) FROM %2").arg(column).arg(table);
        break;
    case MAXVAL:
        query_name = QString("SELECT max(%1) FROM %2").arg(column).arg(table);
        break;
    case SUMVAL:
        query_name = QString("SELECT sum(\"%1\") FROM %2").arg(column).arg(table);
        break;
    case COUNT:
        query_name = QString("SELECT count(%1) FROM %2").arg(column).arg(table);
        break;
    }
    int value=0;
    query.exec(query_name);
    if(query.next()){
        value= query.value(0).toInt();
    }
//    qDebug() << query.value(0) << query_name;
    return value;
}

void DatabaseManager::SignalAllData(QString table, int SIGbuf[][100])
{
    QSqlQuery query;
    query.exec(QString("select *from %1").arg(table));
    int i =0;
    while(query.next()){
        SIGbuf[0][i] = query.value(0).toString().mid(1).toInt(); //X삭제
        SIGbuf[1][i] = query.value(1).toInt();
        SIGbuf[2][i] = query.value(2).toString().mid(1).toInt();//Y삭제
        SIGbuf[3][i] = query.value(3).toInt();
        i++;
    }
}

int DatabaseManager::SelectColumnAll(QString table, QString column, QList<QVariantList> *buf)
{
    QSqlQuery query;
    int idx=0;
    query.exec(QString("select %1 from %2").arg(column).arg(table));
    while (query.next())
    {
        QVariantList list;
        int colIdx = 0;
        while(query.value(colIdx).isValid()){   // 검색 열 갯수검사
//            qDebug() << query.value(colIdx).toString();
            list.append(query.value(colIdx));
            colIdx++;
        }
        buf->append(list);
        idx++;
    }
    return idx;
}


DatabaseManager::~DatabaseManager()
{
    db.close();
}

