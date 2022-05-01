#include "HeaderAndUi/Downloader.h"

//qint64 spentedTime;
Downloader::Downloader(Download* download, QObject* parent)
	:QObject(parent)
{
	this->download = download;
	manager = new QNetworkAccessManager(this);
	connect(this, &Downloader::FinishedThisPeriod, this, [&](qint64 Bytes, qint64 spentedTime) {
		if (spentedTime < 1000) 
		{
			//it's mean pause for other other of one second and wait for next second
			timer.singleShot(995- spentedTime, this, &Downloader::DownloadWithSpeedControlled);
		}
		else
		{

			DownloadWithSpeedControlled();
		}
		});
}

Downloader::~Downloader()
{
	qDebug() << "Downlader Deleted";
	qDeleteAll(PartDownloader_list);
}

bool Downloader::StartDownload()
{
	if (!Is_PreparePartDownloaders)
	{
		if (!ProcessPreparePartDownloaders())
		{
			return false;
		}
	}
	if (CheckForFinishedDownload())
		return true;
	Is_Downloading = true;

	elapsedTimer.start();


	for (PartDownloader* partDownloader : PartDownloader_list)
	{
		PartDownload* partDownload = partDownloader->Get_PartDownload();
		partDownload->LastDownloadedByte = partDownload->start_byte + partDownload->PartDownloadFile->size() - 1;
		if (Is_downloadedCompletePartDownload(partDownload))
		{
			PartDownloader_list.removeOne(partDownloader);
			partDownloader->deleteLater();
			CheckForFinishedDownload();
		}
		else
		{
			ProcessPreparePartDownloaderFrompartDownload(partDownloader, partDownload);
			partDownloader->Resume();
		}
	}
	if (PartDownloader_list.isEmpty())
		return true;
	download->Set_downloadStatus(Download::DownloadStatusEnum::Downloading);
	download->LastTryTime = QDateTime::currentDateTime();
	emit Started();
	Is_Downloading = true;
	elapsedTimer.restart();
	DownloadWithSpeedControlled();
}

bool Downloader::PauseDownload()
{
	Is_Downloading = false;
	elapsedTimer.restart();
	download->Set_downloadStatus(Download::DownloadStatusEnum::Pause);

	for (PartDownloader* partDownloader :PartDownloader_list)
	{
		PartDownload* partDownload=partDownloader->Get_PartDownload();
		partDownloader->Pause();
		if (!partDownload->PartDownloadFile->isOpen())
			partDownload->PartDownloadFile->open(QIODevice::WriteOnly|QIODevice::Append);
		//qDebug() << partDownload->PartDownloadFile->size();
		partDownload->LastDownloadedByte = partDownload->start_byte + partDownload->PartDownloadFile->size() - 1;
		if (Is_downloadedCompletePartDownload(partDownload))
		{
			PartDownloader_list.removeOne(partDownloader);
			partDownloader->deleteLater();
			CheckForFinishedDownload();
		}
	}
	if (PartDownloader_list.isEmpty())
	{
		return true;
	}


	emit Paused();
	return true;
}

bool Downloader::ProcessPreparePartDownloaders()
{
	QList<PartDownload*> PartDownloads = download->get_PartDownloads();
	foreach(PartDownload * partDownload,PartDownloads)
	{
		PartDownloader* tempPartDownloader = new PartDownloader(this);
		PartDownloader_list.append(tempPartDownloader);
		ProcessPreparePartDownloaderFrompartDownload(tempPartDownloader, partDownload);
		Download* download1 = download;


		connect(tempPartDownloader, &PartDownloader::Finished, this, &Downloader::HandelFinishedPartDownloadSignalEmitted);
		connect(tempPartDownloader, &PartDownloader::Paused, this, &Downloader::HandelPausedPartDownloadSignalEmitted);
	}
	Is_PreparePartDownloaders = true;
	return true;
}

bool Downloader::IsDownloading()
{
	return Is_Downloading;
}

void Downloader::DownloadWithSpeedControlled()
{
	qint64 spentedTime = elapsedTimer.restart();
	NumberOfBytesDownloadedInLastPeriod = 0;
	if (MaxSpeedOfThisDownloader > 0)
	{
		while (spentedTime<1000 && MaxSpeedOfThisDownloader*1024 >NumberOfBytesDownloadedInLastPeriod)
		{
			qint64 BytesShouldDownload = MaxSpeedOfThisDownloader*1024 - NumberOfBytesDownloadedInLastPeriod;
			spentedTime += elapsedTimer.elapsed();
			NumberOfBytesDownloadedInLastPeriod += ProcessOfDownload(BytesShouldDownload);

		}
	}
	else
	{
		NumberOfBytesDownloadedInLastPeriod += ProcessOfDownload();
	}

	download->SizeDownloaded += NumberOfBytesDownloadedInLastPeriod;

	qint64 speed = calculatorDownload.CalculateDownloadSpeed(NumberOfBytesDownloadedInLastPeriod, elapsedTimer.elapsed()+ spentedTime);
	QString SpeedString = calculatorDownload.GetSpeedOfDownloadInFormOfString();
	QString TimeLeftString = calculatorDownload.GetTimeLeftOfDownloadInFormOfString(download->DownloadSize - download->SizeDownloaded);
	QString DownloadStatus = calculatorDownload.getStatusForTable(download->SizeDownloaded, download->DownloadSize);
	//emit DownloadedAtAll(download->SizeDownloaded);


	if (SpeedString == "")
	{

	}
	qDebug() << speed;
	qDebug() << SpeedString;
	qDebug() << TimeLeftString;


	emit SignalForUpdateDownloading(DownloadStatus, SpeedString, TimeLeftString);


	if (Is_Downloading)
	{
		emit FinishedThisPeriod(NumberOfBytesDownloadedInLastPeriod, elapsedTimer.elapsed());
	}
}

qint64 Downloader::ProcessOfDownload(qint64 BytesshouldBeDownloadAtThisPeriod)
{
	qint64 DownloadedBytesInThisRun=0;
	if (BytesshouldBeDownloadAtThisPeriod > 0)
	{
		if (PartDownloader_list.count() > 0)
		{
			qint64 BytesThatBeShouldDownloadPerPartDownload = BytesshouldBeDownloadAtThisPeriod / PartDownloader_list.count();
			for (PartDownloader* partDownloader : PartDownloader_list)
			{
				qint64 DownloadedBytesNow = partDownloader->ReadReadybytes(BytesThatBeShouldDownloadPerPartDownload);
				DownloadedBytesInThisRun += DownloadedBytesNow;
				partDownloader->AddByteToLastDownloadedByte(DownloadedBytesNow);
			}
		}
	}
	else
	{
		//There are not limitation
		for (PartDownloader* partDownloader : PartDownloader_list)
		{
			qint64 DownloadedBytesNow = partDownloader->ReadReadybytes();
			DownloadedBytesInThisRun += DownloadedBytesNow;
			partDownloader->AddByteToLastDownloadedByte(DownloadedBytesNow);
		}
	}
	//qDebug() << "Test DownloadedBytesInThisRun:" << DownloadedBytesInThisRun;


	return DownloadedBytesInThisRun;
}

bool Downloader::ProcessPreparePartDownloaderFrompartDownload(PartDownloader* partDownloader,PartDownload* partDownload)
{
	partDownload->LastDownloadedByte= partDownload->start_byte + partDownload->PartDownloadFile->size() - 1;
	if (Is_downloadedCompletePartDownload(partDownload))
	{
		PartDownloader_list.removeOne(partDownloader);
		partDownloader->deleteLater();
		return false;
	}
	QNetworkRequest request(download->get_Url());
	qDebug() <<"downloadUrl:"<<download->get_Url();
	request.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);

	qDebug() << "Started_Byte ::::" << partDownload->start_byte;
	QString rangeBytes = QString("bytes=%1-%2").arg(partDownload->LastDownloadedByte+1).arg(partDownload->end_byte);
	qDebug() << "rangeBytes:" << rangeBytes;
	request.setRawHeader("Range", rangeBytes.toUtf8());

	QNetworkReply* reply = manager->get(request);
	partDownloader->Set_NetworkReply(reply);
	partDownloader->Set_PartDownload(partDownload);
	return true;
}

bool Downloader::CheckForFinishedDownload()
{
	if (PartDownloader_list.isEmpty())
	{
		ProcessOfEndOfDownloading();
		return true;
	}
	else
	{
		return false;
	}
}

void Downloader::ProcessOfEndOfDownloading()
{
	qDebug() << "Process Of End Of Downloading";
	Is_Downloading = false;
	QList<PartDownload*> PartDownloads = download->get_PartDownloads();
	QList<QFile*> FilesOfDownload;
	for (PartDownload* partDownload : PartDownloads)
	{
		qDebug() << partDownload->PartDownloadFile->fileName() << ":" << partDownload->PartDownloadFile->size();
		FilesOfDownload.append(partDownload->PartDownloadFile);
	}




	QFile* NewDownloadFile=DownloadFileWriter::BuildFileFromMultipleFiles(FilesOfDownload, download->get_SavaTo().toString());

	qDebug() << NewDownloadFile->fileName() << ":" << NewDownloadFile->size();
	download->CompletedFile = NewDownloadFile;
	qDeleteAll(PartDownloads);
	download->Set_downloadStatus(Download::Completed);


	//Check for show comlete dialog
	if (SettingInteract::GetValue("Download/ShowCompleteDialog").toBool())
	{
		ShowCompleteDialog(download, NewDownloadFile);
	}


	NewDownloadFile->deleteLater();
	emit CompeletedDownload();
}

Download* Downloader::Get_Download()
{
	return download;
}

void Downloader::HandelFinishedPartDownloadSignalEmitted()
{

	PartDownloader* partDownloader = static_cast<PartDownloader*>(sender());
	qDebug() << partDownloader->Get_PartDownload()->PartDownloadFile->size();
	qDebug() << "PartDownloader Is Finsished";
	if (!PartDownloader_list.removeOne(partDownloader))
	{
		qDebug() << "Can not remove PartDownloader!!";
	}
	CheckForFinishedDownload();
	partDownloader->deleteLater();
}

void Downloader::HandelPausedPartDownloadSignalEmitted()
{
	PartDownloader* partDownloader = static_cast<PartDownloader*>(sender());
	qDebug() << "PartDownloader Is Paused";
}

bool Downloader::ShowCompleteDialog(Download* download,QFile* newDownloadFile)
{
	QString SizeDownloadString = ConverterSizeToSuitableString::ConvertSizeToSuitableString(download->DownloadSize) + QString(" (%1 Bytes)").arg(download->DownloadSize);
	CompleteDownloadDialog* completeDownloadDialog = new CompleteDownloadDialog(newDownloadFile->fileName(), SizeDownloadString, download->Url.toString());
	completeDownloadDialog->show();
	return true;
}

void Downloader::UpdateDownloadInUpdatingInDatabase()
{
	//ToDo 
}

void Downloader::SetMaxSpeed(int maxSpeed)
{
	MaxSpeedOfThisDownloader = maxSpeed;
	emit SpeedChanged(maxSpeed);
}

int Downloader::Get_MaxSpeed()
{
	return MaxSpeedOfThisDownloader;
}

bool Downloader::Is_downloadedCompletePartDownload(PartDownload* partDownload)
{
	if (partDownload->LastDownloadedByte >= partDownload->end_byte)
	{
		qDebug() << "**downloadC last:" << partDownload->LastDownloadedByte << " end:" << partDownload->end_byte;
		return true;
	}
	else
	{
		return false;
	}
}
