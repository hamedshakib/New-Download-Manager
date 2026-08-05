#include "HeaderAndUi/SettingUpDatabase.h"

#include <QThread>
#include <QSqlError>

/*
SettingUpDatabase::SettingUpDatabase(QObject *parent)
	: QObject(parent)
{
}

SettingUpDatabase::~SettingUpDatabase()
{
}
*/

// Guard for the connection store itself (creation/removal).
static QMutex g_connectionStoreMutex;
// A pointer is stored so the returned reference stays valid even if the hash rehashes.
static QHash<QString, QSqlDatabase*> g_connections;

static QString connectionKey()
{
	//A unique connection name per OS thread id.
	return QString("dm_conn_%1").arg(reinterpret_cast<quintptr>(QThread::currentThreadId()));
}

QSqlDatabase& SettingUpDatabase::get_Database()
{
	QMutexLocker storeLocker(&g_connectionStoreMutex);
	const QString key = connectionKey();

	QSqlDatabase*& slot = g_connections[key];
	if (slot == nullptr)
	{
		QSqlDatabase newConnection = QSqlDatabase::addDatabase("QSQLITE", key);
		slot = new QSqlDatabase(newConnection);
		SettingUp(*slot);
	}

	QSqlDatabase& db = *slot;
	if (!db.isOpen())
	{
		db.open();
	}

	return db;
}

void SettingUpDatabase::removeThreadConnection()
{
	QMutexLocker storeLocker(&g_connectionStoreMutex);
	const QString key = connectionKey();

	auto it = g_connections.find(key);
	if (it != g_connections.end())
	{
		QSqlDatabase* db = it.value();
		db->close();
		QSqlDatabase::removeDatabase(key);
		delete db;
		g_connections.erase(it);
	}
}

QMutex& SettingUpDatabase::databaseMutex()
{
	static QMutex mutex;
	return mutex;
}

bool SettingUpDatabase::SettingUp(QSqlDatabase& db)
{
	db.setDatabaseName("DM.db");
	if (!db.open())
	{
		qCritical() << "Can not open Database!!!!" << db.lastError().text();
		return false;
	}
	return true;
}
