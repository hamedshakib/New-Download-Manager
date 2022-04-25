#include "HeaderAndUi/QueueManager.h"

QueueManager::QueueManager(DownloadManager* downloadManager,QObject *parent)
	: QObject(parent)
{
	this->m_downloadManager = downloadManager;
	connect(m_downloadManager, &DownloadManager::FinishedDownload, this, [&](Download* download) {RemoveDownloadFromQueue(download); });
	m_QueueTimeManager = new QueueTimeManager(this);
	connect(m_QueueTimeManager, &QueueTimeManager::StartQueue, this, &QueueManager::StartQueue);
	connect(m_QueueTimeManager, &QueueTimeManager::StopQueue, this, &QueueManager::StopQueue);
}

QueueManager::~QueueManager()
{
}

void QueueManager::StartQueue(Queue* queue)
{
	queue->Is_Downloading = true;
	ProcessDownloadOfQueue(queue);
}

void QueueManager::StopQueue(Queue* queue)
{
	for (Download* download : queue->Downloading_list)
	{
		Downloader* downloader = m_downloadManager->ProcessAchieveDownloader(download);
		downloader->PauseDownload();
	}
	queue->Downloading_list.clear();
	queue->Is_Downloading = false;
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
	else
	{
		queue->Is_Downloading = false;
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
		download->Set_QueueId(0);
		emit RemovedDownloadFromQueue(download->get_Id(), queue->QueueId);
	}
	return true;
}

bool QueueManager::AddDownloadToQueue(Download* download, Queue* queue)
{
	if (DatabaseManager::AddDownloadToQueueOnDatabase(download, queue))
	{
		queue->List_DownloadId.append(download->get_Id());
		download->Set_QueueId(queue->QueueId);
		emit AddedDownloadToQueue(download->get_Id(), queue->QueueId);
		return true;
	}
	return false;
}

bool QueueManager::RemoveDownloadFromQueue(Download* download,Queue* queue)
{
	DatabaseManager::RemoveDownloadFromQueueOnDatabase(download);
	queue->List_DownloadId.removeOne(download->get_Id());
	queue->Downloading_list.removeOne(download);
	emit RemovedDownloadFromQueue(download->get_Id(), queue->QueueId);
	return true;
}

Queue* QueueManager::CreateNewQueue(QString QueueName)
{
	Queue* queue = new Queue(this);
	queue->QueueName = QueueName;
	queue->QueueId =DatabaseManager::CreateNewQueueOnDatabase(queue);
	ListOfQueues.append(queue);
	emit AddedQueue(queue->QueueId);
	return queue;
}

Queue* QueueManager::AchiveQueue(size_t Queue_id)
{
	for (Queue* queue : ListOfQueues)
	{
		if (queue->QueueId == Queue_id)
		{
			return queue;
		}
	}
}

bool QueueManager::DeleteQueueByQueueId(size_t queue_id)
{
	//ToDo
	
	Queue* queue=AchiveQueue(queue_id);
	StopQueue(queue);
	DatabaseManager::ExitAllDownloadFromQueue(queue);
	DatabaseManager::RemoveQueueFromDatabase(queue);
	ListOfQueues.removeOne(queue);
	emit RemovedQueue(queue_id);
	queue->deleteLater();
	return true;
}

QList<Queue*> QueueManager::Get_ListOfQueues()
{
	return ListOfQueues;
}

bool QueueManager::HandelSingleShots()
{
	
	//int DeffirentSecond=QTime::currentTime().secsTo(QTime(23, 59, 59));
	//timer->singleShot(DeffirentSecond, this, &QueueManager::HandelSingleShots);
	
	

	return 0;
}

bool QueueManager::ChangeStartOrStopTimeForQueue(Queue* queue)
{
	m_QueueTimeManager->AddSingleShot(queue);
	return 0;
}
