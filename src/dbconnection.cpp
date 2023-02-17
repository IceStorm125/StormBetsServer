#include "dbconnection.h"

#include <QDebug>
#include <QSqlDatabase>

DBconnection::DBconnection()
{   

}

DBconnection::~DBconnection()
{
    db.close();
}

QSqlDatabase DBconnection::getDB()
{
    if(db.isOpen())
    {
        qInfo() << "DB already open";
        return db;
    }

    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setDatabaseName("my_schema");
    db.setUserName("root");
    db.setPassword("root");

    if(!db.open())
    {
        qInfo() << "DB is not open";
    }
    else
    {
        qInfo() << "DB is open";
    }
    return db;
}

DBconnection &DBconnection::getInstance()
{
    static DBconnection con;
    return con;
}

