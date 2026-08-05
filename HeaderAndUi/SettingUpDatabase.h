#pragma once

#include <QObject>
#include "qsqldatabase.h"
#include "qdebug.h"
#include <QMutex>
#include <QHash>

class SettingUpDatabase : public QObject
{
	Q_OBJECT

public:
	//Return a connection that is dedicated to the calling thread.
	//QSqlDatabase/QSqlQuery are NOT thread-safe, so each thread must use its own
	//connection. This prevents crashes and "database is locked" errors when
	//multiple Download/Part Download threads access SQLite concurrently.
	static QSqlDatabase& get_Database();

	//Close and remove the connection belonging to the current thread.
	//Call this just before a worker thread finishes.
	static void removeThreadConnection();

	//Global mutex used to serialize SQLite operations across all threads.
	//SQLite allows a single writer; serializing exec avoids corruption and
	//"database is locked" errors even with per-thread connections.
	static QMutex& databaseMutex();

private:
	static bool SettingUp(QSqlDatabase& db);
/*
public:
	SettingUpDatabase(QObject *parent);
	~SettingUpDatabase();
*/
};
