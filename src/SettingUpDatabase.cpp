#include "HeaderAndUi/SettingUpDatabase.h"

/*
SettingUpDatabase::SettingUpDatabase(QObject *parent)
	: QObject(parent)
{
}

SettingUpDatabase::~SettingUpDatabase()
{
}
*/

QSqlDatabase& SettingUpDatabase::get_Database()
{
    static QSqlDatabase db;
    if (!db.isValid())
    {
        SettingUp(db);
    }
    if (!db.isOpen())
    {
        db.open();
    }

    return db;
}

bool SettingUpDatabase::SettingUp(QSqlDatabase& db)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("DM.db");
    if (!db.open())
    {
        qCritical() << "Can not open Database!!!!";
        return false;
    }
    return true;
}
