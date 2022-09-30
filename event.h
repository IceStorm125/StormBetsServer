#ifndef EVENT_H
#define EVENT_H
#include <string>

#include <QPair>
#include <QString>
#include <QDateTime>

class Event
{

public:
    virtual std::string toPrint() = 0;

};



#endif // EVENT_H
