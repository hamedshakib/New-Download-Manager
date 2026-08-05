#pragma once

#include <QObject>
#include "qsqldatabase.h"
#include "qdebug.h"
#include "qmutex.h"

class SettingUpDatabase : public QObject
{
	Q_OBJECT

public:
	static QSqlDatabase& get_Database();

private:
	static bool SettingUp(QSqlDatabase& db);
	
	static QMutex& getMutex();
/*
public:
	SettingUpDatabase(QObject *parent);
	~SettingUpDatabase();
*/
};
