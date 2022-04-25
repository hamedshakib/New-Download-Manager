#include "HeaderAndUi/QueueTimeManager.h"

QueueTimeManager::QueueTimeManager(QObject *parent)
	: QObject(parent)
{
	//timer = new QTimer(this);
	//timer->setSingleShot(true);
	//timer->setTimerType(Qt::TimerType::VeryCoarseTimer);
}

QueueTimeManager::~QueueTimeManager()
{
}

bool QueueTimeManager::Is_TodayaDayOfDownload(QStringList DownloadDays)
{
	if (!DownloadDays.isEmpty())
	{
		const QString FirstIndexOfDownloadDays;
		QDate CurrentDate = QDate::currentDate();
		if (IsNameOfDaysOfWeek(FirstIndexOfDownloadDays))
		{

			for (QString DayOfWeek : DownloadDays)
			{
				if (ConvertDayStringToNumberOfDayOfWeek(DayOfWeek) == CurrentDate.dayOfWeek())
				{
					return true;
				}
			}
		}
		else if (IsNumberOfDays(FirstIndexOfDownloadDays))
		{

		}
		else
		{
			//A special date
			QDate StartDay = QDate::fromString(FirstIndexOfDownloadDays);
			if (QDate::currentDate() == StartDay)
			{
				return true;
			}
		}
	}
	else
	{
		return false;
	}
}

bool QueueTimeManager::IsNameOfDaysOfWeek(QString day)
{
	if (day == "Saturday" || day == "Sunday" || day == "Monday" || day == "Tuesday" ||
		day == "Wednesday" || day == "Thursday" || day == "Friday")
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool QueueTimeManager::IsNumberOfDays(QString day)
{
	bool is_Success;
	day.toInt(&is_Success);
	if (is_Success)
	{
		return true;
	}
	else
	{
		return false;
	}
}

int QueueTimeManager::ConvertDayStringToNumberOfDayOfWeek(QString day)
{
	if(day == "Saturday")
	{ 
		return 7;
	}

	else if(day == "Sunday")
	{
		return 1;
	}
	else if (day == "Monday")
	{
		return 2;
	}
	else if (day == "Tuesday")
	{
		return 3;
	}
	else if (day == "Wednesday")
	{
		return 4;
	}
	else if (day == "Thursday")
	{
		return 5;
	}
	else if (day == "Friday")
	{
		return 6;
	}
}

void QueueTimeManager::DayChangedSlot()
{
	emit DayChanged();
	int DeffirentSecond = QTime::currentTime().secsTo(QTime(23, 59, 59)) + 1;
	QTimer::singleShot(DeffirentSecond*1000, this, &QueueTimeManager::DayChangedSlot);
}

void QueueTimeManager::DayTimerSingleShotManage(QList<Queue*> queues)
{
	for (Queue* queue : queues)
	{
		if (Is_TodayaDayOfDownload(queue->DownloadDays))
		{
			if (queue->startDownload.is_active)
			{
				int secondsToStart = QTime::currentTime().secsTo(queue->startDownload.Time);
				if (secondsToStart > 0)
				{
					QTimer::singleShot(secondsToStart*1000, [&, queue]() {CheckQueueForEvent(queue); });
				}
			}
			else if (queue->stopDownload.is_active)
			{
				int secondsToStop = QTime::currentTime().secsTo(queue->stopDownload.Time);
				if (secondsToStop > 0)
				{
					QTimer::singleShot(secondsToStop*1000, [&, queue]() {CheckQueueForEvent(queue); });
				}
			}
		}
	}
}

bool QueueTimeManager::CheckQueueForEvent(Queue* queue)
{
	const int AllowedSecondsError = 5;
	if (queue != nullptr)
	{
		qDebug() <<"Test:"<<QTime::currentTime().secsTo(queue->startDownload.Time);
		if (std::abs(QTime::currentTime().secsTo(queue->startDownload.Time)) < AllowedSecondsError)
		{
			emit StartQueue(queue);
			return true;
		}
		else if (std::abs(QTime::currentTime().secsTo(queue->stopDownload.Time)) < AllowedSecondsError)
		{
			emit StopQueue(queue);
			return true;
		}
	}
	return false;
}

bool QueueTimeManager::AddSingleShot(Queue* queue)
{
	if (Is_TodayaDayOfDownload(queue->DownloadDays))
	{
		if (queue->startDownload.is_active)
		{
			int secondsToStart = QTime::currentTime().secsTo(queue->startDownload.Time);
			if (secondsToStart > 0)
			{
				QTimer::singleShot(secondsToStart*1000, [&, queue]() {CheckQueueForEvent(queue); });
			}
		}
	}
	if (queue->stopDownload.is_active)
	{
			int secondsToStop = QTime::currentTime().secsTo(queue->stopDownload.Time);
			if (secondsToStop > 0)
			{
				QTimer::singleShot(secondsToStop*1000, [&, queue]() {CheckQueueForEvent(queue); });
			}
	}
	
	return false;
}



