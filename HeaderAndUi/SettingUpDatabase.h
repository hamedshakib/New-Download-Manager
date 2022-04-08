#pragma once

#include <QObject>
#include "qsqldatabase.h"
#include "qdebug.h"

class SettingUpDatabase : public QObject
{
	Q_OBJECT

public:
	static QSqlDatabase& get_Database();

private:
	static bool SettingUp(QSqlDatabase& db);
/*
public:
	SettingUpDatabase(QObject *parent);
	~SettingUpDatabase();
*/
};
