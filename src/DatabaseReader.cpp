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
	QSqlQuery* query = DatabaseQueryPreparer::PrepareQueryForLoadDownload(Download_id);
	
	bool is_Ok= query->exec();
	if (is_Ok)
	{
		if (query->next())
		{
			/*
			bool is_Done = query->value(0).toBool();
			QUrl url = query->value(1).toString();
			QUrl SaveTo = query->value(2).toString();
			QString name = query->value(3).toString();
			QString name = query.value(4).toString();
			QString name = query.value(5).toString();
			QString name = query.value(6).toString();
			QString name = query.value(7).toString();
			QString name = query.value(8).toString();
			QString name = query.value(9).toString();
			QString name = query.value(10).toString();
			QString name = query.value(11).toString();
			int salary = query.value(1).toInt();
			qDebug() << name << salary;
			*/
		}
	}
	return 0;
}


/*
QStringList DatabaseReader::LoadSuffixsForMimeType()
{
	QSqlQuery* query =DatabaseQueryPreparer::PrepareQuerySuffixsFromMimeType();
	bool is_Ok = query->exec();
	if (is_Ok)
	{
		while (query->next())
		{

		}
	}
	//return;
}
*/
