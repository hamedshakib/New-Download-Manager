#include "HeaderAndUi/DatabaseInteract.h"

bool DatabaseInteract::ExectionQueryForReadData(QSqlQuery* query)
{
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
