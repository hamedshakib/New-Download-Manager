#pragma once

#include <QObject>
#include "DatabaseQueryPreparer.h"
#include "qsqlquery.h"
#include "qsqlerror.h"
#include "qsqlrecord.h"
#include "qdebug.h"

class DatabaseInteract : public QObject
{
	Q_OBJECT

public:
	// Original query execution methods (thread-safe with proper setup)
	static bool ExectionQueryForReadData(QSqlQuery* query);
	static bool ExectionQueryForUpdateData(QSqlQuery* query);
	static bool ExectionQueryForInsertData(QSqlQuery* query);
	static bool ExectionQueryForCreateTable(QSqlQuery* query);
	static bool ExectionQueryForDeleteData(QSqlQuery* query);
	
	// New helper methods for creating thread-safe queries
	static bool executeReadQuery(const QString& queryText, QSqlRecord* boundValues = nullptr);
	static bool executeUpdateQuery(const QString& queryText, QSqlRecord* boundValues = nullptr);
	static bool executeInsertQuery(const QString& queryText, QSqlRecord* boundValues = nullptr);
	static bool executeDeleteQuery(const QString& queryText, QSqlRecord* boundValues = nullptr);
};