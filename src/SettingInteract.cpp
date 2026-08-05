#include "HeaderAndUi/SettingInteract.h"
#include "qapplication.h"
#include "qdir.h"

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

bool SettingInteract::SetValue(QString Name, QVariant value)
{
	
	Get_settings().setValue(Name, value);
	return true;

}

QVariant SettingInteract::GetValue(QString Name)
{
	return Get_settings().value(Name);
}
