#ifndef DBCONNECTION_H
#define DBCONNECTION_H

#include <QSqlDatabase>

class DBconnection
{
public:
    DBconnection();
    ~DBconnection();

    QSqlDatabase getDB();
private:
    QSqlDatabase db;
};

#endif // DBCONNECTION_H
