#include "HeaderAndUi/QueueManager.h"

QueueManager::QueueManager(DownloadManager* downloadManager,QObject *parent)
	: QObject(parent)
{
	this->m_downloadManager = downloadManager;
	connect(m_downloadManager, &DownloadManager::FinishedDownload, this, [&](Download* download) {RemoveDownloadFromQueue(download); });
}

QueueManager::~QueueManager()
{
}

void QueueManager::StartQueue(Queue* queue)
{
	ProcessDownloadOfQueue(queue);
}

void QueueManager::StopQueue(Queue* queue)
{
	for (Download* download : queue->Downloading_list)
	{
		Downloader* downloader = m_downloadManager->ProcessAchieveDownloader(download);
		downloader->PauseDownload();
	}
}

void QueueManager::ProcessDownloadOfQueue(Queue* queue)
{
	int NumberOfDownload = 0;
	while (queue->Downloading_list.count()<queue->NumberDownloadAtSameTime)
	{
		if (queue->Downloading_list.count() == queue->List_DownloadId.count())
		{
			break;
		}
		for (; NumberOfDownload < queue->List_DownloadId.count(); NumberOfDownload++)
		{
			bool Is_find = false;
			for (Download* Tempdownload:queue->Downloading_list)
			{
				if (Tempdownload->get_Id() == queue->List_DownloadId.at(NumberOfDownload))
				{
					Is_find = true;
					break;
				}
			}
			if (Is_find == false)
			{
				//Download is not exist in Downloading List So can add it
				Download* download = m_downloadManager->ProcessAchieveDownload(queue->List_DownloadId.at(NumberOfDownload));
				Downloader* downloader = m_downloadManager->ProcessAchieveDownloader(download);
				connect(downloader, &Downloader::CompeletedDownload, this, [&, queue]() {FinishDownloadOfQueue(download, queue); });
				queue->Downloading_list.append(download);
				if (downloader->IsDownloading() == false)
				{
					downloader->StartDownload();
				}
				break;
			}
		}

	}
}

void QueueManager::FinishDownloadOfQueue(Download *download, Queue* queue)
{
	if (!Is_QueueIsEmpty(queue))
	{
		ProcessDownloadOfQueue(queue);
	}
}

bool QueueManager::Is_QueueIsEmpty(Queue* queue)
{
	if (queue->List_DownloadId.isEmpty())
	{
		return true;
	}
	else
	{
		return false;
	}
}

void QueueManager::LoadQueuesFormDatabase()
{
	DatabaseManager::LoadAllQueues(ListOfQueues,this);
}

bool QueueManager::RemoveDownloadFromQueue(Download* download)
{
	DatabaseManager::RemoveDownloadFromQueueOnDatabase(download);
	for (Queue* queue : ListOfQueues)
	{
		qDebug() << download->get_Id();
		queue->List_DownloadId.removeOne(download->get_Id());
		queue->Downloading_list.removeOne(download);

	}
	return true;
}

bool QueueManager::RemoveDownloadFromQueue(Download* download,Queue* queue)
{
	DatabaseManager::RemoveDownloadFromQueueOnDatabase(download);
	queue->List_DownloadId.removeOne(download->get_Id());
	queue->Downloading_list.removeOne(download);
	return true;
}
