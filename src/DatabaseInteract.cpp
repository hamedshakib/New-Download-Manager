#include "HeaderAndUi/DatabaseInteract.h"

bool DatabaseInteract::ExectionQueryForReadData(QSqlQuery* query)
{
	// Add nullptr check for safety
	if (!query) {
		qCritical() << "Database Error: query is nullptr";
		return false;
	}
	
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
	// Add nullptr check for safety
	if (!query) {
		qCritical() << "Database Error: query is nullptr";
		return false;
	}
	
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
	// Add nullptr check for safety
	if (!query) {
		qCritical() << "Database Error: query is nullptr";
		return false;
	}
	
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
	// Add nullptr check for safety
	if (!query) {
		qCritical() << "Database Error: query is nullptr";
		return false;
	}
	
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
	// Add nullptr check for safety
	if (!query) {
		qCritical() << "Database Error: query is nullptr";
		return false;
	}
	
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
