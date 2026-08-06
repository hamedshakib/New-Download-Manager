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
	//Resume/load-from-database runs on the main thread. The download, its parts and
	//its DownloadController all share this thread, which avoids the cross-thread
	//QFile/PartDownload access that happened when a separate worker thread was used.
	Download* download = new Download();

	DatabaseManager manager(this);
	if (manager.LoadDownloadComplete(download_id, download))
	{
		return download;
	}
	else
	{
		//The download could not be loaded.
		return nullptr;
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



	connect(newDownloadCreater, &NewDownloadCreater::DownloadNow, this, &DownloadManager::CreateDownloadControllerAndStartDownload);
	newDownloadCreater->StartProcessOfCreateANewDownload(this);
	return true;
}

void DownloadManager::AddCreatedDownloadToDownloadList(Download* download)
{
	ListOfActiveDownloads.append(download);
}

DownloadController* DownloadManager::CreateDownloadController(Download* download)
{

	DownloadController* downloadController = new DownloadController();
	downloadController->moveToThread(download->thread());
	downloadController->initDownloadController(download);

	//Worker-thread lifecycle: when the DownloadController (which lives on the
	//download's worker thread) is destroyed, quit that thread; the QThread object
	//then deletes itself when it finishes. This stops the leaked "Download Thread"
	//QThread objects. The main thread (resume path) is never touched.
	QThread* dlThread = download->thread();
	if (dlThread != nullptr && dlThread != this->thread())
	{
		QObject::connect(downloadController, &QObject::destroyed, dlThread, [dlThread]() {
			dlThread->quit();
		});
		QObject::connect(dlThread, &QThread::finished, dlThread, &QObject::deleteLater);
	}

	connect(downloadController, &DownloadController::DownloadStarted, this, [&, download]() {DatabaseManager::UpdateDownloadInStartOfDownloadOnDatabase(download); });
	connect(downloadController, &DownloadController::UpdateDownloaded, this, [&, download]() {DatabaseManager::UpdateInDownloadingOnDataBase(download); });
	connect(downloadController, &DownloadController::DownloadCompleted, this, [&, download]() {
		/*DatabaseManager::UpdateAllFieldDownloadOnDataBase(download);*/
		DatabaseManager::FinishDownloadOnDatabase(download);
		emit FinishedDownload(download);
		qDebug() << "Finished Update Download"; });
	emit CreatedDownloadController(downloadController);
	downloadController->SetMaxSpeed(SpeedLimit);
	return downloadController;
}

bool DownloadManager::StartDownload(DownloadController* DownloadController)
{
	DownloadController->StartDownload();
	return true;
}

bool DownloadManager::CreateDownloadControllerAndStartDownload(Download* download)
{
	DownloadController* DownloadController = CreateDownloadController(download);
	ListOfDownloadControllers.append(DownloadController);
	StartDownload(DownloadController);

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

DownloadController* DownloadManager::ProcessAchieveDownloadController(Download* download)
{
	for (DownloadController* DownloadController : ListOfDownloadControllers)
	{
		if (DownloadController->Get_Download() == download)
		{
			return DownloadController;
		}
	}

	//Not Found DownloadController So Load From Database
	DownloadController* DownloadController = CreateDownloadController(download);
	ListOfDownloadControllers.append(DownloadController);
	return DownloadController;
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
	DownloadController* DownloadController = ProcessAchieveDownloadController(download);

	DownloadController->PauseDownload();
	ListOfDownloadControllers.removeOne(DownloadController);
	DownloadController->deleteLater();
	DownloadController = nullptr;


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
	DownloadController* DownloadController = ProcessAchieveDownloadController(download);

	DownloadController->PauseDownload();
	ListOfDownloadControllers.removeOne(DownloadController);
	DownloadController->deleteLater();


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
	for(DownloadController* DownloadController :ListOfDownloadControllers)
	{
		DownloadController->PauseDownload();
	}
	return true;
}

bool DownloadManager::CreateNewDownloadsFromBatch(QList<QString> listOfAddress, QString SaveTo, QString Username, QString Password)
{
	QThread* DownloadThread = new QThread(this->thread());
	DownloadThread->setObjectName("Download Thread");
	DownloadThread->start();
	NewDownloadCreater* newDownloadCreater = new NewDownloadCreater();
	newDownloadCreater->moveToThread(DownloadThread);
	//	connect(newDownloadCreater, &NewDownloadCreater::CreatedNewDownload, this, &DownloadManager::AddCreatedDownloadToDownloadList);
	connect(newDownloadCreater, &NewDownloadCreater::CreatedNewDownload, this, [&](Download* download) {
		AddCreatedDownloadToDownloadList(download);
		emit CreatedNewDownload(download);
		});



	connect(newDownloadCreater, &NewDownloadCreater::DownloadNow, this, &DownloadManager::CreateDownloadControllerAndStartDownload);
	newDownloadCreater->StartProcessOfCreateNewDownloadFromBatch(listOfAddress, SaveTo, Username, Password,this);
	return true;
}

bool DownloadManager::SpeedLimitForAllDownload()
{
	for (DownloadController* DownloadController : ListOfDownloadControllers)
	{
		DownloadController->SetMaxSpeed(SpeedLimit);
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

