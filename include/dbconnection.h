#ifndef DBCONNECTION_H
#define DBCONNECTION_H

#include <QSqlDatabase>

class DBconnection
{
public:
    DBconnection();
    DBconnection(DBconnection &other) = delete;
    DBconnection& operator=(DBconnection &other) = delete;
    ~DBconnection();

    static DBconnection& getInstance();

    QSqlDatabase getDB();
private:
    QSqlDatabase db;
};

#endif // DBCONNECTION_H
