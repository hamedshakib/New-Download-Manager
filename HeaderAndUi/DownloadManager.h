#pragma once

#include <QObject>
//#include "Download.h"
#include "Downloader.h"
#include "DatabaseManager.h"
#include "qdebug.h"
#include "qsqldatabase.h"
#include "qeventloop.h"
#include "NewDownloadCreater.h"

class DownloadManager : public QObject
{
	Q_OBJECT

private:

	//bool settingUpDataBase();
	Download* CreateDownloadFromDatabase(int download_id);

public:
	bool CreateNewDownload();
	bool StartDownloader(Downloader* downloader);


	Download* ProcessAchieveDownload(int Download_id);
	Downloader* ProcessAchieveDownloader(Download* download);

private slots:
	void AddCreatedDownloadToDownloadList(Download* download);
	bool CreateDownloaderAndStartDownload(Download* download);
	

private:

	//friend class TableViewController;
	QList<Download*> ListOfDownloads;
	QList<Downloader*> ListOfDownloaders;
public:
	DownloadManager(QObject *parent=nullptr);
	~DownloadManager();
};
