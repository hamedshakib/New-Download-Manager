#include "HeaderAndUi/PartDownloader.h"
#include <QDebug>
#include <QThread>

PartDownloader::PartDownloader(QObject* parent)
    : QObject(parent)
{}

PartDownloader::~PartDownloader()
{
    qDebug() << "delete PartDownloader on thread:" << QThread::currentThread()->objectName();

    if (m_manager) {
        m_manager->deleteLater();
        m_manager = nullptr;
    }

    if (reply) {
        reply->deleteLater();
        reply = nullptr;
    }

    if (downloadFileWriter) {
        downloadFileWriter->deleteLater();
        downloadFileWriter = nullptr;
    }
}

void PartDownloader::InitPartDownloader(PartDownload* partDownload, qint64 readBytesEachTimeCount)
{
    Q_UNUSED(readBytesEachTimeCount);
    this->partDownload = partDownload;

    if (!downloadFileWriter) {
        downloadFileWriter = new DownloadFileWriter(this);
    }

    if (!m_manager) {
        m_manager = new QNetworkAccessManager(this);
    }
}

bool PartDownloader::StartRequest(const QUrl& url, const QString& username, const QString& password, qint64 startByte, qint64 endByte)
{
    if (!m_manager) {
        m_manager = new QNetworkAccessManager(this);
    }

    if (!partDownload) return false;

    partDownload->UpdatePartDownloadLastDownloadedByte();
    if (partDownload->IsPartDownloadFinished()) {
        emit PartDownloaderFinished();
        return true;
    }

    startByte = partDownload->GetLastDownloadedByte() + 1;

    QUrl u = url;
    if (!username.isEmpty() && !password.isEmpty()) {
        u.setUserName(username);
        u.setPassword(password);
    }

    QNetworkRequest request(u);
    request.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);

    QString rangeBytes = QString("bytes=%1-%2").arg(startByte).arg(endByte);
    request.setRawHeader("Range", rangeBytes.toUtf8());

    QNetworkReply* newReply = m_manager->get(request);
    return ProcessSetNewReply(newReply);
}

void PartDownloader::Resume(bool itSelfDownloading)
{
    partDownloaderStatus = PartDownloaderStatus::Downloading;
    if (itSelfDownloading && reply && reply->bytesAvailable() > 0) {
        emit ReadyRead();
    }
}

void PartDownloader::Pause()
{
    partDownloaderStatus = PartDownloaderStatus::Paused;
    if (reply) {
        reply->abort();
    }
}

qint64 PartDownloader::ReadBytes(qint64 byteCount)
{
    if (!reply || (partDownloaderStatus != PartDownloaderStatus::Downloading &&
        partDownloaderStatus != PartDownloaderStatus::FinishedReceiveBytes)) {
        return 0;
    }

    QMutexLocker guard(&mutex);
    QByteArray byteArray;

    if (byteCount > 0) {
        byteArray = reply->read(byteCount);
    }
    else {
        byteArray = reply->readAll(); // خواندن تمام داده‌های باقی‌مانده
    }

    qint64 numberOfBytesRead = byteArray.size();
    if (numberOfBytesRead > 0 && downloadFileWriter && partDownload) {
        downloadFileWriter->WriteDownloadToFile(byteArray, partDownload->PartDownloadFile);
        partDownload->AddToLastDownloadedByte(numberOfBytesRead);
    }

    if (!is_SpeedLimit && numberOfBytesRead > 0) {
        emit DownloadedByteCount(numberOfBytesRead);
    }
    return numberOfBytesRead;
}

void PartDownloader::ReadyRead()
{
    if (!is_SpeedLimit && reply) {
        ReadBytes(reply->bytesAvailable());
        if (partDownloaderStatus == PartDownloaderStatus::FinishedReceiveBytes) {
            CheckFinishedPartDownloader();
        }
    }
}

qint64 PartDownloader::DownloadByteInSpeedControl(qint64 maxReadBytes)
{
    qint64 numberOfBytesRead = 0;
    auto readTask = [this, maxReadBytes, &numberOfBytesRead]() {
        numberOfBytesRead = ReadBytes(maxReadBytes);
        if (partDownloaderStatus == PartDownloaderStatus::FinishedReceiveBytes) {
            CheckFinishedPartDownloader();
        }
        };

    if (QThread::currentThread() == this->thread()) {
        readTask();
    }
    else {
        QMetaObject::invokeMethod(this, readTask, Qt::BlockingQueuedConnection);
    }
    return numberOfBytesRead;
}

PartDownload* PartDownloader::GetPartDownload()
{
    return this->partDownload;
}

bool PartDownloader::ProcessSetNewReply(QNetworkReply* newReply)
{
    if (!newReply) return false;

    if (this->reply && this->reply != newReply) {
        disconnect(this->reply, nullptr, this, nullptr);
        this->reply->deleteLater();
    }

    this->reply = newReply;

    // تنظیم بافر روی ۱۶ کیلوبایت جهت فعال‌سازی سریع TCP Backpressure در لایه شبکه
    this->reply->setReadBufferSize(16 * 1024);

    if (!is_SpeedLimit) {
        connect(reply, &QNetworkReply::readyRead, this, &PartDownloader::ReadyRead, Qt::UniqueConnection);
    }
    connect(reply, &QNetworkReply::finished, this, &PartDownloader::CheckFinishedReceivedBytes, Qt::UniqueConnection);
    return true;
}

void PartDownloader::CheckFinishedReceivedBytes()
{
    if (partDownloaderStatus == PartDownloaderStatus::Downloading) {
        partDownloaderStatus = PartDownloaderStatus::FinishedReceiveBytes;

        if (reply && reply->bytesAvailable() > 0) {
            qint64 remainingBytes = reply->bytesAvailable();
            ReadBytes(remainingBytes);

            if (is_SpeedLimit && remainingBytes > 0) {
                emit DownloadedByteCount(remainingBytes);
            }
        }

        qDebug() << "Finish receive bytes PartDownload";
        emit FinishedReceivedBytes();
        CheckFinishedPartDownloader();
    }
}

void PartDownloader::CheckFinishedPartDownloader()
{
    if (partDownloaderStatus == PartDownloaderStatus::FinishedReceiveBytes) {
        if (partDownload && partDownload->IsPartDownloadFinished()) {
            qDebug() << "Finish PartDownload Completed Successfully";
            emit PartDownloaderFinished();
        }
    }
}

bool PartDownloader::SetSpeedLimited(bool is_SpeedLimited)
{
    if (this->is_SpeedLimit == is_SpeedLimited) return true;

    this->is_SpeedLimit = is_SpeedLimited;
    if (!reply) return true;

    if (!is_SpeedLimited) {
        // اتصال مجدد برای خواندن با حداکثر سرعت شبکه
        connect(reply, &QNetworkReply::readyRead, this, &PartDownloader::ReadyRead, Qt::UniqueConnection);
        if (reply->bytesAvailable() > 0) {
            ReadyRead();
        }
    }
    else {
        // قطع اتصال: لایه شبکه منتظر می‌ماند تا تایمر DownloadForControlSpeed دیتا را بخواند
        disconnect(reply, &QNetworkReply::readyRead, this, &PartDownloader::ReadyRead);
    }
    return true;
}

bool PartDownloader::IsSpeedLimiter()
{
    return this->is_SpeedLimit;
}

bool PartDownloader::IsAvaliableByteForRead()
{
    bool available = false;
    auto checkTask = [this, &available]() {
        available = (reply != nullptr && reply->bytesAvailable() > 0);
        };

    if (QThread::currentThread() == this->thread()) {
        checkTask();
    }
    else {
        QMetaObject::invokeMethod(this, checkTask, Qt::BlockingQueuedConnection);
    }
    return available;
}