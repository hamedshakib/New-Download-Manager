#include "HeaderAndUi/DownloadController.h"
#include <QDebug>
#include <QThread>
#include <QApplication>

DownloadController::DownloadController(QObject* parent)
    : QObject(parent)
{}

DownloadController::~DownloadController()
{}

void DownloadController::initDownloadController(Download* download)
{
    this->download = download;
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &DownloadController::TimerTimeOut);
    connect(this, &DownloadController::DownloadCompleted, this, &DownloadController::ProcessForShowDownloadCompleteDialog);

    elapsedTimer = new QElapsedTimer();
    elapsedTimerForIndependentSpeed = new QElapsedTimer();

    speedControlTimer = new QTimer(this);
    speedControlTimer->setInterval(TICK_INTERVAL_MS);
    connect(speedControlTimer, &QTimer::timeout, this, &DownloadController::DownloadForControlSpeed);
}

bool DownloadController::StartDownload()
{
    this->Is_Downloading = true;
    statusOfDownload = DownloadStatus::Downloading;

    if (!Is_PreparePartDownloaders) {
        if (!ProcessPreparePartDownloaders()) return false;
    }

    if (CheckDownloadFinished()) {
        ProcessFinishDownload();
        return true;
    }

    elapsedTimer->start();
    bool is_SpeedLimited = IsSpeedLimitted();

    for (PartDownloader* partDownloader : PartDownloader_list) {
        PartDownload* partDownload = partDownloader->GetPartDownload();
        if (!partDownload->IsPartDownloadFinished()) {
            ProcessPreparePartDownloaderFromPartDownload(partDownloader, partDownload);
            ProcessSetPartDownloaderMaxSpeed(partDownloader, is_SpeedLimited);
            StartPartDownloader(partDownloader);
        }
    }

    UpdateListOfActivePartDownloaders();

    if (CheckDownloadFinished()) {
        ProcessFinishDownload();
        return true;
    }

    download->Set_downloadStatus(Download::DownloadStatusEnum::Downloading);
    download->LastTryTime = QDateTime::currentDateTime();
    emit DownloadStarted();

    elapsedTimer->restart();
    elapsedTimerForIndependentSpeed->restart();
    m_smoothedSpeed = 0; // ریست سرعت میانگین

    timer->start(1000);

    if (IsSpeedLimitted() && this->MaxSpeed > 0) {
        disconnect(speedControlConnection);
        ProcessScheduleControledLimittedSpeed();
        emit FinishedLastControlledSpeedPriod(elapsedTimer->elapsed());
    }
    return true;
}

bool DownloadController::PauseDownload()
{
    this->Is_Downloading = false;
    statusOfDownload = DownloadStatus::Paused;

    // توقف تایمر کنترل سرعت
    if (speedControlTimer) {
        speedControlTimer->stop();
    }

    for (PartDownloader* partDownloader : PartDownloader_list) {
        PartDownload* partDownload = partDownloader->GetPartDownload();
        if (!partDownload->IsPartDownloadFinished()) {
            StopPartDownloader(partDownloader);
        }
    }

    download->Set_downloadStatus(Download::DownloadStatusEnum::Pause);
    timer->stop();
    emit DownloadPaused();
    return true;
}

bool DownloadController::IsDownloading()
{
    return this->Is_Downloading;
}

Download* DownloadController::Get_Download()
{
    return this->download;
}

void DownloadController::SetMaxSpeed(int maxSpeed)
{
    this->MaxSpeed = maxSpeed;
    SetMaxSpeedForPartDownloaders();

    if (Is_Downloading) {
        if (maxSpeed > 0) {
            ProcessScheduleControledLimittedSpeed();
        }
        else if (speedControlTimer) {
            speedControlTimer->stop();
        }
    }
    emit SpeedChanged(maxSpeed);
}

int DownloadController::Get_MaxSpeed()
{
    return this->MaxSpeed;
}

bool DownloadController::IsSpeedLimitted()
{
    return (this->MaxSpeed > 0);
}

void DownloadController::SetMaxSpeedForPartDownloaders()
{
    bool is_SpeedLimited = IsSpeedLimitted();
    QReadLocker guard(&locker);
    for (auto partDownloader : ActivePartDownloader_list) {
        ProcessSetPartDownloaderMaxSpeed(partDownloader, is_SpeedLimited);
    }
}

void DownloadController::ProcessSetPartDownloaderMaxSpeed(PartDownloader* partDownloader, bool is_SpeedLimited)
{
    if (statusOfDownload == DownloadStatus::Downloading || statusOfDownload == DownloadStatus::Paused) {
        QMetaObject::invokeMethod(partDownloader, [partDownloader, is_SpeedLimited]() {
            partDownloader->SetSpeedLimited(is_SpeedLimited);
            }, Qt::QueuedConnection);
    }
}

bool DownloadController::ProcessPreparePartDownloaders()
{
    QList<PartDownload*> PartDownloads = download->get_PartDownloads();
    for (PartDownload* partDownload : PartDownloads) {
        if (!partDownload->IsPartDownloadFinished()) {
            PartDownloader* tempPartDownloader = new PartDownloader();
            tempPartDownloader->moveToThread(partDownload->thread());
            PartDownloader_list.append(tempPartDownloader);

            QMetaObject::invokeMethod(tempPartDownloader, [tempPartDownloader, partDownload]() {
                tempPartDownloader->InitPartDownloader(partDownload, 50000000);
                }, Qt::QueuedConnection);

            connect(tempPartDownloader, &PartDownloader::DownloadStarted, this, &DownloadController::HandelStartedPartDownloaderSignalEmitted);
            connect(tempPartDownloader, &PartDownloader::DownloadPaused, this, &DownloadController::HandelPausedPartDownloaderSignalEmitted);
            connect(tempPartDownloader, &PartDownloader::FinishedReceivedBytes, this, &DownloadController::HandelFinishedReceivedBytesPartDownloaderSignalEmitted);
            connect(tempPartDownloader, &PartDownloader::PartDownloaderFinished, this, &DownloadController::HandelFinishedPartDownloaderSignalEmitted);
            connect(tempPartDownloader, &PartDownloader::DownloadedByteCount, this, &DownloadController::HandelDownloadedBytesPartDownloaderSignalEmitted, Qt::UniqueConnection);
        }
    }
    Is_PreparePartDownloaders = true;
    return true;
}

bool DownloadController::ProcessPreparePartDownloaderFromPartDownload(PartDownloader* partDownloader, PartDownload* partDownload)
{
    if (partDownload->IsPartDownloadFinished()) return false;

    QUrl url = download->get_Url();
    if (!download->Username.isEmpty() && !download->Password.isEmpty()) {
        url.setUserName(download->Username);
        url.setPassword(download->Password);
    }

    qint64 startByte = partDownload->GetLastDownloadedByte() + 1;
    qint64 endByte = partDownload->end_byte;
    QString user = download->Username;
    QString pass = download->Password;

    QMetaObject::invokeMethod(partDownloader, [partDownloader, url, user, pass, startByte, endByte]() {
        partDownloader->StartRequest(url, user, pass, startByte, endByte);
        }, Qt::QueuedConnection);

    return true;
}

void DownloadController::HandelStartedPartDownloaderSignalEmitted() {}
void DownloadController::HandelPausedPartDownloaderSignalEmitted() {}
void DownloadController::HandelFinishedReceivedBytesPartDownloaderSignalEmitted() {}

// **حل مشکل تقدم و تاخر پایان دانلود**:
void DownloadController::HandelFinishedPartDownloaderSignalEmitted()
{
    qDebug() << "Receive finished of partDownload";

    // ۱. ابتدا وضعیت لیست دانلودرهای فعال باید به‌روزرسانی شود
    UpdateListOfActivePartDownloaders();

    // ۲. سپس بررسی کنیم که آیا کل دانلود تمام شده است یا نه
    if (CheckDownloadFinished()) {
        qDebug() << "All parts completed. Starting Finish Process...";
        ProcessFinishDownload();
    }
}

void DownloadController::HandelDownloadedBytesPartDownloaderSignalEmitted(qint64 ReadedBytes)
{
    this->NumberOfBytesDownloadedInLastPeriod += ReadedBytes;
    download->SizeDownloaded += ReadedBytes;
}

bool DownloadController::StartPartDownloader(PartDownloader* partDownloader)
{
    QMetaObject::invokeMethod(partDownloader, [partDownloader]() {
        partDownloader->Resume();
        }, Qt::QueuedConnection);
    return true;
}

bool DownloadController::StopPartDownloader(PartDownloader* partDownloader)
{
    QMetaObject::invokeMethod(partDownloader, [partDownloader]() {
        partDownloader->Pause();
        }, Qt::QueuedConnection);
    return true;
}

bool DownloadController::CheckDownloadFinished()
{
    if (statusOfDownload == DownloadStatus::Downloading || statusOfDownload == DownloadStatus::Paused) {
        for (PartDownloader* partDownloader : PartDownloader_list) {
            PartDownload* partDownload = partDownloader->GetPartDownload();
            if (partDownload && !partDownload->IsPartDownloadFinished()) {
                return false;
            }
        }
    }
    return true;
}

bool DownloadController::ProcessFinishDownload()
{
    if (statusOfDownload == DownloadStatus::Finidshed || statusOfDownload == DownloadStatus::FinishProcessStaretd) {
        return false;
    }

    statusOfDownload = DownloadStatus::FinishProcessStaretd;
    disconnect(speedControlConnection);
    Is_Downloading = false;
    timer->stop();

    QList<PartDownload*> PartDownloads = download->get_PartDownloads();
    QList<QFile*> FilesOfDownload;
    for (PartDownload* partDownload : PartDownloads) {
        FilesOfDownload.append(partDownload->PartDownloadFile);
    }

    // ساخت فایل نهایی پس از اطمینان از بسته شدن و اتمام نوشتن در همه بخش‌ها
    QFile* NewDownloadFile = DownloadFileWriter::BuildFileFromMultipleFiles(FilesOfDownload, download->get_SavaTo().toString());

    if (NewDownloadFile) {
        qDebug() << "File built successfully:" << NewDownloadFile->fileName() << ":" << NewDownloadFile->size();
        download->CompletedFile = NewDownloadFile;
        NewDownloadFile->deleteLater();
    }

    for (PartDownload* partDownload : PartDownloads) {
        partDownload->deleteLater();
    }

    download->Set_downloadStatus(Download::Completed);
    statusOfDownload = DownloadStatus::Finidshed;

    emit DownloadCompleted();
    return true;
}

void DownloadController::TimerTimeOut()
{
    qint64 downloadedBytes = this->NumberOfBytesDownloadedInLastPeriod;
    if (downloadedBytes >= 0) { // حتی اگر 0 بود محاسبه شود تا در صورت قطع شبکه سرعت صفر شود[cite: 2]
        this->NumberOfBytesDownloadedInLastPeriod = 0;

        qint64 elapsedMs = elapsedTimerForIndependentSpeed->restart();
        if (elapsedMs <= 0) elapsedMs = 1000; // جلوگیری از تقسیم بر صفر

        // ۱. محاسبه سرعت لحظه‌ای (بایت بر ثانیه)
        qint64 instantSpeed = (downloadedBytes * 1000LL) / elapsedMs;

        // ۲. استفاده از Exponential Moving Average (EMA) برای هموارسازی سرعت
        // وزن ۳۰٪ به سرعت جدید و ۷۰٪ به سرعت قبلی جهت جلوگیری از پرش‌های شدید
        if (m_smoothedSpeed == 0) {
            m_smoothedSpeed = instantSpeed;
        }
        else {
            m_smoothedSpeed = (instantSpeed * 3 + m_smoothedSpeed * 7) / 10;
        }

        // ۳. محاسبه زمان باقی‌مانده (ETA) پایدار بر اساس سرعت هموارسازی‌شده
        qint64 remainingBytes = qMax(0LL, download->DownloadSize - download->SizeDownloaded);
        qint64 secondsLeft = (m_smoothedSpeed > 0) ? (remainingBytes / m_smoothedSpeed) : 0;

        // تبدیل به رشته (رشته زمان باقی‌مانده پایدار و بدون پرش خواهد بود)[cite: 2]
        QString SpeedString = ConverterSizeToSuitableString::ConvertSizeToSuitableString(m_smoothedSpeed) + "/s";
        QString TimeLeftString = FormatTimeLeft(secondsLeft);
        QString DownloadStatus = calculatorDownload.getStatusForTable(download->SizeDownloaded, download->DownloadSize);

        QList<qint64> DownloadedBytesEachPartDownloadList;
        for (PartDownload* partDownload : download->get_PartDownloads()) {
            DownloadedBytesEachPartDownloadList.append(qMax(0LL, partDownload->GetLastDownloadedByte() - partDownload->start_byte + 1));
        }

        emit UpdateDownloaded(DownloadStatus, SpeedString, TimeLeftString, DownloadedBytesEachPartDownloadList);
    }
}

QString DownloadController::FormatTimeLeft(qint64 seconds)
{
    if (seconds <= 0 || seconds > 86400 * 30) { // بیشتر از 30 روز یا نامشخص
        return "--:--:--";
    }
    qint64 h = seconds / 3600;
    qint64 m = (seconds % 3600) / 60;
    qint64 s = seconds % 60;
    return QString("%1:%2:%3")
        .arg(h, 2, 10, QChar('0'))
        .arg(m, 2, 10, QChar('0'))
        .arg(s, 2, 10, QChar('0'));
}

void DownloadController::ProcessForShowDownloadCompleteDialog()
{
    if (SettingInteract::GetValue("Download/ShowCompleteDialog").toBool()) {
        Download* download1 = download;
        QMetaObject::invokeMethod(qApp, [&,download1]() {
            ShowCompleteDialog(download1, download1->get_SavaTo().toString());
            }, Qt::QueuedConnection);
    }
}

void DownloadController::ShowCompleteDialog(Download* download, QString SaveTo)
{
    QString SizeDownloadString = ConverterSizeToSuitableString::ConvertSizeToSuitableString(download->DownloadSize) + QString(" (%1 Bytes)").arg(download->DownloadSize);
    CompleteDownloadDialog* completeDownloadDialog = new CompleteDownloadDialog(SaveTo, SizeDownloadString, download->Url.toString());
    completeDownloadDialog->show();
}

void DownloadController::UpdateListOfActivePartDownloaders()
{
    QWriteLocker guard(&locker);
    int numberOfActivePartDownloaders = ActivePartDownloader_list.count();
    QList<PartDownloader*> newActiveList;

    for (auto partDownloader : PartDownloader_list) {
        if (!partDownloader->GetPartDownload()->IsPartDownloadFinished()) {
            newActiveList.append(partDownloader);
        }
    }

    ActivePartDownloader_list = newActiveList;
    if (numberOfActivePartDownloaders != ActivePartDownloader_list.count()) {
        RecentlyUpdatedActivePartDownloader_list = true;
    }
}

void DownloadController::DownloadForControlSpeed()
{
    if (!Is_Downloading || MaxSpeed <= 0) {
        if (speedControlTimer) speedControlTimer->stop();
        return;
    }

    QReadLocker listGuard(&locker);

    // ۱. محاسبه سهمیه (توکن) این بازه ۱۰۰ میلی‌ثانیه‌ای
    // مثال: سرعت ۱۰۰ کیلوبایت بر ثانیه -> هر ۱۰۰ میلی‌ثانیه ۱۰,۲۴۰ بایت سهمیه اضافه می‌شود
    qint64 tokensToAdd = (MaxSpeed * 1024LL * TICK_INTERVAL_MS) / 1000LL;
    m_tokenBucket += tokensToAdd;

    // ۲. جلوگیری از انباشت بیش از حد توکن (حداکثر سهمیه معادل ۱ ثانیه دانلود)
    // تا اگر شبکه چند ثانیه قطع شد، ناگهان با سرعت نامحدود دانلود نکند
    qint64 maxBucketSize = MaxSpeed * 1024LL;
    if (m_tokenBucket > maxBucketSize) {
        m_tokenBucket = maxBucketSize;
    }

    // ۳. خواندن داده‌ها از دانلودرهای فعال به اندازه سهمیه موجود در سطل
    for (PartDownloader* partDownloader : ActivePartDownloader_list) {
        if (m_tokenBucket <= 0) {
            break; // سهمیه این تیک تمام شد؛ ادامه در ۱۰۰ میلی‌ثانیه بعدی
        }

        if (partDownloader->IsAvaliableByteForRead()) {
            // فقط به اندازه توکن باقی‌مانده اجازه خواندن می‌دهیم
            qint64 bytesRead = partDownloader->DownloadByteInSpeedControl(m_tokenBucket);

            if (bytesRead > 0) {
                m_tokenBucket -= bytesRead; // کسر حجم خوانده‌شده از سطل توکن
                this->NumberOfBytesDownloadedInLastPeriod += bytesRead;
                download->SizeDownloaded += bytesRead;
            }
        }
    }
}

void DownloadController::ProcessScheduleControledLimittedSpeed()
{
    m_tokenBucket = 0; // ریست سطل توکن در شروع
    if (speedControlTimer && !speedControlTimer->isActive()) {
        speedControlTimer->start();
    }
}