#pragma once

#include <QObject>
#include "Queue.h"
#include "Download.h"
#include "DownloadManager.h"
#include "QueueTimeManager.h"
#include "qdebug.h"

class QueueManager : public QObject
{
	Q_OBJECT


private:
	QList<Queue*> ListOfQueues;
	DownloadManager* m_downloadManager;
	QueueTimeManager* m_QueueTimeManager;

public:
	void StartQueue(Queue* queue);
	void StopQueue(Queue* queue);
	void LoadQueuesFormDatabase();

	Queue* CreateNewQueue(QString QueueName);
	bool DeleteQueueByQueueId(size_t queue_id);

	QList<Queue*> Get_ListOfQueues();
	Queue* AchiveQueue(size_t Queue_id);

public slots:
	void ProcessRemoveADownloadFromQueue(size_t download_id);
	void ProcessRemoveADownloadFromQueue(Download* download);
	bool AddDownloadToQueue(Download* download, Queue* queue);
	bool ChangeStartOrStopTimeForQueue(Queue* queue);

	bool MoveDownloadInQueue(Queue* queue,Download* download,int moveNumber);
	bool MoveDownloadInQueue(size_t download_id,int moveNumber);

private slots:
	void FinishDownloadOfQueue(Download* download, Queue* queue);
	void ProcessDownloadOfQueue(Queue* queue);
	bool Is_QueueIsEmpty(Queue* queue);


	//bool RemoveDownloadFromQueue(Download* download,Queue* queue);

	bool RemoveDownloadFromQueue(Download* download);

	bool HandelSingleShots();


signals:
	void AddedQueue(int Queue_id);
	void RemovedQueue(int Queue_id);
	void AddedDownloadToQueue(int download_id, int Queue_id);
	void RemovedDownloadFromQueue(int download_id, int Queue_id);

public:
	QueueManager(DownloadManager* downloadManager,QObject *parent);
	~QueueManager();
};
