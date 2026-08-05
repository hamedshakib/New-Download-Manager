#pragma once

#include <QObject>
#include "qfile.h"
#include "qdebug.h"
#include <QMutex>
//#include <utility>

class PartDownload : public QObject
{
	Q_OBJECT

public:
	bool is_Finished = false;
	int id_PartDownload;
	qint64 start_byte;
	qint64 end_byte;
	qint64 LastDownloadedByte;
	QFile* PartDownloadFile;
	size_t id_download;

	friend class PartDownloader;
	friend class NewDownloadCreater;
	friend class ProcessDatabaseOutput;

private:
	//PartDownload lives on a PartDownload thread (its LastDownloadedByte is written
	//there) but is also read by the Download thread, so its state is guarded.
	mutable QMutex stateMutex;

public:
	bool IsPartDownloadFinished();
	void UpdatePartDownloadLastDownloadedByte();

	//Thread-safe accessors for LastDownloadedByte.
	qint64 GetLastDownloadedByte() const;
	void SetLastDownloadedByte(qint64 value);
	void AddToLastDownloadedByte(qint64 bytes);

	PartDownload(QObject *parent=nullptr);
	~PartDownload();
};
