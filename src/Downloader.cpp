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
	DownloadWithSpeedControlled();
}

bool Downloader::PauseDownload()
{
	Is_Downloading = false;
	return true;
}

bool Downloader::StopDownload()
{
	Is_Downloading = false;
	return 0;
}

bool Downloader::StartPartDownloader(PartDownloader* partDownloader)
{

	return 0;
}

bool Downloader::ProcessPreparePartDownloaders()
{
	QList<PartDownload*> PartDownloads = download->get_PartDownloads();
	foreach(PartDownload * partDownload,PartDownloads)
	{
		PartDownloader* tempPartDownloader = new PartDownloader(this);
		ProcessPreparePartDownloaderFrompartDownload(tempPartDownloader, partDownload);
		PartDownloader_list.append(tempPartDownloader);
		connect(tempPartDownloader, &PartDownloader::Finished, this, [&]() {
			PartDownloader* partDownloader = static_cast<PartDownloader*>(sender());
			qDebug() << "PartDownloader Is Finsished";
			if (!PartDownloader_list.removeOne(partDownloader))
			{

				qDebug() << "Can not remove PartDownloader!!";
			}
			CheckForFinishedDownload();
			partDownloader->deleteLater();
			qDebug() << "After Delete Later";
			});

	}
	Is_PreparePartDownloaders = true;
	return true;
}

void Downloader::DownloadWithSpeedControlled()
{
	qint64 spentedTime = elapsedTimer.restart();
	qint64 speed=calculatorSpeed.CalculateDownloadSpeed(NumberOfBytesDownloadedInLastPeriod, spentedTime);
	qDebug() << "Speed:" << speed;
	NumberOfBytesDownloadedInLastPeriod = 0;
	if (MaxSpeedOfThisDownloader > 0)
	{
		qDebug() << "didam9";
		while (spentedTime<1000 && MaxSpeedOfThisDownloader * 1024>NumberOfBytesDownloadedInLastPeriod)
		{
			qint64 BytesShouldDownload = MaxSpeedOfThisDownloader * 1024 - NumberOfBytesDownloadedInLastPeriod;
			spentedTime += elapsedTimer.elapsed();
			NumberOfBytesDownloadedInLastPeriod +=ProcessOfDownload();
		}
	}
	else
	{
		NumberOfBytesDownloadedInLastPeriod += ProcessOfDownload();
		calculatorSpeed.CalculateDownloadSpeed(NumberOfBytesDownloadedInLastPeriod, elapsedTimer.elapsed());
	}
	if (Is_Downloading)
	{
		emit FinishedThisPeriod(NumberOfBytesDownloadedInLastPeriod, elapsedTimer.elapsed());
	}
}

qint64 Downloader::ProcessOfDownload(qint64 BytesshouldBeDownloadAtThisPeriod)
{
	/*
	//qint64 BytesThatBeShouldDownloadPerSecondPerPartDownload;
	qint64 DownloadedBytesInThisSecond=0;
	if (MaxSpeedOfThisDownloader > 0) 
	{
		qint64 BytesThatBeShouldDownloadPerSecondPerPartDownload = (qint64)MaxSpeedOfThisDownloader * 1024 / PartDownloader_list.count();
		for (PartDownloader* partDownloader : PartDownloader_list)
		{
			DownloadedBytesInThisSecond += partDownloader->ReadReadybytes(BytesThatBeShouldDownloadPerSecondPerPartDownload);
		}
	}
	else
	{
		for (PartDownloader* partDownloader : PartDownloader_list)
		{
			DownloadedBytesInThisSecond += partDownloader->ReadReadybytes();
		}
	}
	
	calculatorSpeed.CalculateDownloadSpeed(DownloadedBytesInThisSecond,elapsedTimer.restart());
	return DownloadedBytesInThisSecond;
	*/


	qint64 DownloadedBytesInThisRun=0;
	if (BytesshouldBeDownloadAtThisPeriod > 0)
	{
		qint64 BytesThatBeShouldDownloadPerPartDownload = BytesshouldBeDownloadAtThisPeriod / PartDownloader_list.count();
		for (PartDownloader* partDownloader : PartDownloader_list)
		{
			DownloadedBytesInThisRun += partDownloader->ReadReadybytes(BytesThatBeShouldDownloadPerPartDownload);
		}
	}
	else
	{
		//There are not limitation
		for (PartDownloader* partDownloader : PartDownloader_list)
		{
			DownloadedBytesInThisRun += partDownloader->ReadReadybytes();
		}
	}



	return DownloadedBytesInThisRun;
}

bool Downloader::ProcessPreparePartDownloaderFrompartDownload(PartDownloader* partDownloader,PartDownload* partDownload)
{
	qDebug() << "diadam100";
	QNetworkRequest request(download->get_Url());
	qDebug() <<"downloadUrl:"<<download->get_Url();
	request.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);
	QString rangeBytes = QString("bytes=%1-%2").arg(partDownload->start_byte).arg(partDownload->end_byte);
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

	qDebug() << "Endddddddddddddddddddddddddddddddddd";
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

}