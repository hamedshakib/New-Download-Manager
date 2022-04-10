#include "HeaderAndUi/DatabaseQueryPreparer.h"

DatabaseQueryPreparer::DatabaseQueryPreparer(QObject *parent)
	: QObject(parent)
{
}

DatabaseQueryPreparer::~DatabaseQueryPreparer()
{
}

QSqlQuery* DatabaseQueryPreparer::PrepareQueryForLoadDownload(int Download_id)
{
	QString queryString = QString(
		"Select is_Done,Url,SaveTo,Category,DownloadSize,SizeDownloaded,FileType,TimeLeft,LastTryTime,ResumeCapability,MaxSpeed,description,Queue_id "
		"From Download "
		"where id='%1' "
	).arg(Download_id);
	QSqlQuery* query = new QSqlQuery(queryString, SettingUpDatabase::get_Database());
	return query;
}

QSqlQuery* DatabaseQueryPreparer::PrepareQueryChangeLastbitDownloaded(int PartDownload_id,qint64 LastbitDownloaded)
{
	QString queryString = QString(
		"UPDATE PartDownload "
		"SET LastDownloaded_bit =%1 "
		"WHERE id=%2 "
	).arg(LastbitDownloaded).arg(PartDownload_id);
	QSqlQuery* query = new QSqlQuery(queryString, SettingUpDatabase::get_Database());
	return query;
}

QSqlQuery* DatabaseQueryPreparer::PrepareQuerySuffixsFromMimeType(QString MimeType)
{
	QString queryString = QString(
		"Select suffix "
		"From MimeType "
		"WHERE mimeType='%1' "
	).arg(MimeType);
	qDebug() << queryString;
	QSqlQuery* query = new QSqlQuery(queryString, SettingUpDatabase::get_Database());
	return query;
}

QSqlQuery* DatabaseQueryPreparer::PrepareQueryForCreateNewDownload(Download* download)
{
	SettingUpDatabase::get_Database();
	QString queryString = QString(
		"INSERT INTO Download(DownloadStatus_id,"
		"Url,SaveTo,"
		"Suffix,DownloadSize,"
		"SizeDownloaded,description,"
		"LastTryTime,TimeLeft,"
		"MaxSpeed,ResumeCapability_id,"
		"Category_id,Queue_id"
		") "


		"VALUES(1,"
		":url,:saveTo,"
		":suffix,:downloadSize,"
		"0,:description,"
		"NULL,NULL,"
		"0,:resumeCapability_id,"
		"NULL,:queue_id"
		");"
	);

	QSqlQuery* query = new QSqlQuery();
	query->prepare(queryString);

	query->bindValue(":url", download->Url.toString());
	query->bindValue(":saveTo", download->SaveTo.toString());
	query->bindValue(":suffix", download->suffix);
	query->bindValue(":downloadSize", download->DownloadSize);
	query->bindValue(":description", download->description);
	query->bindValue(":resumeCapability_id", ProcessEnum::ConvertResumeCapabilityEnumToResumeCapabilityId(download->ResumeCapability));


	//	query->bindValue(":Category_id", download->Category);



	query->bindValue(":queue_id", download->Queue_id > -1 ? QVariant(download->Queue_id) : QVariant("NULL"));

	return query;
	

}

QSqlQuery* DatabaseQueryPreparer::PrepareQueryForCreateNewPartDownload(PartDownload* partDownload)
{
	//Todo
	QString queryString = QString(
		"INSERT INTO PartDownload(Download_id,"
		"Start_byte,End_byte,"
		"PartDownload_SaveTo,LastDownloaded_byte"
		") "


		"VALUES(:download_id,"
		":start_byte,:end_byte,"
		":partDownload_SaveTo,0"
		");"
	);

	
	SettingUpDatabase::get_Database();
	QSqlQuery* query = new QSqlQuery();
	query->prepare(queryString);
	query->bindValue(":download_id", partDownload->id_download);
	query->bindValue(":start_byte", partDownload->start_byte);
	query->bindValue(":end_byte", partDownload->end_byte);
	query->bindValue(":partDownload_SaveTo", partDownload->PartDownloadFile->fileName());
	

	return query;
}

QSqlQuery* DatabaseQueryPreparer::PrepareQueryForLoadDownloadForMainTable()
{
	QString queryString = QString(
		"Select * "
		"From Download "
	);
	
	QSqlQuery* query = new QSqlQuery(queryString, SettingUpDatabase::get_Database());
	return query;
}
