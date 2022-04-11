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
	static bool ExectionQueryForReadData(QSqlQuery* query);
	static bool ExectionQueryForUpdateData(QSqlQuery* query);
	static bool ExectionQueryForInsertData(QSqlQuery* query);
	static bool ExectionQueryForCreateTable(QSqlQuery* query);
};
