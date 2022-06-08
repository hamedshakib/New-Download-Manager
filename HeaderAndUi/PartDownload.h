#pragma once

#include <QObject>
#include "qfile.h"
#include "qdebug.h"
//#include <utility>

class PartDownload : public QObject
{
	Q_OBJECT

public:
	bool is_Finished = false;
	size_t PartDownload_id;
	qint64 start_byte;
	qint64 end_byte;
	qint64 LastDownloadedByte;
	QFile* PartDownloadFile;
	size_t download_id;

	friend class PartDownloader;
	friend class NewDownloadCreater;
	friend class ProcessDatabaseOutput;

public:
	bool IsPartDownloadFinished();
	void UpdatePartDownloadLastDownloadedByte();


	void Set_StartByte(qint64);
	void Set_EndByte(qint64);
	void Set_LastDownloadedByte(qint64);
	void Set_PartDownloadFile(QFile*);
	void Set_PartDownloadId(size_t);
	void Set_DownloadId(size_t);
	
	qint64 get_StartByte();
	qint64 get_EndByte();
	qint64 get_LastDownloadedByte();
	QFile* get_PartDownloadFile();
	size_t get_PartDownloadId();
	size_t get_DownloadId();
	

	PartDownload(QObject *parent=nullptr);
	~PartDownload();
};
