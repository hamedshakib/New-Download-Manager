#pragma once

#include <QWidget>
#include "ui_ShowSchedule.h"
#include "QueueManager.h"

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

private:
	QueueManager* m_queueManager;
	QListWidgetItem *CurrentQueueItemSelected;

	Ui::ShowSchedule ui;
};
