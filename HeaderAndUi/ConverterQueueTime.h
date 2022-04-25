#pragma once

#include <QObject>

class ConverterQueueTime : public QObject
{
	Q_OBJECT

public:
	static bool IsNameOfDaysOfWeek(QString day);
	static bool IsNumberOfDays(QString day);


public:
	ConverterQueueTime(QObject *parent);
	~ConverterQueueTime();
};
