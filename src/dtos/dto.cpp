#include "dtos/dto.h"
#include <QDebug>
#include <QSqlError>

#include "spdlog/spdlog.h"

DTO::DTO()
{
}

DTO::~DTO()
{
}


bool DTO::exec(QSqlQuery &query)
{
    std::unique_lock<std::mutex> lock(mt);
    if(!query.exec())
    {
        spdlog::warn(__PRETTY_FUNCTION__ + query.lastError().text().toStdString());
        return false;
    }

    return true;
}
