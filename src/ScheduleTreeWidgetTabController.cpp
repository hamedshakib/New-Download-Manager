#include "HeaderAndUi/ScheduleTreeWidgetTabController.h"
Queue* m_Currentqueue111;
ScheduleTreeWidgetTabController::ScheduleTreeWidgetTabController(QTreeWidget* treeWidget,QObject *parent)
	: QObject(parent)
{
	this->m_TreeWidget = treeWidget;
	//Set Header 

	treeWidget->header();
	horizontalHeader = treeWidget->header();
	horizontalHeader->setSectionsMovable(false);
	connect(horizontalHeader, &QHeaderView::sectionResized, this, [&](int numberOfColum, int oldsize, int newSize) {ChangeColumnWidth(numberOfColum, newSize); });



	for (int columNumber = 0; columNumber < horizontalHeader->count(); columNumber++)
	{
		QString SettingStringKey = "TreeWidget/ScheduleTreeWidget/WidthColum" + QString::number(columNumber);
		int ColumWidth = SettingInteract::GetValue(SettingStringKey).toInt();
		treeWidget->setColumnWidth(columNumber, ColumWidth);
	}
}

ScheduleTreeWidgetTabController::~ScheduleTreeWidgetTabController()
{
}

void ScheduleTreeWidgetTabController::Set_QueueManager(QueueManager* queueManager)
{
	this->m_QueueManager = queueManager;
}

void ScheduleTreeWidgetTabController::ChangedCurrentQueue(Queue* NewQueue)
{
	m_CurrentQueue = NewQueue;
	UpdateAllDataOfTreeWidget();
}

void ScheduleTreeWidgetTabController::UpdateAllDataOfTreeWidget()
{
	m_TreeWidget->clear();
	TreeWidgetItems.clear();
	LoadDownloadInformationOfQueueForScheduleTreeWidgetFromDatabase();

	std::sort(TreeWidgetItems.begin(), TreeWidgetItems.end(), [&](const QTreeWidgetItem* v1, const QTreeWidgetItem* v2) {return v1->data(4, 0).toInt() < v2->data(4, 0).toInt(); });
	m_TreeWidget->insertTopLevelItems(0, TreeWidgetItems);
}

void ScheduleTreeWidgetTabController::LoadDownloadInformationOfQueueForScheduleTreeWidgetFromDatabase()
{
	DatabaseManager::LoadDownloadInformationOfQueueForScheduleTreeWidget(TreeWidgetItems, m_CurrentQueue);
}

size_t ScheduleTreeWidgetTabController::GetDownloadIdFromTreeWidgetItem(QTreeWidgetItem* item)
{
	return item->data(5, 0).toInt();
}

void ScheduleTreeWidgetTabController::ClickedOnUpButton()
{
	auto items = m_TreeWidget->selectedItems();
	if (!items.isEmpty())
	{
		auto item = items[0];
		size_t download_id = GetDownloadIdFromTreeWidgetItem(item);
		m_QueueManager->MoveDownloadInQueue(download_id, -1);
		UpdateAllDataOfTreeWidget();
	}
}

void ScheduleTreeWidgetTabController::ClickedOnDownButton()
{
	auto items= m_TreeWidget->selectedItems();
	if (!items.isEmpty())
	{
		auto item = items[0];
		size_t download_id = GetDownloadIdFromTreeWidgetItem(item);

		m_QueueManager->MoveDownloadInQueue(download_id, +1);
		UpdateAllDataOfTreeWidget();
	}
}

void ScheduleTreeWidgetTabController::ClickedOnRemoveButton()
{
	for (QTreeWidgetItem* item : m_TreeWidget->selectedItems())
	{
		size_t download_id=GetDownloadIdFromTreeWidgetItem(item);
		m_QueueManager->ProcessRemoveADownloadFromQueue(download_id);
	}
	UpdateAllDataOfTreeWidget();
}

void ScheduleTreeWidgetTabController::ChangeColumnWidth(int numberOfColumn, int NewColumnWidth)
{
	QString SettingStringKey = "TreeWidget/ScheduleTreeWidget/WidthColum" + QString::number(numberOfColumn);
	SettingInteract::SetValue(SettingStringKey, NewColumnWidth);
}

