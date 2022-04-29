#pragma once

#include <QObject>
#include "Download.h"

class Queue : public QObject
{
	Q_OBJECT

private:
	int QueueId;
	QString QueueName;
	size_t MaxSpeed; //Kilobytes
	QList<size_t> List_DownloadId;
	QList<Download*> Downloading_list;
	int NumberDownloadAtSameTime;

	struct EventTime
	{
		bool is_active=false;
		QTime Time;
	}startDownload, stopDownload;

	QStringList DownloadDays;


	bool Is_Downloading = false;

	friend class QueueManager;
	friend class ProcessDatabaseOutput;
	friend class QueueTimeManager;
	friend class ShowSchedule;

public:
	int Get_QueueId();
	QString Get_QueueName();
	size_t Get_MaxSpeed(); //Kilobytes
	QList<size_t> Get_ListOfDownloadId();
	QList<Download*> Get_QueueDownloadingList();
	int Get_NumberDownloadAtSameTime();
	bool Get_IsDownloading();

	QStringList Get_DaysOfDownoad();
	bool Has_StartTimeActive();
	bool Has_StopTimeActive();
	QTime Get_StartTime();
	QTime Get_StopTime();

	
	void Set_NumberDownloadAtSameTime(int numberDownloadAtSameTime);
public:
	Queue(QObject *parent);
	~Queue();
};
