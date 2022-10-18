#include "dto.h"
#include <QDebug>
#include <QSqlError>

DTO::DTO()
{
    open();
}

DTO::~DTO()
{
   db.close();
}

void DTO::open()
{
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setDatabaseName("my_schema");
    db.setUserName("root");
    db.setPassword("root");

    if(!db.open())
    {
        qInfo() << "DB is not open";
        qInfo() << db.lastError().text();
    }
    qInfo() << "DB is open";
}

void DTO::begin()
{
    db.transaction();
}

void DTO::rollBack()
{
    db.rollback();
}

void DTO::commit()
{
    db.commit();
}

void DTO::close()
{
    db.close();
}

bool DTO::exec(QSqlQuery &query)
{
    begin();
    if(!query.exec())
    {
        qInfo() << "Fail: " << query.lastError().text();
        rollBack();
        return false;
    }

    commit();

    return true;
}
