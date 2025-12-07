#ifndef DBCONNECTION_H
#define DBCONNECTION_H

#include <QSqlDatabase>

class DBconnection {
public:
    DBconnection() = delete;

    static QSqlDatabase connection();

private:
    static QSqlDatabase createConnection();

    static bool ping(QSqlDatabase &db); // 👈 объявляем функцию
};

#endif // DBCONNECTION_H
