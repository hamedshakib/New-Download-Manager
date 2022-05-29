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


	enum PartDownloaderStatus {
		PartDownloadPaused, PartDownloadDownloading, PartDownloadFinishedReciveBytes
	};

private:
	qint64 ReadBytesInEachTime=500000000000000000; //Bytes
	QNetworkReply* reply = nullptr;
	PartDownload* partDownload=nullptr;
	DownloadFileWriter* downloadFileWriter;
	bool is_SpeedLimit;
	bool is_Downloading = false;
	QTimer timer;
	QMutex mutex;
	PartDownloaderStatus partDownloaderStatus= PartDownloadPaused;

public:
	PartDownload* Get_PartDownload();
	bool ProcessSetNewReply(QNetworkReply* reply);
	bool SetSpeedLimited(bool is_SpeedLimitted);

public slots:
	void initPartDownlolader(PartDownload* paerDownload,qint64 readBytesEachTimes);
	void Resume(bool ItSelf=true);
	void Pause();

	qint64 DownloadByteInSpeedControl(qint64 maxReadBytes);
	bool IsAvaliableByteForRead();

private slots:
	void ReadyRead();
	qint64 ReadBytes(qint64 bytes);
	void CheckFinishedRecivedBytes();
	void CheckFinishedPartDownloader();
	bool IsSpeedLimiter();



signals:
	void Started();
	void Paused();
	void FinishedRecivedBytes();
	void Finished();
	void DownloadedBytes(qint64 ReadedBytes);



public:
	PartDownloader(QObject *parent=nullptr);
	~PartDownloader();
};
