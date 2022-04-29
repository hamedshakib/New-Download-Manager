#pragma once

#include <QObject>
#include "qtreewidget.h"
#include "QueueManager.h"
#include "DatabaseManager.h"
#include "qheaderview.h"

class ScheduleTreeWidgetTabController : public QObject
{
	Q_OBJECT

public:
	void Set_QueueManager(QueueManager* queueManager);

public slots:
	void UpdateAllDataOfTreeWidget();
	void ChangedCurrentQueue(Queue* NewQueue);

	void ClickedOnUpButton();
	void ClickedOnDownButton();
	void ClickedOnRemoveButton();
private slots:
	void LoadDownloadInformationOfQueueForScheduleTreeWidgetFromDatabase();
	size_t GetDownloadIdFromTreeWidgetItem(QTreeWidgetItem* item);


	void ChangeColumnWidth(int numberOfColumn, int NewColumnWidth);

private:
	QList<QTreeWidgetItem*> TreeWidgetItems;
	QTreeWidget* m_TreeWidget;
	QueueManager* m_QueueManager;
	QHeaderView* horizontalHeader;

	Queue* m_CurrentQueue;

public:
	ScheduleTreeWidgetTabController(QTreeWidget* treeWidget,QObject *parent);
	~ScheduleTreeWidgetTabController();
};
