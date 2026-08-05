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
		PartDownloadPaused, PartDownloadDownloading, PartDownloadFinishedReciveBytes
	};

private:
	qint64 ReadBytesInEachTime=500000000000000000; //Bytes
	QNetworkReply* reply = nullptr;
	QNetworkAccessManager* m_manager = nullptr;
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

	//Start a ranged HTTP request for this part. Called (queued) from the
	//DownloadControl thread but executes on this object's own thread so that
	//this PartDownloader owns its QNetworkAccessManager/reply and writes its
	//QFile on the same thread (true per-part parallelism).
	bool StartRequest(const QUrl& url, const QString& username, const QString& password, qint64 startByte, qint64 endByte);

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
