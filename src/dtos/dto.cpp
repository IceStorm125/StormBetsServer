#include "dtos/dto.h"
#include <QDebug>
#include <QSqlError>

DTO::DTO()
{
    db = DBconnection::getInstance().getDB();
}

DTO::~DTO()
{
    //db.close();
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
