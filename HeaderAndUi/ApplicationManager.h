#pragma once

#include <QObject>
#include "qdebug.h"
#include "DownloadManager.h"
#include "MainWindow.h"

class ApplicationManager : public QObject
{
	Q_OBJECT



private:
	DownloadManager* downloadManager;
	MainWindow* mainWindow;

public:
	ApplicationManager(QObject *parent);
	~ApplicationManager();
};
