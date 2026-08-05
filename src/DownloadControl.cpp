#include "HeaderAndUi/DownloadControl.h"
#include "qdebug.h"

DownloadControl::DownloadControl(QObject *parent)
	: QObject(parent)
{
	// Initialize speedControlConnection to a valid (disconnected) state
	speedControlConnection = QMetaObject::Connection();
}

DownloadControl::~DownloadControl()
{
	// Stop any ongoing downloads
	PauseDownload();
	
	// Disconnect speed control connection if active
	if (speedControlConnection.isConnected()) {
		disconnect(speedControlConnection);
	}
	
	// Delete PartDownloader objects
	qDeleteAll(PartDownloader_list);
	PartDownloader_list.clear();
	
	// Delete ActivePartDownloader_list objects
	qDeleteAll(ActivePartDownloader_list);
	ActivePartDownloader_list.clear();
	
	// Delete other resources
	if (manager != nullptr) {
		manager->deleteLater();
		manager = nullptr;
	}
	
	if (timer != nullptr) {
		timer->stop();
		timer->deleteLater();
		timer = nullptr;
	}
	
	if (elapsedTimer != nullptr) {
		elapsedTimer->deleteLater();
		elapsedTimer = nullptr;
	}
	
	if (elapsedTimerForIndependentSpeed != nullptr) {
		elapsedTimerForIndependentSpeed->deleteLater();
		elapsedTimerForIndependentSpeed = nullptr;
	}
}

void DownloadControl::initDownloadControl(Download* download)
{
	this->download = download;
	manager = new QNetworkAccessManager();
	manager->moveToThread(this->thread());
	
	timer = new QTimer();
	timer->moveToThread(this->thread());  // Fix: Move timer to the correct thread
	connect(timer, &QTimer::timeout, this, &DownloadControl::TimerTimeOut, Qt::QueuedConnection);
	// Use Qt::QueuedConnection for cross-thread safety when emitting signals across threads
connect(this, &DownloadControl::CompletedDownload, this, &DownloadControl::ProcessForShowDownloadCompleteDialog, Qt::QueuedConnection);
	
	elapsedTimer = new QElapsedTimer();
	elapsedTimerForIndependentSpeed = new QElapsedTimer();
	
	// Initialize elapsed timers
	elapsedTimer->start();
	elapsedTimerForIndependentSpeed->start();
}

bool DownloadControl::StartDownload()
{
	Is_Downloading.store(true);
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



	elapsedTimer->start();

	bool is_SpeedLimited = IsSpeedLimitted();
	for (PartDownloader* partDownloader : PartDownloader_list)
	{
		PartDownload* partDownload = partDownloader->Get_PartDownload();
		partDownload->UpdatePartDownloadLastDownloadedByte();
		if (!partDownload->IsPartDownloadFinished())
		{
			ProcessPreparePartDownloaderFromPartdownload(partDownloader, partDownload);
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
	emit Started();
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

bool DownloadControl::PauseDownload()
{
	Is_Downloading.store(false);
	statusOfDownload = DownloadStatus::Pause;
	for (PartDownloader* partDownloader : PartDownloader_list)
	{
		PartDownload* partDownload = partDownloader->Get_PartDownload();
		if (!partDownload->IsPartDownloadFinished())
		{
			StopPartDownloader(partDownloader);
		}
		partDownload->UpdatePartDownloadLastDownloadedByte();

	}
	download->Set_downloadStatus(Download::DownloadStatusEnum::Pause);
	timer->stop();
	emit Paused();
	return true;
}

bool DownloadControl::IsDownloading()
{
	return Is_Downloading.load();
}

Download* DownloadControl::Get_Download()
{
	return this->download;
}

void DownloadControl::SetMaxSpeed(int maxSpeed)
{
	this->MaxSpeed = maxSpeed;
	SetMaxSpeedForPartDownloaders();
	qDebug() << "After Set M";
	//qDebug() << "Max Speed is " << maxSpeed;
	if (Is_Downloading.load())
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

int DownloadControl::Get_MaxSpeed()
{
	return this->MaxSpeed;
}

bool DownloadControl::IsSpeedLimitted()
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

void DownloadControl::SetMaxSpeedForPartDownloaders()
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

void DownloadControl::ProcessSetPartDownloaderMaxSpeed(PartDownloader* partDownloader,bool is_SpeedLimited)
{
	if (statusOfDownload == DownloadStatus::Downloading || statusOfDownload == DownloadStatus::Pause)
		partDownloader->SetSpeedLimited(is_SpeedLimited);
}

/*bool DownloadControl::CreatePartDownloaderFromDatabase()
{

}*/

bool DownloadControl::ProcessPreparePartDownloaders()
{
	QList<PartDownload*> PartDownloads = download->get_PartDownloads();
	foreach(PartDownload* partDownload, PartDownloads)
	{
		if (!partDownload->IsPartDownloadFinished())
		{
			partDownload->UpdatePartDownloadLastDownloadedByte();
			PartDownloader* tempPartDownloader = new PartDownloader();
			tempPartDownloader->moveToThread(partDownload->thread());
			qDebug() << partDownload->thread();

			PartDownloader_list.append(tempPartDownloader);
			// Use default buffer size (1MB) instead of hardcoded large value
			tempPartDownloader->initPartDownlolader(partDownload, 0);  // 0 will use default 1MB

			Download* download1 = download;
			connect(tempPartDownloader, &PartDownloader::Started, this, &DownloadControl::HandelStartedPartDownloaderSignalEmitted);
			connect(tempPartDownloader, &PartDownloader::Paused, this, &DownloadControl::HandelPausedPartDownloaderSignalEmitted);
			connect(tempPartDownloader, &PartDownloader::FinishedReceivedBytes, this, &DownloadControl::HandelFinishedRecivedBytesPartDownloaderSignalEmitted);
			connect(tempPartDownloader, &PartDownloader::Finished, this, &DownloadControl::HandelFinishedPartDownloaderSignalEmitted);
			connect(tempPartDownloader, &PartDownloader::DownloadedBytes, this, &DownloadControl::HandelDownloadedBytesPartDownloaderSignalEmitted,Qt::ConnectionType::UniqueConnection);
		}
	}
	Is_PreparePartDownloaders = true;
	return true;
}

bool DownloadControl::ProcessPreparePartDownloaderFromPartdownload(PartDownloader* partDownloader, PartDownload* partDownload)
{
	partDownload->UpdatePartDownloadLastDownloadedByte();
	if (partDownload->IsPartDownloadFinished())
	{
		return false;
	}

	QNetworkRequest request;
	QUrl url = download->get_Url();
	if (!download->Username.isEmpty() && download->Password.isEmpty())
	{
		url.setUserName(download->Username);
		url.setPassword(download->Password);
	}
	request.setUrl(url);


	//qDebug() << "downloadUrl:" << download->get_Url();
	request.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);

	//qDebug() << "Started_Byte ::::" << partDownload->start_byte;
	QString rangeBytes = QString("bytes=%1-%2").arg(partDownload->LastDownloadedByte + 1).arg(partDownload->end_byte);
	//qDebug() << "rangeBytes:" << rangeBytes;
	request.setRawHeader("Range", rangeBytes.toUtf8());

	//qDebug() << manager->thread()->objectName();
	//qDebug() << QThread::currentThread()->objectName();
	//qDebug() << this->thread();

	QNetworkReply* reply = manager->get(request);
	//qDebug() << reply->bytesAvailable();
	
	reply->moveToThread(download->thread());
	//qDebug() << reply->thread()->objectName();
	partDownloader->moveToThread(download->thread());

	partDownloader->ProcessSetNewReply(reply);
	

	return true;
}

void DownloadControl::HandelStartedPartDownloaderSignalEmitted()
{

}

void DownloadControl::HandelPausedPartDownloaderSignalEmitted()
{

}

void DownloadControl::HandelFinishedRecivedBytesPartDownloaderSignalEmitted()
{

}

void DownloadControl::HandelFinishedPartDownloaderSignalEmitted()
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

void DownloadControl::HandelDownloadedBytesPartDownloaderSignalEmitted(qint64 ReadedBytes)
{
	this->NumberOfBytesDownloadedInLastPeriod += ReadedBytes;
	download->SizeDownloaded += ReadedBytes;
}

bool DownloadControl::StartPartDownloader(PartDownloader* partDownloader)
{
	partDownloader->Resume();
	return true;
}

bool DownloadControl::StopPartDownloader(PartDownloader* partDownloader)
{
	partDownloader->Pause();
	return true;
}

bool DownloadControl::CheckDownloadFinished()
{
	QMutexLocker locker(&mutex);
	qDebug() << "Check For Download Finish";
	if (statusOfDownload == DownloadStatus::Downloading || statusOfDownload == DownloadStatus::Pause)
	{
		for (PartDownloader* partDownloader : PartDownloader_list)
		{
			//qDebug() << "Count PartDownloaders:" << PartDownloader_list.count();
			PartDownload* partDownload = partDownloader->Get_PartDownload();
			if (partDownload != nullptr)
			{
				//qDebug() << "In Check Download Finsish";
				partDownload->UpdatePartDownloadLastDownloadedByte();
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

bool DownloadControl::ProcessFinishDownload()
{
	if (statusOfDownload == DownloadStatus::Finished)
	{
		return false;
	}
	statusOfDownload = DownloadStatus::StartFinish;
	disconnect(speedControlConnection);
	qDebug() << "Process Of End Of Downloading "<<QThread::currentThread()->objectName() ;
	Is_Downloading.store(false);
	timer->stop();
	
	// Use PartDownloader_list instead of get_PartDownloads() to properly access PartDownload objects
	QList<QFile*> FilesOfDownload;
	for (PartDownloader* partDownloader : PartDownloader_list)
	{
		PartDownload* partDownload = partDownloader->Get_PartDownload();
		if (partDownload && partDownload->PartDownloadFile)
		{
			//qDebug() << partDownload->PartDownloadFile->fileName() << ":" << partDownload->PartDownloadFile->size();
			FilesOfDownload.append(partDownload->PartDownloadFile);
		}
	}
	
	QFile* NewDownloadFile = DownloadFileWriter::BuildFileFromMultipleFiles(FilesOfDownload, download->get_SavaTo().toString());
	if (!NewDownloadFile) {
		qCritical() << "DownloadControl: Failed to build file from multiple files for download ID:" << download->get_Id();
		download->Set_downloadStatus(Download::Error);
		emit ErrorDownload();
		statusOfDownload = DownloadStatus::Error;
		return false;
	}
	qDebug() << NewDownloadFile->fileName() << ":" << NewDownloadFile->size();
	download->CompletedFile = NewDownloadFile;
	
	// Properly delete PartDownload objects through PartDownloader_list
	// Note: PartDownloader objects will be deleted in DownloadControl destructor
	// which calls qDeleteAll(PartDownloader_list) and qDeleteAll(ActivePartDownloader_list)
	
	download->Set_downloadStatus(Download::Completed);
	NewDownloadFile->deleteLater();
	emit CompletedDownload();
	statusOfDownload = DownloadStatus::Finished;
	return true;
}

void DownloadControl::TimerTimeOut()
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
			DownloadedBytesEachPartDownloadList.append(partDownload->LastDownloadedByte - partDownload->start_byte + 1);
		}

		emit UpdateDownloaded(DownloadStatus, SpeedString, TimeLeftString, DownloadedBytesEachPartDownloadList);
	}
}

void DownloadControl::ProcessForShowDownloadCompleteDialog()
{
	
	if (SettingInteract::GetValue("Download/ShowCompleteDialog").toBool())
	{
		Download* download1 = download;
		// Use QueuedConnection to safely call ShowCompleteDialog from main thread
		// No mutex needed as qApp handles thread safety for queued connections
		QMetaObject::invokeMethod(qApp, [&, download1]() {ShowCompleteDialog(download1,download1->get_SavaTo().toString()); }, Qt::QueuedConnection);
	}
}

void DownloadControl::ShowCompleteDialog(Download* download, QString SaveTo)
{
	if (!download) {
		qCritical() << "ShowCompleteDialog called with null download";
		return;
	}
	
	QString SizeDownloadString = ConverterSizeToSuitableString::ConvertSizeToSuitableString(download->DownloadSize) + QString(" (%1 Bytes)").arg(download->DownloadSize);
	CompleteDownloadDialog* completeDownloadDialog = new CompleteDownloadDialog(SaveTo, SizeDownloadString, download->Url.toString());
	completeDownloadDialog->setAttribute(Qt::WA_DeleteOnClose);
	completeDownloadDialog->show();
}

void DownloadControl::UpdateListOfActivePartDownloaders()
{
	locker.lockForWrite();
	int numberOfActivePartDownloaders = ActivePartDownloader_list.count();
	QList<PartDownloader*> tempActivePartDownloaders;
	for (auto partDownloader : PartDownloader_list)
	{
		if (!partDownloader->Get_PartDownload()->IsPartDownloadFinished())
			tempActivePartDownloaders.append(partDownloader);
	}
	
	ActivePartDownloader_list = tempActivePartDownloaders;

	if (numberOfActivePartDownloaders != ActivePartDownloader_list.count())
		RecentlyUpdatedActivePartDownloader_list = true;

	locker.unlock();
}

void DownloadControl::DownloadForControlSpeed()
{
	if (Is_Downloading.load())
	{
		// Note: The mutex was a local variable and didn't protect any shared data
		// The ActivePartDownloader_list is already protected by the locker mutex
		// which is managed by SetMaxSpeedForPartDownloaders and UpdateListOfActivePartDownloaders
		
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

void DownloadControl::ProcessScheduleControledLimittedSpeed()
{
	speedControlConnection =connect(this, &DownloadControl::FinishedLastControlledSpeedPriod, this, [&](qint64 spentedTime) {
	if (spentedTime < 1000)
	{
		//it's mean pause for other other of one second and wait for next second
		QTimer::singleShot(999 - spentedTime, this, &DownloadControl::DownloadForControlSpeed);
	}
	else
	{
		DownloadForControlSpeed();
		//elapsedTimer->restart();
	}
	},Qt::ConnectionType::UniqueConnection);
}
