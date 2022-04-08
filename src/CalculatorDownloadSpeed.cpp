#include "HeaderAndUi/CalculatorDownloadSpeed.h"

CalculatorDownloadSpeed::CalculatorDownloadSpeed(QObject *parent)
	: QObject(parent)
{
}

CalculatorDownloadSpeed::~CalculatorDownloadSpeed()
{
}

qint64 CalculatorDownloadSpeed::CalculateDownloadSpeed(qint64 NowDownloadedbyte,qint64 SpentedTime_Millisecond)
{
	//Bytes per Millisecond 

	qint64 DiffrencebytesDownloaded = NowDownloadedbyte - downloadedByteAtLastTime;
	downloadedByteAtLastTime = NowDownloadedbyte;
	if (SpentedTime_Millisecond == 0)
	{
		return 0;
	}
	LastSpeedBytesPerMillisecond = (DiffrencebytesDownloaded / SpentedTime_Millisecond);
	return LastSpeedBytesPerMillisecond;
}

QString CalculatorDownloadSpeed::GetSpeedOfDownloadInFormOfString()
{
	//bytes per second
	QString SpeedInString;
	qint64 Speed_BytesPerSecond =LastSpeedBytesPerMillisecond * 1000;
	if (Speed_BytesPerSecond < 1024)
	{
		SpeedInString = QString::number(Speed_BytesPerSecond) + " Byte/sec";
	}
	else if (Speed_BytesPerSecond >= 1024 && Speed_BytesPerSecond < 1024 * 1024)
	{
		SpeedInString = QString::number(Speed_BytesPerSecond/1024) + " KByte/sec";
	}
	else if (Speed_BytesPerSecond >= 1024 * 1024 && Speed_BytesPerSecond < 1024 * 1024 * 1024)
	{
		SpeedInString = QString::number(Speed_BytesPerSecond / 1024*1024) + " MByte/sec";
	}
	else if (Speed_BytesPerSecond >= 1024 * 1024 * 1024 && Speed_BytesPerSecond < 1024 * 1024 * 1024 * 1024)
	{
		SpeedInString = QString::number(Speed_BytesPerSecond / 1024 * 1024 *1024) + " GByte/sec";
	}
	return SpeedInString;
}
