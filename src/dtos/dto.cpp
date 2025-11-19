#include "dtos/dto.h"
#include <QDebug>
#include <QSqlError>

#include "spdlog/spdlog.h"

DTO::DTO()
{
    connection = std::make_unique<DBconnection>();
    db = connection->getDB();
}

DTO::~DTO()
{
    db.close();
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
    std::unique_lock<std::mutex> lock(mt);

    if(db.isOpen())
    {
        db.close();
    }
}

bool DTO::exec(QSqlQuery &query)
{
    std::unique_lock<std::mutex> lock(mt);
    begin();
    if(!query.exec())
    {
        spdlog::warn("exec" + query.lastError().text().toStdString());
        rollBack();
        return false;
    }

    commit();

    return true;
}
