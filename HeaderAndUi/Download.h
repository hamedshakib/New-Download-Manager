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
		Commpleted
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

public:
	QUrl get_Url();
	size_t get_MaxSpeed();
	QList<PartDownload*> get_PartDownloads();
	QUrl get_SavaTo();

public:
	Download(QObject *parent);
	~Download();
};
