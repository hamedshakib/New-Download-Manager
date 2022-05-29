#pragma once

#include <QObject>
//#include "Download.h"
#include "DownloadControl.h"
#include "DatabaseManager.h"
#include "qdebug.h"
#include "qsqldatabase.h"
#include "qeventloop.h"
#include "qthread.h"
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
	bool CreateNewDownloadsFromBatch(QList<QString> listOfAddress, QString SaveTo, QString Username, QString Password);
	bool StartDownload(DownloadControl* downloadControl);
	bool StopAllDownload();
	bool Set_SpeedLimit(int maxSpeed);



	Download* ProcessAchieveDownload(int Download_id);
	DownloadControl* ProcessAchieveDownloadControl(Download* download);

	bool ProcessRemoveDownload(int download_id,bool is_RemoveFromDisk=false);
	bool ProcessRemoveDownload(Download* download, bool is_RemoveFromDisk = false);

private slots:
	void AddCreatedDownloadToDownloadList(Download* download);
	bool CreateDownloadControlAndStartDownload(Download* download);
	DownloadControl* CreateDownloadControl(Download* download);
	bool SpeedLimitForAllDownload();

private:

	//friend class TableViewController;
	QList<Download*> ListOfActiveDownloads;
	QList<DownloadControl*> ListOfDownloadControls;
	int SpeedLimit = 0;


signals:
	void CreatedDownloadControl(DownloadControl* downloadControl);
	void CreatedNewDownload(Download* download);
	void FinishedDownload(Download* download);

public:
	DownloadManager(QObject *parent=nullptr);
	~DownloadManager();
};
