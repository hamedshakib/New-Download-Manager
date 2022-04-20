#include "HeaderAndUi/ProcessDatabaseOutput.h"

ProcessDatabaseOutput::ProcessDatabaseOutput(QObject *parent)
	: QObject(parent)
{
}

ProcessDatabaseOutput::~ProcessDatabaseOutput()
{
}

bool ProcessDatabaseOutput::ProcessPutLoadedDownloadInformationInDownloadObject(const QSqlRecord& record, Download* download, int download_id)
{
	
//	"select D.id,D.FileName,Ds.Name as Status,Url,SaveTo,Suffix,DownloadSize,SizeDownloaded,description,TimeLeft,LastTryTime,RC.Name as ResumeCapability,Category_id,Queue_id "
//	"From Download as D join DownloadStatus as DS on D.id = DS.id join ResumeCapability as RC on D.ResumeCapability_id = RC.id "
//	"where id=:id "
	
	download->IdDownload = record.value("id").toInt();
	download->FileName = record.value("FileName").toString();
	download->downloadStatus = ProcessEnum::ConvertStringToDownloadStatusEnum(record.value("Status").toString());
	download->Url = record.value("Url").toString();
	download->DownloadSize = record.value("DownloadSize").toLongLong();
	download->SizeDownloaded = record.value("SizeDownloaded").toLongLong();
	download->suffix= record.value("Suffix").toString();
	download->SaveTo = record.value("SaveTo").toString();
	download->description = record.value("description").toString();
	download->LastTryTime= DateTimeManager::GetDateTimeFromString(record.value("LastTryTime").toString());
	download->MaxSpeed = record.value("MaxSpeed").toInt();
	download->ResumeCapability = ProcessEnum::ConvertDatabseStringToResumeCapabilityEnum(record.value("ResumeCapability").toString());

//	download->Category = record.value("Url").toString();
	download->Queue_id = record.value("Queue_id").toInt();
	download->Url = record.value("Url").toString();


	return true;
}

QString ProcessDatabaseOutput::ProcessLoadedSuffixsForMimeType(const QSqlRecord& record)
{
	return record.value("suffix").toString();
}

void ProcessDatabaseOutput::ProcessPrepareLoadedInformationForMainTableView(const QSqlRecord& record, QStandardItemModel* model)
{
	int id=record.value("id").toInt();
	QString FileName = record.value("FileName").toString();
	qint64 DownloadSize = record.value("DownloadSize").toLongLong();
	qint64 DownloadedSize = record.value("SizeDownloaded").toLongLong();
	QString TempStatus = record.value("DownloadStatus").toString();

	qDebug() <<"Temp st:" << TempStatus;


	QString LastTryTime = DateTimeManager::ConvertDataTimeToString(DateTimeManager::GetDateTimeFromString(record.value("LastTryTime").toString()));  //ToDo Improved with Galaly
	QString Description = record.value("description").toString();
	QString SaveTo = record.value("SaveTo").toString();
	QString Status;
	if (TempStatus == "Completed")
	{
		Status = "Complete";
	}
	/*else if (TempStatus == "NotStarted")
	{
		Status = "0%";
	}*/
	else
	{
		float Present = (long double)DownloadedSize / DownloadSize;
		Status = QString::number(Present*100, 'f', 2)+"%";
	}
	

	model->appendRow(TableViewRowCreater::PrepareDataForRow(id, FileName, ConverterSizeToSuitableString::ConvertSizeToSuitableString(DownloadSize), Status, "", "", LastTryTime, Description, SaveTo));
	//auto preparedDataForRow = tableViewController1->PrepareDataForRow(id, FileName, ConverterSizeToSuitableString::ConvertSizeToSuitableString(DownloadSize).remove("Size: "), Status, "", "", LastTryTime, Description, SaveTo);
	//tableViewController1->model->appendRow(preparedDataForRow);
}

bool ProcessDatabaseOutput::ProcessPutLoadedPartDownloadInInPartDownloadObject(const QSqlRecord& record, PartDownload* partDownload,int Download_id)
{
	//"SELECT id,"
	//	"Start_byte,"
	//	"End_byte,"
	//	"PartDownload_SaveTo,"
	//	"LastDownloaded_byte "
	//	"FROM PartDownload "

	partDownload->id_download = Download_id;
	partDownload->id_PartDownload= record.value("id").toInt();


	partDownload->start_byte = record.value("Start_byte").toLongLong();
	partDownload->end_byte = record.value("End_byte").toLongLong();
	partDownload->PartDownloadFile = new QFile(record.value("PartDownload_SaveTo").toString());

	partDownload->PartDownloadFile->open(QIODevice::WriteOnly | QIODevice::Append);


	partDownload->LastDownloadedByte = partDownload->start_byte+partDownload->PartDownloadFile->size()-1;


	return true;
}

bool ProcessDatabaseOutput::ProcessPutLoadedQueueInformationInQueueObject(const QSqlRecord& record, Queue* queue)
{
	queue->QueueId=record.value("id").toInt();
	queue->QueueName = record.value("Name").toString();
	queue->MaxSpeed=record.value("MaxSpeed").toInt();
	queue->NumberDownloadAtSameTime= record.value("NumberDownloadSameTime").toInt();
	return true;
}

bool ProcessDatabaseOutput::PutDownloadIdInQueueDownloadList(const QSqlRecord& record, Queue* queue)
{
	queue->List_DownloadId.append(record.value("id").toInt());
	return true;
}
