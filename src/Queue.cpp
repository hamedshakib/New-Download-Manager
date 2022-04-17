#include "HeaderAndUi/Queue.h"

Queue::Queue(QObject *parent)
	: QObject(parent)
{
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

QList<Download*> Queue::Get_QueueDownloadList()
{
	return download_List;
}

int Queue::Get_NumberDownloadAtSameTime()
{
	return NumberDownloadAtSameTime;
}
