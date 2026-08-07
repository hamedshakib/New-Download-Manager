#include "HeaderAndUi/QueueManager.h"

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
		DownloadController* DownloadController = m_downloadManager->ProcessAchieveDownloadController(download);
		DownloadController->PauseDownload();
	}
	queue->Downloading_list.clear();
	queue->Is_Downloading = false;
}

void QueueManager::ProcessDownloadOfQueue(Queue* queue)
{
	if (!queue || !queue->Is_Downloading) return;

	// تا زمانی که ظرفیت دانلود همزمان خالی است و دانلودهای صف تمام نشده‌اند
	while (queue->Downloading_list.count() < queue->NumberDownloadAtSameTime)
	{
		Download* nextDownloadToStart = nullptr;

		// جستجو در لیست آی‌دی‌های صف برای یافتن اولین دانلودی که:
		// ۱. قبلاً کامل نشده باشد (Status != Completed)
		// ۲. در حال حاضر در لیست دانلودهای فعال (Downloading_list) نباشد
		for (size_t download_id : queue->List_DownloadId)
		{
			Download* download = m_downloadManager->ProcessAchieveDownload(download_id);
			if (!download) continue;

			bool isAlreadyDownloading = queue->Downloading_list.contains(download);
			bool isCompleted = (download->get_Status() == Download::Completed);

			if (!isAlreadyDownloading && !isCompleted) {
				nextDownloadToStart = download;
				break; // اولین دانلود واجد شرایط پیدا شد
			}
		}

		// اگر هیچ دانلود جدیدی برای شروع پیدا نشد، حلقه را می‌شکنیم
		if (!nextDownloadToStart) {
			break;
		}

		// اضافه کردن به لیست فعال‌های صف و شروع دانلود
		queue->Downloading_list.append(nextDownloadToStart);
		DownloadController* downloadController = m_downloadManager->ProcessAchieveDownloadController(nextDownloadToStart);

		// استفاده از Lambda ایمن برای مدیریت اتمام دانلود
		connect(downloadController, &DownloadController::DownloadCompleted, this,
			[this, nextDownloadToStart, queue]() {
				FinishDownloadOfQueue(nextDownloadToStart, queue);
			}, Qt::UniqueConnection);

		if (!downloadController->IsDownloading()) {
			downloadController->StartDownload();
		}
	}

	// اگر لیست فعال‌ها خالی است و آیتمی نمانده، یعنی کل صف تمام شده است
	if (queue->Downloading_list.isEmpty()) {
		queue->Is_Downloading = false;
		qDebug() << "All downloads in queue completed:" << queue->Get_QueueName();
	}
}

void QueueManager::FinishDownloadOfQueue(Download* download, Queue* queue)
{
	if (!queue || !download) return;

	// ۱. حیاتی: حذف دانلود تکمیل‌شده از لیست دانلودهای فعال صف
	queue->Downloading_list.removeOne(download);

	// ۲. بررسی خالی بودن صف و جایگزین کردن دانلود بعدی در ظرفیت آزاد شده
	if (!Is_QueueIsEmpty(queue) && queue->Is_Downloading) {
		ProcessDownloadOfQueue(queue);
	}
	else if (queue->Downloading_list.isEmpty()) {
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
	DatabaseManager::LoadAllQueues(ListOfQueues, this);
	// اضافه کردن این خط حیاتی است:
	for (Queue* queue : ListOfQueues) {
		m_QueueTimeManager->AddSingleShot(queue);
	}
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
	ProcessRemoveADownloadFromQueue(download);
}

bool QueueManager::RemoveDownloadFromQueue(Download* download)
{
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
