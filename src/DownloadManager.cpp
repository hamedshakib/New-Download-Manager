#include "HeaderAndUi/DownloadManager.h"

DownloadManager::DownloadManager(QObject *parent)
	: QObject(parent)
{
	mainWindow.show();

	connect(&mainWindow, &MainWindow::AddNewDownload, this, &DownloadManager::CreateNewDownload);

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

bool DownloadManager::CreateDownloadFromDatabase(int download_id)
{
	Download* download = new Download(this);
	DatabaseManager manager(this);
	if (manager.LoadDownloadComplite(download_id, download))
	{
		return true;
	}
	else
	{
		download->deleteLater();
		return false;
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
	qDebug() << "Didam1";
	return true;
}