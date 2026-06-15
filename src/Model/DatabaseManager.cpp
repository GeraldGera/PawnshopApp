#include "Model/DatabaseManager.h"

#include <QSqlError>

DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager manager;
    return manager;
}

bool DatabaseManager::connectToDatabase(const DatabaseConfig& config, QString* errorMessage)
{
    if (QSqlDatabase::contains(m_connectionName)) {
        m_database = QSqlDatabase::database(m_connectionName);
    } else {
        m_database = QSqlDatabase::addDatabase(QStringLiteral("QPSQL"), m_connectionName);
    }

    if (m_database.isOpen()) {
        m_database.close();
    }

    m_database.setHostName(config.host);
    m_database.setPort(config.port);
    m_database.setDatabaseName(config.databaseName);
    m_database.setUserName(config.userName);
    m_database.setPassword(config.password);

    if (!m_database.open()) {
        if (errorMessage) {
            *errorMessage = m_database.lastError().text();
        }
        return false;
    }

    return true;
}

bool DatabaseManager::isOpen() const
{
    return m_database.isValid() && m_database.isOpen();
}

QSqlDatabase DatabaseManager::database() const
{
    return m_database;
}

bool DatabaseManager::beginTransaction(QString* errorMessage)
{
    if (!m_database.transaction()) {
        if (errorMessage) {
            *errorMessage = m_database.lastError().text();
        }
        return false;
    }
    return true;
}

bool DatabaseManager::commit(QString* errorMessage)
{
    if (!m_database.commit()) {
        if (errorMessage) {
            *errorMessage = m_database.lastError().text();
        }
        return false;
    }
    return true;
}

void DatabaseManager::rollback()
{
    m_database.rollback();
}
