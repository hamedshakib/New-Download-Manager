#include "HeaderAndUi/Queue.h"

Queue::Queue(QObject *parent)
	: QObject(parent)
{
	NumberDownloadAtSameTime = 2;
}

Queue::~Queue()
{
}

int Queue::Get_QueueId()
{
	return QueueId;
}

QString Queue::Get_QueueName()
{
	return QueueName;
}

size_t Queue::Get_MaxSpeed() //Kilobytes
{
	return MaxSpeed;
}

QList<size_t> Queue::Get_ListOfDownloadId()
{
	return List_DownloadId;
}

QList<Download*> Queue::Get_QueueDownloadingList()
{
	return Downloading_list;
}

int Queue::Get_NumberDownloadAtSameTime()
{
	return NumberDownloadAtSameTime;
}
