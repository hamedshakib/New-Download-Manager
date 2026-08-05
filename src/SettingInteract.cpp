#include "HeaderAndUi/SettingInteract.h"
#include "qapplication.h"
#include "qdir.h"
#include "qmutex.h"

/*
SettingInteract::SettingInteract(QObject *parent)
	: QObject(parent)
{
}

SettingInteract::~SettingInteract()
{
}
*/

QSettings& SettingInteract::Get_settings()
{
	// Use absolute path based on application directory to ensure
	// settings file is always found regardless of working directory
	static QString settingsPath = QDir(qApp->applicationDirPath()).filePath("OptionAndSettings.ini");
	static QSettings settings(settingsPath, QSettings::Format::IniFormat);
	return settings;
}

QMutex& SettingInteract::GetMutex()
{
	static QMutex mutex;
	return mutex;
}

bool SettingInteract::SetValue(QString Name, QVariant value)
{
	QMutexLocker locker(&GetMutex());
	Get_settings().setValue(Name, value);
	return true;
}

QVariant SettingInteract::GetValue(QString Name)
{
	QMutexLocker locker(&GetMutex());
	return Get_settings().value(Name);
}
