#pragma once

#include <QObject>
#include "PartDownload.h"
//#include "ProcessDatabaseOutput.h"
#include "qfile.h"
#include "qtimer.h"
#include "qdatetime.h"
#include "qurl.h"
#include "qlist.h"

//class Downloader;
class Download : public QObject
{
	Q_OBJECT


public:
	enum DownloadStatusEnum
	{
		NotStarted,
		Downloading,
		Pause,
		Completed
	};
	enum ResumeCapabilityEnum
	{
		UnKnown,
		Yes,
		No
	};

	struct Category
	{
		qint32 IdCategory;
		QString Name;
		QUrl BaseUrl;
	};


private:
	qint32 IdDownload;
	QString FileName;
	size_t MaxSpeed; //Kilobytes
	qint64 DownloadSize;
	qint64 SizeDownloaded;
	QFile* CompletedFile = nullptr;
	QString description;
	DownloadStatusEnum downloadStatus;
	QTime TimeLeft;
	QDateTime LastTryTime;
	QUrl Url;
	QString FullFileName;
	QUrl SaveTo;
	QString suffix;
	Category Category;
	QList<PartDownload*> DownloadParts;
	ResumeCapabilityEnum ResumeCapability;
	int Queue_id = -1;




	friend class ProcessDatabaseOutput;
	friend class NewDownloadCreater;
	friend class DatabaseQueryPreparer;
	friend class TableViewRowCreater;
	

public:
	QUrl get_Url();
	size_t get_MaxSpeed();
	QList<PartDownload*> get_PartDownloads();
	QUrl get_SavaTo();
	size_t get_Id();
	DownloadStatusEnum get_Status();


	void Set_downloadStatus(Download::DownloadStatusEnum status);


//private:
//	static QList<Download*> ListOfDownloads;
//public:
	//static bool AddDownloadToDownloadList(Download* download);
	//static Download* get_Download(int DownloadId);
	//static bool RemoveDownloadFromDownloadList(Download* download);
	//static bool RemoveDownloadFromDownloadList(int download_id);

	friend class Downloader;

public:
	Download(QObject *parent);
	~Download();
};
