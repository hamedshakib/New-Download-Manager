#include "HeaderAndUi/DatabaseManager.h"

DatabaseManager::DatabaseManager(QObject *parent)
	: QObject(parent)
{
}

DatabaseManager::~DatabaseManager()
{
	SettingUpDatabase::get_Database().close();
}

bool DatabaseManager::LoadDownloadComplete(int Download_id,Download* download)
{
	QSqlQuery* query = DatabaseQueryPreparer::PrepareQueryForLoadDownload(Download_id);
	if (DatabaseInteract::ExectionQueryForReadData(query))
	{
		while (query->next())
		{
			if (ProcessDatabaseOutput::ProcessPutLoadedDownloadInformationInDownloadObject(query->record(), download, Download_id))
			{
				delete query;
				return true;
			}
		}
	}
	delete query;
	return false;
}

QStringList DatabaseManager::LoadSuffixsForMimeType(QString MimeType)
{
	QSqlQuery* query = DatabaseQueryPreparer::PrepareQuerySuffixsFromMimeType(MimeType);
	if (DatabaseInteract::ExectionQueryForReadData(query))
	{
		QStringList suffixs;
		while (query->next())
		{
			suffixs.append(ProcessDatabaseOutput::ProcessLoadedSuffixsForMimeType(query->record()));
			
		}
		delete query;
		return suffixs;

	}
	delete query;
}

size_t DatabaseManager::CreateNewDownloadOnDatabase(Download* download)
{
	QSqlQuery* query = DatabaseQueryPreparer::PrepareQueryForCreateNewDownload(download);
	size_t downloadId{};
	if (DatabaseInteract::ExectionQueryForInsertData(query))
	{
		downloadId = query->lastInsertId().toInt();
		delete query;
		return downloadId;
	}
	delete query;
	return downloadId;
}

size_t DatabaseManager::CreateNewPartDownloadOnDatabase(PartDownload* partDownload)
{
	QSqlQuery* query = DatabaseQueryPreparer::PrepareQueryForCreateNewPartDownload(partDownload);
	size_t downloadId{};
	if (DatabaseInteract::ExectionQueryForInsertData(query))
	{
		downloadId= query->lastInsertId().toInt();
		delete query;
		return downloadId;
	}
	delete query;
	return downloadId;
}

bool DatabaseManager::LoadAllDownloadsForMainTable(QStandardItemModel* model)
{
	QSqlQuery* query = DatabaseQueryPreparer::PrepareQueryForLoadDownloadForMainTable();
	if (DatabaseInteract::ExectionQueryForReadData(query))
	{
		while (query->next())
		{
			ProcessDatabaseOutput::ProcessPrepareLoadedInformationForMainTableView(query->record(), model);
		}
		delete query;
		return true;
	}
	delete query;
	return false;
}

bool DatabaseManager::UpdateAllFieldDownloadOnDataBase(Download* download)
{
	QSqlQuery* query = DatabaseQueryPreparer::PrepareQueryForUpdateAllFieldDownload(download);
	if (DatabaseInteract::ExectionQueryForUpdateData(query))
	{
		while (query->next())
		{
		//	ProcessDatabaseOutput::ProcessPrepareLoadedInformationForMainTableView(query->record(), model);
		}
		delete query;
		return true;
	}
	delete query;
	return false;
}

bool DatabaseManager::UpdateDownloadInStartOfDownloadOnDatabase(Download* download)
{
	//ToDo
	QSqlQuery* query= DatabaseQueryPreparer::PrepareQueryForUpdateInStartDownload(download);
	if (DatabaseInteract::ExectionQueryForUpdateData(query))
	{
		delete query;
		return true;
	}
	delete query;
	return false;
}

bool DatabaseManager::UpdateInDownloadingOnDataBase(Download* download)
{
	auto Queries = DatabaseQueryPreparer::PrepareQueriesForUpdateInDownloading(download);
	for (int i = 0; i < Queries.size(); i++)
	{
		DatabaseInteract::ExectionQueryForUpdateData(Queries[i]);
	}


	qDeleteAll(Queries);
	return true;
}

bool DatabaseManager::FinishDownloadOnDatabase(Download* download)
{
	auto Queries = DatabaseQueryPreparer::PrepareQueryForFinishDownload(download);
	for (int i = 0; i < Queries.size(); i++)
	{
		DatabaseInteract::ExectionQueryForUpdateData(Queries[i]);
	}
	qDeleteAll(Queries);
	return true;
}

QList<PartDownload*> DatabaseManager::CreatePartDownloadsOfDownload(int Download_id)
{
	QList<PartDownload*> ListOfPartDownloadsOfDownload;
	QSqlQuery* query = DatabaseQueryPreparer::PrepareQueryForLoadPartDownloadOfDownload(Download_id);
	if (DatabaseInteract::ExectionQueryForReadData(query))
	{
		while (query->next())
		{
				PartDownload* partDownload = new PartDownload(nullptr);
				if (ProcessDatabaseOutput::ProcessPutLoadedPartDownloadInInPartDownloadObject(query->record(), partDownload, Download_id))
				{
					ListOfPartDownloadsOfDownload.append(partDownload);
				}

		}
	}
	delete query;
	return ListOfPartDownloadsOfDownload;
}

bool DatabaseManager::RemoveDownloadCompleteWithPartDownloadsFromDatabase(Download* download)
{
	bool Is_Success = true;
	QSqlQuery* PartDownloadsDeletequery= DatabaseQueryPreparer::PrepareQueryForRemovePartDownloadsOfDownloadFromDatabase(download);
	if (DatabaseInteract::ExectionQueryForDeleteData(PartDownloadsDeletequery))
	{
		QSqlQuery* DownloadDeletequery = DatabaseQueryPreparer::PrepareQueryForRemoveDownloadFromDatabase(download);
		if (DatabaseInteract::ExectionQueryForDeleteData(DownloadDeletequery))
		{
			delete DownloadDeletequery;
		}
		else
		{
			delete DownloadDeletequery;
			Is_Success = false;
		}


		delete PartDownloadsDeletequery;
	}
	else
	{
		delete PartDownloadsDeletequery;
		Is_Success = false;
	}
	return Is_Success;
}

bool DatabaseManager::LoadAllQueues(QList<Queue*>& listOfQueues, QObject* object)
{
	bool Is_Success = true;
	QSqlQuery* GeneralInformationOfQueueQuery = DatabaseQueryPreparer::PrepareQueryForLoadAllQueuesGeneralInformationFromDatabase();
	if (DatabaseInteract::ExectionQueryForReadData(GeneralInformationOfQueueQuery))
	{
		while (GeneralInformationOfQueueQuery->next())
		{
			
			Queue *queue = new Queue(object);
			if (ProcessDatabaseOutput::ProcessPutLoadedQueueInformationInQueueObject(GeneralInformationOfQueueQuery->record(), queue))
			{
				QSqlQuery* downloadIdsOfQueueQuery = DatabaseQueryPreparer::PrepareQueryForGetDownloadIdOfQueue(queue);
				if (DatabaseInteract::ExectionQueryForReadData(downloadIdsOfQueueQuery))
				{
					while (downloadIdsOfQueueQuery->next())
					{
						ProcessDatabaseOutput::PutDownloadIdInQueueDownloadList(downloadIdsOfQueueQuery->record(), queue);
					}
				}
				else
				{
					delete downloadIdsOfQueueQuery;
					Is_Success = false;
				}
			}
			listOfQueues.append(queue);
		}
		delete GeneralInformationOfQueueQuery;

	}
	else
	{
		delete GeneralInformationOfQueueQuery;
		Is_Success = false;
	}
	return Is_Success;
}

bool DatabaseManager::RemoveDownloadFromQueueOnDatabase(Download* download)
{
	QSqlQuery* query = DatabaseQueryPreparer::PrepareQueryForRemoveDownloadFromQueueOnDatabase(download);
	if (DatabaseInteract::ExectionQueryForUpdateData(query))
	{
		delete query;
		return true;
	}
	delete query;
	return false ;
}

size_t DatabaseManager::CreateNewQueueOnDatabase(Queue* queue)
{
	//ToDo
	size_t queue_id;
	QSqlQuery* query = DatabaseQueryPreparer::PrepareQueryForCreateNewQueue(queue);
	if (DatabaseInteract::ExectionQueryForUpdateData(query))
	{
		queue_id = query->lastInsertId().toInt();
	}
	delete query;
	return queue_id;
}

bool DatabaseManager::RemoveQueueFromDatabase(Queue* queue)
{
	//ToDo
	QSqlQuery* query = DatabaseQueryPreparer::PrepareQueryForRemoveQueueFromDatabase(queue);
	if (DatabaseInteract::ExectionQueryForDeleteData(query))
	{
		delete query;
		return true;
	}
	delete query;
	return false;
}

bool DatabaseManager::ExitDownloadFromQueue(Download* download)
{
	//ToDo
	return 0;
}

bool DatabaseManager::ExitAllDownloadFromQueue(Queue* queue)
{
	//ToDo
	QSqlQuery* query = DatabaseQueryPreparer::PrepareQueryForExitAllDownloadFromQueue(queue);
	if (DatabaseInteract::ExectionQueryForUpdateData(query))
	{
		delete query;
		return true;
	}
	delete query;
	return false;
}

bool DatabaseManager::AddDownloadToQueueOnDatabase(Download* Download, Queue* queue)
{
	QSqlQuery* query = DatabaseQueryPreparer::PrepareQueryFroAddDownloadToQueue(Download,queue);
	if (DatabaseInteract::ExectionQueryForUpdateData(query))
	{
		delete query;
		return true;
	}
	delete query;
	return false;
}

bool DatabaseManager::UpdateTimeQueueEvents(Queue* queue)
{
	//TODO
	QSqlQuery* query = DatabaseQueryPreparer::PrepareQueryFroEditTimeEventsOfQueue(queue);
	if (DatabaseInteract::ExectionQueryForUpdateData(query))
	{
		delete query;
		return true;
	}
	delete query;
	return false;
}

bool DatabaseManager::AddDownloadTo_Queue_Download(Queue* queue, Download* download)
{
	QSqlQuery* query = DatabaseQueryPreparer::PrepareQueryForAddDownloadTo_Queue_Download(queue,download);
	if (DatabaseInteract::ExectionQueryForInsertData(query))
	{
		delete query;
		return true;
	}
	delete query;
	return false;
}

bool DatabaseManager::RemoveDownloadFrom_Queue_Download(Download* download)
{
	QSqlQuery* query = DatabaseQueryPreparer::PrepareQueryForRemoveDownloadFrom_Queue_Download(download);
	if (DatabaseInteract::ExectionQueryForDeleteData(query))
	{
		delete query;
		return true;
	}
	delete query;
	return false;
}

bool DatabaseManager::DecreaseDownloadNumberOfQueueListForNextDownloadInQueueListOn_Queue_Download(Queue* queue, int DownloadNumberInQueueList)
{
	QSqlQuery* query = DatabaseQueryPreparer::PrepareQueryForDecreaseDownloadNumberOfQueueListForNextDownloadInQueueListOn_Queue_Download(queue, DownloadNumberInQueueList);
	if (DatabaseInteract::ExectionQueryForUpdateData(query))
	{
		delete query;
		return true;
	}
	delete query;
	return false;
}

bool DatabaseManager::MoveDownloadIn_Queue_Download(Queue* queue, Download* download, int moveNumber)
{
	QSqlQuery* query = DatabaseQueryPreparer::PrepareQueryForMoveDownloadIn_Queue_Download(queue, download,moveNumber);
	if (DatabaseInteract::ExectionQueryForUpdateData(query))
	{
		delete query;
		return true;
	}
	delete query;
	return false;
}
bool DatabaseManager::ExitAllDownloadFrom_Queue_Download(Queue* queue)
{
	QSqlQuery* query = DatabaseQueryPreparer::PrepareQueryForExitAllDownloadFrom_Queue_Download(queue);
	if (DatabaseInteract::ExectionQueryForDeleteData(query))
	{
		delete query;
		return true;
	}
	delete query;
	return false;
}

size_t DatabaseManager::GetNumberDownloadInListOfQueue(Download* download)
{
	size_t numberInList;
	QSqlQuery* query = DatabaseQueryPreparer::PrepareQueryForGetNumberInListDownload(download);
	if (DatabaseInteract::ExectionQueryForReadData(query))
	{
		if (query->next())
		{
			numberInList = query->value(0).toInt();
		}
	}
	delete query;
	return numberInList;
}

size_t DatabaseManager::GetturnInIdOfDownload(Queue* queue,int NumnberDownloadInList)
{
	size_t queue_id;
	QSqlQuery* query = DatabaseQueryPreparer::PrepareQueryForGetturnInIdOfDownload(queue, NumnberDownloadInList);
	if (DatabaseInteract::ExectionQueryForReadData(query))
	{
		if (query->next())
		{
			queue_id = query->value(0).toInt();
		}
	}
	delete query;
	return queue_id;
}

void DatabaseManager::LoadDownloadInformationOfQueueForScheduleTreeWidget(QList<QTreeWidgetItem*>& TreeWidgetItems, Queue* queue)
{
	QSqlQuery* query = DatabaseQueryPreparer::PrepareQueryForLoadDownloadInformationOfQueueForScheduleTreeWidget(queue);
	if (DatabaseInteract::ExectionQueryForReadData(query))
	{
		while(query->next())
		{
			QTreeWidgetItem* item = new QTreeWidgetItem();
			ProcessDatabaseOutput::PutDownloadInformationOfQueueForScheduleTreeWidget(query->record(), item);
			TreeWidgetItems.append(item);
		}
	}
	delete query;
}

bool DatabaseManager::UpdateNumberOfDownloadAtSameTimeOfQueue(Queue* queue)
{
	QSqlQuery* query = DatabaseQueryPreparer::PrepareQueryForUpdateNumberOfDownloadAtSameTimeOfQueue(queue);
	if (DatabaseInteract::ExectionQueryForUpdateData(query))
	{
		delete query;
		return true;
	}
	delete query;
	return false;
}