#pragma once

#include <QObject>
#include "qdebug.h"
#include "DownloadManager.h"
#include "MainWindow.h"
#include <QSystemTrayIcon>
#include "qmenu.h"
#include "DateTimeManager.h"
#include "ProxyManager.h"
#include "QueueManager.h"

class ApplicationManager : public QObject
{
	Q_OBJECT



private:
	DownloadManager* downloadManager;
	QueueManager* queueManager;
	MainWindow* mainWindow;

private slots:
	void AddMainSystemTrayToTaskbar();
	void LoadProxySettings();

private:
	QSystemTrayIcon* m_trayIcon;

public:
	ApplicationManager(QObject *parent);
	~ApplicationManager();
};
