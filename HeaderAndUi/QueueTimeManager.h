#pragma once

#include <QObject>
#include "qtimer.h"
#include "qdatetime.h"
#include "Queue.h"

class QueueTimeManager : public QObject
{
	Q_OBJECT

public:
	void DayTimerSingleShotManage(QList<Queue*> queues);
	bool Is_TodayaDayOfDownload(QStringList DownloadDays);

	bool CheckQueueForEvent(Queue* queue);

	bool AddSingleShot(Queue* queue);

private:
	QTimer* timer;
	bool IsNameOfDaysOfWeek(QString day);
	bool IsNumberOfDays(QString day);
	void DayChangedSlot();
	
	int ConvertDayStringToNumberOfDayOfWeek(QString day);

signals:
	void DayChanged();
	void StartQueue(Queue* queue);
	void StopQueue(Queue* queue);

public:
	QueueTimeManager(QObject *parent);
	~QueueTimeManager();
};
