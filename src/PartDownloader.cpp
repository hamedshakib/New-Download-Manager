#include "HeaderAndUi/PartDownloader.h"

PartDownloader::PartDownloader(QObject *parent)
	: QObject(parent)
{
}

PartDownloader::~PartDownloader()
{
	qDebug() << "delete PartDownloader";
}

bool PartDownloader::Set_PartDownload(PartDownload* partDownload)
{
	this->partDownload = partDownload;
	return true;
}

qint64 PartDownloader::ReadReadybytes(qint64 bytes)
{
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



	
	if (DownloadFileWriter::WriteDownloadToFile(byteArray, partDownload->PartDownloadFile))
	{

		if (Is_PartDownloadEndInBuffer && reply->bytesAvailable() == 0)
		{
			this->partDownload->PartDownloadFile->close();
			emit Finished();
		}



		return ReadedBytes;
	}

	return true;
}

bool PartDownloader::Set_NetworkReply(QNetworkReply* reply)
{
	this->reply = reply;
	connect(reply, &QNetworkReply::finished, this, [&]() {
		Is_PartDownloadEndInBuffer = true;});

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

