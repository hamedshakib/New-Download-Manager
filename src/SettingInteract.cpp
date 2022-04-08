#include "HeaderAndUi/SettingInteract.h"

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
	static QSettings settings("OptionAndSettings.ini", QSettings::Format::IniFormat);
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
