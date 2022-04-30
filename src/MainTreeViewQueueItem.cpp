#include "HeaderAndUi/MainTreeViewQueueItem.h"

MainTreeViewQueueItem::MainTreeViewQueueItem(Queue* queue, QObject* parent)
	:m_queue(queue),QStandardItem(queue->Get_QueueName()),QWidget()
{

}

MainTreeViewQueueItem::~MainTreeViewQueueItem()
{

}

Queue* MainTreeViewQueueItem::Get_Queue()
{
	return m_queue;
}
