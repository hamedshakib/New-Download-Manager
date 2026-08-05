#include "HeaderAndUi/DatabaseManager.h"
#include "qdebug.h"
#include "HeaderAndUi/SettingUpDatabase.h"
#include "HeaderAndUi/DatabaseInteract.h"

// Helper function to create a thread-safe query with the thread-specific database
static QSqlQuery* createThreadSafeQuery(const QString& queryText = QString())
{
    QSqlQuery* query = new QSqlQuery(SettingUpDatabase::getThreadDatabase());
    if (!queryText.isEmpty()) {
        query->setQuery(queryText);
    }
    return query;
}

DatabaseManager::DatabaseManager(QObject *parent)
	: QObject(parent)
{
}

DatabaseManager::~DatabaseManager()
{
	// Note: The database connection is managed by SettingUpDatabase singleton
	// We should NOT close it here as it may be used by other threads/objects
	// The database will be closed automatically when the application exits
}

bool DatabaseManager::LoadDownloadComplete(int Download_id, Download* download)
{
	QSqlQuery* query = createThreadSafeQuery();
	if (!DatabaseQueryPreparer::PrepareQueryForLoadDownload(query, Download_id)) {
		delete query;
		return false;
	}
	
	if (DatabaseInteract::ExectionQueryForReadData(query)) {
		while (query->next()) {
			if (ProcessDatabaseOutput::ProcessPutLoadedDownloadInformationInDownloadObject(query->record(), download, Download_id)) {
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
	QSqlQuery* query = createThreadSafeQuery();
	if (!DatabaseQueryPreparer::PrepareQuerySuffixsFromMimeType(query, MimeType)) {
		delete query;
		return QStringList();
	}
	
	QStringList suffixs;
	if (DatabaseInteract::ExectionQueryForReadData(query)) {
		while (query->next()) {
			suffixs.append(ProcessDatabaseOutput::ProcessLoadedSuffixsForMimeType(query->record()));
		}
	}
	delete query;
	return suffixs;
}

size_t DatabaseManager::CreateNewDownloadOnDatabase(Download* download)
{
	QSqlQuery* query = createThreadSafeQuery();
	if (!DatabaseQueryPreparer::PrepareQueryForCreateNewDownload(query, download)) {
		delete query;
		return 0;
	}
	
	size_t downloadId{};
	if (DatabaseInteract::ExectionQueryForInsertData(query)) {
		downloadId = query->lastInsertId().toInt();
	}
	delete query;
	return downloadId;
}

size_t DatabaseManager::CreateNewPartDownloadOnDatabase(PartDownload* partDownload)
{
	QSqlQuery* query = createThreadSafeQuery();
	if (!DatabaseQueryPreparer::PrepareQueryForCreateNewPartDownload(query, partDownload)) {
		delete query;
		return 0;
	}
	
	size_t downloadId{};
	if (DatabaseInteract::ExectionQueryForInsertData(query)) {
		downloadId = query->lastInsertId().toInt();
	}
	delete query;
	return downloadId;
}

bool DatabaseManager::LoadAllDownloadsForMainTable(QStandardItemModel* model)
{
	QSqlQuery* query = createThreadSafeQuery();
	if (!DatabaseQueryPreparer::PrepareQueryForLoadDownloadForMainTable(query)) {
		delete query;
		return false;
	}
	
	if (DatabaseInteract::ExectionQueryForReadData(query)) {
		while (query->next()) {
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
	QSqlQuery* query = createThreadSafeQuery();
	if (!DatabaseQueryPreparer::PrepareQueryForUpdateAllFieldDownload(query, download)) {
		delete query;
		return false;
	}
	
	if (DatabaseInteract::ExectionQueryForUpdateData(query)) {
		delete query;
		return true;
	}
	delete query;
	return false;
}

bool DatabaseManager::UpdateDownloadInStartOfDownloadOnDatabase(Download* download)
{
	QSqlQuery* query = createThreadSafeQuery();
	if (!DatabaseQueryPreparer::PrepareQueryForUpdateInStartDownload(query, download)) {
		delete query;
		return false;
	}
	
	if (DatabaseInteract::ExectionQueryForUpdateData(query)) {
		delete query;
		return true;
	}
	delete query;
	return false;
}

bool DatabaseManager::UpdateInDownloadingOnDataBase(Download* download)
{
	QSqlDatabase& db = SettingUpDatabase::getThreadDatabase();
	
	// Use transaction for better performance and data consistency
	if (db.transaction()) {
		QList<QSqlQuery*> queries = DatabaseQueryPreparer::PrepareQueriesForUpdateInDownloading();
		bool success = true;
		
		for (QSqlQuery* q : queries) {
			if (!DatabaseInteract::ExectionQueryForUpdateData(q)) {
				success = false;
				break;
			}
		}
		
		qDeleteAll(queries);
		
		if (success) {
			db.commit();
			return true;
		} else {
			db.rollback();
			return false;
		}
	}
	
	return false;
}

bool DatabaseManager::FinishDownloadOnDatabase(Download* download)
{
	QSqlDatabase& db = SettingUpDatabase::getThreadDatabase();
	
	// Use transaction for better performance and data consistency
	if (db.transaction()) {
		QList<QSqlQuery*> queries = DatabaseQueryPreparer::PrepareQueryForFinishDownload();
		bool success = true;
		
		for (QSqlQuery* q : queries) {
			if (!DatabaseInteract::ExectionQueryForUpdateData(q)) {
				success = false;
				break;
			}
		}
		
		qDeleteAll(queries);
		
		if (success) {
			db.commit();
			return true;
		} else {
			db.rollback();
			return false;
		}
	}
	
	return false;
}

QList<PartDownload*> DatabaseManager::CreatePartDownloadsOfDownload(int Download_id)
{
	QList<PartDownload*> ListOfPartDownloadsOfDownload;
	QSqlQuery* query = createThreadSafeQuery();
	if (!DatabaseQueryPreparer::PrepareQueryForLoadPartDownloadOfDownload(query, Download_id)) {
		delete query;
		return ListOfPartDownloadsOfDownload;
	}
	
	if (DatabaseInteract::ExectionQueryForReadData(query)) {
		while (query->next()) {
			// Removed unnecessary thread creation for each PartDownload
			// PartDownload objects now run in the same thread as their parent Download
			PartDownload* partDownload = new PartDownload(nullptr);
			
			if (ProcessDatabaseOutput::ProcessPutLoadedPartDownloadInInPartDownloadObject(query->record(), partDownload, Download_id)) {
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
	QSqlQuery* PartDownloadsDeletequery = createThreadSafeQuery();
	if (!DatabaseQueryPreparer::PrepareQueryForRemovePartDownloadsOfDownloadFromDatabase(PartDownloadsDeletequery, download)) {
		delete PartDownloadsDeletequery;
		return false;
	}
	
	if (DatabaseInteract::ExectionQueryForDeleteData(PartDownloadsDeletequery)) {
		QSqlQuery* DownloadDeletequery = createThreadSafeQuery();
		if (!DatabaseQueryPreparer::PrepareQueryForRemoveDownloadFromDatabase(DownloadDeletequery, download)) {
			delete DownloadDeletequery;
			delete PartDownloadsDeletequery;
			return false;
		}
		
		if (DatabaseInteract::ExectionQueryForDeleteData(DownloadDeletequery)) {
			delete DownloadDeletequery;
		} else {
			delete DownloadDeletequery;
			Is_Success = false;
		}
		delete PartDownloadsDeletequery;
	} else {
		delete PartDownloadsDeletequery;
		Is_Success = false;
	}
	return Is_Success;
}

bool DatabaseManager::LoadAllQueues(QList<Queue*>& listOfQueues, QObject* object)
{
	bool Is_Success = true;
	QSqlQuery* GeneralInformationOfQueueQuery = createThreadSafeQuery();
	if (!DatabaseQueryPreparer::PrepareQueryForLoadAllQueuesGeneralInformationFromDatabase(GeneralInformationOfQueueQuery)) {
		delete GeneralInformationOfQueueQuery;
		return false;
	}
	
	if (DatabaseInteract::ExectionQueryForReadData(GeneralInformationOfQueueQuery)) {
		while (GeneralInformationOfQueueQuery->next()) {
			Queue* queue = new Queue(object);
			if (ProcessDatabaseOutput::ProcessPutLoadedQueueInformationInQueueObject(GeneralInformationOfQueueQuery->record(), queue)) {
				QSqlQuery* downloadIdsOfQueueQuery = createThreadSafeQuery();
				if (!DatabaseQueryPreparer::PrepareQueryForGetDownloadIdOfQueue(downloadIdsOfQueueQuery, queue)) {
					delete downloadIdsOfQueueQuery;
					Is_Success = false;
				} else if (DatabaseInteract::ExectionQueryForReadData(downloadIdsOfQueueQuery)) {
					while (downloadIdsOfQueueQuery->next()) {
						ProcessDatabaseOutput::PutDownloadIdInQueueDownloadList(downloadIdsOfQueueQuery->record(), queue);
					}
					delete downloadIdsOfQueueQuery;
				} else {
					delete downloadIdsOfQueueQuery;
					Is_Success = false;
				}
			}
			listOfQueues.append(queue);
		}
		delete GeneralInformationOfQueueQuery;
	} else {
		delete GeneralInformationOfQueueQuery;
		Is_Success = false;
	}
	return Is_Success;
}

bool DatabaseManager::RemoveDownloadFromQueueOnDatabase(Download* download)
{
	QSqlQuery* query = createThreadSafeQuery();
	if (!DatabaseQueryPreparer::PrepareQueryForRemoveDownloadFromQueueOnDatabase(query, download)) {
		delete query;
		return false;
	}
	
	if (DatabaseInteract::ExectionQueryForUpdateData(query)) {
		delete query;
		return true;
	}
	delete query;
	return false;
}

size_t DatabaseManager::CreateNewQueueOnDatabase(Queue* queue)
{
	size_t queue_id = 0;
	QSqlQuery* query = createThreadSafeQuery();
	if (!DatabaseQueryPreparer::PrepareQueryForCreateNewQueue(query, queue)) {
		delete query;
		return queue_id;
	}
	
	if (DatabaseInteract::ExectionQueryForInsertData(query)) {
		queue_id = query->lastInsertId().toInt();
	}
	delete query;
	return queue_id;
}

bool DatabaseManager::RemoveQueueFromDatabase(Queue* queue)
{
	QSqlQuery* query = createThreadSafeQuery();
	if (!DatabaseQueryPreparer::PrepareQueryForRemoveQueueFromDatabase(query, queue)) {
		delete query;
		return false;
	}
	
	if (DatabaseInteract::ExectionQueryForDeleteData(query)) {
		delete query;
		return true;
	}
	delete query;
	return false;
}

bool DatabaseManager::ExitDownloadFromQueue(Download* download)
{
	QSqlQuery* query = createThreadSafeQuery();
	if (!DatabaseQueryPreparer::PrepareQueryForRemoveDownloadFromQueueOnDatabase(query, download)) {
		delete query;
		return false;
	}
	
	if (DatabaseInteract::ExectionQueryForDeleteData(query)) {
		delete query;
		return true;
	}
	delete query;
	return false;
}

bool DatabaseManager::ExitAllDownloadFromQueue(Queue* queue)
{
	QSqlQuery* query = createThreadSafeQuery();
	if (!DatabaseQueryPreparer::PrepareQueryForExitAllDownloadFromQueue(query, queue)) {
		delete query;
		return false;
	}
	
	if (DatabaseInteract::ExectionQueryForDeleteData(query)) {
		delete query;
		return true;
	}
	delete query;
	return false;
}

bool DatabaseManager::AddDownloadToQueueOnDatabase(Download* Download, Queue* queue)
{
	QSqlQuery* query = createThreadSafeQuery();
	if (!DatabaseQueryPreparer::PrepareQueryFroAddDownloadToQueue(query, Download, queue)) {
		delete query;
		return false;
	}
	
	if (DatabaseInteract::ExectionQueryForUpdateData(query)) {
		delete query;
		return true;
	}
	delete query;
	return false;
}

bool DatabaseManager::UpdateTimeQueueEvents(Queue* queue)
{
	QSqlQuery* query = createThreadSafeQuery();
	if (!DatabaseQueryPreparer::PrepareQueryFroEditTimeEventsOfQueue(query, queue)) {
		delete query;
		return false;
	}
	
	if (DatabaseInteract::ExectionQueryForUpdateData(query)) {
		delete query;
		return true;
	}
	delete query;
	return false;
}

bool DatabaseManager::AddDownloadTo_Queue_Download(Queue* queue, Download* download)
{
	QSqlQuery* query = createThreadSafeQuery();
	if (!DatabaseQueryPreparer::PrepareQueryForAddDownloadTo_Queue_Download(query, queue, download)) {
		delete query;
		return false;
	}
	
	if (DatabaseInteract::ExectionQueryForInsertData(query)) {
		delete query;
		return true;
	}
	delete query;
	return false;
}

bool DatabaseManager::RemoveDownloadFrom_Queue_Download(Download* download)
{
	QSqlQuery* query = createThreadSafeQuery();
	if (!DatabaseQueryPreparer::PrepareQueryForRemoveDownloadFrom_Queue_Download(query, download)) {
		delete query;
		return false;
	}
	
	if (DatabaseInteract::ExectionQueryForDeleteData(query)) {
		delete query;
		return true;
	}
	delete query;
	return false;
}

bool DatabaseManager::DecreaseDownloadNumberOfQueueListForNextDownloadInQueueListOn_Queue_Download(Queue* queue, int DownloadNumberInQueueList)
{
	QSqlQuery* query = createThreadSafeQuery();
	if (!DatabaseQueryPreparer::PrepareQueryForDecreaseDownloadNumberOfQueueListForNextDownloadInQueueListOn_Queue_Download(query, queue, DownloadNumberInQueueList)) {
		delete query;
		return false;
	}
	
	if (DatabaseInteract::ExectionQueryForUpdateData(query)) {
		delete query;
		return true;
	}
	delete query;
	return false;
}

bool DatabaseManager::MoveDownloadIn_Queue_Download(Queue* queue, Download* download, int moveNumber)
{
	QList<QSqlQuery*> querylist = DatabaseQueryPreparer::PrepareQueryForMoveDownloadIn_Queue_Download();
	if (querylist.isEmpty()) {
		return false;
	}
	
	if (DatabaseInteract::ExectionQueryForUpdateData(querylist[0])) {
		if (DatabaseInteract::ExectionQueryForUpdateData(querylist[1])) {
			qDeleteAll(querylist);
			return true;
		}
	}
	qDeleteAll(querylist);
	return false;
}

bool DatabaseManager::ExitAllDownloadFrom_Queue_Download(Queue* queue)
{
	QSqlQuery* query = createThreadSafeQuery();
	if (!DatabaseQueryPreparer::PrepareQueryForExitAllDownloadFrom_Queue_Download(query, queue)) {
		delete query;
		return false;
	}
	
	if (DatabaseInteract::ExectionQueryForDeleteData(query)) {
		delete query;
		return true;
	}
	delete query;
	return false;
}

size_t DatabaseManager::GetNumberDownloadInListOfQueue(Download* download)
{
	size_t numberInList = 0;
	QSqlQuery* query = createThreadSafeQuery();
	if (!DatabaseQueryPreparer::PrepareQueryForGetNumberInListDownload(query, download)) {
		delete query;
		return numberInList;
	}
	
	if (DatabaseInteract::ExectionQueryForReadData(query)) {
		if (query->next()) {
			numberInList = query->value(0).toInt();
		}
	}
	delete query;
	return numberInList;
}

size_t DatabaseManager::GetturnInIdOfDownload(Queue* queue, int NumnberDownloadInList)
{
	size_t queue_id = 0;
	QSqlQuery* query = createThreadSafeQuery();
	if (!DatabaseQueryPreparer::PrepareQueryForGetturnInIdOfDownload(query, queue, NumnberDownloadInList)) {
		delete query;
		return queue_id;
	}
	
	if (DatabaseInteract::ExectionQueryForReadData(query)) {
		if (query->next()) {
			queue_id = query->value(0).toInt();
		}
	}
	delete query;
	return queue_id;
}

void DatabaseManager::LoadDownloadInformationOfQueueForScheduleTreeWidget(QList<QTreeWidgetItem*>& TreeWidgetItems, Queue* queue)
{
	QSqlQuery* query = createThreadSafeQuery();
	if (!DatabaseQueryPreparer::PrepareQueryForLoadDownloadInformationOfQueueForScheduleTreeWidget(query, queue)) {
		delete query;
		return;
	}
	
	if (DatabaseInteract::ExectionQueryForReadData(query)) {
		while (query->next()) {
			QTreeWidgetItem* item = new QTreeWidgetItem();
			ProcessDatabaseOutput::PutDownloadInformationOfQueueForScheduleTreeWidget(query->record(), item);
			TreeWidgetItems.append(item);
		}
	}
	delete query;
}

bool DatabaseManager::UpdateNumberOfDownloadAtSameTimeOfQueue(Queue* queue)
{
	QSqlQuery* query = createThreadSafeQuery();
	if (!DatabaseQueryPreparer::PrepareQueryForUpdateNumberOfDownloadAtSameTimeOfQueue(query, queue)) {
		delete query;
		return false;
	}
	
	if (DatabaseInteract::ExectionQueryForUpdateData(query)) {
		delete query;
		return true;
	}
	delete query;
	return false;
}

bool DatabaseManager::RemoveAllCompletedDownload()
{
	QSqlQuery* query = createThreadSafeQuery();
	if (!DatabaseQueryPreparer::PrepareQueryForRemoveAllCompletedDownload(query)) {
		delete query;
		return false;
	}
	
	if (DatabaseInteract::ExectionQueryForDeleteData(query)) {
		delete query;
		return true;
	}
	delete query;
	return false;
}