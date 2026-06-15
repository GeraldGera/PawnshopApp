#include "Config/AppConfig.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QSettings>

QString AppConfig::defaultConfigPath()
{
    const QString appDirPath = QCoreApplication::applicationDirPath() + QStringLiteral("/config.ini");
    if (QFileInfo::exists(appDirPath)) {
        return appDirPath;
    }

    return QDir::current().absoluteFilePath(QStringLiteral("config.ini"));
}

DatabaseConfig AppConfig::loadDatabaseConfig(const QString& filePath)
{
    const QString path = filePath.isEmpty() ? defaultConfigPath() : filePath;
    QSettings settings(path, QSettings::IniFormat);

    DatabaseConfig config;
    config.host = settings.value(QStringLiteral("database/host"), QStringLiteral("localhost")).toString();
    config.port = settings.value(QStringLiteral("database/port"), 5432).toInt();
    config.databaseName = settings.value(QStringLiteral("database/name"), QStringLiteral("pawnshop_db")).toString();
    config.userName = settings.value(QStringLiteral("database/user"), QStringLiteral("postgres")).toString();
    config.password = settings.value(QStringLiteral("database/password"), QStringLiteral("postgres")).toString();
    return config;
}
