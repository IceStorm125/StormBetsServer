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
    Match(int ID_, const QString &teamName1, const QString &teamName2, double koef1, double koef2, double koefDraw_ ,const QDateTime &time_);

    std::string toPrint() override;   

    int getID() const;

    QPair<QString, double> getTeam1() const;

    QPair<QString, double> getTeam2() const;

    QDateTime getTime() const;

    double getKoefDraw() const;

private:
    int ID;
    QPair<QString, double> team1;
    QPair<QString, double> team2;
    double koefDraw;
    QDateTime time;
};

#endif // MATCH_H
