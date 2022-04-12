#pragma once

#include <QObject>
#include "PartDownload.h"
#include "DownloadFileWriter.h"
#include "qnetworkreply.h"
#include "qmutex.h"
#include "qtimer.h"

class PartDownloader : public QObject
{
	Q_OBJECT

	
	QNetworkReply* reply;
	PartDownload* partDownload;
	bool is_Downloading = false;
	QTimer timer;
	QMutex mutex;


public slots:
	qint64 ReadReadybytes(qint64 bytes=-1);


public:
	bool Set_PartDownload(PartDownload* partDownload);
	bool Set_NetworkReply(QNetworkReply* reply);
	PartDownload* Get_PartDownload();

	void AddByteToLastDownloadedByte(qint64 NumberOfBytes);

signals:
	void Finished();

public:
	PartDownloader(QObject *parent);
	~PartDownloader();
};
