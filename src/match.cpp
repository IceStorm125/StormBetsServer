#include "match.h"

#include <sstream>

#include <fmt/core.h>

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
    std::string out = fmt::format("{} ({}) vs {} ({})\n", team1.first.toStdString(), team1.second, team2.first.toStdString(), team2.second);
    if(koefDraw != 0) out+= fmt::format("Draw: {}\n", koefDraw);
    out += fmt::format("{}\n\n", time.toString("dd.MM.yyyy hh:mm").toStdString());

    return out;
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


