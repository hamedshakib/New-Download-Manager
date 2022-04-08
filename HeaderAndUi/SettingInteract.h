#pragma once

#include <QObject>
#include "qsettings.h"

class SettingInteract : public QObject
{
	Q_OBJECT

public:
	static bool SetValue(QString Name, QVariant value);
	static QVariant GetValue(QString Name);


private:
	static QSettings& Get_settings();

/*
public:
	SettingInteract(QObject *parent);
	~SettingInteract();
	*/
};

