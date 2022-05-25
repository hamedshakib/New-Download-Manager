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
	//timer->start(500);
	connect(timer, &QTimer::timeout, this, &DownloadControl::TimerTimeOut);
	connect(this, &DownloadControl::CompeletedDownload, this, &DownloadControl::ProcessForShowDownloadCompleteDialog);
	elapsedTimer = new QElapsedTimer();
}

bool DownloadControl::StartDownload()
{
	this->Is_Downloading = true;

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


	for (PartDownloader* partDownloader : PartDownloader_list)
	{
		PartDownload* partDownload = partDownloader->Get_PartDownload();
		partDownload->UpdatePartDownloadLastDownloadedByte();
		if (!partDownload->IsPartDownloadFinished())
		{
			ProcessPreparePartDownloaderFromPartdownload(partDownloader, partDownload);
			StartPartDownloader(partDownloader);
		}
		
	}

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
	timer->start(500);

	//[deprature]
	//DownloadWithSpeedControlled();

	//ProcessDownload();
}

bool DownloadControl::PauseDownload()
{
	this->Is_Downloading = false;
	for (PartDownloader* partDownloader : PartDownloader_list)
	{
		PartDownload* partDownload = partDownloader->Get_PartDownload();
		if (!partDownload->IsPartDownloadFinished())
		{
			StopPartDownloader(partDownloader);
		}
		partDownload->UpdatePartDownloadLastDownloadedByte();

	}
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
	SpeedChanged(maxSpeed);
}

int DownloadControl::Get_MaxSpeed()
{
	return this->MaxSpeed;
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

			PartDownloader_list.append(tempPartDownloader);
			tempPartDownloader->initPartDownlolader(partDownload, 500000000);
			qDebug() << partDownload->thread();

			Download* download1 = download;
			connect(tempPartDownloader, &PartDownloader::Started, this, &DownloadControl::HandelStartedPartDownloaderSignalEmitted);
			connect(tempPartDownloader, &PartDownloader::Paused, this, &DownloadControl::HandelPausedPartDownloaderSignalEmitted);
			connect(tempPartDownloader, &PartDownloader::FinishedReadyReadBytes, this, &DownloadControl::HandelFinishedReadyReadBytesPartDownloaderSignalEmitted);
			connect(tempPartDownloader, &PartDownloader::Finished, this, &DownloadControl::HandelFinishedPartDownloaderSignalEmitted);
			connect(tempPartDownloader, &PartDownloader::DownloadedBytes, this, &DownloadControl::HandelDownloadedBytesPartDownloaderSignalEmitted);
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


	qDebug() << "downloadUrl:" << download->get_Url();
	request.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);

	qDebug() << "Started_Byte ::::" << partDownload->start_byte;
	QString rangeBytes = QString("bytes=%1-%2").arg(partDownload->LastDownloadedByte + 1).arg(partDownload->end_byte);
	qDebug() << "rangeBytes:" << rangeBytes;
	request.setRawHeader("Range", rangeBytes.toUtf8());

	qDebug() << manager->thread()->objectName();
	qDebug() << QThread::currentThread()->objectName();
	qDebug() << this->thread();

	QNetworkReply* reply = manager->get(request);
	qDebug() << reply->bytesAvailable();
	
	reply->moveToThread(download->thread());
	qDebug() << reply->thread()->objectName();
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

void DownloadControl::HandelFinishedReadyReadBytesPartDownloaderSignalEmitted()
{

}

void DownloadControl::HandelFinishedPartDownloaderSignalEmitted()
{
	if (CheckDownloadFinished())
	{
		ProcessFinishDownload();
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
	for (PartDownloader* partDownloader : PartDownloader_list)
	{
		qDebug() << "Count PartDownloaders:" << PartDownloader_list.count();
		PartDownload* partDownload=partDownloader->Get_PartDownload();
		partDownload->UpdatePartDownloadLastDownloadedByte();
		if (!partDownload->IsPartDownloadFinished())
		{
			return false;
		}
	}
	return true;
}

bool DownloadControl::ProcessFinishDownload()
{
	qDebug() << "Process Of End Of Downloading";
	Is_Downloading = false;
	timer->stop();
	QList<PartDownload*> PartDownloads = download->get_PartDownloads();
	QList<QFile*> FilesOfDownload;
	for (PartDownload* partDownload : PartDownloads)
	{
		qDebug() << partDownload->PartDownloadFile->fileName() << ":" << partDownload->PartDownloadFile->size();
		FilesOfDownload.append(partDownload->PartDownloadFile);
	}




	QFile* NewDownloadFile = DownloadFileWriter::BuildFileFromMultipleFiles(FilesOfDownload, download->get_SavaTo().toString());

	qDebug() << NewDownloadFile->fileName() << ":" << NewDownloadFile->size();
	download->CompletedFile = NewDownloadFile;
	qDeleteAll(PartDownloads);
	download->Set_downloadStatus(Download::Completed);


	//Check for show comlete dialog
	if (SettingInteract::GetValue("Download/ShowCompleteDialog").toBool())
	{
		Download* download1 = download;
		QMutex mutux;
		mutux.lock();
//		QMetaObject::invokeMethod(qApp, [&, download1, NewDownloadFile]() {ShowCompleteDialog(download1, NewDownloadFile); }, Qt::QueuedConnection);
		mutux.unlock();
	}


	NewDownloadFile->deleteLater();
	emit CompeletedDownload();
	return true;
}

void DownloadControl::TimerTimeOut()
{
	qint64 downloadedByte = this->NumberOfBytesDownloadedInLastPeriod;
	this->NumberOfBytesDownloadedInLastPeriod = 0;

	qint64 speed = calculatorDownload.CalculateDownloadSpeed(downloadedByte, 500);
	QString SpeedString = calculatorDownload.GetSpeedOfDownloadInFormOfString();
	qDebug() << "Speed:" << SpeedString;
	QString TimeLeftString = calculatorDownload.GetTimeLeftOfDownloadInFormOfString(download->DownloadSize - download->SizeDownloaded);
	QString DownloadStatus = calculatorDownload.getStatusForTable(download->SizeDownloaded, download->DownloadSize);
	//emit DownloadedAtAll(download->SizeDownloaded);
	QList<qint64> DownloadedBytesEachPartDownloadList;
	for (PartDownload* partDownload : download->get_PartDownloads())
	{
		DownloadedBytesEachPartDownloadList.append(partDownload->LastDownloadedByte - partDownload->start_byte + 1);
	}

	emit UpdateDownloaded(DownloadStatus, SpeedString,TimeLeftString,DownloadedBytesEachPartDownloadList);
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
