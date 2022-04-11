#include "HeaderAndUi/DownloadManager.h"

DownloadManager::DownloadManager(QObject *parent)
	: QObject(parent)
{


}

DownloadManager::~DownloadManager()
{

}


/*

bool DownloadManager::settingUpDataBase()
{
	
	DataBase = QSqlDatabase::addDatabase("QSQLITE");
	DataBase.setDatabaseName("DM.BAK");
	if (!DataBase.open())
		return false;
	

	if (DataBase.tables().isEmpty())
		return false;
	else
		return true;

		
}
*/

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
	connect(newDownloadCreater, &NewDownloadCreater::CreatedNewDownload, this, &DownloadManager::AddCreatedDownloadToDownloadList);
	connect(newDownloadCreater, &NewDownloadCreater::DownloadNow, this, &DownloadManager::CreateDownloaderAndStartDownload);
	newDownloadCreater->StartProcessOfCreateANewDownload(this);
	return true;
}


void DownloadManager::AddCreatedDownloadToDownloadList(Download* download)
{
	ListOfDownloads.append(download);
}


bool DownloadManager::StartDownloader(Downloader* downloader)
{
	downloader->StartDownload();
	return true;
}

bool DownloadManager::CreateDownloaderAndStartDownload(Download* download)
{
	Downloader* downloader = new Downloader(download,this);
	StartDownloader(downloader);


	connect(downloader, &Downloader::CompeletedDownload, this, [&, download]() {
		DatabaseManager::UpdateAllFieldDownloadOnDataBase(download); qDebug() << "Da---------------------"; });


	return true;
}

Download* DownloadManager::ProcessAchieveDownload(int Download_id)
{
	for (Download* download : ListOfDownloads)
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
	Downloader* downloader = new Downloader(download, this);
	ListOfDownloaders.append(downloader);
	return downloader;
}
