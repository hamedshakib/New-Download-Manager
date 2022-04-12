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
				return true;
			}
		}
	}
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
		return suffixs;

	}
	
}

size_t DatabaseManager::CreateNewDownloadOnDatabase(Download* download)
{
	QSqlQuery* query = DatabaseQueryPreparer::PrepareQueryForCreateNewDownload(download);
	size_t downloadId{};
	if (DatabaseInteract::ExectionQueryForInsertData(query))
	{
		return query->lastInsertId().toInt();
	}
	return downloadId;
}

size_t DatabaseManager::CreateNewPartDownloadOnDatabase(PartDownload* partDownload)
{
	QSqlQuery* query = DatabaseQueryPreparer::PrepareQueryForCreateNewPartDownload(partDownload);
	size_t downloadId{};
	if (DatabaseInteract::ExectionQueryForInsertData(query))
	{
		return query->lastInsertId().toInt();
	}
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
		return true;
	}
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
		return true;
	}
	return false;
}

bool DatabaseManager::UpdateInDownloadingOnDataBase(Download* download)
{
	auto Queries = DatabaseQueryPreparer::PrepareQueriesForUpdateInDownloading(download);
	for (int i = 0; i < Queries.size(); i++)
	{
		if (DatabaseInteract::ExectionQueryForUpdateData(Queries[0]))
		{

		}
	}


	qDeleteAll(Queries);
	return true;
}

