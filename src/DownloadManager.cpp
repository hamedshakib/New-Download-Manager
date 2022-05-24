#include "HeaderAndUi/DownloadManager.h"

DownloadManager::DownloadManager(QObject *parent)
	: QObject(parent)
{
	if (SettingInteract::GetValue("Download/IsSpeedLimitted").toBool())
	{
		this->SpeedLimit = SettingInteract::GetValue("Download/DefaultSpeedLimit").toInt();
	}

}

DownloadManager::~DownloadManager()
{

}

Download* DownloadManager::CreateDownloadFromDatabase(int download_id)
{
	QThread* DownloadThread = new QThread(this->thread());
	DownloadThread->setObjectName("Download Thread");
	DownloadThread->start();
	Download* download = new Download();
	download->moveToThread(DownloadThread);

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
	QThread* DownloadThread = new QThread(this->thread());
	DownloadThread->setObjectName("Download Thread");
	DownloadThread->start();
	NewDownloadCreater *newDownloadCreater=new NewDownloadCreater();
	newDownloadCreater->moveToThread(DownloadThread);
//	connect(newDownloadCreater, &NewDownloadCreater::CreatedNewDownload, this, &DownloadManager::AddCreatedDownloadToDownloadList);
	connect(newDownloadCreater, &NewDownloadCreater::CreatedNewDownload, this, [&](Download* download) {
		AddCreatedDownloadToDownloadList(download);
		emit CreatedNewDownload(download);
		});



	connect(newDownloadCreater, &NewDownloadCreater::DownloadNow, this, &DownloadManager::CreateDownloadControlAndStartDownload);
	newDownloadCreater->StartProcessOfCreateANewDownload(this);
	return true;
}

void DownloadManager::AddCreatedDownloadToDownloadList(Download* download)
{
	ListOfActiveDownloads.append(download);
}

DownloadControl* DownloadManager::CreateDownloadControl(Download* download)
{

	DownloadControl* downloadControl = new DownloadControl();
	downloadControl->moveToThread(download->thread());
	downloadControl->initDownloadControl(download);
	connect(downloadControl, &DownloadControl::Started, this, [&, download]() {DatabaseManager::UpdateDownloadInStartOfDownloadOnDatabase(download); });
	connect(downloadControl, &DownloadControl::UpdateDownloaded, this, [&, download]() {DatabaseManager::UpdateInDownloadingOnDataBase(download); });
	connect(downloadControl, &DownloadControl::CompeletedDownload, this, [&, download]() {
		/*DatabaseManager::UpdateAllFieldDownloadOnDataBase(download);*/
		DatabaseManager::FinishDownloadOnDatabase(download);
		emit FinishedDownload(download);
		qDebug() << "Finished Update Download"; });
	emit CreatedDownloadControl(downloadControl);
	downloadControl->SetMaxSpeed(SpeedLimit);
	return downloadControl;
}

bool DownloadManager::StartDownload(DownloadControl* downloadControl)
{
	downloadControl->StartDownload();
	return true;
}

bool DownloadManager::CreateDownloadControlAndStartDownload(Download* download)
{
	DownloadControl* downloadControl = CreateDownloadControl(download);
	ListOfDownloadControls.append(downloadControl);
	StartDownload(downloadControl);

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

DownloadControl* DownloadManager::ProcessAchieveDownloadControl(Download* download)
{
	for (DownloadControl* downloadControl : ListOfDownloadControls)
	{
		if (downloadControl->Get_Download() == download)
		{
			return downloadControl;
		}
	}

	//Not Found DownloadControl So Load From Database
	DownloadControl* downloadControl = CreateDownloadControl(download);
	ListOfDownloadControls.append(downloadControl);
	return downloadControl;
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
	DownloadControl* downloadControl = ProcessAchieveDownloadControl(download);

	downloadControl->PauseDownload();
	ListOfDownloadControls.removeOne(downloadControl);
	downloadControl->deleteLater();
	downloadControl = nullptr;


	if (is_RemoveFromDisk)
	{
		QString DownloadFileAddress = download->get_SavaTo().toString();
		QFile file(DownloadFileAddress);
		file.remove();
	}
	DatabaseManager::RemoveDownloadFrom_Queue_Download(download);
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
	DownloadControl* downloadControl = ProcessAchieveDownloadControl(download);

	downloadControl->PauseDownload();
	ListOfDownloadControls.removeOne(downloadControl);
	downloadControl->deleteLater();


	if (is_RemoveFromDisk)
	{
		QString DownloadFileAddress = download->get_SavaTo().toString();
		QFile file(DownloadFileAddress);
		file.remove();
	}
	DatabaseManager::RemoveDownloadFrom_Queue_Download(download);
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
	for(DownloadControl* downloadControl :ListOfDownloadControls)
	{
		downloadControl->PauseDownload();
	}
	return true;
}

bool DownloadManager::CreateNewDownloadsFromBatch(QList<QString> listOfAddress, QString SaveTo, QString Username, QString Password)
{
	NewDownloadCreater* newDownloadCreater = new NewDownloadCreater(this);
	//	connect(newDownloadCreater, &NewDownloadCreater::CreatedNewDownload, this, &DownloadManager::AddCreatedDownloadToDownloadList);
	connect(newDownloadCreater, &NewDownloadCreater::CreatedNewDownload, this, [&](Download* download) {
		AddCreatedDownloadToDownloadList(download);
		emit CreatedNewDownload(download);
		});



	connect(newDownloadCreater, &NewDownloadCreater::DownloadNow, this, &DownloadManager::CreateDownloadControlAndStartDownload);
	newDownloadCreater->StartProcessOfCreateNewDownloadFromBatch(listOfAddress, SaveTo, Username, Password,this);
	return true;
}

bool DownloadManager::SpeedLimitForAllDownload()
{
	for (DownloadControl* downloadControl : ListOfDownloadControls)
	{
		downloadControl->SetMaxSpeed(SpeedLimit);
	}
	return true;
}

bool DownloadManager::Set_SpeedLimit(int maxSpeed)
{
	this->SpeedLimit = maxSpeed;
	if (SpeedLimitForAllDownload())
		return true;
	else
		return false;

}

