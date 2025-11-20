#include "dbconnection.h"

#include <thread>
#include <mutex>

#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>

#include "spdlog/spdlog.h"
#include "EnvManager.hpp"

DBconnection::DBconnection()
{   
    if(!db.isOpen())
    {
        auto host = EnvManager::getInstance().getValue("DB_HOST");
        auto port = EnvManager::getInstance().getValue("DB_PORT");
        auto user = EnvManager::getInstance().getValue("DB_USER");
        auto pass = EnvManager::getInstance().getValue("DB_PASSWORD");
        auto db_name = EnvManager::getInstance().getValue("DB_NAME");

        if(host.has_value() && port.has_value() && user.has_value() && pass.has_value() && db_name.has_value())
        {
            db = QSqlDatabase::addDatabase("QMYSQL");
            db.setHostName(QString::fromStdString(host.value()));
            db.setDatabaseName(QString::fromStdString(db_name.value()));
            db.setUserName(QString::fromStdString(user.value()));
            db.setPassword(QString::fromStdString(pass.value()));
            db.setPort(std::stoi(port.value()));
        }
        else
        {
            spdlog::error("Database connection parameters not found");
            exit(1);
        }

        while (!db.open())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}


DBconnection::~DBconnection()
{
    if(db.isOpen())
    {
        db.close();
    }
}

QSqlDatabase DBconnection::getDB()
{
    return db;
}


