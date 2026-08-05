#pragma once

#include <QObject>
#include "qfile.h"
#include "qdebug.h"
#include "qatomicbool.h"

class PartDownload : public QObject
{
	Q_OBJECT

signals:
	void Finished();

public:
	QAtomicBool is_Finished = false;
	int id_PartDownload;
	qint64 start_byte;
	qint64 end_byte;
	qint64 LastDownloadedByte;
	QFile* PartDownloadFile;
	size_t id_download;

	friend class PartDownloader;
	friend class NewDownloadCreater;
	friend class ProcessDatabaseOutput;

public:
	bool IsPartDownloadFinished();
	void UpdatePartDownloadLastDownloadedByte();

	PartDownload(QObject *parent=nullptr);
	~PartDownload();
};
