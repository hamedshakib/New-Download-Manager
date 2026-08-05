#pragma once

#include <QObject>
#include "qsettings.h"
#include "qmutex.h"

class SettingInteract : public QObject
{
	Q_OBJECT

public:
	static bool SetValue(QString Name, QVariant value);
	static QVariant GetValue(QString Name);


private:
	static QSettings& Get_settings();
	static QMutex& GetMutex();

/*
public:
	SettingInteract(QObject *parent);
	~SettingInteract();
	*/
};

