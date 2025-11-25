#ifndef DBCONNECTION_H
#define DBCONNECTION_H

#include <QSqlDatabase>

class DBconnection
{
public:
    DBconnection();
    ~DBconnection();
    
    static QSqlDatabase& connection();
    static QSqlDatabase createConnection();
private:
};

#endif // DBCONNECTION_H
