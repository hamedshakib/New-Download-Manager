#include "HeaderAndUi/DatabaseConnectionPool.h"
#include "HeaderAndUi/SettingUpDatabase.h"

QMutex DatabaseConnectionPool::mutex;

QSqlDatabase& DatabaseConnectionPool::getDefaultConnection()
{
    // Return the singleton connection from SettingUpDatabase
    // The mutex in SettingUpDatabase::get_Database() handles thread safety
    return SettingUpDatabase::get_Database();
}

QSqlDatabase& DatabaseConnectionPool::getConnection(const QString& connectionName)
{
    // For compatibility, return the same default connection
    // No need to create multiple connections for SQLite
    Q_UNUSED(connectionName);
    return getDefaultConnection();
}

void DatabaseConnectionPool::releaseConnection(const QString& connectionName)
{
    // No-op since we use a single connection managed by SettingUpDatabase
    // The connection is kept open until application exit
    Q_UNUSED(connectionName);
}

void DatabaseConnectionPool::initialize(const QString& dbName)
{
    // SettingUpDatabase handles initialization in get_Database()
    Q_UNUSED(dbName);
}

void DatabaseConnectionPool::cleanup()
{
    // SettingUpDatabase manages connection lifecycle
    // Connection is closed automatically when application exits
}