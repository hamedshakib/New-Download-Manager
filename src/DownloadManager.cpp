#include "HeaderAndUi/DownloadManager.h"
#include "qdebug.h"

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
	// Stop all downloads before cleanup
	StopAllDownload();
	
	// Clean up all download controls
	QMutexLocker locker(&mutex);
	qDeleteAll(ListOfDownloadControls);
	ListOfDownloadControls.clear();
	
	// Clean up all active downloads
	qDeleteAll(ListOfActiveDownloads);
	ListOfActiveDownloads.clear();
}

Download* DownloadManager::CreateDownloadFromDatabase(int download_id)
{
    QMutexLocker locker(&mutex);
	QThread* DownloadThread = new QThread(this->thread());
	DownloadThread->setObjectName("Download Thread");
	DownloadThread->start();
	Download* download = new Download();
	download->moveToThread(DownloadThread);

	DatabaseManager manager(this);
	if (manager.LoadDownloadComplete(download_id, download))
	{
		connect(DownloadThread, &QThread::finished, DownloadThread, &QThread::deleteLater);
		// When download is deleted, also delete its thread
		connect(download, &Download::destroyed, DownloadThread, &QThread::quit);
		return download;
	}
	else
	{
		// Return nullptr if download loading failed
		download->deleteLater();
		DownloadThread->quit();
		DownloadThread->wait(1000);  // Wait up to 1 second for thread to finish
		DownloadThread->deleteLater();
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
	
	// Connect cleanup when thread finishes
	connect(DownloadThread, &QThread::finished, DownloadThread, &QThread::deleteLater);
	connect(DownloadThread, &QThread::finished, newDownloadCreater, &NewDownloadCreater::deleteLater);
	
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
    QMutexLocker locker(&mutex);
	ListOfActiveDownloads.append(download);
}

DownloadControl* DownloadManager::CreateDownloadControl(Download* download)
{
    QMutexLocker locker(&mutex);
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
	if (!downloadControl) {
		qCritical() << "DownloadManager: DownloadControl pointer is null";
		return false;
	}
	
	downloadControl->StartDownload();
	return downloadControl->IsDownloading();
}

bool DownloadManager::CreateDownloadControlAndStartDownload(Download* download)
{
    QMutexLocker locker(&mutex);
	DownloadControl* downloadControl = CreateDownloadControl(download);
	ListOfDownloadControls.append(downloadControl);
	StartDownload(downloadControl);

	return true;
}

Download* DownloadManager::ProcessAchieveDownload(int Download_id)
{
    QMutexLocker locker(&mutex);
	for (Download* download : ListOfActiveDownloads)
	{
		if (download && download->get_Id() == Download_id)
		{
			return download;
		}
	}


	//Not Found Download So Load From Database
	Download* downloadWithSpecialId = CreateDownloadFromDatabase(Download_id);
	if (downloadWithSpecialId != nullptr)
	{
		if (CreatePartDownloadAndPutInDownloadFromDatabase(downloadWithSpecialId))
		{
			AddCreatedDownloadToDownloadList(downloadWithSpecialId);
		}
		else
		{
			qCritical() << "Failed to create part downloads for download ID:" << Download_id;
			downloadWithSpecialId->deleteLater();
			return nullptr;
		}
	}
	else
	{
		qCritical() << "Download not found in database with ID:" << Download_id;
	}
	return downloadWithSpecialId;
}

DownloadControl* DownloadManager::ProcessAchieveDownloadControl(Download* download)
{
    QMutexLocker locker(&mutex);
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
    QMutexLocker locker(&mutex);
	Download* download = ProcessAchieveDownload(download_id);
	if (!download)
	{
		qCritical() << "Download not found with ID:" << download_id << "for removal";
		return false;
	}
	
	DownloadControl* downloadControl = ProcessAchieveDownloadControl(download);
	if (!downloadControl)
	{
		qCritical() << "DownloadControl not found for download ID:" << download_id;
		return false;
	}

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
	if (!download)
	{
		qCritical() << "Download pointer is null in ProcessRemoveDownload";
		return false;
	}
    
    QMutexLocker locker(&mutex);
	DownloadControl* downloadControl = ProcessAchieveDownloadControl(download);
	if (!downloadControl)
	{
		qCritical() << "DownloadControl not found for download ID:" << download->get_Id();
		return false;
	}

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
    QMutexLocker locker(&mutex);
	for(DownloadControl* downloadControl :ListOfDownloadControls)
	{
		downloadControl->PauseDownload();
	}
	return true;
}

bool DownloadManager::SpeedLimitForAllDownload()
{
    QMutexLocker locker(&mutex);
	for (DownloadControl* downloadControl : ListOfDownloadControls)
	{
		downloadControl->SetMaxSpeed(SpeedLimit);
	}
	return true;
}

bool DownloadManager::Set_SpeedLimit(int maxSpeed)
{
    QMutexLocker locker(&mutex);
 	this->SpeedLimit = maxSpeed;
 	if (SpeedLimitForAllDownload())
 		return true;
 	else
 		return false;

 }

bool DownloadManager::CreateNewDownloadsFromBatch(QList<QString> listOfAddress, QString SaveTo, QString Username, QString Password)
{
	QThread* DownloadThread = new QThread(this->thread());
	DownloadThread->setObjectName("Download Thread");
	DownloadThread->start();
	NewDownloadCreater* newDownloadCreater = new NewDownloadCreater();
	newDownloadCreater->moveToThread(DownloadThread);
	
	// Connect cleanup when thread finishes
	connect(DownloadThread, &QThread::finished, DownloadThread, &QThread::deleteLater);
	connect(DownloadThread, &QThread::finished, newDownloadCreater, &NewDownloadCreater::deleteLater);
	
	//	connect(newDownloadCreater, &NewDownloadCreater::CreatedNewDownload, this, &DownloadManager::AddCreatedDownloadToDownloadList);
	connect(newDownloadCreater, &NewDownloadCreater::CreatedNewDownload, this, [&](Download* download) {
		AddCreatedDownloadToDownloadList(download);
		emit CreatedNewDownload(download);
		});



	connect(newDownloadCreater, &NewDownloadCreater::DownloadNow, this, &DownloadManager::CreateDownloadControlAndStartDownload);
	newDownloadCreater->StartProcessOfCreateNewDownloadFromBatch(listOfAddress, SaveTo, Username, Password,this);
	return true;
}

