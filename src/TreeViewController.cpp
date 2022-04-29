#include "HeaderAndUi/TreeViewController.h"

TreeViewController::TreeViewController(QTreeView* treeView,QObject *parent)
	: QObject(parent)
{
	this->m_TreeView = treeView;
}

TreeViewController::~TreeViewController()
{
}

void TreeViewController::Set_QueueManager(QueueManager* queueManager)
{
	m_queueManager = queueManager;
	connect(queueManager, &QueueManager::RemovedQueue, this, &TreeViewController::RemoveQueueFromTreeView);
	connect(queueManager, &QueueManager::AddedQueue, this, &TreeViewController::AddQueueToTreeView);
}

void TreeViewController::LoadTreeView()
{
	model = new QStandardItemModel(this);
	m_TreeView->setModel(model);
	m_TreeView->setContextMenuPolicy(Qt::CustomContextMenu);


	Categoryitem = new QStandardItem(tr("Categories"));
	Categoryitem->setEditable(false);
	Queueitem = new QStandardItem(tr("Queues"));
	Queueitem->setEditable(false);


	for (Queue* queue : m_queueManager->Get_ListOfQueues())
	{
		QStandardItem* child1 = new QStandardItem(queue->Get_QueueName());
		child1->setData(queue->Get_QueueId(),NumberOfDataOfQueue);
		child1->setEditable(false);
		child1->setDropEnabled(1);
		Queueitem->appendRow(child1);
	}

	connect(m_TreeView, &QTreeView::customContextMenuRequested, this,&TreeViewController::customContextMenuRequested );



	model->setItem(0, Categoryitem);
	model->setItem(1, Queueitem);

	model->setHorizontalHeaderItem(0, new QStandardItem(tr("Category")));
}

void TreeViewController::customContextMenuRequested(const QPoint& point)
{
	QTreeView* treeView = static_cast<QTreeView*>(sender());
	QModelIndex index = treeView->indexAt(point);

	if (!index.isValid()) 
	{
		return;
	}

	if (index.parent().model() == Queueitem->model())
	{
		//Queue right clicked
		size_t Queue_id=index.data(NumberOfDataOfQueue).toInt();
		Queue* queue=m_queueManager->AchiveQueue(Queue_id);

		CreateMenuForQueueRightClicked(point, queue);
	}
	else if (index.parent().model() == Categoryitem->model())
	{
		//Category right clicked
		QMenu* RightClickMenu = new QMenu(m_TreeView);
	}
	
}

void TreeViewController::CreateMenuForQueueRightClicked(const QPoint& Point, Queue* queue)
{

	QueueManager* queueManager = m_queueManager;
	QMenu* RightClickMenu = new QMenu(m_TreeView);
	bool is_Downloading = queue->Get_IsDownloading();
	if (is_Downloading == true)
	{
		QAction* PauseQueueAction = new QAction(tr("Pause"),this);
		connect(PauseQueueAction, &QAction::triggered, this, [&, queue, queueManager]() {queueManager->StopQueue(queue); sender()->deleteLater(); });
		RightClickMenu->addAction(PauseQueueAction);
	}
	else
	{
		QAction* ResumeQueueAction = new QAction(tr("Resume"),this);
		connect(ResumeQueueAction, &QAction::triggered, this, [&, queue, queueManager]() {queueManager->StartQueue(queue); sender()->deleteLater(); });
		RightClickMenu->addAction(ResumeQueueAction);
	}


	if (queue->Get_QueueId() > 1)
	{
		QAction* DeleteQueue = new QAction(tr("Delete"), this);
		connect(DeleteQueue, &QAction::triggered, this, [&, queue, queueManager]() {queueManager->DeleteQueueByQueueId(queue->Get_QueueId()); sender()->deleteLater(); });
		RightClickMenu->addAction(DeleteQueue);
	}


	RightClickMenu->exec(m_TreeView->viewport()->mapToGlobal(Point));
}

void TreeViewController::AddQueueToTreeView(int Queue_id)
{
	Queue* queue = m_queueManager->AchiveQueue(Queue_id);
	QStandardItem* child = new QStandardItem(queue->Get_QueueName());
	child->setData(queue->Get_QueueId(), NumberOfDataOfQueue);
	child->setEditable(false);
	child->setDropEnabled(1);
	Queueitem->appendRow(child);

	connect(m_TreeView, &QTreeView::customContextMenuRequested, this, &TreeViewController::customContextMenuRequested);
}

void TreeViewController::RemoveQueueFromTreeView(int Queue_id)
{
	for (int i = 0; i < Queueitem->rowCount(); i++)
	{
		int QueuItemId=Queueitem->child(i)->data(NumberOfDataOfQueue).toInt();
		if (QueuItemId == Queue_id)
		{
			Queueitem->removeRow(i);
		}
		
	}
	
}
