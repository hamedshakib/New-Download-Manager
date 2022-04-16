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

