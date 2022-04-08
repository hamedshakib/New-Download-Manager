#pragma once

#include <QObject>
#include "qdebug.h"

class ConverterSizeToSuitableString : public QObject
{
	Q_OBJECT
public:
	static QString ConvertSizeToSuitableString(qint64 size);
};
