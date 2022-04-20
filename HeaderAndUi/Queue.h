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

	bool Is_Downloading = false;

	friend class QueueManager;
	friend class ProcessDatabaseOutput;


public:
	int Get_QueueId();
	QString Get_QueueName();
	size_t Get_MaxSpeed(); //Kilobytes
	QList<size_t> Get_ListOfDownloadId();
	QList<Download*> Get_QueueDownloadingList();
	int Get_NumberDownloadAtSameTime();
	bool Get_IsDownloading();

public:
	Queue(QObject *parent);
	~Queue();
};
