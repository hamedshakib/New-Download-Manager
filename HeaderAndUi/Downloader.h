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

class Downloader : public QObject
{
	Q_OBJECT

private:
	QNetworkAccessManager* manager;
	Download* download;
	QList<PartDownloader*> PartDownloader_list;
	CalculatorDownload calculatorDownload;
	size_t MaxSpeedOfThisDownloader; //   KB/Sec
	bool Is_PreparePartDownloaders=false;
	bool Is_Downloading = false;
	QElapsedTimer elapsedTimer;
	QTimer timer;
	qint64 NumberOfBytesDownloadedInLastPeriod = 0;

	bool DownloadFinished = false;

public:
	void initDownloader(Download* download);
	bool StartDownload();
	bool PauseDownload();


	bool ProcessPreparePartDownloaders();

	bool IsDownloading();

	Download* Get_Download();
	void SetMaxSpeed(int maxSpeed);
	int Get_MaxSpeed();

private:
	bool CreatePartDownloaderFromDatabase();
	bool StartPartDownloader(PartDownloader* partDownloader);

	qint64 ProcessOfDownload(qint64 shouldBeDownloadAtThisPeriod=-1);
	qint64 CalculateDownloadedInPeriod();
	bool ProcessPreparePartDownloaderFrompartDownload(PartDownloader* partDownloader, PartDownload* partDownload);
	bool ShowCompleteDialog(Download* download,QFile* newDownloadFile);

private slots:
	void DownloadWithSpeedControlled();
	bool CheckForFinishedDownload();
	void ProcessOfEndOfDownloading();
	void HandelFinishedPartDownloadSignalEmitted();
	void HandelPausedPartDownloadSignalEmitted();
	void UpdateDownloadInUpdatingInDatabase();

	bool Is_downloadedCompletePartDownload(PartDownload* partDownload);

signals:
	void FinishedThisPeriod(qint64 bytesDownloaded,qint64 millisecond);
	void CompeletedDownload();
	void DownloadedAtAll(qint64 downloadedsize);
	void SignalForUpdateDownloading(QString Status,QString speed, QString TimeLeft);
	void Started();
	void Paused();
	void SpeedChanged(int speed);

private:
	friend class DatabaseQueryPreparer;
public:
	Downloader(QObject *parent=nullptr);
	~Downloader();
};

