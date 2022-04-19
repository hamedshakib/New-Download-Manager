#pragma once

#include <QObject>
#include "QueueManager.h"

class ProcessAddNewDownloadToQueue : public QObject
{
	Q_OBJECT

public:
	QList<Queue*> Get_QueuesList();
	 

public:
	QueueManager* m_queueManager;
	ProcessAddNewDownloadToQueue(QObject *parent);
	~ProcessAddNewDownloadToQueue();
};
