#pragma once

#include <QObject>
#include "PartDownload.h"
#include "DownloadFileWriter.h"
#include "qnetworkreply.h"
#include "qmutex.h"
#include "qtimer.h"
#include "qthread.h"

class PartDownloader : public QObject
{
	Q_OBJECT

	/*
	bool Is_PartDownloadEndInBuffer = false;
	bool is_Paused = false;
	bool Is_FinishedPartDownload=false;
	bool Is_Downloading;
	bool Is_DownloadItSelf = false;
	QNetworkReply* reply=nullptr;
	PartDownload* partDownload;
	DownloadFileWriter* downloadFileWriter;
	//bool is_Downloading = false;
	QTimer timer;
	QMutex mutex;

	bool DeletedBefore = false;


public slots:
	qint64 ReadReadybytes(qint64 bytes=-1);
	void Resume();
	void Pause();

public:
	bool Set_PartDownload(PartDownload* partDownload);
	bool Set_NetworkReply(QNetworkReply* reply);
	bool Set_DownloadFileWriter(DownloadFileWriter* downloadFileWriter);
	PartDownload* Get_PartDownload();

	void AddByteToLastDownloadedByte(qint64 NumberOfBytes);

	bool StartDownloadItSelf();
	bool StopDownloadItSelf();

private slots:
	void ProcessApplyPauseOrFinishedPartDownloader();
	void ProcessDownloadItSelf(qint64 downloadedInLastPeriod);

signals:
	void Started();
	void Paused();
	void FinishedReadyReadBytes();
	void Finished();
	void DownloadedBytes(qint64 ReadedBytes);
	*/

	enum PartDownloaderStatus {
		PartDownloadPaused, PartDownloadDownloading, PartDownloadFinishedReciveBytes
	};


private:
	qint64 ReadBytesInEachTime; //Bytes
	QNetworkReply* reply = nullptr;
	PartDownload* partDownload=nullptr;
	DownloadFileWriter* downloadFileWriter;
	bool is_Downloading = false;
	QTimer timer;
	QMutex mutex;
	PartDownloaderStatus partDownloaderStatus= PartDownloadPaused;

public:
	PartDownload* Get_PartDownload();
	bool ProcessSetNewReply(QNetworkReply* reply);

public slots:
	void initPartDownlolader(PartDownload* paerDownload,qint64 readBytesEachTimes);
	void Resume(bool ItSelf=true);
	void Pause();

private slots:
	void ReadyRead();
	void CheckFinished();






signals:
	void Started();
	void Paused();
	void FinishedReadyReadBytes();
	void Finished();
	void DownloadedBytes(qint64 ReadedBytes);



public:
	PartDownloader(QObject *parent=nullptr);
	~PartDownloader();
};
