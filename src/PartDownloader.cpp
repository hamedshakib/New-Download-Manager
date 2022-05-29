#include "HeaderAndUi/PartDownloader.h"

PartDownloader::PartDownloader(QObject *parent)
	: QObject(parent)
{
}

PartDownloader::~PartDownloader()
{
	qDebug() << "delete PartDownloader";

	if (reply != nullptr)
	{
		reply->deleteLater();
		reply = nullptr;
	}
}

void PartDownloader::initPartDownlolader(PartDownload* partDownload,qint64 readBytesEachTimes)
{
	this->partDownload = partDownload;
	this->downloadFileWriter = new DownloadFileWriter();
	downloadFileWriter->moveToThread(this->thread());
	readBytesEachTimes = 50000000;
}

void PartDownloader::Resume(bool ItSelf)
{
	this->is_Downloading = true;
	partDownloaderStatus = PartDownloaderStatus::PartDownloadDownloading;
	qDebug() << "range partDownload:" << partDownload->start_byte << "-" << partDownload->end_byte;

	qDebug() << this->reply->bytesAvailable();
	if (ItSelf)
	{
		if (reply->bytesAvailable() > 0)
		{
			emit ReadyRead();
		}
	}
}

void PartDownloader::Pause()
{
	this->is_Downloading = false;
	partDownloaderStatus = PartDownloaderStatus::PartDownloadPaused;
	this->reply->abort();
}

qint64 PartDownloader::ReadBytes(qint64 bytes)
{
	if (!this->is_Downloading)
	{
		return 0;
	}


	mutex.lock();
	qint64 ReadedBytes = 0;
	QByteArray byteArray;
	if (bytes > 0)
	{
		byteArray = reply->read(bytes);
		ReadedBytes = byteArray.size();
	}
	if (ReadedBytes > 0)
	{
		downloadFileWriter->WriteDownloadToFile(byteArray, partDownload->PartDownloadFile);
	}
	qDebug() << "Downloaded " << ReadedBytes << "Bytes From Thread " << QThread::currentThread()->objectName();
	partDownload->LastDownloadedByte += ReadedBytes;
	mutex.unlock();
	if (!is_SpeedLimit)
	{
		emit DownloadedBytes(ReadedBytes);
	}
	return ReadedBytes;
}

void PartDownloader::ReadyRead()
{
	if (!is_SpeedLimit)
	{
		ReadBytes(reply->bytesAvailable());
		if (partDownloaderStatus == PartDownloaderStatus::PartDownloadFinishedReciveBytes)
		{
			CheckFinishedPartDownloader();
		}
	}
}

qint64 PartDownloader::DownloadByteInSpeedControl(qint64 maxReadBytes)
{

	qint64 ReadedBytes= ReadBytes(maxReadBytes);
	if (partDownloaderStatus == PartDownloaderStatus::PartDownloadFinishedReciveBytes)
	{
		CheckFinishedPartDownloader();
	}
	return ReadedBytes;
}

PartDownload* PartDownloader::Get_PartDownload()
{
	if (this->partDownload != nullptr)
		return this->partDownload;
	else
		return nullptr;
}

bool PartDownloader::ProcessSetNewReply(QNetworkReply* reply)
{
	if (this->reply != reply)
	{
		disconnect(reply, &QNetworkReply::readyRead, this, &PartDownloader::ReadyRead);
		disconnect(reply, &QNetworkReply::finished, this, &PartDownloader::CheckFinishedRecivedBytes);
		this->reply->deleteLater();
		this->reply = reply;
		if (QObject::receivers("readyRead") == 0)
		{
			connect(reply, &QNetworkReply::readyRead, this, &PartDownloader::ReadyRead, Qt::ConnectionType::UniqueConnection);
		}
		connect(reply, &QNetworkReply::finished, this, &PartDownloader::CheckFinishedRecivedBytes,Qt::DirectConnection);
	}
	return true;
}

void PartDownloader::CheckFinishedRecivedBytes()
{
	if (partDownloaderStatus == PartDownloaderStatus::PartDownloadDownloading)
	{
		partDownloaderStatus = PartDownloaderStatus::PartDownloadFinishedReciveBytes;
		qDebug() << "Finsih receive bytes PartDownload";
		emit FinishedRecivedBytes();
	}
}

void PartDownloader::CheckFinishedPartDownloader()
{
	if (partDownloaderStatus == PartDownloaderStatus::PartDownloadFinishedReciveBytes)
	{
		if (partDownload->IsPartDownloadFinished())
		{
			qDebug() << "Finsih PartDownload";
			emit Finished();
		}
	}
}

bool PartDownloader::SetSpeedLimited(bool is_SpeedLimited)
{
	//qDebug() << QObject::receivers("readyRead");
	if (this->is_SpeedLimit == is_SpeedLimited)
	{
		return true;
	}
	else
	{
		this->is_SpeedLimit = is_SpeedLimited;
		if (is_SpeedLimited == false)
		{
			//qDebug() <<"Count ReadyRead signal" << receivers("ReadyRead");
			if (QObject::receivers("readyRead") == 0)
			{
				connect(reply, &QNetworkReply::readyRead, this, &PartDownloader::ReadyRead, Qt::ConnectionType::UniqueConnection);
				if (reply->bytesAvailable() > 0)
					ReadyRead();
				}
			}
		else
		{
			//Speed Limitted
			disconnect(reply, &QNetworkReply::readyRead, this, &PartDownloader::ReadyRead);
		}
		return true;
		
	}
}

bool PartDownloader::IsSpeedLimiter()
{
	return this->is_SpeedLimit;
}

bool PartDownloader::IsAvaliableByteForRead()
{
	return reply->bytesAvailable()>0 ? true:false;
}

