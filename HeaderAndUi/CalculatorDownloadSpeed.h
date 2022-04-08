#pragma once

#include <QObject>
#include "qdatetime.h"

class CalculatorDownloadSpeed : public QObject
{
	Q_OBJECT

private:
	qint64 downloadedByteAtLastTime = 0;
	qint64 LastSpeedBytesPerMillisecond=0;

public:
	//qint64 CalculateDownloadSpeed(qint64 NowDownloadedbyte,qint64 SpentedTime_Millisecond);
	qint64 CalculateDownloadSpeed(qint64 NumberOfBytesThatDownloadedInLastPeriod, qint64 SpentedTime_Millisecond);
	QString GetSpeedOfDownloadInFormOfString();

public:
	CalculatorDownloadSpeed(QObject *parent=nullptr);
	~CalculatorDownloadSpeed();
};
