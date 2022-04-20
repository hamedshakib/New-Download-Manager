#include "HeaderAndUi/ShowSchedule.h"

ShowSchedule::ShowSchedule(QueueManager* queueManager,QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	this->setWindowFlags(Qt::Window);
	this->m_queueManager = queueManager;
	connect(ui.listWidget, &QListWidget::currentItemChanged, this, [&](QListWidgetItem* current, QListWidgetItem* pervious) {CurrentQueueItemSelected = current; });
	PrepareQueues();
}

ShowSchedule::~ShowSchedule()
{
}

void ShowSchedule::PrepareQueues()
{
	for(Queue* queue : m_queueManager->Get_ListOfQueues())
	{
		AddQueueToListWidget(queue);
	}
}

void ShowSchedule::AddQueueToListWidget(Queue* queue)
{
	QListWidgetItem* listwidgetItem = new QListWidgetItem(ui.listWidget);
	listwidgetItem->setData(0, queue->Get_QueueName());
	listwidgetItem->setData(1, queue->Get_QueueId());

	ui.listWidget->addItem(listwidgetItem);
	ui.listWidget->setCurrentItem(listwidgetItem);
}

void ShowSchedule::on_StartNow_pushButton_clicked()
{
	m_queueManager->StartQueue(m_queueManager->AchiveQueue(CurrentQueueItemSelected->data(1).toInt()));
}

void ShowSchedule::on_StopNow_pushButton_clicked()
{
	m_queueManager->StopQueue(m_queueManager->AchiveQueue(CurrentQueueItemSelected->data(1).toInt()));
}

void ShowSchedule::on_Apply_pushButton_clicked()
{

}

void ShowSchedule::on_Cancel_pushButton_clicked()
{

}

void ShowSchedule::on_AddQueue_pushButton_clicked()
{
	QString newQueueName = ui.lineEdit->text();
	if (newQueueName.isEmpty())
	{
		return;
	}


	Queue *queue=m_queueManager->CreateNewQueue(newQueueName);
	QListWidgetItem* listwidgetItem = new QListWidgetItem(ui.listWidget);
	//listwidgetItem->setText(newQueueName);
	listwidgetItem->setData(0, newQueueName);
	listwidgetItem->setData(1, queue->Get_QueueId());

	ui.listWidget->addItem(listwidgetItem);
	ui.listWidget->setCurrentItem(listwidgetItem);
}

void ShowSchedule::on_DeleteQueue_pushButton_clicked()
{
	m_queueManager->DeleteQueueByQueueId(CurrentQueueItemSelected->data(1).toInt());
	ui.listWidget->removeItemWidget(CurrentQueueItemSelected);
	delete CurrentQueueItemSelected;
}

void ShowSchedule::AddDownloadsToTreeWidget()
{
	//Queue* queue;
	//=queue->Get_ListOfDownloadId();
}