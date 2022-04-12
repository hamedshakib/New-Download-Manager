#pragma once

#include <QObject>
#include "qdatetime.h"


class CalculatorDownload : public QObject
{
	Q_OBJECT

private:
	qint64 downloadedByteAtLastTime = 0;
	qint64 LastSpeedBytesPerMillisecond=0;
	QTime RemainedTimeToFinish{};

public:
	//qint64 CalculateDownloadSpeed(qint64 NowDownloadedbyte,qint64 SpentedTime_Millisecond);
	qint64 CalculateDownloadSpeed(qint64 NumberOfBytesThatDownloadedInLastPeriod, qint64 SpentedTime_Millisecond);
	QString GetSpeedOfDownloadInFormOfString();
	QString GetTimeLeftOfDownloadInFormOfString(qint64 NumberRemainedBytes);
	QString getStatusForTable(qint64 DownloadedSize, qint64 SizeDownload);

public:
	CalculatorDownload(QObject *parent=nullptr);
	~CalculatorDownload();
};
