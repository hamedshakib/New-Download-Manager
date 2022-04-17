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
	QList<Download*> download_List;
	QList<Download*> Downloading_list;
	int NumberDownloadAtSameTime;

	friend class QueueManager;


public:
	int Get_QueueId();
	QString Get_QueueName();
	size_t Get_MaxSpeed(); //Kilobytes
	QList<Download*> Get_QueueDownloadList();
	int Get_NumberDownloadAtSameTime();

public:
	Queue(QObject *parent);
	~Queue();
};
