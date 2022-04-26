#include "HeaderAndUi/ShowSchedule.h"

ShowSchedule::ShowSchedule(QueueManager* queueManager,QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	this->setWindowFlags(Qt::Window);
	this->m_queueManager = queueManager;
	connect(ui.listWidget, &QListWidget::currentItemChanged, this, [&](QListWidgetItem* current, QListWidgetItem* pervious) {CurrentQueueItemSelected = current; LoadInformationOfChangedQueue(); });
	PrepareQueues();



	connect(ui.StartAt_checkBox, &QCheckBox::stateChanged, this, [&](int) {UpdateSchedule(); });
	connect(ui.StopAt_checkBox, &QCheckBox::stateChanged, this, [&](int) {UpdateSchedule(); });
	connect(ui.OnceTime_radioButton, &QRadioButton::clicked, this, [&](int) {UpdateSchedule(); });
	connect(ui.Periodic_radioButton, &QRadioButton::clicked, this, [&](int) {UpdateSchedule(); });
	connect(ui.daily_radioButton, &QRadioButton::clicked, this, [&](int) {UpdateSchedule(); });
	connect(ui.radioButton_4, &QRadioButton::clicked, this, [&](int) {UpdateSchedule(); });
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
	Queue* queue = m_queueManager->AchiveQueue(CurrentQueueItemSelected->data(1).toInt());
	PutInformationInQueue(queue);
	m_queueManager->ChangeStartOrStopTimeForQueue(queue);
	DatabaseManager::UpdateTimeQueueEvents(queue);
}

void ShowSchedule::on_Cancel_pushButton_clicked()
{
	this->close();
	this->deleteLater();
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

void ShowSchedule::LoadInformationOfChangedQueue()
{
	ui.checkBox_2->setChecked(false);
	ui.checkBox_3->setChecked(false);
	ui.checkBox_4->setChecked(false);
	ui.checkBox_5->setChecked(false);
	ui.checkBox_6->setChecked(false);
	ui.checkBox_7->setChecked(false);
	ui.checkBox_8->setChecked(false);
	Queue* queue = m_queueManager->AchiveQueue(CurrentQueueItemSelected->data(1).toInt());
	//StartTime
	if (queue->Has_StartTimeActive())
	{
		ui.StartAt_checkBox->setChecked(true);

	}
	else
	{
		ui.StartAt_checkBox->setChecked(false);
	}

	//StopTime
	if (queue->Has_StopTimeActive())
	{
		ui.StopAt_checkBox->setChecked(true);
	}
	else
	{
		ui.StopAt_checkBox->setChecked(false);
	}

	if (!queue->DownloadDays.isEmpty())
	{
		if (ConverterQueueTime::IsNumberOfDays(queue->DownloadDays[0]))
		{
			ui.Periodic_radioButton->setChecked(true);
			ui.radioButton_4->setChecked(true);
			ui.spinBox->setValue(queue->DownloadDays[0].toInt());
		}
		else if (ConverterQueueTime::IsNameOfDaysOfWeek(queue->DownloadDays[0]))
		{
			ui.Periodic_radioButton->setChecked(true);
			ui.daily_radioButton->setChecked(true);
			for (QString string : queue->DownloadDays)
			{
				if (string == "Saturday")
				{
					ui.checkBox_2->setChecked(true);
				}
				else if (string == "Sunday")
				{
					ui.checkBox_3->setChecked(true);
				}
				else if (string == "Monday")
				{
					ui.checkBox_4->setChecked(true);
				}
				else if (string == "Tuesday")
				{
					ui.checkBox_5->setChecked(true);
				}
				else if (string == "Wednesday")
				{
					ui.checkBox_6->setChecked(true);
				}
				else if (string == "Thursday")
				{
					ui.checkBox_7->setChecked(true);
				}
				else if (string == "Friday")
				{
					ui.checkBox_8->setChecked(true);
				}
			}
		}
		else
		{
			//OnceTime 
			ui.OnceTime_radioButton->setChecked(true);
			ui.dateEdit->setDate(QDate::fromString(queue->DownloadDays[0]));
		}
	}
	UpdateSchedule();
}

void ShowSchedule::UpdateSchedule()
{
	//StartAt
	if (ui.StartAt_checkBox->isChecked())
	{
		ui.OnceTime_radioButton->setEnabled(true);
		ui.Periodic_radioButton->setEnabled(true);
		ui.Start_timeEdit->setEnabled(true);

	}
	else
	{
		ui.OnceTime_radioButton->setEnabled(false);
		ui.Periodic_radioButton->setEnabled(false);
		ui.Start_timeEdit->setEnabled(false);

		ui.OnceTime_radioButton->setAutoExclusive(false);
		ui.OnceTime_radioButton->setChecked(false);
		ui.OnceTime_radioButton->setAutoExclusive(true);

		ui.Periodic_radioButton->setAutoExclusive(false);
		ui.Periodic_radioButton->setChecked(false);
		ui.Periodic_radioButton->setAutoExclusive(true);
	}

	//StopAt
	if (ui.StopAt_checkBox->isChecked())
	{
		ui.Stop_timeEdit->setEnabled(true);

	}
	else
	{
		ui.Stop_timeEdit->setEnabled(false);
	}

	//OnceTime
	if (ui.OnceTime_radioButton->isChecked())
	{
		ui.dateEdit->setEnabled(true);

		ui.daily_radioButton->setAutoExclusive(false);
		ui.daily_radioButton->setChecked(false);
		ui.daily_radioButton->setAutoExclusive(true);

		ui.radioButton_4->setAutoExclusive(false);
		ui.radioButton_4->setChecked(false);
		ui.radioButton_4->setAutoExclusive(true);
	}
	else
	{
		ui.dateEdit->setEnabled(false);
	}

	//PeriodicTime
	if (ui.Periodic_radioButton->isChecked())
	{
		ui.daily_radioButton->setEnabled(true);
		ui.radioButton_4->setEnabled(true);
	}
	else
	{
		ui.daily_radioButton->setEnabled(false);
		ui.radioButton_4->setEnabled(false);

		ui.daily_radioButton->setAutoExclusive(false);
		ui.daily_radioButton->setChecked(false);
		ui.daily_radioButton->setAutoExclusive(true);

		ui.radioButton_4->setAutoExclusive(false);
		ui.radioButton_4->setChecked(false);
		ui.radioButton_4->setAutoExclusive(true);


		ui.widget_4->setEnabled(false);
		ui.spinBox->setEnabled(false);


	}

	//Daily from PeriodicTime
	if (ui.daily_radioButton->isChecked())
	{
		ui.widget_4->setEnabled(true);
	}
	else
	{
		ui.widget_4->setEnabled(false);
	}

	//EachDay from PeriodicTime
	if (ui.radioButton_4->isChecked())
	{
		ui.spinBox->setEnabled(true);
	}
	else
	{
		ui.spinBox->setEnabled(false);
	}
}

void ShowSchedule::PutInformationInQueue(Queue* queue)
{
	if (ui.StartAt_checkBox->isChecked())
	{
		queue->startDownload.is_active = true;
		queue->startDownload.Time = ui.Start_timeEdit->time();
	}
	else
	{
		queue->startDownload.is_active = false;
	}

	//StopAt
	if (ui.StopAt_checkBox->isChecked())
	{
		queue->stopDownload.is_active = true;
		queue->stopDownload.Time = ui.Stop_timeEdit->time();
	}
	else
	{
		queue->stopDownload.is_active = false;
	}


	
	if (ui.OnceTime_radioButton->isChecked())
	{
		queue->DownloadDays.clear();
		queue->DownloadDays.append(ui.dateEdit->date().toString());
	}
	else 
	{
		//Periodic Synchronization
		queue->DownloadDays.clear();
		if (ui.daily_radioButton->isChecked())
		{
			if (ui.checkBox_2->isChecked())
			{
				queue->DownloadDays.append("Saturday");
			}
			if (ui.checkBox_3->isChecked())
			{
				queue->DownloadDays.append("Sunday");
			}
			if (ui.checkBox_4->isChecked())
			{
				queue->DownloadDays.append("Monday");
			}
			if (ui.checkBox_5->isChecked())
			{
				queue->DownloadDays.append("Tuesday");
			}
			if (ui.checkBox_6->isChecked())
			{
				queue->DownloadDays.append("Wednesday");
			}
			if (ui.checkBox_7->isChecked())
			{
				queue->DownloadDays.append("Thursday");
			}
			if (ui.checkBox_8->isChecked())
			{
				queue->DownloadDays.append("Friday");
			}
		}
		else
		{
			//Each 
			queue->DownloadDays.append(QString::number(ui.spinBox->value()));

		}
	}



}