#pragma once

#include <QObject>
#include "qnetworkaccessmanager.h"
#include "qnetworkreply.h"
#include "qnetworkrequest.h"
#include "Download.h"
#include "PartDownload.h"
#include "PartDownloader.h"
#include "CalculatorDownload.h"
#include "DownloadFileWriter.h"
#include "CompleteDownloadDialog.h"
#include "ConverterSizeToSuitableString.h"
#include "SettingInteract.h"
#include "qlist.h"
#include "qelapsedtimer.h"
#include "qtimer.h"
#include "qfileinfo.h"

class DownloadControl : public QObject
{
	Q_OBJECT

public:
	DownloadControl(QObject *parent=nullptr);
	~DownloadControl();



private:
	QNetworkAccessManager* manager;
	Download* download;
	QList<PartDownloader*> PartDownloader_list;
	CalculatorDownload calculatorDownload;
	size_t MaxSpeed; //   KB/Sec
	bool Is_PreparePartDownloaders = false;
	bool Is_Downloading = false;
	bool DownloadFinished = false;
	QElapsedTimer *elapsedTimer;
	QTimer *timer;
	qint64 NumberOfBytesDownloadedInLastPeriod = 0;



public:
	void initDownloadControl(Download* download);
	bool StartDownload();
	bool PauseDownload();


	bool IsDownloading();
	Download* Get_Download();
	void SetMaxSpeed(int maxSpeed);
	int Get_MaxSpeed();

signals:
	void Started();
	void Paused();
	void CompeletedDownload();
	void UpdateDownloaded();
	void SpeedChanged(int speed);

private slots:
	bool ProcessPreparePartDownloaders();
	//bool CreatePartDownloaderFromDatabase();
	bool ProcessPreparePartDownloaderFromPartdownload(PartDownloader* partDownloader, PartDownload* partDownload);
	bool StartPartDownloader(PartDownloader* partDownloader);
	bool StopPartDownloader(PartDownloader* partDownloader);

	void HandelStartedPartDownloaderSignalEmitted();
	void HandelPausedPartDownloaderSignalEmitted();
	void HandelFinishedReadyReadBytesPartDownloaderSignalEmitted();
	void HandelFinishedPartDownloaderSignalEmitted();
	void HandelDownloadedBytesPartDownloaderSignalEmitted(qint64 ReadedBytes);

	bool CheckDownloadFinished();
	bool ProcessFinishDownload();
};
