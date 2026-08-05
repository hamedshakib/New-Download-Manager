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
#include "qatomicbool.h"

class ApplicationManager : public QObject
{
	Q_OBJECT



private:
	DownloadManager* downloadManager;
	QueueManager* queueManager;
	MainWindow* mainWindow;
	ProxyManager* proxyManager;

private slots:
	void AddMainSystemTrayToTaskbar();
	void LoadProxySettings();
	void ProcessArguments(int argc, char* argv[]);

private:
	QSystemTrayIcon* m_trayIcon;
QAtomicBool is_Silent = false;

public:
	ApplicationManager(QObject *parent,int argc,char* argv[]);
	~ApplicationManager();
};
