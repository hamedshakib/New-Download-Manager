#pragma once

#include <QObject>
#include "qsqldatabase.h"
#include "qdebug.h"
#include "qmutex.h"
#include "qhash.h"
#include "qthreadstorage.h"

class SettingUpDatabase : public QObject
{
	Q_OBJECT

public:
	// Main database connection (singleton)
	static QSqlDatabase& get_Database();
	
// Thread-specific database connections for safe concurrent access
	static QSqlDatabase& getThreadDatabase();
	static void releaseThreadDatabase();

private:
	static bool SettingUp(QSqlDatabase& db);
	
	static QMutex& getMutex();
	static QMutex& getThreadDatabaseMutex();
	
	// Thread-specific database storage
	static QThreadStorage<QSqlDatabase*> m_threadDatabases;

/*
public:
	SettingUpDatabase(QObject *parent);
	~SettingUpDatabase();
*/
};