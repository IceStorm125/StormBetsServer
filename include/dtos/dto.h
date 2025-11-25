#ifndef DTO_H
#define DTO_H
#include <QSqlQuery>
#include <QSqlDatabase>

#include <mutex>
#include <memory>

#include "dbconnection.h"
class DTO
{
public:
    DTO();
    ~DTO();
private:
    std::mutex mt;

protected:
    bool exec(QSqlQuery &query);
};

#endif // DTO_H
