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

void PartDownloader::initPartDownlolader(PartDownload* partDownload, QNetworkReply* reply, qint64 readBytesEachTimes)
{
	this->partDownload = partDownload;
	this->reply = reply;
	qDebug()<<"range partDownload:"<<partDownload->start_byte<<"-" <<partDownload->end_byte<<"  " << this->reply->bytesAvailable();
	this->downloadFileWriter = new DownloadFileWriter();
	downloadFileWriter->moveToThread(this->thread());
	connect(reply, &QNetworkReply::readyRead, this, &PartDownloader::ReadyRead);
	connect(reply, &QNetworkReply::finished, this, &PartDownloader::CheckFinished);
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

void PartDownloader::ReadyRead()
{
	if (!this->is_Downloading)
	{
		return;
	}

	qint64 ReadedBytes = 0;
	QByteArray byteArray;
	//if (bytes > 0)
	//{
		byteArray = reply->read(5000000);
		ReadedBytes =byteArray.size();
	//}
	downloadFileWriter->WriteDownloadToFile(byteArray,partDownload->PartDownloadFile);
	qDebug() << "Downloaded " << ReadedBytes << "Bytes From Thread " << QThread::currentThread()->objectName();
	emit DownloadedBytes(ReadedBytes);
}

PartDownload* PartDownloader::Get_PartDownload()
{
	return this->partDownload;
}

bool PartDownloader::ProcessSetNewReply(QNetworkReply* reply)
{
	if (this->reply != reply)
	{
		this->reply->deleteLater();
		this->reply = reply;
		connect(reply, &QNetworkReply::readyRead, this, &PartDownloader::ReadyRead, Qt::ConnectionType::QueuedConnection);
		connect(reply, &QNetworkReply::finished, this, &PartDownloader::CheckFinished);
	}
	return true;
}

void PartDownloader::CheckFinished()
{
	if (partDownloaderStatus == PartDownloaderStatus::PartDownloadDownloading)
	{
		emit Finished();
	}
}

