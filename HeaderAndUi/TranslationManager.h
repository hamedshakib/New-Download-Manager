#pragma once

#include <QObject>
#include "qtranslator.h"
#include "qlocale.h"
#include "qapplication.h"
#include "SettingInteract.h"

class TranslationManager : public QObject
{
	Q_OBJECT

public:
	bool Translate();
	
private:
	QTranslator translator;
	bool TranslateLanguage(QString Language);
	QApplication* m_app;
public:
	TranslationManager(QApplication* app,QObject *parent);
	~TranslationManager();
};
