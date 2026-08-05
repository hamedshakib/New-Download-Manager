#include "HeaderAndUi/QueueManager.h"
#include "qdebug.h"

QueueManager::QueueManager(DownloadManager* downloadManager,QObject *parent)
	: QObject(parent)
{
	this->m_downloadManager = downloadManager;
	connect(m_downloadManager, &DownloadManager::FinishedDownload, this, [&](Download* download) {if(download->get_QueueId()>0) ProcessRemoveADownloadFromQueue(download); });
	m_QueueTimeManager = new QueueTimeManager(this);
	connect(m_QueueTimeManager, &QueueTimeManager::StartQueue, this, &QueueManager::StartQueue);
	connect(m_QueueTimeManager, &QueueTimeManager::StopQueue, this, &QueueManager::StopQueue);
}

QueueManager::~QueueManager()
{
	// Stop all queues before cleanup
	for (Queue* queue : ListOfQueues) {
		StopQueue(queue);
	}
	
	// Clean up queue time manager
	if (m_QueueTimeManager) {
		m_QueueTimeManager->deleteLater();
		m_QueueTimeManager = nullptr;
	}
	
	// Clean up queue objects
	qDeleteAll(ListOfQueues);
	ListOfQueues.clear();
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
		DownloadControl* downloadControl = m_downloadManager->ProcessAchieveDownloadControl(download);
		if (downloadControl) {
			downloadControl->PauseDownload();
		}
	}
	queue->Downloading_list.clear();
	queue->Is_Downloading = false;
}

void QueueManager::ProcessDownloadOfQueue(Queue* queue)
{
	int NumberOfDownload =0;
	while (queue->Downloading_list.count() < queue->NumberDownloadAtSameTime)
	{
		if (queue->Downloading_list.count() == queue->List_DownloadId.count())
		{
			break;
		}

		//Should Find Download for downloading
		int download_id = DatabaseManager::GetturnInIdOfDownload(queue, NumberOfDownload + 1);

		Download* download = m_downloadManager->ProcessAchieveDownload(download_id);
		if (!download) {
			qWarning() << "Failed to load download with ID:" << download_id;
			break;
		}
		
		DownloadControl* downloadControl = m_downloadManager->ProcessAchieveDownloadControl(download);
		if (!downloadControl) {
			qWarning() << "Failed to create download control for download ID:" << download_id;
			break;
		}
		
		connect(downloadControl, &DownloadControl::CompeletedDownload, this, [&, queue]() {FinishDownloadOfQueue(download, queue); });
		queue->Downloading_list.append(download);
		if (downloadControl->IsDownloading() == false)
		{
			downloadControl->StartDownload();
		}

		NumberOfDownload++;
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

void QueueManager::ProcessRemoveADownloadFromQueue(Download* download)
{
	Queue* queue=AchiveQueue(download->get_QueueId());
	int DownloadNumberInQueueList=DatabaseManager::GetNumberDownloadInListOfQueue(download);
	RemoveDownloadFromQueue(download);
	DatabaseManager::DecreaseDownloadNumberOfQueueListForNextDownloadInQueueListOn_Queue_Download(queue,DownloadNumberInQueueList);
}

void QueueManager::ProcessRemoveADownloadFromQueue(size_t download_id)
{
	Download* download=m_downloadManager->ProcessAchieveDownload(download_id);
	if (!download) {
		qWarning() << "QueueManager: Failed to load download with ID:" << download_id << "for removal";
		return;
	}
	ProcessRemoveADownloadFromQueue(download);
}

bool QueueManager::RemoveDownloadFromQueue(Download* download)
{
	// Lock mutex to ensure thread-safe access to ListOfQueues
	QMutexLocker locker(&mutex);
	
	DatabaseManager::RemoveDownloadFrom_Queue_Download(download);
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
		if (DatabaseManager::AddDownloadTo_Queue_Download(queue, download))
		{
			queue->List_DownloadId.append(download->get_Id());
			download->Set_QueueId(queue->QueueId);
			emit AddedDownloadToQueue(download->get_Id(), queue->QueueId);
			return true;
		}
	}
	return false;
}

/*
bool QueueManager::RemoveDownloadFromQueue(Download* download,Queue* queue)
{
	DatabaseManager::RemoveDownloadFromQueueOnDatabase(download);
	queue->List_DownloadId.removeOne(download->get_Id());
	queue->Downloading_list.removeOne(download);
	emit RemovedDownloadFromQueue(download->get_Id(), queue->QueueId);
	return true;
}
*/

Queue* QueueManager::CreateNewQueue(QString QueueName)
{
	// Lock mutex to ensure thread-safe access to ListOfQueues
	QMutexLocker locker(&mutex);
	
	Queue* queue = new Queue(this);
	queue->QueueName = QueueName;
	queue->QueueId = DatabaseManager::CreateNewQueueOnDatabase(queue);
	ListOfQueues.append(queue);
	emit AddedQueue(queue->QueueId);
	return queue;
}

Queue* QueueManager::AchiveQueue(size_t Queue_id)
{
	// Lock mutex to ensure thread-safe access to ListOfQueues
	QMutexLocker locker(&mutex);
	
	for (Queue* queue : ListOfQueues)
	{
		if (queue->QueueId == Queue_id)
		{
			return queue;
		}
	}
	return nullptr;  // Return nullptr if queue not found
}

bool QueueManager::DeleteQueueByQueueId(size_t queue_id)
{
	// Lock mutex to ensure thread-safe access to ListOfQueues
	QMutexLocker locker(&mutex);
	
	Queue* queue = AchiveQueue(queue_id);
	
	if (!queue) {
		qWarning() << "QueueManager: Queue not found for deletion, ID:" << queue_id;
		return false;
	}
	
	StopQueue(queue);
	DatabaseManager::ExitAllDownloadFrom_Queue_Download(queue);
	DatabaseManager::ExitAllDownloadFromQueue(queue);
	DatabaseManager::RemoveQueueFromDatabase(queue);
	ListOfQueues.removeOne(queue);
	emit RemovedQueue(queue_id);
	queue->deleteLater();
	return true;
}

QList<Queue*> QueueManager::Get_ListOfQueues()
{
	// Lock mutex to ensure thread-safe access to ListOfQueues
	QMutexLocker locker(&mutex);
	
	return ListOfQueues;
}

bool QueueManager::HandelSingleShots()
{
	//int DeffirentSecond = QTime::currentTime().secsTo(QTime(23, 59, 59));
	//timer->singleShot(DeffirentSecond, this, &QueueManager::HandelSingleShots);
	
	// Placeholder implementation - needs proper timer-based scheduling logic
	// This method should handle single-shot queue scheduling
	
	return true;
}

bool QueueManager::ChangeStartOrStopTimeForQueue(Queue* queue)
{
	if (!queue) {
		qWarning() << "QueueManager::ChangeStartOrStopTimeForQueue called with null queue";
		return false;
	}
	
	return m_QueueTimeManager->AddSingleShot(queue);
}

bool QueueManager::MoveDownloadInQueue(Queue* queue, Download* download, int moveNumber)
{
	if (DatabaseManager::MoveDownloadIn_Queue_Download(queue, download, moveNumber))
		return true;
	else
		return false;
}

bool QueueManager::MoveDownloadInQueue(size_t download_id, int moveNumber)
{
	Download* download=m_downloadManager->ProcessAchieveDownload(download_id);
	Queue* queue=AchiveQueue(download->get_QueueId());
	if (MoveDownloadInQueue(queue, download, moveNumber))
		return true;
	else
		return false;

}
