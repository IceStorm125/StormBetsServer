#include "dbconnection.h"

#include <QDebug>
#include <QSqlDatabase>

#include "spdlog/spdlog.h"

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
        // spdlog::info("DB already open");
        return db;
    }

    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setDatabaseName("stormbets");
    db.setUserName("root");
    db.setPassword("root");

    db.open() ? spdlog::info("DB is open") : spdlog::critical("DB is not open");

    return db;
}

DBconnection &DBconnection::getInstance()
{
    static DBconnection con;
    return con;
}

