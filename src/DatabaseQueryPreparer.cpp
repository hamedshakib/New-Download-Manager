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
	SettingUpDatabase::get_Database();
	QString queryString = QString(
		"select D.id,D.FileName,Ds.Name as Status,Url,SaveTo,Suffix,DownloadSize,SizeDownloaded,description,TimeLeft,LastTryTime,RC.Name as ResumeCapability,Category_id,Queue_id "
		"From Download as D join DownloadStatus as DS on D.id = DS.id join ResumeCapability as RC on D.ResumeCapability_id = RC.id "
		"where D.id=:id "
	);






		//"Select DownloadStatus_id,FileName,Url,SaveTo,Suffix,DownloadSize,SizeDownloaded,TimeLeft,LastTryTime,ResumeCapability_id,Category_id,MaxSpeed,description,Queue_id "
		//"From Download "
		//"where id='%1' "
	
	QSqlQuery* query = new QSqlQuery();
	query->prepare(queryString);

	query->bindValue(":id", Download_id);

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
		"INSERT INTO Download(DownloadStatus_id,FileName,"
		"Url,SaveTo,"
		"Suffix,DownloadSize,"
		"SizeDownloaded,description,"
		"LastTryTime,TimeLeft,"
		"MaxSpeed,ResumeCapability_id,"
		"Category_id,Queue_id"
		") "


		"VALUES(1,:fileName,"
		":url,:saveTo,"
		":suffix,:downloadSize,"
		"0,:description,"
		"NULL,NULL,"
		"0,:resumeCapability_id,"
		":category_id,:queue_id"
		");"
	);

	QSqlQuery* query = new QSqlQuery();
	query->prepare(queryString);

	query->bindValue(":fileName", download->FileName);
	query->bindValue(":url", download->Url.toString());
	query->bindValue(":saveTo", download->SaveTo.toString());
	query->bindValue(":suffix", download->suffix);
	query->bindValue(":downloadSize", download->DownloadSize);
	query->bindValue(":description", download->description);
	query->bindValue(":resumeCapability_id", ProcessEnum::ConvertResumeCapabilityEnumToResumeCapabilityId(download->ResumeCapability));


	query->bindValue(":category_id", QVariant("NULL"));



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
		"select D.id,D.FileName,Ds.Name as DownloadStatus,Url,SaveTo,Suffix,DownloadSize,SizeDownloaded,description,TimeLeft,LastTryTime,Queue_id "
		"From Download as D join DownloadStatus as DS on D.DownloadStatus_id = DS.id"
	);
	
	QSqlQuery* query = new QSqlQuery(queryString, SettingUpDatabase::get_Database());
	return query;
}

QSqlQuery* DatabaseQueryPreparer::PrepareQueryForUpdateAllFieldDownload(Download* download)
{
	//Todo
	QString queryString = QString(
		"UPDATE Download "
		"SET FileName = :fileName,"
		"DownloadStatus_id = :downloadStatus_id,"
		"Url = :url,"
		"SaveTo = :saveTo,"
		"SizeDownloaded = :sizeDownloaded,"
		"description = :description,"
//		"LastTryTime = :lastTryTime,"
//		"TimeLeft = :timeLeft,"
		"MaxSpeed = :maxSpeed,"
		"ResumeCapability_id = :resumeCapability_id,"
		"Category_id = :category_id,"
		"Queue_id = :queue_id "
		"WHERE id = :id;"
	);


	SettingUpDatabase::get_Database();
	QSqlQuery* query = new QSqlQuery();
	query->prepare(queryString);


	query->bindValue(":fileName", download->FileName);
	query->bindValue(":downloadStatus_id", ProcessEnum::ConvertDownloadStatusEnumToDownloadStatusId(download->downloadStatus));
	query->bindValue(":url", download->Url);

	query->bindValue(":saveTo", download->SaveTo);
	query->bindValue(":SizeDownloaded", download->SizeDownloaded);
	query->bindValue(":description", download->description);




//	query->bindValue(":lastTryTime", download->LastTryTime);
//	query->bindValue(":timeLeft", download->TimeLeft);
	query->bindValue(":maxSpeed", download->MaxSpeed>0 ? QString::number(download->MaxSpeed):QVariant("NULL"));
	query->bindValue(":resumeCapability_id",ProcessEnum::ConvertResumeCapabilityEnumToResumeCapabilityId(download->ResumeCapability));

	query->bindValue(":category_id", QVariant("NULL"));



	query->bindValue(":queue_id", download->Queue_id > -1 ? download->Queue_id : QVariant("NULL"));
	
	query->bindValue(":id",download->IdDownload);

	return query;
}

QSqlQuery* DatabaseQueryPreparer::PrepareQueryForUpdateInDownloading(Downloader* downloader)
{
	Download* download=downloader->Get_Download();

	for (PartDownload* partDownload: download->get_PartDownloads())
	{

	}


	QString queryString = QString(
		"UPDATE Download "
		"SET SizeDownloaded = :sizeDownloaded "
		"WHERE id = :id;"
	);


	SettingUpDatabase::get_Database();
	QSqlQuery* query = new QSqlQuery();
	query->prepare(queryString);


	query->bindValue(":fileName", download->FileName);
	query->bindValue(":downloadStatus_id", ProcessEnum::ConvertDownloadStatusEnumToDownloadStatusId(download->downloadStatus));
	query->bindValue(":url", download->Url);

	query->bindValue(":saveTo", download->SaveTo);
	query->bindValue(":SizeDownloaded", download->SizeDownloaded);
	query->bindValue(":description", download->description);




	//	query->bindValue(":lastTryTime", download->LastTryTime);
	//	query->bindValue(":timeLeft", download->TimeLeft);
	query->bindValue(":maxSpeed", download->MaxSpeed > 0 ? QString::number(download->MaxSpeed) : QVariant("NULL"));
	query->bindValue(":resumeCapability_id", ProcessEnum::ConvertResumeCapabilityEnumToResumeCapabilityId(download->ResumeCapability));

	query->bindValue(":category_id", QVariant("NULL"));



	query->bindValue(":queue_id", download->Queue_id > -1 ? download->Queue_id : QVariant("NULL"));

	query->bindValue(":id", download->IdDownload);

	return query;
}
