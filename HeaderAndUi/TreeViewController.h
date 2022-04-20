#pragma once

#include <QObject>
#include "qtreeview.h"
#include "qstandarditemmodel.h"
#include "QueueManager.h"
#include "qmenu.h"

#define NumberOfDataOfQueueId 10

class TreeViewController : public QObject
{
	Q_OBJECT


public:
	void Set_QueueManager(QueueManager* queueManager);
	void LoadTreeView();


private:
	QStandardItemModel* model;
	QTreeView* m_TreeView;
	QueueManager* m_queueManager;


	QStandardItem* Categoryitem;
	QStandardItem* Queueitem;

	const int NumberOfDataOfQueue = NumberOfDataOfQueueId;

private slots:
	void customContextMenuRequested(const QPoint& point);
	void CreateMenuForQueueRightClicked(const QPoint& Point, Queue* queue);

	void AddQueueToTreeView(int Queue_id);
	void RemoveQueueFromTreeView(int Queue_id);

public:
	TreeViewController(QTreeView* treeView,QObject *parent);
	~TreeViewController();
};
