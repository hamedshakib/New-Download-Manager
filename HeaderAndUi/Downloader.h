#pragma once

#include <QObject>
#include "qnetworkaccessmanager.h"
#include "qnetworkreply.h"
#include "qnetworkrequest.h"
#include "Download.h"
#include "PartDownload.h"
#include "PartDownloader.h"
#include "CalculatorDownloadSpeed.h"
#include "DownloadFileWriter.h"
#include "qlist.h"
#include "qelapsedtimer.h"
#include "qtimer.h"

class Downloader : public QObject
{
	Q_OBJECT

private:
	QNetworkAccessManager* manager;
	Download* download;
	QList<PartDownloader*> PartDownloader_list;
	CalculatorDownloadSpeed calculatorSpeed;
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

public:
	Downloader(Download* download,QObject *parent);
	~Downloader();
};

