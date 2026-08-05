#pragma once

#include <QSqlDatabase>
#include <QMutex>
#include <QString>

// Database connection pool for managing database connections
// Uses SettingUpDatabase singleton as the single connection source
// This class acts as a wrapper for thread-safe database access
class DatabaseConnectionPool
{
private:
	static QMutex mutex;

	DatabaseConnectionPool() = delete;
	~DatabaseConnectionPool() = delete;

public:
	// Get the default database connection (singleton pattern)
	// This provides thread-safe access to the database
	// Uses SettingUpDatabase singleton internally
	static QSqlDatabase& getDefaultConnection();

	// Get a connection (alias for getDefaultConnection for compatibility)
	// This allows code reuse without creating multiple connections
	static QSqlDatabase& getConnection(const QString& connectionName = QString());

	// Release a connection (no-op since we use single connection)
	// Kept for API compatibility
	static void releaseConnection(const QString& connectionName);

	// Initialize the connection pool (calls SettingUpDatabase::get_Database)
	static void initialize(const QString& dbName);

	// Close all connections (calls SettingUpDatabase cleanup if needed)
	static void cleanup();
};