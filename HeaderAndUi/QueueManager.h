#pragma once

#include <QObject>
#include "Queue.h"
#include "Download.h"
#include "DownloadManager.h"

class QueueManager : public QObject
{
	Q_OBJECT


private:
	QList<Queue*> QueueList;
	DownloadManager* m_downloadManager;

public:
	void StartQueue(Queue* queue);
	void StopQueue(Queue* queue);


private slots:
	void FinishDownloadOfQueue(Download* download, Queue* queue);
	void ProcessDownloadOfQueue(Queue* queue);
	bool Is_QueueIsEmpty(Queue* queue);
public:
	QueueManager(DownloadManager* downloadManager,QObject *parent);
	~QueueManager();
};
