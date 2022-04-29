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
		"select D.id,D.FileName,DS.Name as Status,Url,SaveTo,Suffix,DownloadSize,SizeDownloaded,description,LastTryTime,RC.Name as ResumeCapability,Category_id,Queue_id "
		"From Download as D join DownloadStatus as DS on D.DownloadStatus_id = DS.id join ResumeCapability as RC on D.ResumeCapability_id = RC.id "
		"where D.id=:id "
	);
	
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
		"LastTryTime,"
		"MaxSpeed,ResumeCapability_id,"
		"Category_id,Queue_id"
		") "


		"VALUES(1,:fileName,"
		":url,:saveTo,"
		":suffix,:downloadSize,"
		"0,:description,"
		":lastTryTime,"
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
	query->bindValue(":lastTryTime", DateTimeManager::ConvertDataTimeToString(download->LastTryTime));
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
		"select D.id,D.FileName,Ds.Name as DownloadStatus,Url,SaveTo,Suffix,DownloadSize,SizeDownloaded,description,LastTryTime,Queue_id "
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
		"LastTryTime = :lastTryTime,"
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



	query->bindValue(":lastTryTime", DateTimeManager::ConvertDataTimeToString(download->LastTryTime));
	query->bindValue(":maxSpeed", download->MaxSpeed>0 ? QString::number(download->MaxSpeed):QVariant("NULL"));
	query->bindValue(":resumeCapability_id",ProcessEnum::ConvertResumeCapabilityEnumToResumeCapabilityId(download->ResumeCapability));

	query->bindValue(":category_id", QVariant("NULL"));



	query->bindValue(":queue_id", download->Queue_id > -1 ? download->Queue_id : QVariant("NULL"));
	
	query->bindValue(":id",download->IdDownload);

	return query;
}

QSqlQuery* DatabaseQueryPreparer::PrepareQueryForUpdateInStartDownload(Download* download)
{
	//ToDo
	QString queryString = QString(
		"UPDATE Download "
		"SET DownloadStatus_id = :downloadStatus_id,"
		"LastTryTime = :lastTryTime "
		"WHERE id = :id;"
	);
	QSqlQuery* query = new QSqlQuery();
	query->prepare(queryString);

	query->bindValue(":downloadStatus_id", ProcessEnum::ConvertDownloadStatusEnumToDownloadStatusId(Download::DownloadStatusEnum::Downloading));
	query->bindValue(":lastTryTime", DateTimeManager::ConvertDataTimeToString(download->LastTryTime));
	query->bindValue(":id", download->IdDownload);
	return query;
}

QList<QSqlQuery*> DatabaseQueryPreparer::PrepareQueriesForUpdateInDownloading(Download* download)
{
	SettingUpDatabase::get_Database();
	QList< QSqlQuery*> listOfQueries;



	QString queryString = QString(
		"UPDATE Download "
		"SET SizeDownloaded = :sizeDownloaded "
		"WHERE id = :id;"
	);
	QSqlQuery* query = new QSqlQuery();
	query->prepare(queryString);

	query->bindValue(":sizeDownloaded", download->SizeDownloaded);
	query->bindValue(":id", download->IdDownload);
	listOfQueries.append(query);


	for (PartDownload* partDownload: download->get_PartDownloads())
	{
		QString queryString1 = QString(
			"UPDATE PartDownload "
			"SET  LastDownloaded_byte = :lastDownloaded_byte "
			"WHERE id = :id; "
		);
		QSqlQuery* query1 = new QSqlQuery();
		query1->prepare(queryString1);

		query1->bindValue(":lastDownloaded_byte", partDownload->LastDownloadedByte);
		query1->bindValue(":id", partDownload->id_PartDownload);
		listOfQueries.append(query1);
	}






	return listOfQueries;
}

QList<QSqlQuery*> DatabaseQueryPreparer::PrepareQueryForFinishDownload(Download* download)
{
	SettingUpDatabase::get_Database();
	QList< QSqlQuery*> listOfQueries;

	QString queryString = QString(
		"UPDATE Download "
		"SET SizeDownloaded = :sizeDownloaded,"
		"DownloadStatus_id = :downloadStatus_id "
		"WHERE id = :id;"
	);
	QSqlQuery* query = new QSqlQuery();
	query->prepare(queryString);

	query->bindValue(":sizeDownloaded", download->SizeDownloaded);
	query->bindValue(":downloadStatus_id", ProcessEnum::ConvertDownloadStatusEnumToDownloadStatusId(Download::Completed));
	query->bindValue(":id", download->IdDownload);
	listOfQueries.append(query);


	QString queryString1 = QString(
		"DELETE FROM PartDownload "
		"WHERE Download_id = :download_id;"
	);


	QSqlQuery* query1 = new QSqlQuery();
	query1->prepare(queryString1);
	query1->bindValue(":download_id", download->IdDownload);
	listOfQueries.append(query1);
	return listOfQueries;
}

QSqlQuery* DatabaseQueryPreparer::PrepareQueryForLoadPartDownloadOfDownload(int Download_id)
{
	SettingUpDatabase::get_Database();
	QString queryString = QString(
		"SELECT id,"
		"Start_byte,"
		"End_byte,"
		"PartDownload_SaveTo,"
		"LastDownloaded_byte "
		"FROM PartDownload "
		"where Download_id = :download_id; "
	);
	QSqlQuery* query = new QSqlQuery();
	query->prepare(queryString);
	query->bindValue(":download_id", Download_id);
	return query;
}

QSqlQuery* DatabaseQueryPreparer::PrepareQueryForRemovePartDownloadsOfDownloadFromDatabase(Download* download)
{
	SettingUpDatabase::get_Database();
	QString queryString = QString(
		"DELETE FROM PartDownload "
		"WHERE Download_id = :download_id; "
	);
	QSqlQuery* query = new QSqlQuery();
	query->prepare(queryString);
	query->bindValue(":download_id", download->IdDownload);
	return query;
}

QSqlQuery* DatabaseQueryPreparer::PrepareQueryForRemoveDownloadFromDatabase(Download* download)
{
	SettingUpDatabase::get_Database();
	QString queryString = QString(
		"DELETE FROM Download "
		"WHERE id = :download_id; "
	);
	QSqlQuery* query = new QSqlQuery();
	query->prepare(queryString);
	query->bindValue(":download_id", download->IdDownload);
	return query;
}

QSqlQuery* DatabaseQueryPreparer::PrepareQueryForLoadAllQueuesGeneralInformationFromDatabase()
{
	SettingUpDatabase::get_Database();

	QString queryString = QString(
		"Select * FROM Queue; "
	);
	QSqlQuery* query = new QSqlQuery();
	query->prepare(queryString);
	return query;
}

QSqlQuery* DatabaseQueryPreparer::PrepareQueryForGetDownloadIdOfQueue(Queue* queue)
{
	//ToDo
	SettingUpDatabase::get_Database();

	QString queryString = QString(
		"Select id "
		"FROM Download "
		"where Queue_id =:queue_id; "
	);
	QSqlQuery* query = new QSqlQuery();
	query->prepare(queryString);
	query->bindValue(":queue_id", queue->Get_QueueId());
	return query;
}

QSqlQuery* DatabaseQueryPreparer::PrepareQueryForRemoveDownloadFromQueueOnDatabase(Download* download)
{
	SettingUpDatabase::get_Database();

	QString queryString = QString(
		"UPDATE Download "
		"SET Queue_id = :queue_id "
		"WHERE id = :id;"
	);
	QSqlQuery* query = new QSqlQuery();
	query->prepare(queryString);
	query->bindValue(":queue_id", QVariant("NULL"));
	query->bindValue(":id", download->IdDownload);
	return query;
}

QSqlQuery* DatabaseQueryPreparer::PrepareQueryForCreateNewQueue(Queue* queue)
{
	//ToDo
	SettingUpDatabase::get_Database();
	QString queryString = QString(
		"INSERT INTO Queue(Name,NumberDownloadSameTime) "
		"VALUES(:name,:numberDownloadSameTime); "
	);
	QSqlQuery* query = new QSqlQuery();
	query->prepare(queryString);
	query->bindValue(":name", queue->Get_QueueName());
	query->bindValue(":numberDownloadSameTime", queue->Get_NumberDownloadAtSameTime());
	return query;
}

QSqlQuery* DatabaseQueryPreparer::PrepareQueryForExitAllDownloadFromQueue(Queue* queue)
{
	SettingUpDatabase::get_Database();
	QString queryString = QString(
		"UPDATE Download "
		"SET Queue_id = :null "
		"WHERE Queue_id = :queue_id; "
	);
	QSqlQuery* query = new QSqlQuery();
	query->prepare(queryString);

	query->bindValue(":null", QVariant("NULL"));
	query->bindValue(":queue_id", queue->Get_QueueId());
	return query;
}

QSqlQuery* DatabaseQueryPreparer::PrepareQueryForRemoveQueueFromDatabase(Queue* queue)
{
	SettingUpDatabase::get_Database();
	QString queryString = QString(
		"DELETE FROM Queue "
		"WHERE id = :id; "
	);
	QSqlQuery* query = new QSqlQuery();
	query->prepare(queryString);

	query->bindValue(":id", queue->Get_QueueId());
	return query;
}

QSqlQuery* DatabaseQueryPreparer::PrepareQueryFroAddDownloadToQueue(Download* download, Queue* queue)
{
	SettingUpDatabase::get_Database();
	QString queryString = QString(
		"UPDATE Download "
		"SET Queue_id = :queue_id "
		"WHERE id = :id; "
	);
	QSqlQuery* query = new QSqlQuery();
	query->prepare(queryString);

	query->bindValue(":queue_id", queue->Get_QueueId());
	query->bindValue(":id", download->IdDownload);
	return query;
}

QSqlQuery* DatabaseQueryPreparer::PrepareQueryFroEditTimeEventsOfQueue(Queue* queue)
{
	SettingUpDatabase::get_Database();
	QString queryString = QString(
		"UPDATE Queue "
		"SET NumberDownloadSameTime = :numberDownloadSameTime, "
		"StartTime = :startTime, "
		"StopTime = :stopTime, "
		"OnceTimeAt = :onceTimeAt, "
		"EachDays = :eachDays, "
		"DaysOfWeek = :daysOfWeek "
		"WHERE id = :queue_id; "
	);
	QSqlQuery* query = new QSqlQuery();
	query->prepare(queryString);

	query->bindValue(":numberDownloadSameTime", queue->Get_NumberDownloadAtSameTime());
	//ToDo check null time

	if (queue->Has_StopTimeActive())
	{
		query->bindValue(":stopTime", queue->Get_StopTime().toString());
	}
	else
	{
		query->bindValue(":stopTime", QVariant("NULL"));
	}


	if (queue->Has_StartTimeActive())
	{
		query->bindValue(":startTime", queue->Get_StartTime().toString());
		if (ConverterQueueTime::IsNameOfDaysOfWeek(queue->Get_DaysOfDownoad()[0]))
		{
			QString DownloadDaysOfWeek;
			for (QString day:queue->Get_DaysOfDownoad())
			{
				DownloadDaysOfWeek.append(day + ",");
			}
			query->bindValue(":daysOfWeek", DownloadDaysOfWeek);
			query->bindValue(":onceTimeAt", QVariant("NULL"));
			query->bindValue(":eachDays", QVariant("NULL"));
		}
		else if (ConverterQueueTime::IsNumberOfDays(queue->Get_DaysOfDownoad()[0]))
		{
			query->bindValue(":eachDays", queue->Get_DaysOfDownoad()[0]);
			query->bindValue(":onceTimeAt", QVariant("NULL"));
			query->bindValue(":daysOfWeek", QVariant("NULL"));
		}
		else
		{
			query->bindValue(":onceTimeAt", queue->Get_DaysOfDownoad()[0]);
			query->bindValue(":eachDays", QVariant("NULL"));
			query->bindValue(":daysOfWeek", QVariant("NULL"));
		}
	}
	else
	{
		query->bindValue(":startTime", QVariant("NULL"));
		query->bindValue(":onceTimeAt", QVariant("NULL"));
		query->bindValue(":eachDays", QVariant("NULL"));
		query->bindValue(":daysOfWeek", QVariant("NULL"));
	}
	



	query->bindValue(":queue_id", queue->Get_QueueId());
	return query;
}

//
QSqlQuery* DatabaseQueryPreparer::PrepareQueryForAddDownloadTo_Queue_Download(Queue* queue, Download* download)
{

	SettingUpDatabase::get_Database();
	QString queryString = QString(

		"INSERT INTO Queue_Download ( "
		"Queue_id, "
		"Download_id, "
		"NumbersInList "
	") "
		"VALUES( "
			":queue_id, "
			":download_id, "
			"(SELECT(CASE WHEN max(NumbersInList) IS not NULL THEN max(NumbersInList)+1 ELSE 1 END) as 'NumbersInList' FROM Queue_Download where Queue_id = :queue_id) ); "
	);



	QSqlQuery* query = new QSqlQuery();
	query->prepare(queryString);

	query->bindValue(":queue_id", queue->Get_QueueId());
	query->bindValue(":download_id", download->IdDownload);
	return query;
}

QSqlQuery* DatabaseQueryPreparer::PrepareQueryForRemoveDownloadFrom_Queue_Download(Download* download)
{
	SettingUpDatabase::get_Database();
	QString queryString = QString(
		"DELETE FROM Queue_Download "
		"WHERE Download_id = :download_id; "
	);

	QSqlQuery* query = new QSqlQuery();
	query->prepare(queryString);

	query->bindValue(":download_id", download->IdDownload);
	return query;
}

QSqlQuery* DatabaseQueryPreparer::PrepareQueryForDecreaseDownloadNumberOfQueueListForNextDownloadInQueueListOn_Queue_Download(Queue* queue, int DownloadNumberInQueueList)
{
	SettingUpDatabase::get_Database();
	QString queryString = QString(
		"UPDATE Queue_Download "
		"SET NumbersInList = NumbersInList - 1 "
		"WHERE Queue_id = :queue_id AND "
		"NumbersInList > :numbersInList; "
	);

	QSqlQuery* query = new QSqlQuery();
	query->prepare(queryString);

	query->bindValue(":queue_id", queue->Get_QueueId());
	query->bindValue(":numbersInList", DownloadNumberInQueueList);
	return query;
}

QSqlQuery* DatabaseQueryPreparer::PrepareQueryForMoveDownloadIn_Queue_Download(Queue* queue, Download* download, int moveNumber)
{
	SettingUpDatabase::get_Database();
	QString queryString = QString(
		"UPDATE Queue_Download "
		"SET NumbersInList = NumbersInList + :moveNumber "
		"WHERE Queue_id = :queue_id AND Download_id = :download_id; "
	);

	QSqlQuery* query = new QSqlQuery();
	query->prepare(queryString);


	query->bindValue(":moveNumber", moveNumber);
	query->bindValue(":queue_id", queue->Get_QueueId());
	query->bindValue(":download_id", download->IdDownload);
	return query;
}

QSqlQuery* DatabaseQueryPreparer::PrepareQueryForExitAllDownloadFrom_Queue_Download(Queue* queue)
{
	SettingUpDatabase::get_Database();
	QString queryString = QString(
		"DELETE FROM Queue_Download "
		"WHERE Queue_id = :queue_id'; "
	);

	QSqlQuery* query = new QSqlQuery();
	query->prepare(queryString);

	query->bindValue(":queue_id", queue->Get_QueueId());
	return query;
}

QSqlQuery* DatabaseQueryPreparer::PrepareQueryForGetNumberInListDownload(Download* download)
{
	SettingUpDatabase::get_Database();
	QString queryString = QString(
		"Select NumbersInList "
		"From Queue_Download "
		"WHERE Download_id = :download_id; "
	);

	QSqlQuery* query = new QSqlQuery();

	query->prepare(queryString);


	query->bindValue(":download_id", download->IdDownload);
	return query;
}

QSqlQuery* DatabaseQueryPreparer::PrepareQueryForGetturnInIdOfDownload(Queue* queue, int NumnberDownloadInList)
{
	SettingUpDatabase::get_Database();
	QString queryString = QString(
		"Select Download_id "
		"From Queue_Download "
		"where NumbersInList = :numbersInList And Queue_id = :queue_id ; "
	);

	QSqlQuery* query = new QSqlQuery();
	query->prepare(queryString);


	query->bindValue(":queue_id", queue->Get_QueueId());
	query->bindValue(":numbersInList", NumnberDownloadInList);
	return query;
}

QSqlQuery* DatabaseQueryPreparer::PrepareQueryForLoadDownloadInformationOfQueueForScheduleTreeWidget(Queue* queue)
{
	SettingUpDatabase::get_Database();
	QString queryString = QString(
		"SELECT QD.Download_id,QD.NumbersInList, "
		"D.FileName, DownloadSize, DS.Name as Status "
		"FROM Queue_Download as QD join Download as D on QD.Download_id = D.id join DownloadStatus as DS on D.DownloadStatus_id = DS.id "
		"Where QD.Queue_id = :queue_id; "
	);

	QSqlQuery* query = new QSqlQuery();
	query->prepare(queryString);


	query->bindValue(":queue_id", queue->Get_QueueId());
	return query;
}

QSqlQuery* DatabaseQueryPreparer::PrepareQueryForUpdateNumberOfDownloadAtSameTimeOfQueue(Queue* queue)
{
	SettingUpDatabase::get_Database();
	QString queryString = QString(
		"UPDATE Queue "
		"SET NumberDownloadSameTime = :numberDownloadSameTime "
		"WHERE id = :queue_id; "
	);

	QSqlQuery* query = new QSqlQuery();
	query->prepare(queryString);

	query->bindValue(":numberDownloadSameTime", queue->Get_NumberDownloadAtSameTime());
	query->bindValue(":queue_id", queue->Get_QueueId());

	return query;
}

QSqlQuery* DatabaseQueryPreparer::PrepareQueryForRemoveAllCompletedDownload()
{
	SettingUpDatabase::get_Database();
	QString queryString = QString(
		"DELETE FROM Download "
		"WHERE DownloadStatus_id = 3; "
	);

	QSqlQuery* query = new QSqlQuery();
	query->prepare(queryString);

	return query;
}