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
			});

	}
	Is_PreparePartDownloaders = true;
	return true;
}

void Downloader::DownloadWithSpeedControlled()
{
	qint64 spentedTime = elapsedTimer.restart();
	qint64 speed= calculatorDownload.CalculateDownloadSpeed(NumberOfBytesDownloadedInLastPeriod, spentedTime);
	QString SpeedString = calculatorDownload.GetSpeedOfDownloadInFormOfString();
	QString TimeLeftString = calculatorDownload.GetTimeLeftOfDownloadInFormOfString(download->DownloadSize-download->SizeDownloaded);
	QString DownloadStatus = calculatorDownload.getStatusForTable(download->SizeDownloaded, download->DownloadSize);
	NumberOfBytesDownloadedInLastPeriod = 0;
	if (MaxSpeedOfThisDownloader > 0)
	{
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
	}

	download->SizeDownloaded += NumberOfBytesDownloadedInLastPeriod;
	emit DownloadedAtAll(download->SizeDownloaded);
	//qDebug() << SpeedString << " " << TimeLeftString;
	emit SignalForUpdateDownloading(DownloadStatus,SpeedString, TimeLeftString);

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
	qDebug() << "diadam100";
	QNetworkRequest request(download->get_Url());
	qDebug() <<"downloadUrl:"<<download->get_Url();
	request.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);
	
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
		FilesOfDownload.append(partDownload->PartDownloadFile);
	}



	QFile* NewDownloadFile=DownloadFileWriter::BuildFileFromMultipleFiles(FilesOfDownload, download->get_SavaTo().toString());
	qDebug() << "NewFile Download" << NewDownloadFile;
	qDeleteAll(PartDownloads);
	download->Set_downloadStatus(Download::Completed);
	emit CompeletedDownload();
}

Download* Downloader::Get_Download()
{
	return download;
}