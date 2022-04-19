#pragma once

#include <QObject>
#include "Queue.h"
#include "Download.h"
#include "DownloadManager.h"
#include "qdebug.h"

class QueueManager : public QObject
{
	Q_OBJECT


private:
	QList<Queue*> ListOfQueues;
	DownloadManager* m_downloadManager;

public:
	void StartQueue(Queue* queue);
	void StopQueue(Queue* queue);
	void LoadQueuesFormDatabase();

	Queue* CreateNewQueue(QString QueueName);
	bool DeleteQueueByQueueId(size_t queue_id);

	QList<Queue*> Get_ListOfQueues();
	Queue* AchiveQueue(size_t Queue_id);

private slots:
	void FinishDownloadOfQueue(Download* download, Queue* queue);
	void ProcessDownloadOfQueue(Queue* queue);
	bool Is_QueueIsEmpty(Queue* queue);


	bool RemoveDownloadFromQueue(Download* download);
	bool RemoveDownloadFromQueue(Download* download,Queue* queue);

public:
	QueueManager(DownloadManager* downloadManager,QObject *parent);
	~QueueManager();
};
