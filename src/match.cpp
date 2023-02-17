#include "match.h"

#include <sstream>

Match::Match()
{

}

Match::Match(int ID_, const QString &teamName1, const QString &teamName2, double koef1, double koef2, double koefDraw_, const QDateTime &time_)
{
    ID = ID_;
    team1 = {teamName1, koef1};
    team2 = {teamName2, koef2};
    koefDraw = koefDraw_;
    time = time_;
}

std::string Match::toPrint()
{
    std::stringstream ss;
    ss << team1.first.toStdString() << " (" << team1.second << ")";
    ss << " vs ";
    ss << team2.first.toStdString() << " (" << team2.second << ")\n";
    if(koefDraw != 0) ss << "Draw: " << koefDraw << "\n";
    ss << time.toString("dd.MM.yyyy hh:mm").toStdString();
    ss << "\n\n";
    return ss.str();
}

int Match::getID() const
{
    return ID;
}

QPair<QString, double> Match::getTeam1() const
{
    return team1;
}

QPair<QString, double> Match::getTeam2() const
{
    return team2;
}

QDateTime Match::getTime() const
{
    return time;
}

double Match::getKoefDraw() const
{
    return koefDraw;
}


