#include "HeaderAndUi/DateTimeManager.h"

QString DateTimeManager::DateTimeFormat = "yyyy/MM/dd hh:mm:ss";

QDateTime DateTimeManager::GetCurrentDateTime()
{
	return QDateTime::currentDateTime();
}

QDateTime DateTimeManager::GetDateTimeFromString(QString dateTime)
{
	QDateTime dateAndTime;
	return dateAndTime.fromString(dateTime, DateTimeFormat);
}

QString DateTimeManager::ConvertDataTimeToString(QDateTime dateTime)
{
	return dateTime.toString(DateTimeFormat);
}
