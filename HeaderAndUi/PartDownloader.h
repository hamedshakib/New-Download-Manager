#pragma once

#include <QObject>
#include "PartDownload.h"
#include "DownloadFileWriter.h"
#include "qnetworkreply.h"
#include "qnetworkaccessmanager.h"
#include "qnetworkrequest.h"
#include "qmutex.h"
#include "qtimer.h"
#include "qthread.h"
#include <QUrl>

class PartDownloader : public QObject
{
	Q_OBJECT


	enum PartDownloaderStatus {
		Init, Downloading, Paused, FinishedReceiveBytes, Finished
	};

private:
	qint64 ReadBytesInEachTime=500000000000000000; //Bytes
	QNetworkReply* reply = nullptr;
	QNetworkAccessManager* m_manager = nullptr;
	PartDownload* partDownload=nullptr;
	DownloadFileWriter* downloadFileWriter;
	bool is_SpeedLimit;
	//bool is_Downloading = false;
	QTimer timer;
	QMutex mutex;
	PartDownloaderStatus partDownloaderStatus = Init;

public:
	PartDownload* GetPartDownload();
	bool ProcessSetNewReply(QNetworkReply* reply);
	bool SetSpeedLimited(bool isSpeedLimited);

public slots:
	void InitPartDownloader(PartDownload* partDownload,qint64 readBytesEachTimeCount);
	void Resume(bool itSelfDownload=true);
	void Pause();

	//Start a ranged HTTP request for this part. Called (queued) from the
	//DownloadController thread but executes on this object's own thread so that
	//this PartDownloader owns its QNetworkAccessManager/reply and writes its
	//QFile on the same thread (true per-part parallelism).
	bool StartRequest(const QUrl& url, const QString& username, const QString& password, qint64 startByte, qint64 endByte);

	qint64 DownloadByteInSpeedControl(qint64 maxReadBytes);
	bool IsAvaliableByteForRead();

private slots:
	void ReadyRead();
	qint64 ReadBytes(qint64 byteCount);
	void CheckFinishedReceivedBytes();
	void CheckFinishedPartDownloader();
	bool IsSpeedLimiter();



signals:
	void DownloadStarted();
	void DownloadPaused();
	void FinishedReceivedBytes();

	void DownloadedByteCount(qint64 downloadedBytesCount);
	void PartDownloaderFinished();


public:
	PartDownloader(QObject *parent=nullptr);
	~PartDownloader();
};
