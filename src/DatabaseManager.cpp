#include "HeaderAndUi/DatabaseManager.h"

DatabaseManager::DatabaseManager(QObject *parent)
	: QObject(parent)
{
}

DatabaseManager::~DatabaseManager()
{
	SettingUpDatabase::get_Database().close();
}

bool DatabaseManager::LoadDownloadComplite(int Download_id,Download* download)
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

