#ifndef MATCH_H
#define MATCH_H

#include <QPair>
#include <QString>
#include <QDateTime>

#include "event.h"
class Match : public Event
{
public:

    Match();
    Match(long ID_, const QString &teamName1, const QString &teamName2, double koef1, double koef2, const QDateTime &time_);

    std::string toPrint() override;   

    long getID() const;

    QPair<QString, double> getTeam1() const;

    QPair<QString, double> getTeam2() const;

    QDateTime getTime() const;

private:
    long ID;
    QPair<QString, double> team1;
    QPair<QString, double> team2;
    QDateTime time;
};

#endif // MATCH_H
