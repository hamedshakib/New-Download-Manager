#include "HeaderAndUi/PartDownloader.h"

// Retry configuration
const int MAX_RETRY_ATTEMPTS = 3;
const int RETRY_DELAY_MS = 1000;

PartDownloader::PartDownloader(QObject *parent)
	: QObject(parent)
{
    // Initialize retry counter
    retryCount = 0;
}

PartDownloader::~PartDownloader()
{
	qDebug() << "delete PartDownloader";

	// Clean up downloadFileWriter
	if (downloadFileWriter != nullptr) {
		downloadFileWriter->deleteLater();
		downloadFileWriter = nullptr;
	}

	if (reply != nullptr)
	{
		// Disconnect all signals before deleting reply
		disconnect(reply, &QNetworkReply::readyRead, this, &PartDownloader::ReadyRead);
		disconnect(reply, &QNetworkReply::finished, this, &PartDownloader::CheckFinishedRecivedBytes);
		disconnect(reply, &QNetworkReply::errorOccurred, this, &PartDownloader::HandleNetworkError);
		
		reply->deleteLater();
		reply = nullptr;
	}
}

void PartDownloader::initPartDownlolader(PartDownload* partDownload, qint64 readBytesEachTimes)
{
    // Move timer to the correct thread first
    timer.moveToThread(this->thread());
    
    this->partDownload = partDownload;
    this->downloadFileWriter = new DownloadFileWriter();
    downloadFileWriter->moveToThread(this->thread());
    
    // Validate and set read bytes each time
    // Maximum reasonable buffer size is 1MB (1048576 bytes)
    // Minimum is 4KB (4096 bytes) for efficient I/O
    const qint64 MAX_BUFFER_SIZE = 1048576;  // 1MB
    const qint64 MIN_BUFFER_SIZE = 4096;     // 4KB
    
    if (readBytesEachTimes <= 0) {
        this->readBytesEachTimes = MAX_BUFFER_SIZE;  // Default to 1MB
    } else if (readBytesEachTimes > MAX_BUFFER_SIZE) {
        this->readBytesEachTimes = MAX_BUFFER_SIZE;  // Cap at 1MB
    } else if (readBytesEachTimes < MIN_BUFFER_SIZE) {
        this->readBytesEachTimes = MIN_BUFFER_SIZE;  // Minimum 4KB
    } else {
        this->readBytesEachTimes = readBytesEachTimes;
    }
    
    qDebug() << "PartDownloader buffer size set to:" << this->readBytesEachTimes << "bytes";
}

void PartDownloader::Resume(bool ItSelf)
{
	is_Downloading.store(true);
	partDownloaderStatus = PartDownloaderStatus::PartDownloadDownloading;
	qDebug() << "range partDownload:" << partDownload->start_byte << "-" << partDownload->end_byte;

	if (reply != nullptr) {
		qDebug() << this->reply->bytesAvailable();
		if (ItSelf)
		{
			if (reply->bytesAvailable() > 0)
			{
				emit ReadyRead();
			}
		}
	}
}

void PartDownloader::Pause()
{
	is_Downloading.store(false);
	partDownloaderStatus = PartDownloaderStatus::PartDownloadPaused;
	
	if (reply) {
		reply->abort();
	} else {
		qWarning() << "PartDownloader::Pause() called but reply is null";
	}
}

qint64 PartDownloader::ReadBytes(qint64 bytes)
{
	// Check is_Downloading atomically before proceeding
	if (!is_Downloading.load())
	{
		return 0;
	}

	// Lock mutex for thread-safe access to shared resources
	mutex.lock();
	
	qint64 ReadedBytes = 0;
	QByteArray byteArray;
	
	// Check if reply is still valid
	if (!reply) {
		qWarning() << "ReadBytes called but reply is null";
		mutex.unlock();
		return 0;
	}
	
	// Double-check download state after acquiring lock
	if (!is_Downloading.load()) {
		qWarning() << "ReadBytes called but download is not active";
		mutex.unlock();
		return 0;
	}
	
	if (bytes > 0)
	{
		byteArray = reply->read(bytes);
		ReadedBytes = byteArray.size();
	}
	
	// Check downloadFileWriter validity before use
	if (ReadedBytes > 0) {
		if (downloadFileWriter) {
			downloadFileWriter->WriteDownloadToFile(byteArray, partDownload->PartDownloadFile, false);
		}
	}
	
	qDebug() << "Downloaded " << ReadedBytes << "Bytes From Thread " << QThread::currentThread()->objectName();
	partDownload->LastDownloadedByte += ReadedBytes;
    mutex.unlock();
    
    // Emit signal outside of lock to prevent potential deadlocks
    if (!is_SpeedLimit.load())
    {
        emit DownloadedBytes(ReadedBytes);
    }
    return ReadedBytes;
}

void PartDownloader::ReadyRead()
{
	if (!is_SpeedLimit.load())
	{
		// Safety check: ensure reply is valid before reading
		if (!reply) {
			qWarning() << "ReadyRead called but reply is null";
			return;
		}
		
		// Check if download is still active
		if (!is_Downloading.load()) {
			qWarning() << "ReadyRead called but download is not active";
			return;
		}
		
		// Check for available bytes with timeout protection
		qint64 availableBytes = reply->bytesAvailable();
		if (availableBytes <= 0) {
			// No bytes available yet, wait for next signal
			return;
		}
		
		// Use minimum of available bytes and configured buffer size to prevent
		// reading too much data at once which could cause memory issues
		qint64 readSize = qMin(availableBytes, this->readBytesEachTimes);
		
		// Safety check: ensure readSize is reasonable
		if (readSize > 1048576) {  // 1MB max per read
			readSize = 1048576;
		}
		
		ReadBytes(readSize);
		
		if (partDownloaderStatus == PartDownloaderStatus::PartDownloadFinishedReciveBytes)
		{
			CheckFinishedPartDownloader();
		}
	}
}

qint64 PartDownloader::DownloadByteInSpeedControl(qint64 maxReadBytes)
{
	qint64 ReadedBytes = ReadBytes(maxReadBytes);
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

void PartDownloader::HandleNetworkError(QNetworkReply::NetworkError error)
{
    // Use mutex to ensure thread-safe access
    QMutexLocker locker(&mutex);
    
    if (!is_Downloading.load() || partDownloaderStatus == PartDownloaderStatus::PartDownloadPaused) {
        return;  // Don't retry if download is paused or stopped
    }
    
    if (!reply) {
        qWarning() << "Network error but reply is null - cannot retry";
        return;
    }
    
    QString errorString = reply->errorString();
    retryCount++;
    
    qWarning() << "Network error" << static_cast<int>(error) << ":" << errorString;
    qWarning() << "Retry attempt" << retryCount << "/" << MAX_RETRY_ATTEMPTS;
    
    emit DownloadError(errorString, retryCount);
    
    if (retryCount >= MAX_RETRY_ATTEMPTS) {
        qCritical() << "Max retry attempts reached. Download will be paused.";
        Pause();
        return;
    }
    
    // Store current state before scheduling retry
    // The mutex is locked during this entire method, so the state is safe
    bool canRetry = true;
    
    // Schedule retry after delay
    // Note: The lambda captures by value where needed, and we rely on the
    // mutex-protected state check in Resume to ensure thread safety
    QTimer::singleShot(RETRY_DELAY_MS, this, [this, errorString]() {
        // Check if we should retry - use mutex for thread safety
        QMutexLocker locker(&mutex);
        
        if (!this->reply) {
            qWarning() << "Cannot retry - reply no longer valid";
            return;
        }
        
        if (!this->is_Downloading.load()) {
            qWarning() << "Cannot retry - download is not active";
            return;
        }
        
        // Reset retry status and resume
        this->Resume(false);  // Don't emit ReadyRead immediately
    });
}

bool PartDownloader::ProcessSetNewReply(QNetworkReply* reply)
{
	if (this->reply != reply)
	{
		if (this->reply != nullptr) {
			disconnect(this->reply, &QNetworkReply::readyRead, this, &PartDownloader::ReadyRead);
			disconnect(this->reply, &QNetworkReply::finished, this, &PartDownloader::CheckFinishedRecivedBytes);
			disconnect(this->reply, &QNetworkReply::errorOccurred, this, &PartDownloader::HandleNetworkError);
			this->reply->deleteLater();
		}
		this->reply = reply;
        
        // Reset retry counter when creating new reply
        retryCount = 0;
        
		if (QObject::receivers("readyRead") == 0)
		{
			connect(reply, &QNetworkReply::readyRead, this, &PartDownloader::ReadyRead, Qt::ConnectionType::UniqueConnection);
		}
		// Use default connection type instead of Qt::DirectConnection to avoid thread safety issues
		connect(reply, &QNetworkReply::finished, this, &PartDownloader::CheckFinishedRecivedBytes);
        
        // Connect error handling
        connect(reply, &QNetworkReply::errorOccurred, this, &PartDownloader::HandleNetworkError, Qt::ConnectionType::UniqueConnection);
	}
	return true;
}

void PartDownloader::CheckFinishedRecivedBytes()
{
	if (partDownloaderStatus == PartDownloaderStatus::PartDownloadDownloading)
	{
		partDownloaderStatus = PartDownloaderStatus::PartDownloadFinishedReciveBytes;
		qDebug() << "Finsih receive bytes PartDownload";
		emit FinishedReceivedBytes();
	}
}

void PartDownloader::CheckFinishedPartDownloader()
{
	if (partDownloaderStatus == PartDownloaderStatus::PartDownloadFinishedReciveBytes)
	{
		if (partDownload && partDownload->IsPartDownloadFinished())
		{
			qDebug() << "Finsih PartDownload";
			emit Finished();
		}
	}
}

bool PartDownloader::SetSpeedLimited(bool is_SpeedLimited)
{
	//qDebug() << QObject::receivers("readyRead");
	if (this->is_SpeedLimit.load() == is_SpeedLimited)
	{
		return true;
	}
	else
	{
		is_SpeedLimit.store(is_SpeedLimited);
		if (is_SpeedLimited == false)
		{
			//qDebug() <<"Count ReadyRead signal" << receivers("ReadyRead");
			if (QObject::receivers("readyRead") == 0)
			{
				if (reply) {
					connect(reply, &QNetworkReply::readyRead, this, &PartDownloader::ReadyRead, Qt::ConnectionType::UniqueConnection);
					if (reply->bytesAvailable() > 0)
						ReadyRead();
				}
			}
		}
		else
		{
			//Speed Limitted
			if (reply) {
				disconnect(reply, &QNetworkReply::readyRead, this, &PartDownloader::ReadyRead);
			}
		}
		return true;
	}
}

bool PartDownloader::IsSpeedLimiter()
{
	return is_SpeedLimit.load();
}

bool PartDownloader::IsAvaliableByteForRead()
{
	if (!reply) {
		return false;
	}
	return reply->bytesAvailable() > 0;
}