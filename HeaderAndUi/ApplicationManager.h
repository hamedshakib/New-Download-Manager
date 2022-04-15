#pragma once

#include <QObject>
#include "qdebug.h"
#include "DownloadManager.h"
#include "MainWindow.h"
#include <QSystemTrayIcon>
#include "qmenu.h"
#include "DateTimeManager.h"

class ApplicationManager : public QObject
{
	Q_OBJECT



private:
	DownloadManager* downloadManager;
	MainWindow* mainWindow;

	void AddMainSystemTrayToTaskbar();

private:
	QSystemTrayIcon* m_trayIcon;

public:
	ApplicationManager(QObject *parent);
	~ApplicationManager();
};
