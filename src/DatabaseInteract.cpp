#include "HeaderAndUi/DatabaseInteract.h"

//Serialize all SQLite operations across threads. SQLite only allows a single
//writer; without this, concurrent per-thread connections produce
//"database is locked" errors and possible corruption.

bool DatabaseInteract::ExectionQueryForReadData(QSqlQuery* query)
{
	QMutexLocker locker(&SettingUpDatabase::databaseMutex());
	bool is_Ok = query->exec();
	if (is_Ok)
	{
			return true;
	}
	else
	{
		qCritical() << "Database Error:" << query->lastError();
		return false;
	}
}

bool DatabaseInteract::ExectionQueryForUpdateData(QSqlQuery* query)
{
	QMutexLocker locker(&SettingUpDatabase::databaseMutex());
	bool is_Ok = query->exec();
	if (is_Ok)
	{
		return true;
	}
	else
	{
		qCritical() << "Database Error:" << query->lastError();
		return false;
	}
}

bool DatabaseInteract::ExectionQueryForInsertData(QSqlQuery* query)
{
	QMutexLocker locker(&SettingUpDatabase::databaseMutex());
	bool is_Ok = query->exec();
	if (is_Ok)
	{
		return true;
	}
	else
	{
		qCritical() << "Database Error:" << query->lastError();
		return false;
	}
}

bool DatabaseInteract::ExectionQueryForCreateTable(QSqlQuery* query)
{
	QMutexLocker locker(&SettingUpDatabase::databaseMutex());
	bool is_Ok = query->exec();
	if (is_Ok)
	{
		return true;
	}
	else
	{
		qCritical() << "Database Error:" << query->lastError();
		return false;
	}
}

bool DatabaseInteract::ExectionQueryForDeleteData(QSqlQuery* query)
{
	QMutexLocker locker(&SettingUpDatabase::databaseMutex());
	bool is_Ok = query->exec();
	if (is_Ok)
	{
		return true;
	}
	else
	{
		qCritical() << "Database Error:" << query->lastError();
		return false;
	}
}
