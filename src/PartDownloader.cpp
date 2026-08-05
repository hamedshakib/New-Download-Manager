#include "HeaderAndUi/PartDownloader.h"

PartDownloader::PartDownloader(QObject *parent)
	: QObject(parent)
{
}

PartDownloader::~PartDownloader()
{
	qDebug() << "delete PartDownloader";

	if (m_manager != nullptr)
	{
		m_manager->deleteLater();
		m_manager = nullptr;
	}

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
	//This object lives on its PartDownload thread, so the network manager is
	//created on / moved to that thread too.
	if (m_manager == nullptr)
	{
		m_manager = new QNetworkAccessManager();
		m_manager->moveToThread(this->thread());
	}
}

bool PartDownloader::StartRequest(const QUrl& url, const QString& username, const QString& password, qint64 startByte, qint64 endByte)
{
	if (m_manager == nullptr)
	{
		m_manager = new QNetworkAccessManager();
		m_manager->moveToThread(this->thread());
	}

	QUrl u = url;
	if (!username.isEmpty() && !password.isEmpty())
	{
		u.setUserName(username);
		u.setPassword(password);
	}

	QNetworkRequest request;
	request.setUrl(u);
	request.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);

	QString rangeBytes = QString("bytes=%1-%2").arg(startByte).arg(endByte);
	request.setRawHeader("Range", rangeBytes.toUtf8());

	QNetworkReply* newReply = m_manager->get(request);
	return ProcessSetNewReply(newReply);
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

	//QMutexLocker is exception-safe (unlike manual lock/unlock).
	QMutexLocker guard(&mutex);
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
	partDownload->AddToLastDownloadedByte(ReadedBytes);
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
	//This is called from the DownloadControl thread while this object lives on
	//the PartDownload thread, so run the actual read on our own thread and block
	//until it returns. Safe because the part thread never blocks on us: its
	//signals are delivered to the download thread via queued connections.
	qint64 ReadedBytes = 0;
	QMetaObject::invokeMethod(this, [this, maxReadBytes, &ReadedBytes]() {
		ReadedBytes = ReadBytes(maxReadBytes);
		if (partDownloaderStatus == PartDownloaderStatus::PartDownloadFinishedReciveBytes)
		{
			CheckFinishedPartDownloader();
		}
	}, Qt::BlockingQueuedConnection);
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
	if (reply == nullptr)
	{
		return false;
	}

	if (this->reply != nullptr && this->reply != reply)
	{
		disconnect(this->reply, &QNetworkReply::readyRead, this, &PartDownloader::ReadyRead);
		disconnect(this->reply, &QNetworkReply::finished, this, &PartDownloader::CheckFinishedRecivedBytes);
		this->reply->deleteLater();
	}
	this->reply = reply;
	//UniqueConnection prevents duplicate connections regardless of how often this is called.
	connect(reply, &QNetworkReply::readyRead, this, &PartDownloader::ReadyRead, Qt::ConnectionType::UniqueConnection);
	connect(reply, &QNetworkReply::finished, this, &PartDownloader::CheckFinishedRecivedBytes);
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
	if (this->is_SpeedLimit == is_SpeedLimited)
	{
		return true;
	}
	else
	{
		this->is_SpeedLimit = is_SpeedLimited;
		if (is_SpeedLimited == false)
		{
			//Not limited: read as soon as bytes arrive.
			connect(reply, &QNetworkReply::readyRead, this, &PartDownloader::ReadyRead, Qt::ConnectionType::UniqueConnection);
			if (reply != nullptr && reply->bytesAvailable() > 0)
			{
				ReadyRead();
			}
		}
		else
		{
			//Speed limited: reading is driven by DownloadForControlSpeed instead of readyRead.
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
	//reply lives on this object's thread; query it there (see DownloadByteInSpeedControl).
	bool available = false;
	QMetaObject::invokeMethod(this, [this, &available]() {
		available = (reply != nullptr && reply->bytesAvailable() > 0);
	}, Qt::BlockingQueuedConnection);
	return available;
}

