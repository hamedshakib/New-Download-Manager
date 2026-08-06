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
    timer->start(1000);

    if (is_SpeedLimited && this->MaxSpeed > 0) {
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
        disconnect(speedControlConnection);
        if (maxSpeed > 0) {
            ProcessScheduleControledLimittedSpeed();
            emit FinishedLastControlledSpeedPriod(elapsedTimer->elapsed());
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
    qint64 downloadedByte = this->NumberOfBytesDownloadedInLastPeriod;
    if (downloadedByte > 0) {
        this->NumberOfBytesDownloadedInLastPeriod = 0;
        qint64 timerSpent = elapsedTimerForIndependentSpeed->restart();
        qint64 speed = calculatorDownload.CalculateDownloadSpeed(downloadedByte, timerSpent);
        QString SpeedString = calculatorDownload.GetSpeedOfDownloadInFormOfString();
        QString TimeLeftString = calculatorDownload.GetTimeLeftOfDownloadInFormOfString(download->DownloadSize - download->SizeDownloaded);
        QString DownloadStatus = calculatorDownload.getStatusForTable(download->SizeDownloaded, download->DownloadSize);

        QList<qint64> DownloadedBytesEachPartDownloadList;
        for (PartDownload* partDownload : download->get_PartDownloads()) {
            DownloadedBytesEachPartDownloadList.append(partDownload->GetLastDownloadedByte() - partDownload->start_byte + 1);
        }

        emit UpdateDownloaded(DownloadStatus, SpeedString, TimeLeftString, DownloadedBytesEachPartDownloadList);
    }
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
    if (!Is_Downloading) return;

    QReadLocker listGuard(&locker);
    elapsedTimer->restart();
    NumberOfBytesDownloadedInLastPeriodOfDownloadSpeedLimitted = 0;
    bool anyDownloaded = false;

    if (MaxSpeed > 0) {
        qint64 spentedTimeOfThisPeriod = 0;
        while (spentedTimeOfThisPeriod < 995 && (MaxSpeed * 1024) > NumberOfBytesDownloadedInLastPeriodOfDownloadSpeedLimitted) {
            qint64 BytesShouldDownload = (MaxSpeed * 1024) - NumberOfBytesDownloadedInLastPeriodOfDownloadSpeedLimitted;
            RecentlyUpdatedActivePartDownloader_list = false;

            for (PartDownloader* partDownloader : ActivePartDownloader_list) {
                if (partDownloader->IsAvaliableByteForRead()) {
                    anyDownloaded = true;
                    qint64 ReadedBytes = partDownloader->DownloadByteInSpeedControl(BytesShouldDownload);
                    this->NumberOfBytesDownloadedInLastPeriod += ReadedBytes;
                    this->NumberOfBytesDownloadedInLastPeriodOfDownloadSpeedLimitted += ReadedBytes;
                    download->SizeDownloaded += ReadedBytes;
                    BytesShouldDownload -= ReadedBytes;

                    if (RecentlyUpdatedActivePartDownloader_list) break;
                }
            }
            if (!anyDownloaded) break;
            spentedTimeOfThisPeriod = elapsedTimer->elapsed();
        }
        emit FinishedLastControlledSpeedPriod(elapsedTimer->elapsed());
    }
}

void DownloadController::ProcessScheduleControledLimittedSpeed()
{
    speedControlConnection = connect(this, &DownloadController::FinishedLastControlledSpeedPriod, this, [this](qint64 spentedTime) {
        if (spentedTime < 1000) {
            QTimer::singleShot(999 - spentedTime, this, &DownloadController::DownloadForControlSpeed);
        }
        else {
            DownloadForControlSpeed();
        }
        }, Qt::UniqueConnection);
}