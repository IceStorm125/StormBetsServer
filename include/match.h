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
    Match(int ID_,const QString api_id_, const QString &tournamentName_, const QString &teamName1, const QString &teamName2, double koef1, double koefDraw_, double koef2 ,const QDateTime &time_);


    std::string toPrint() override;   

    int getID() const;

    QString getApiID() const;

    QPair<QString, double> getTeam1() const;

    QPair<QString, double> getTeam2() const;

    QDateTime getTime() const;

    double getKoefDraw() const;

private:
    int ID;
    QString api_id;
    QString tournamentName;
    QPair<QString, double> team1;
    QPair<QString, double> team2;
    double koefDraw;
    QDateTime time;
};

#endif // MATCH_H
