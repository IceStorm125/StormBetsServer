#include "dbconnection.h"

#include <thread>
#include <mutex>

#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QThread>
#include <QSqlQuery>

#include "spdlog/spdlog.h"
#include "EnvManager.hpp"

bool DBconnection::ping(QSqlDatabase &db) {
    QSqlQuery q(db);
    return q.exec("SELECT 1");
}

QSqlDatabase DBconnection::connection() {
    thread_local QSqlDatabase db = createConnection();
    if (!db.isOpen() || !ping(db)) {
        db.close();
        db.open();
    }
    return db;
}

QSqlDatabase DBconnection::createConnection() {
    QString connectionName = QString("conn_%1").arg((quintptr) QThread::currentThreadId());
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", connectionName);

    auto host = EnvManager::getInstance().getValue("DB_HOST");
    auto port = EnvManager::getInstance().getValue("DB_PORT");
    auto user = EnvManager::getInstance().getValue("DB_USER");
    auto pass = EnvManager::getInstance().getValue("DB_PASSWORD");
    auto db_name = EnvManager::getInstance().getValue("DB_NAME");

    if (host.has_value() && port.has_value() && user.has_value() && pass.has_value() && db_name.has_value()) {
        db.setHostName(QString::fromStdString(host.value()));
        db.setDatabaseName(QString::fromStdString(db_name.value()));
        db.setUserName(QString::fromStdString(user.value()));
        db.setPassword(QString::fromStdString(pass.value()));
        db.setPort(std::stoi(port.value()));
    } else {
        spdlog::error("Database connection parameters not found");
        exit(1);
    }
    return db;
}
