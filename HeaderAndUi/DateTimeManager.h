#pragma once

#include <QObject>
#include "qdatetime.h"
#include "qdebug.h"

class DateTimeManager : public QObject
{
	Q_OBJECT

public:
	static QDateTime GetCurrentDateTime();
	static QDateTime GetDateTimeFromString(QString dateTime);
	static QString ConvertDataTimeToString(QDateTime dateTime);


public:
	static QString DateTimeFormat;
};