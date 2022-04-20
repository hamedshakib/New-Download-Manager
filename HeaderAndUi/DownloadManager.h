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
	bool CreatePartDownloadAndPutInDownloadFromDatabase(Download* download);

public:
	bool CreateNewDownload();
	bool StartDownloader(Downloader* downloader);


	Download* ProcessAchieveDownload(int Download_id);
	Downloader* ProcessAchieveDownloader(Download* download);

	bool ProcessRemoveDownload(int download_id,bool is_RemoveFromDisk=false);
	bool ProcessRemoveDownload(Download* download, bool is_RemoveFromDisk = false);

private slots:
	void AddCreatedDownloadToDownloadList(Download* download);
	bool CreateDownloaderAndStartDownload(Download* download);
	Downloader* CreateDownloader(Download* download);



private:

	//friend class TableViewController;
	QList<Download*> ListOfActiveDownloads;
	QList<Downloader*> ListOfDownloaders;


signals:
	void CreatedDownloader(Downloader* downloader);
	void CreatedNewDownload(Download* download);
	void FinishedDownload(Download* download);

public:
	DownloadManager(QObject *parent=nullptr);
	~DownloadManager();
};
