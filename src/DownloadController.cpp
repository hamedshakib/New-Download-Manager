#include "HeaderAndUi/DownloadController.h"

DownloadController::DownloadController(QObject *parent)
	: QObject(parent)
{
}

DownloadController::~DownloadController()
{
}

void DownloadController::initDownloadController(Download* download)
{
	this->download = download;
	//Each PartDownloader now owns its own QNetworkAccessManager on its own thread,
	//so no shared manager is created here.
	timer = new QTimer();
	//timer->moveToThread(this->thread());
	connect(timer, &QTimer::timeout, this, &DownloadController::TimerTimeOut);
	connect(this, &DownloadController::DownloadCompleted, this, &DownloadController::ProcessForShowDownloadCompleteDialog);
	elapsedTimer = new QElapsedTimer();
	elapsedTimerForIndependentSpeed= new QElapsedTimer();
}

bool DownloadController::StartDownload()
{
	this->Is_Downloading = true;
	statusOfDownload = DownloadStatus::Downloading;

	if (!Is_PreparePartDownloaders)
	{
		if (!ProcessPreparePartDownloaders())
		{
			return false;
		}
	}
	if (CheckDownloadFinished())
	{
		ProcessFinishDownload();
		return true;
	}



	Is_Downloading = true;
	elapsedTimer->start();

	bool is_SpeedLimited = IsSpeedLimitted();
	for (PartDownloader* partDownloader : PartDownloader_list)
	{
		PartDownload* partDownload = partDownloader->GetPartDownload();
		if (!partDownload->IsPartDownloadFinished())
		{
			ProcessPreparePartDownloaderFromPartDownload(partDownloader, partDownload);
			ProcessSetPartDownloaderMaxSpeed(partDownloader, is_SpeedLimited);
			StartPartDownloader(partDownloader);
		}
		
	}
	UpdateListOfActivePartDownloaders();

	if (CheckDownloadFinished())
	{
		ProcessFinishDownload();
		return true;
	}

	download->Set_downloadStatus(Download::DownloadStatusEnum::Downloading);
	download->LastTryTime = QDateTime::currentDateTime();
	emit DownloadStarted();
	Is_Downloading = true;
	elapsedTimer->restart();
	timer->start(1000);
	if (is_SpeedLimited)
	{
		disconnect(speedControlConnection);
		if (this->MaxSpeed > 0)
		{
			ProcessScheduleControledLimittedSpeed();
			//DownloadForControlSpeed();
			emit FinishedLastControlledSpeedPriod(elapsedTimer->elapsed());
		}

	}
}

bool DownloadController::PauseDownload()
{
	this->Is_Downloading = false;
	statusOfDownload = DownloadStatus::Paused;
	for (PartDownloader* partDownloader : PartDownloader_list)
	{
		PartDownload* partDownload = partDownloader->GetPartDownload();
		if (!partDownload->IsPartDownloadFinished())
		{
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
	qDebug() << "After Set M";
	//qDebug() << "Max Speed is " << maxSpeed;
	if (Is_Downloading)
	{

		disconnect(speedControlConnection);
		if (maxSpeed > 0)
		{
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
	if (this->MaxSpeed > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void DownloadController::SetMaxSpeedForPartDownloaders()
{
	//UpdateListOfActivePartDownloaders();
	bool is_SpeedLimited=IsSpeedLimitted();
	locker.lockForRead();
	if (ActivePartDownloader_list.count() > 0)
	{
		qDebug() <<"count of ActivePartDownloader_list:" << ActivePartDownloader_list.count();
		for (auto partDownloader : ActivePartDownloader_list)
		{
			ProcessSetPartDownloaderMaxSpeed(partDownloader, is_SpeedLimited);
		}
	}
	locker.unlock();
}

void DownloadController::ProcessSetPartDownloaderMaxSpeed(PartDownloader* partDownloader,bool is_SpeedLimited)
{
	if (statusOfDownload == DownloadStatus::Downloading || statusOfDownload == DownloadStatus::Paused)
	{
		//SetSpeedLimited (de)connects the reply readyRead signal, so it must run on
		//the PartDownloader's own thread.
		QMetaObject::invokeMethod(partDownloader, [partDownloader, is_SpeedLimited]() {
			partDownloader->SetSpeedLimited(is_SpeedLimited);
		}, Qt::QueuedConnection);
	}
}

/*bool DownloadController::CreatePartDownloaderFromDatabase()
{

}*/

bool DownloadController::ProcessPreparePartDownloaders()
{
	QList<PartDownload*> PartDownloads = download->get_PartDownloads();
	foreach(PartDownload* partDownload, PartDownloads)
	{
		if (!partDownload->IsPartDownloadFinished())
		{
			PartDownloader* tempPartDownloader = new PartDownloader();
			tempPartDownloader->moveToThread(partDownload->thread());
			qDebug() << partDownload->thread();

			PartDownloader_list.append(tempPartDownloader);
			//Run init on the PartDownloader's own thread (queued). This is where its
			//QNetworkAccessManager and file writer are created on the correct thread.
			QMetaObject::invokeMethod(tempPartDownloader, [tempPartDownloader, partDownload]() {
				tempPartDownloader->InitPartDownloader(partDownload, 500000000);
			}, Qt::QueuedConnection);

			Download* download1 = download;
			connect(tempPartDownloader, &PartDownloader::DownloadStarted, this, &DownloadController::HandelStartedPartDownloaderSignalEmitted);
			connect(tempPartDownloader, &PartDownloader::DownloadPaused, this, &DownloadController::HandelPausedPartDownloaderSignalEmitted);
			connect(tempPartDownloader, &PartDownloader::FinishedReceivedBytes, this, &DownloadController::HandelFinishedReceivedBytesPartDownloaderSignalEmitted);
			connect(tempPartDownloader, &PartDownloader::PartDownloaderFinished, this, &DownloadController::HandelFinishedPartDownloaderSignalEmitted);
			connect(tempPartDownloader, &PartDownloader::DownloadedByteCount, this, &DownloadController::HandelDownloadedBytesPartDownloaderSignalEmitted,Qt::ConnectionType::UniqueConnection);
		}
	}
	Is_PreparePartDownloaders = true;
	return true;
}

bool DownloadController::ProcessPreparePartDownloaderFromPartDownload(PartDownloader* partDownloader, PartDownload* partDownload)
{
	if (partDownload->IsPartDownloadFinished())
	{
		return false;
	}

	QUrl url = download->get_Url();
	if (!download->Username.isEmpty() && !download->Password.isEmpty())
	{
		url.setUserName(download->Username);
		url.setPassword(download->Password);
	}

	qint64 startByte = partDownload->GetLastDownloadedByte() + 1;
	qint64 endByte = partDownload->end_byte;
	QString user = download->Username;
	QString pass = download->Password;

	//The PartDownloader issues the ranged request on its own thread using its own
	//QNetworkAccessManager; this yields real per-part parallelism.
	QMetaObject::invokeMethod(partDownloader, [partDownloader, url, user, pass, startByte, endByte]() {
		partDownloader->StartRequest(url, user, pass, startByte, endByte);
	}, Qt::QueuedConnection);

	return true;
}

void DownloadController::HandelStartedPartDownloaderSignalEmitted()
{

}

void DownloadController::HandelPausedPartDownloaderSignalEmitted()
{

}

void DownloadController::HandelFinishedReceivedBytesPartDownloaderSignalEmitted()
{

}

void DownloadController::HandelFinishedPartDownloaderSignalEmitted()
{
	//SetMaxSpeedForPartDownloaders();
	qDebug() << "receive finished of partDownload";
	if (CheckDownloadFinished())
	{
		qDebug() << "Before Process Of End Of Downloading";
		ProcessFinishDownload();
	}
	else
	{
		UpdateListOfActivePartDownloaders();
		qDebug() << "After Update Actie Downloader";
	}
}

void DownloadController::HandelDownloadedBytesPartDownloaderSignalEmitted(qint64 ReadedBytes)
{
	this->NumberOfBytesDownloadedInLastPeriod += ReadedBytes;
	download->SizeDownloaded += ReadedBytes;
}

bool DownloadController::StartPartDownloader(PartDownloader* partDownloader)
{
	//Resume must run on the PartDownloader's own thread.
	QMetaObject::invokeMethod(partDownloader, [partDownloader]() {
		partDownloader->Resume();
	}, Qt::QueuedConnection);
	return true;
}

bool DownloadController::StopPartDownloader(PartDownloader* partDownloader)
{
	//Pause must run on the PartDownloader's own thread.
	QMetaObject::invokeMethod(partDownloader, [partDownloader]() {
		partDownloader->Pause();
	}, Qt::QueuedConnection);
	return true;
}

bool DownloadController::CheckDownloadFinished()
{
	qDebug() << "Check For Download Finish";
	if (statusOfDownload == DownloadStatus::Downloading || statusOfDownload == DownloadStatus::Paused)
	{
		for (PartDownloader* partDownloader : PartDownloader_list)
		{
			//qDebug() << "Count PartDownloaders:" << PartDownloader_list.count();
			PartDownload* partDownload = partDownloader->GetPartDownload();
			if (partDownload != nullptr)
			{
				//qDebug() << "In Check Download Finsish";
				if (!partDownload->IsPartDownloadFinished())
				{
					qDebug() << " Exit In Check Download Finsish: not finish";
					return false;
				}
			}
			else
			{
				continue;
			}
		}
	}
	qDebug() << " Exit In Check Download Finsish: finished";
	return true;
}

bool DownloadController::ProcessFinishDownload()
{
	if (statusOfDownload == DownloadStatus::Finidshed)
	{
		return false;
	}
	statusOfDownload = DownloadStatus::FinishProcessStaretd;
	disconnect(speedControlConnection);
	qDebug() << "Process Of End Of Downloading " << QThread::currentThread()->objectName();
	Is_Downloading = false;
	timer->stop();
	QList<PartDownload*> PartDownloads = download->get_PartDownloads();
	QList<QFile*> FilesOfDownload;
	for (PartDownload* partDownload : PartDownloads)
	{
		//qDebug() << partDownload->PartDownloadFile->fileName() << ":" << partDownload->PartDownloadFile->size();
		FilesOfDownload.append(partDownload->PartDownloadFile);
	}

	QFile* NewDownloadFile = DownloadFileWriter::BuildFileFromMultipleFiles(FilesOfDownload, download->get_SavaTo().toString());

	qDebug() << NewDownloadFile->fileName() << ":" << NewDownloadFile->size();
	download->CompletedFile = NewDownloadFile;
	//deleteLater() queues deletion on each PartDownload's own thread. PartDownload objects
	//may live on worker threads, so a direct qDeleteAll() from here (a different thread)
	//would be an unsafe cross-thread deletion of QObjects.
	for (PartDownload* partDownload : PartDownloads)
	{
		partDownload->deleteLater();
	}
	download->Set_downloadStatus(Download::Completed);

	NewDownloadFile->deleteLater();
	emit DownloadCompleted();
	statusOfDownload = DownloadStatus::Finidshed;
	return true;
}

void DownloadController::TimerTimeOut()
{
	qint64 downloadedByte = this->NumberOfBytesDownloadedInLastPeriod;
	if (downloadedByte > 0)
	{
		this->NumberOfBytesDownloadedInLastPeriod = 0;
		qint64 timerSpent = elapsedTimerForIndependentSpeed->restart();
		qint64 speed = calculatorDownload.CalculateDownloadSpeed(downloadedByte, timerSpent);
		QString SpeedString = calculatorDownload.GetSpeedOfDownloadInFormOfString();
		QString TimeLeftString = calculatorDownload.GetTimeLeftOfDownloadInFormOfString(download->DownloadSize - download->SizeDownloaded);
		QString DownloadStatus = calculatorDownload.getStatusForTable(download->SizeDownloaded, download->DownloadSize);
		//emit DownloadedAtAll(download->SizeDownloaded);
		QList<qint64> DownloadedBytesEachPartDownloadList;
		for (PartDownload* partDownload : download->get_PartDownloads())
		{
			DownloadedBytesEachPartDownloadList.append(partDownload->GetLastDownloadedByte() - partDownload->start_byte + 1);
		}

		emit UpdateDownloaded(DownloadStatus, SpeedString, TimeLeftString, DownloadedBytesEachPartDownloadList);
	}
}

void DownloadController::ProcessForShowDownloadCompleteDialog()
{
	if (SettingInteract::GetValue("Download/ShowCompleteDialog").toBool())
	{
		Download* download1 = download;
		QMetaObject::invokeMethod(qApp, [&, download1]() {ShowCompleteDialog(download1,download1->get_SavaTo().toString()); }, Qt::QueuedConnection);
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
	//A local build list is used to avoid the member being modified while iterating.
	QList<PartDownloader*> newActiveList;
	for (auto partDownloader : PartDownloader_list)
	{
		if (!partDownloader->GetPartDownload()->IsPartDownloadFinished())
			newActiveList.append(partDownloader);
	}

	ActivePartDownloader_list = newActiveList;

	if (numberOfActivePartDownloaders != ActivePartDownloader_list.count())
		RecentlyUpdatedActivePartDownloader_list = true;
}

void DownloadController::DownloadForControlSpeed()
{
	if (Is_Downloading)
	{
		//Protect the shared ActivePartDownloader_list (also written under the same lock).
		QReadLocker listGuard(&locker);

		qint64 spentedTimeFromLastPeriod = elapsedTimer->restart();
		NumberOfBytesDownloadedInLastPeriodOfDownloadSpeedLimitted = 0;
		bool anyDownloaded = false;
		qint64 ReadedBytes = 0;
		qDebug() << "Max Speed is " << MaxSpeed;
		if (MaxSpeed > 0)
		{
			qint64 spentedTimeOfThisPeriod = 0;
			while (spentedTimeOfThisPeriod<995 && MaxSpeed * 1024 >NumberOfBytesDownloadedInLastPeriodOfDownloadSpeedLimitted)
			{
				qint64 BytesShouldDownload = MaxSpeed * 1024 - NumberOfBytesDownloadedInLastPeriodOfDownloadSpeedLimitted;
				RecentlyUpdatedActivePartDownloader_list = false;
				for (PartDownloader* partDownloader : ActivePartDownloader_list)
				{
					if (partDownloader->IsAvaliableByteForRead())
					{
						anyDownloaded = true;
						ReadedBytes = partDownloader->DownloadByteInSpeedControl(BytesShouldDownload);
						this->NumberOfBytesDownloadedInLastPeriod += ReadedBytes;
						this->NumberOfBytesDownloadedInLastPeriodOfDownloadSpeedLimitted += ReadedBytes;
						download->SizeDownloaded += ReadedBytes;
						BytesShouldDownload -= ReadedBytes;
						if (RecentlyUpdatedActivePartDownloader_list)
						{
							break;
						}
					}
				}
				if (!anyDownloaded)
				{
					break;
				}
				spentedTimeOfThisPeriod = elapsedTimer->elapsed();

			}
		emit FinishedLastControlledSpeedPriod(elapsedTimer->elapsed());
		}
	}
}

void DownloadController::ProcessScheduleControledLimittedSpeed()
{
	speedControlConnection =connect(this, &DownloadController::FinishedLastControlledSpeedPriod, this, [&](qint64 spentedTime) {
	if (spentedTime < 1000)
	{
		//it's mean pause for other other of one second and wait for next second
		QTimer::singleShot(999 - spentedTime, this, &DownloadController::DownloadForControlSpeed);
	}
	else
	{
		DownloadForControlSpeed();
		//elapsedTimer->restart();
	}
	},Qt::ConnectionType::UniqueConnection);
}
