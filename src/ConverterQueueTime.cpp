#include "HeaderAndUi/ConverterQueueTime.h"

ConverterQueueTime::ConverterQueueTime(QObject *parent)
	: QObject(parent)
{
}

ConverterQueueTime::~ConverterQueueTime()
{
}

bool ConverterQueueTime::IsNameOfDaysOfWeek(QString day)
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

bool ConverterQueueTime::IsNumberOfDays(QString day)
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
