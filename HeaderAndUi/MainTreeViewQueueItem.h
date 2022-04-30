#pragma once

#include <QStandardItem>
#include "Queue.h"
#include <QtWidgets/qwidget.h>

class MainTreeViewQueueItem : public QWidget, public QStandardItem
{
	Q_OBJECT

private:
	Queue* m_queue;


public:
	Queue* Get_Queue();


public:
	MainTreeViewQueueItem(Queue* queue, QObject* parent);
	~MainTreeViewQueueItem();
};
