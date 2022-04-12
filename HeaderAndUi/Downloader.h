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
	qint64 MaxSpeedOfThisDownloader; //   KB/Sec
	bool Is_PreparePartDownloaders=false;
	bool Is_Downloading = false;
	QElapsedTimer elapsedTimer;
	QTimer timer;
	qint64 NumberOfBytesDownloadedInLastPeriod = 0;

public:
	bool StartDownload();
	bool PauseDownload();
	bool StopDownload();


	bool ProcessPreparePartDownloaders();

	Download* Get_Download();


private:
	bool CreatePartDownloaderFromDatabase();
	bool StartPartDownloader(PartDownloader* partDownloader);

	qint64 ProcessOfDownload(qint64 shouldBeDownloadAtThisPeriod=-1);
	qint64 CalculateDownloadedInPeriod();
	bool ProcessPreparePartDownloaderFrompartDownload(PartDownloader* partDownloader, PartDownload* partDownload);

private slots:
	void DownloadWithSpeedControlled();
	bool CheckForFinishedDownload();
	void ProcessOfEndOfDownloading();
		//BuildOneFileFromMultiParts();

signals:
	void FinishedThisPeriod(qint64 bytesDownloaded,qint64 millisecond);
	void CompeletedDownload();
	void DownloadedAtAll(qint64 downloadedsize);
	void SignalForUpdateDownloading(QString Status,QString speed, QString TimeLeft);

private:
	friend class DatabaseQueryPreparer;
public:
	Downloader(Download* download,QObject *parent);
	~Downloader();
};

