#ifndef MATCH_H
#define MATCH_H

#include <QPair>
#include <QString>
#include <QDateTime>

class Match {
public:
    Match();

    Match(int ID_, const std::string api_id_, const std::string &tournamentName_, const std::string &teamName1,
          const std::string &teamName2, double koef1, double koefDraw_, double koef2, const QDateTime &time_);


    std::string toPrint();

    int getID() const;

    std::string getApiID() const;

    std::pair<std::string, double> getTeam1() const;

    std::pair<std::string, double> getTeam2() const;

    QDateTime getTime() const;

    double getKoefDraw() const;

private:
    int ID;
    std::string api_id;
    std::string tournamentName;
    std::pair<std::string, double> team1;
    std::pair<std::string, double> team2;
    double koefDraw;
    QDateTime time;
};

#endif // MATCH_H
