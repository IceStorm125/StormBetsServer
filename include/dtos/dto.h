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
    QSqlDatabase db;
    std::unique_ptr<DBconnection> connection;
    std::mutex mt;

protected:
    void begin();
    void rollBack();
    void commit();
    void close();
    bool exec(QSqlQuery &query);
};

#endif // DTO_H
