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

/*
bool PartDownloader::Set_PartDownload(PartDownload* partDownload)
{
	this->partDownload = partDownload;
	return true;
}

qint64 PartDownloader::ReadReadybytes(qint64 bytes)
{
	qDebug() << "T";
	int b = 1 + 2;
	qDebug() <<"++:" << QThread::currentThread()->objectName();
	qint64 ReadedBytes = 0;
	QByteArray byteArray;

	if (bytes == -1 || reply->bytesAvailable() <= bytes)
	{
		ReadedBytes = reply->bytesAvailable();
		byteArray = reply->read(ReadedBytes);
		//qDebug() << "ReadedBytes:" << ReadedBytes;
	}
	else
	{
		byteArray = reply->read(bytes);
		ReadedBytes = bytes;
	}

	if (ReadedBytes > 0)
		Is_Downloading = true;
	
	if (downloadFileWriter->WriteDownloadToFile(byteArray, partDownload->PartDownloadFile))
	{

		if (Is_PartDownloadEndInBuffer && reply->bytesAvailable() == 0)
		{
			if (Is_FinishedPartDownload)
			{
				this->partDownload->PartDownloadFile->close();
				emit Finished();
			}
			else
			{
				delete reply;
				//reply->deleteLater();
				reply = nullptr;
				emit Paused();
			}
		}


		if (Is_DownloadItSelf)
		{
			emit DownloadedBytes(ReadedBytes);
		}

		return ReadedBytes;
	}
	return true;
}

bool PartDownloader::Set_NetworkReply(QNetworkReply* reply)
{
	this->reply = reply;

	return true;
}

PartDownload* PartDownloader::Get_PartDownload()
{
	return partDownload;
}

void PartDownloader::AddByteToLastDownloadedByte(qint64 NumberOfBytes)
{
	partDownload->LastDownloadedByte += NumberOfBytes;
}

void PartDownloader::Resume()
{
	if (partDownload->LastDownloadedByte >= partDownload->end_byte)
	{
		Is_FinishedPartDownload = true;
		return;
	}
	is_Paused = false;
	Is_PartDownloadEndInBuffer = false;
	connect(reply, &QNetworkReply::finished, this, &PartDownloader::ProcessApplyPauseOrFinishedPartDownloader);
}

void PartDownloader::Pause()
{
	is_Paused = true;
	Is_Downloading = false;
	if (reply != nullptr)
	{
		reply->abort();
		disconnect(reply, &QNetworkReply::finished, this, &PartDownloader::ProcessApplyPauseOrFinishedPartDownloader);
	}
}

void PartDownloader::ProcessApplyPauseOrFinishedPartDownloader()
{
	if (!Is_Downloading)
		return;
	Is_PartDownloadEndInBuffer = true;
	if (!is_Paused)
	{
		Is_FinishedPartDownload = true;
	}
}

bool PartDownloader::Set_DownloadFileWriter(DownloadFileWriter* downloadFileWriter)
{
	this->downloadFileWriter = downloadFileWriter;
	return true;
}

bool PartDownloader::StartDownloadItSelf()
{
	Is_DownloadItSelf = true;
	connect(this, &PartDownloader::DownloadedBytes, this, &PartDownloader::ProcessDownloadItSelf);
	return true;
}

bool PartDownloader::StopDownloadItSelf()
{
	Is_DownloadItSelf = false;
	return true;
}

void PartDownloader::ProcessDownloadItSelf(qint64 downloadedInLastPeriod)
{
	AddByteToLastDownloadedByte(downloadedInLastPeriod);
	qDebug() << "Downloaded:" << downloadedInLastPeriod;
	if (Is_Downloading)
	{
		if (Is_DownloadItSelf)
		{
			if(!Is_FinishedPartDownload && this)
				ReadReadybytes();
		}
	}
}
*/

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
		qDebug() << "Strange byte:" << reply->bytesAvailable();
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
		qDebug() << "Finsih recive byte PartDownload emit";
		emit FinishedRecivedBytes();
	}
}

void PartDownloader::CheckFinishedPartDownloader()
{
	if (partDownloaderStatus == PartDownloaderStatus::PartDownloadFinishedReciveBytes)
	{
		if (partDownload->IsPartDownloadFinished())
		{
			qDebug() << "Finsih PartDownload emit";
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

