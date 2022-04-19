#include "HeaderAndUi/ProcessAddNewDownloadToQueue.h"

ProcessAddNewDownloadToQueue::ProcessAddNewDownloadToQueue(QObject *parent)
	: QObject(parent)
{
}

ProcessAddNewDownloadToQueue::~ProcessAddNewDownloadToQueue()
{
}

QList<Queue*> ProcessAddNewDownloadToQueue::Get_QueuesList()
{
	return m_queueManager->Get_ListOfQueues();
}
