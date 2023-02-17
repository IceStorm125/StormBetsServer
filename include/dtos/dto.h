#ifndef DTO_H
#define DTO_H
#include <QSqlQuery>
#include <QSqlDatabase>

class DTO
{
public:
    DTO();
    ~DTO();
private:
    QSqlDatabase db;
protected:
    void open();
    void begin();
    void rollBack();
    void commit();
    void close();
    bool exec(QSqlQuery &query);
};

#endif // DTO_H
