#include "HeaderAndUi/TranslationManager.h"

TranslationManager::TranslationManager(QApplication* app,QObject *parent)
	: QObject(parent)
{
    m_app = app;
}

TranslationManager::~TranslationManager()
{
}

bool TranslationManager::Translate()
{
    QString Language=SettingInteract::GetValue("Language/Program").toString();
    if (Language == "English")
    {
        return true;
    }
    else
    {
        return TranslateLanguage(Language);
    }
}

bool TranslationManager::TranslateLanguage(QString Language)
{
    QString LoadLanguagePath = QString("translations/") + "app_" + Language + ".qm";
    if (translator.load(LoadLanguagePath))
    {
        if(m_app->installTranslator(&translator))
        return true;
    }
    return false;
}
