#pragma once

#include <QString>

struct DatabaseConfig
{
    QString host;
    int port = 5432;
    QString databaseName;
    QString userName;
    QString password;
};

class AppConfig
{
public:
    static QString defaultConfigPath();
    static DatabaseConfig loadDatabaseConfig(const QString& filePath = QString());
};
