#include "dtos/dto.h"
#include <QDebug>
#include <QSqlError>

#include "spdlog/spdlog.h"

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
        spdlog::warn(query.lastError().text().toStdString());
        rollBack();
        return false;
    }

    commit();

    return true;
}
