#include "HeaderAndUi/DownloadControl.h"

DownloadControl::DownloadControl(QObject *parent)
	: QObject(parent)
{
}

DownloadControl::~DownloadControl()
{
}

void DownloadControl::initDownloadControl(Download* download)
{
	this->download = download;
	manager = new QNetworkAccessManager();
	manager->moveToThread(this->thread());
	timer = new QTimer();
	//timer->moveToThread(this->thread());
	connect(timer, &QTimer::timeout, this, &DownloadControl::TimerTimeOut);
	connect(this, &DownloadControl::CompeletedDownload, this, &DownloadControl::ProcessForShowDownloadCompleteDialog);
	elapsedTimer = new QElapsedTimer();
	elapsedTimerForIndependentSpeed= new QElapsedTimer();
}

bool DownloadControl::StartDownload()
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
	Is_Downloading = true;
	elapsedTimer->restart();
	timer->start(1000);
	if (is_SpeedLimited)
	{
		//elapsedTimer->restart();
		//disconnect(this, &DownloadControl::FinishedLastControlledSpeedPriod, this, [&](qint64 spentedTime);
		//disconnect()
		//disconnect(this, &DownloadControl::FinishedLastControlledSpeedPriod, this);
		//ProcessScheduleControledLimittedSpeed();
		//DownloadForControlSpeed();

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
	this->Is_Downloading = false;
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
	return this->Is_Downloading;
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
	if (Is_Downloading)
	{
		//elapsedTimer->restart();
		//DownloadForControlSpeed();
		disconnect(speedControlConnection);
		if (maxSpeed > 0)
		{
			ProcessScheduleControledLimittedSpeed();
			//DownloadForControlSpeed();
			emit FinishedLastControlledSpeedPriod(elapsedTimer->elapsed());
		}
		//DownloadForControlSpeed();
		//ProcessScheduleControledLimittedSpeed();
	}
/*
	if (maxSpeed>0)
	{
		disconnect(timer, &QTimer::timeout, this, &DownloadControl::TimerTimeOut);
	}
	else
	{
		connect(timer, &QTimer::timeout, this, &DownloadControl::TimerTimeOut, Qt::ConnectionType::UniqueConnection);
	}
*/
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
	QMutex mutex;
	mutex.lock();
	bool is_SpeedLimited=IsSpeedLimitted();
	if (ActivePartDownloader_list.count() > 0)
	{
		qDebug() <<"count of ActivePartDownloader_list:" << ActivePartDownloader_list.count();
		for (auto partDownloader : ActivePartDownloader_list)
		{
			ProcessSetPartDownloaderMaxSpeed(partDownloader, is_SpeedLimited);
			qDebug() << "After One set dolwnloader speed:";
		}
	}
	mutex.unlock();
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
			tempPartDownloader->initPartDownlolader(partDownload, 500000000);

			Download* download1 = download;
			connect(tempPartDownloader, &PartDownloader::Started, this, &DownloadControl::HandelStartedPartDownloaderSignalEmitted);
			connect(tempPartDownloader, &PartDownloader::Paused, this, &DownloadControl::HandelPausedPartDownloaderSignalEmitted);
			connect(tempPartDownloader, &PartDownloader::FinishedRecivedBytes, this, &DownloadControl::HandelFinishedRecivedBytesPartDownloaderSignalEmitted);
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
	qDebug() << "recuved finished of partDownload emited";
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
	QMutex mutex;
	mutex.lock();
	if(statusOfDownload==DownloadStatus::Downloading || statusOfDownload == DownloadStatus::Pause)
	for (PartDownloader* partDownloader : PartDownloader_list)
	{
		//qDebug() << "Count PartDownloaders:" << PartDownloader_list.count();
		PartDownload* partDownload=partDownloader->Get_PartDownload();
		if (partDownload != nullptr)
		{
			qDebug() << "In Check Download Finsish";
			partDownload->UpdatePartDownloadLastDownloadedByte();
			if (!partDownload->IsPartDownloadFinished())
			{
				qDebug() << " Exit In Check Download Finsish not fin";
				mutex.unlock();
				return false;
			}
		}
		else
		{
			continue;
		}
	}
	qDebug() << " Exit In Check Download Finsish yes fin";
	mutex.unlock();
	return true;
}

bool DownloadControl::ProcessFinishDownload()
{
	QMutex mutex;
	mutex.lock();
	if (statusOfDownload == DownloadStatus::Finidshed)
	{
		mutex.unlock();
		return false;
	}
	statusOfDownload = DownloadStatus::StartFinsh;
	disconnect(speedControlConnection);
	qDebug() << "Process Of End Of Downloading "<<QThread::currentThread()->objectName() ;
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
	qDeleteAll(PartDownloads);
	download->Set_downloadStatus(Download::Completed);

	NewDownloadFile->deleteLater();
	emit CompeletedDownload();
	statusOfDownload = DownloadStatus::Finidshed;
	mutex.unlock();
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
		QMutex mutux;
		mutux.lock();
		QMetaObject::invokeMethod(qApp, [&, download1]() {ShowCompleteDialog(download1,download1->get_SavaTo().toString()); }, Qt::QueuedConnection);
		mutux.unlock();
	}
}

void DownloadControl::ShowCompleteDialog(Download* download, QString SaveTo)
{
	QString SizeDownloadString = ConverterSizeToSuitableString::ConvertSizeToSuitableString(download->DownloadSize) + QString(" (%1 Bytes)").arg(download->DownloadSize);
	CompleteDownloadDialog* completeDownloadDialog = new CompleteDownloadDialog(SaveTo, SizeDownloadString, download->Url.toString());
	completeDownloadDialog->show();
}

void DownloadControl::UpdateListOfActivePartDownloaders()
{
	int numberOfActivePartDownloaders = ActivePartDownloader_list.count();
	QList<PartDownloader*> ActivePartDownloader_list;
	for (auto partDownloader : PartDownloader_list)
	{
		if (!partDownloader->Get_PartDownload()->IsPartDownloadFinished())
			ActivePartDownloader_list.append(partDownloader);
	}
	
	this->ActivePartDownloader_list = ActivePartDownloader_list;

	if (numberOfActivePartDownloaders != this->ActivePartDownloader_list.count())
		RecentlyUpdatedActivePartDownloader_list = true;
}

void DownloadControl::DownloadForControlSpeed()
{
	if (Is_Downloading)
	{
		QMutex mutex;
		mutex.lock();

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
		mutex.unlock();
	}
}

void DownloadControl::ProcessScheduleControledLimittedSpeed()
{





	speedControlConnection =connect(this, &DownloadControl::FinishedLastControlledSpeedPriod, this, [&](qint64 spentedTime) {
//		qint64 timeSpented = elapsedTimer->elapsed();
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

	//DownloadForControlSpeed();
}
