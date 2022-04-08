#pragma once

#include <QObject>
#include "qfile.h"
//#include <utility>

class PartDownload : public QObject
{
	Q_OBJECT

public:
	int id_PartDownload;
	qint64 start_byte;
	qint64 end_byte;
	qint64 LastDownloadedByte;
	QFile* PartDownloadFile;
	size_t id_download;

	friend class PartDownloader;
	friend class NewDownloadCreater;

public:
	PartDownload(QObject *parent);
	~PartDownload();
};
