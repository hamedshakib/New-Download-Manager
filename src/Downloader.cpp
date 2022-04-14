#include "HeaderAndUi/Downloader.h"

Downloader::Downloader(Download* download, QObject* parent)
	:QObject(parent)
{
	this->download = download;
	manager = new QNetworkAccessManager(this);
	connect(this, &Downloader::FinishedThisPeriod, this, [&](qint64 Bytes, qint64 spentedTime) {
		if (spentedTime < 1000) 
		{
			timer.singleShot(1000 - spentedTime, this, &Downloader::DownloadWithSpeedControlled);
			//QTimer::singleShot(1000 - spentedTime, this, &Downloader::DownloadWithSpeedControlled);
		}
		else
		{
			DownloadWithSpeedControlled();
		}
		});
}

Downloader::~Downloader()
{

}

bool Downloader::StartDownload()
{
	qDebug() << "didam5";
	if (!Is_PreparePartDownloaders)
	{
		qDebug() << "didam6";
		if (!ProcessPreparePartDownloaders())
		{
			qDebug() << "didam7";
			return false;
		}
	}

	qDebug() << "didam8";
	Is_Downloading = true;

	elapsedTimer.start();


	for (PartDownloader* partDownloader : PartDownloader_list)
	{
		PartDownload* partDownload = partDownloader->Get_PartDownload();
		partDownload->LastDownloadedByte = partDownload->start_byte + partDownload->PartDownloadFile->size() - 1;
		ProcessPreparePartDownloaderFrompartDownload(partDownloader, partDownload);
		partDownloader->Resume();
	}
	download->downloadStatus = Download::DownloadStatusEnum::Downloading;
	emit Started();
	Is_Downloading = true;
	DownloadWithSpeedControlled();
}

bool Downloader::PauseDownload()
{
	Is_Downloading = false;
	elapsedTimer.restart();
	download->downloadStatus = Download::DownloadStatusEnum::Pause;

	for (PartDownloader* partDownloader :PartDownloader_list)
	{
		PartDownload* partDownload=partDownloader->Get_PartDownload();
		partDownloader->Pause();
		if (!partDownload->PartDownloadFile->isOpen())
			partDownload->PartDownloadFile->open(QIODevice::WriteOnly|QIODevice::Append);
		qDebug() << partDownload->PartDownloadFile->size();
		partDownload->LastDownloadedByte = partDownload->start_byte + partDownload->PartDownloadFile->size() - 1;
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
		ProcessPreparePartDownloaderFrompartDownload(tempPartDownloader, partDownload);
		PartDownloader_list.append(tempPartDownloader);
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
		while (spentedTime<1000 && MaxSpeedOfThisDownloader * 1024>NumberOfBytesDownloadedInLastPeriod)
		{
			qint64 BytesShouldDownload = MaxSpeedOfThisDownloader * 1024 - NumberOfBytesDownloadedInLastPeriod;
			spentedTime += elapsedTimer.elapsed();
			NumberOfBytesDownloadedInLastPeriod += ProcessOfDownload();
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
		qint64 BytesThatBeShouldDownloadPerPartDownload = BytesshouldBeDownloadAtThisPeriod / PartDownloader_list.count();
		for (PartDownloader* partDownloader : PartDownloader_list)
		{
			qint64 DownloadedBytesNow= partDownloader->ReadReadybytes(BytesThatBeShouldDownloadPerPartDownload);
			DownloadedBytesInThisRun += DownloadedBytesNow;
			partDownloader->AddByteToLastDownloadedByte(DownloadedBytesNow);
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
	if (download->downloadStatus==Download::Pause)
	{
		//Not Finished Download Just Paused;
		return;
	}
	qDebug() << "Process Of End Of Downloading";
	Is_Downloading = false;
	QList<PartDownload*> PartDownloads = download->get_PartDownloads();
	QList<QFile*> FilesOfDownload;
	for (PartDownload* partDownload : PartDownloads)
	{
		FilesOfDownload.append(partDownload->PartDownloadFile);
	}



	QFile* NewDownloadFile=DownloadFileWriter::BuildFileFromMultipleFiles(FilesOfDownload, download->get_SavaTo().toString());
	qDebug() << "NewFile Download" << NewDownloadFile;
	qDeleteAll(PartDownloads);
	download->Set_downloadStatus(Download::Completed);







	download->downloadStatus = Download::DownloadStatusEnum::Completed;
	emit CompeletedDownload();
}

Download* Downloader::Get_Download()
{
	return download;
}

void Downloader::HandelFinishedPartDownloadSignalEmitted()
{


	PartDownloader* partDownloader = static_cast<PartDownloader*>(sender());
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

void Downloader::UpdateDownloadInUpdatingInDatabase()
{
	//ToDo 
}
