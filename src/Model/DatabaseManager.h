#pragma once

#include "Config/AppConfig.h"

#include <QSqlDatabase>
#include <QString>

class DatabaseManager
{
public:
    static DatabaseManager& instance();

    bool connectToDatabase(const DatabaseConfig& config, QString* errorMessage = nullptr);
    bool isOpen() const;
    QSqlDatabase database() const;

    bool beginTransaction(QString* errorMessage = nullptr);
    bool commit(QString* errorMessage = nullptr);
    void rollback();

    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

private:
    DatabaseManager() = default;

    QString m_connectionName = QStringLiteral("pawnshop_connection");
    QSqlDatabase m_database;
};
