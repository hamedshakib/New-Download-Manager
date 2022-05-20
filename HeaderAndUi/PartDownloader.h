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

	bool Is_PartDownloadEndInBuffer = false;
	bool is_Paused = false;
	bool Is_FinishedPartDownload=false;
	bool Is_Downloading;
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


private slots:
	void ProcessApplyPauseOrFinishedPartDownloader();

signals:
	void Finished();
	void Paused();

public:
	PartDownloader(QObject *parent=nullptr);
	~PartDownloader();
};
