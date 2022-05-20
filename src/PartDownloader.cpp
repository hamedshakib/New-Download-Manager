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

bool PartDownloader::Set_PartDownload(PartDownload* partDownload)
{
	this->partDownload = partDownload;
	return true;
}

qint64 PartDownloader::ReadReadybytes(qint64 bytes)
{
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

