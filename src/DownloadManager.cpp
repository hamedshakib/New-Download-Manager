#include "HeaderAndUi/DownloadManager.h"

DownloadManager::DownloadManager(QObject *parent)
	: QObject(parent)
{


}

DownloadManager::~DownloadManager()
{

}

Download* DownloadManager::CreateDownloadFromDatabase(int download_id)
{
	Download* download = new Download(this);
	DatabaseManager manager(this);
	if (manager.LoadDownloadComplete(download_id, download))
	{
		return download;
	}
	else
	{
		Download* download;
		return download;
	}
}

bool DownloadManager::CreateNewDownload()
{
	NewDownloadCreater *newDownloadCreater=new NewDownloadCreater(this);
//	connect(newDownloadCreater, &NewDownloadCreater::CreatedNewDownload, this, &DownloadManager::AddCreatedDownloadToDownloadList);
	connect(newDownloadCreater, &NewDownloadCreater::CreatedNewDownload, this, [&](Download* download) {
		AddCreatedDownloadToDownloadList(download);
		emit CreatedNewDownload(download);
		});



	connect(newDownloadCreater, &NewDownloadCreater::DownloadNow, this, &DownloadManager::CreateDownloaderAndStartDownload);
	newDownloadCreater->StartProcessOfCreateANewDownload(this);
	return true;
}

void DownloadManager::AddCreatedDownloadToDownloadList(Download* download)
{
	ListOfActiveDownloads.append(download);
}

Downloader* DownloadManager::CreateDownloader(Download* download)
{
	Downloader* downloader = new Downloader(download, this);
	connect(downloader, &Downloader::Started, this, [&, download]() {DatabaseManager::UpdateDownloadInStartOfDownloadOnDatabase(download); });
	connect(downloader, &Downloader::SignalForUpdateDownloading, this, [&, download]() {DatabaseManager::UpdateInDownloadingOnDataBase(download); });
	connect(downloader, &Downloader::CompeletedDownload, this, [&, download]() {
		/*DatabaseManager::UpdateAllFieldDownloadOnDataBase(download);*/
		DatabaseManager::FinishDownloadOnDatabase(download);
		emit FinishedDownload(download);
		qDebug() << "Finished Update Download"; });
	emit CreatedDownloader(downloader);
	return downloader;
}

bool DownloadManager::StartDownloader(Downloader* downloader)
{
	downloader->StartDownload();
	return true;
}

bool DownloadManager::CreateDownloaderAndStartDownload(Download* download)
{
	Downloader* downloader = CreateDownloader(download);
	ListOfDownloaders.append(downloader);
	StartDownloader(downloader);

	//connect(downloader, &Downloader::SignalForUpdateDownloading, this, [&, download]() {DatabaseManager::UpdateInDownloadingOnDataBase(download); });
	//connect(downloader, &Downloader::CompeletedDownload, this, [&, download]() {
	//	/*DatabaseManager::UpdateAllFieldDownloadOnDataBase(download);*/
	//	DatabaseManager::FinishDownloadOnDatabase(download);
	//	qDebug() << "Finished Update Download"; });


	return true;
}

Download* DownloadManager::ProcessAchieveDownload(int Download_id)
{
	for (Download* download : ListOfActiveDownloads)
	{
if (download->get_Id() == Download_id)
{
	return download;
}
	}


	//Not Found Download So Load From Database
	Download* downloadWithSpecialId = CreateDownloadFromDatabase(Download_id);
	if (downloadWithSpecialId != nullptr)
	{
		CreatePartDownloadAndPutInDownloadFromDatabase(downloadWithSpecialId);
		AddCreatedDownloadToDownloadList(downloadWithSpecialId);
	}
	return downloadWithSpecialId;
}

Downloader* DownloadManager::ProcessAchieveDownloader(Download* download)
{
	for (Downloader* downloader : ListOfDownloaders)
	{
		if (downloader->Get_Download() == download)
		{
			return downloader;
		}
	}

	//Not Found Downloader So Load From Database
	Downloader* downloader = CreateDownloader(download);
	ListOfDownloaders.append(downloader);
	return downloader;
}

bool DownloadManager::CreatePartDownloadAndPutInDownloadFromDatabase(Download* download)
{
	QList<PartDownload*> ListOfPartDownloadsOfDownload = DatabaseManager::CreatePartDownloadsOfDownload(download->get_Id());
	for (PartDownload* partDownload : ListOfPartDownloadsOfDownload)
	{
		download->AppendPartDownloadToPartDownloadListOfDownload(partDownload);
	}
	return true;
}

bool DownloadManager::ProcessRemoveDownload(int download_id, bool is_RemoveFromDisk)
{
	Download* download = ProcessAchieveDownload(download_id);
	Downloader* downloader = ProcessAchieveDownloader(download);

	downloader->Paused();
	ListOfDownloaders.removeOne(downloader);
	downloader->deleteLater();
	downloader = nullptr;


	if (is_RemoveFromDisk)
	{
		QString DownloadFileAddress = download->get_SavaTo().toString();
		QFile file(DownloadFileAddress);
		file.remove();
	}
	if (DatabaseManager::RemoveDownloadCompleteWithPartDownloadsFromDatabase(download))
	{
		ListOfActiveDownloads.removeOne(download);
		return true;
	}
	else
	{
		return false;
	}
}

bool DownloadManager::ProcessRemoveDownload(Download* download, bool is_RemoveFromDisk)
{
	Downloader* downloader = ProcessAchieveDownloader(download);

	downloader->Paused();
	ListOfDownloaders.removeOne(downloader);
	downloader->deleteLater();


	if (is_RemoveFromDisk)
	{
		QString DownloadFileAddress = download->get_SavaTo().toString();
		QFile file(DownloadFileAddress);
		file.remove();
	}
	if (DatabaseManager::RemoveDownloadCompleteWithPartDownloadsFromDatabase(download))
	{
		ListOfActiveDownloads.removeOne(download);
		return true;
	}
	else
	{
		return false;
	}
}

bool DownloadManager::StopAllDownload()
{
	for(Downloader* downloader:ListOfDownloaders)
	{
		downloader->PauseDownload();
	}
	return true;
}

