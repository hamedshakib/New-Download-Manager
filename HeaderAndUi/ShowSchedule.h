#pragma once

#include <QWidget>
#include "ui_ShowSchedule.h"
#include "QueueManager.h"
#include "ConverterQueueTime.h"
#include "ScheduleTreeWidgetTabController.h"

class ShowSchedule : public QWidget
{
	Q_OBJECT

public:
	ShowSchedule(QueueManager* queueManager,QWidget *parent = nullptr);
	void PrepareQueues();
	~ShowSchedule();

private slots:
	void on_StartNow_pushButton_clicked();
	void on_StopNow_pushButton_clicked();
	void on_Apply_pushButton_clicked();
	void on_Cancel_pushButton_clicked();
	void on_AddQueue_pushButton_clicked();
	void on_DeleteQueue_pushButton_clicked();

	void AddQueueToListWidget(Queue *queue);

	void AddDownloadsToTreeWidget();


	void LoadInformationOfChangedQueue();

	void PutInformationInQueue(Queue* queue);

	void UpdateSchedule();

public slots:
	void UpdateScheduleTreeWidgetTab(Queue* queue);

private:
	QueueManager* m_queueManager;
	QListWidgetItem *CurrentQueueItemSelected;
	ScheduleTreeWidgetTabController* scheduleTreeWidgetTabController;

	Ui::ShowSchedule ui;
};
