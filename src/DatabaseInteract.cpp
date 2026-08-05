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
		// Get detailed error information
		QSqlError error = query->lastError();
		qCritical() << "Database Read Error:" << error.text();
		qCritical() << "Database Error Code:" << error.number();
		qCritical() << "Database Driver Error:" << error.driverText();
		qCritical() << "Database Query:" << query->lastQuery();
		
		// Check for specific common errors
		if (error.number() == -1) {
			qCritical() << "Database Error: Database is locked or busy. Try again later.";
		}
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
		// Get detailed error information
		QSqlError error = query->lastError();
		qCritical() << "Database Update Error:" << error.text();
		qCritical() << "Database Error Code:" << error.number();
		qCritical() << "Database Driver Error:" << error.driverText();
		qCritical() << "Database Query:" << query->lastQuery();
		
		if (error.number() == -1) {
			qCritical() << "Database Error: Database is locked. Retrying may help.";
		}
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
		// Get detailed error information
		QSqlError error = query->lastError();
		qCritical() << "Database Insert Error:" << error.text();
		qCritical() << "Database Error Code:" << error.number();
		qCritical() << "Database Driver Error:" << error.driverText();
		qCritical() << "Database Query:" << query->lastQuery();
		
		if (error.number() == -1) {
			qCritical() << "Database Error: Database is locked. Retrying may help.";
		}
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
		// Get detailed error information
		QSqlError error = query->lastError();
		qCritical() << "Database CreateTable Error:" << error.text();
		qCritical() << "Database Error Code:" << error.number();
		qCritical() << "Database Driver Error:" << error.driverText();
		qCritical() << "Database Query:" << query->lastQuery();
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
		// Get detailed error information
		QSqlError error = query->lastError();
		qCritical() << "Database Delete Error:" << error.text();
		qCritical() << "Database Error Code:" << error.number();
		qCritical() << "Database Driver Error:" << error.driverText();
		qCritical() << "Database Query:" << query->lastQuery();
		
		if (error.number() == -1) {
			qCritical() << "Database Error: Database is locked. Retrying may help.";
		}
		return false;
	}
}
