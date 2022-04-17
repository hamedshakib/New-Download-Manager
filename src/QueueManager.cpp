#include "HeaderAndUi/QueueManager.h"

QueueManager::QueueManager(DownloadManager* downloadManager,QObject *parent)
	: QObject(parent)
{
	this->m_downloadManager = downloadManager;
}

QueueManager::~QueueManager()
{
}

void QueueManager::StartQueue(Queue* queue)
{
	auto DownloadList=queue->Get_QueueDownloadList();
	Download *download= DownloadList[0];

}

void QueueManager::StopQueue(Queue* queue)
{

}

void QueueManager::ProcessDownloadOfQueue(Queue* queue)
{
	
}

void QueueManager::FinishDownloadOfQueue(Download *download, Queue* queue)
{
	queue->Downloading_list.removeOne(download);
	queue->download_List.removeOne(download);

	if (!Is_QueueIsEmpty(queue))
	{
		ProcessDownloadOfQueue(queue);
	}
}

bool QueueManager::Is_QueueIsEmpty(Queue* queue)
{
	if (queue->download_List.isEmpty())
	{
		return true;
	}
	else
	{
		return false;
	}
}
