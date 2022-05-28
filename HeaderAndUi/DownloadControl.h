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

	enum DownloadStatus
	{
		Downloading,Pause,StartFinsh,Finidshed
	};

public:
	DownloadControl(QObject *parent=nullptr);
	~DownloadControl();



private:
	DownloadStatus statusOfDownload = DownloadStatus::Pause;



	QMetaObject::Connection speedControlConnection;
	QNetworkAccessManager* manager;
	Download* download;
	QList<PartDownloader*> PartDownloader_list;
	QList<PartDownloader*> ActivePartDownloader_list;
	CalculatorDownload calculatorDownload;
	size_t MaxSpeed; //   KB/Sec
	bool Is_PreparePartDownloaders = false;
	bool Is_Downloading = false;
	bool DownloadFinished = false;

	bool RecentlyUpdatedActivePartDownloader_list = false;
	QElapsedTimer *elapsedTimer;
	QElapsedTimer* elapsedTimerForIndependentSpeed;
	qint64 spentedTimeFromLastPeriod;
	QTimer *timer;
	qint64 NumberOfBytesDownloadedInLastPeriod = 0;
	qint64 NumberOfBytesDownloadedInLastPeriodOfDownloadSpeedLimitted = 0;


public:
	void initDownloadControl(Download* download);
	bool StartDownload();
	bool PauseDownload();


	bool IsDownloading();
	Download* Get_Download();
	void SetMaxSpeed(int maxSpeed);
	int Get_MaxSpeed();
	bool IsSpeedLimitted();


signals:
	void Started();
	void Paused();
	void CompeletedDownload();
	//void UpdateDownloaded();
	void SpeedChanged(int speed);
	void UpdateDownloaded(QString Status, QString speed, QString TimeLeft,QList<qint64> DownloadedBytesEachPartDownloadList);

	void FinishedLastControlledSpeedPriod(qint64 spentedTime);

private slots:
	bool ProcessPreparePartDownloaders();
	//bool CreatePartDownloaderFromDatabase();
	bool ProcessPreparePartDownloaderFromPartdownload(PartDownloader* partDownloader, PartDownload* partDownload);
	bool StartPartDownloader(PartDownloader* partDownloader);
	bool StopPartDownloader(PartDownloader* partDownloader);

	void HandelStartedPartDownloaderSignalEmitted();
	void HandelPausedPartDownloaderSignalEmitted();
	void HandelFinishedRecivedBytesPartDownloaderSignalEmitted();
	void HandelFinishedPartDownloaderSignalEmitted();
	void HandelDownloadedBytesPartDownloaderSignalEmitted(qint64 ReadedBytes);

	bool CheckDownloadFinished();
	bool ProcessFinishDownload();

	void ProcessForShowDownloadCompleteDialog();
	void ShowCompleteDialog(Download* download,QString SaveTo);

	void SetMaxSpeedForPartDownloaders();
	void ProcessSetPartDownloaderMaxSpeed(PartDownloader* partDownlader, bool is_SpeedLimited);
	void UpdateListOfActivePartDownloaders();

	void ProcessScheduleControledLimittedSpeed();
	void DownloadForControlSpeed();

	void TimerTimeOut();
};
