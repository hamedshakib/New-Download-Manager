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

bool Queue::Get_IsDownloading()
{
	return Is_Downloading;
}

QStringList Queue::Get_DaysOfDownoad()
{
	return DownloadDays;
}

bool Queue::Has_StartTimeActive()
{
	return startDownload.is_active;
}

bool Queue::Has_StopTimeActive()
{
	return stopDownload.is_active;
}

QTime Queue::Get_StartTime()
{
	return startDownload.Time;
}

QTime Queue::Get_StopTime()
{
	return stopDownload.Time;
}

void Queue::Set_NumberDownloadAtSameTime(int numberDownloadAtSameTime)
{
	this->NumberDownloadAtSameTime = numberDownloadAtSameTime;
}
