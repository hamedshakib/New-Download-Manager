#pragma once

#include <QObject>
#include "MainWindow.h"
#include "Download.h"
#include "Downloader.h"
#include "DatabaseManager.h"
#include "qdebug.h"
#include "qsql.h"
#include "qsqldatabase.h"
#include "qeventloop.h"

class DownloadManager : public QObject
{
	Q_OBJECT

private:
	MainWindow mainWindow;

	//bool settingUpDataBase();
	bool CreateDownloadFromDatabase(int download_id);

public:
	bool CreateNewDownload();
	bool StartDownloader(Downloader* downloader);

private slots:
	void AddCreatedDownloadToDownloadList(Download* download);
	bool CreateDownloaderAndStartDownload(Download* download);

private:
	QList<Download*> ListOfDownloads;
	QList<Downloader*> ListOfDownloaders;
public:
	DownloadManager(QObject *parent=nullptr);
	~DownloadManager();
};
