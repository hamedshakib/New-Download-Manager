#include "HeaderAndUi/DatabaseReader.h"

DatabaseReader::DatabaseReader(QObject *parent)
	: QObject(parent)
{
}

DatabaseReader::~DatabaseReader()
{
}

bool DatabaseReader::LoadDownloadFromDatabase(int Download_id, Download* download)
{
	if (!download) {
		qCritical() << "DatabaseReader: Download pointer is null";
		return false;
	}
	
	QSqlQuery* query = DatabaseQueryPreparer::PrepareQueryForLoadDownload(Download_id);
	if (!query) {
		qCritical() << "DatabaseReader: Failed to create query";
		return false;
	}
	
	// Query is already prepared by PrepareQueryForLoadDownload
	bool is_Ok = query->exec();
	if (is_Ok) {
		if (query->next()) {
			ProcessDatabaseOutput::ProcessPutLoadedDownloadInformationInDownloadObject(query->record(), download, Download_id);
		} else {
			qWarning() << "DatabaseReader: No record found for download ID:" << Download_id;
			return false;
		}
	} else {
		qCritical() << "DatabaseReader: Query execution failed:" << query->lastError().text();
		return false;
	}
	
	delete query;
	return true;
}
