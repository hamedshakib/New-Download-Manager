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
	// Fix NULL value handling for proper SQLite integration
	// All database fields that can be NULL should be checked before conversion
	
	download->IdDownload = record.value("id").toInt();
	download->FileName = record.value("FileName").toString();
	download->Set_downloadStatus(ProcessEnum::ConvertStringToDownloadStatusEnum(record.value("Status").toString()));
	download->Url = record.value("Url").toString();
	download->DownloadSize = record.value("DownloadSize").toLongLong();
	download->SizeDownloaded = record.value("SizeDownloaded").toLongLong();
	download->suffix = record.value("Suffix").toString();
	download->SaveTo = record.value("SaveTo").toString();
	
	// Fix NULL handling for description
	if (record.value("description").isNull()) {
		download->description = "";
	} else {
		download->description = record.value("description").toString();
	}
	
	// Fix NULL handling for LastTryTime
	QString lastTryTimeString = record.value("LastTryTime").toString();
	if (!lastTryTimeString.isEmpty()) {
		download->LastTryTime = DateTimeManager::GetDateTimeFromString(lastTryTimeString);
	} else {
		download->LastTryTime = QDateTime::currentDateTime();
	}
	
	// Fix NULL handling for MaxSpeed (NULL means unlimited speed)
	if (record.value("MaxSpeed").isNull()) {
		download->MaxSpeed = 0;  // 0 means unlimited speed
	} else {
		download->MaxSpeed = record.value("MaxSpeed").toInt();
	}
	
	download->ResumeCapability = ProcessEnum::ConvertDatabseStringToResumeCapabilityEnum(record.value("ResumeCapability").toString());

	// Fix NULL handling for Queue_id (-1 means no queue assigned)
	if (record.value("Queue_id").isNull()) {
		download->Queue_id = -1;  // -1 indicates no queue assignment
	} else {
		download->Queue_id = record.value("Queue_id").toInt();
	}
	
	// Fix NULL handling for Username
	if (record.value("User").isNull()) {
		download->Username = "";
	} else {
		download->Username = record.value("User").toString();
	}
	
	// Fix NULL handling for Password
	if (record.value("Password").isNull()) {
		download->Password = "";
	} else {
		download->Password = record.value("Password").toString();
	}

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


	QString LastTryTime = DateTimeManager::ConvertDataTimeToString(DateTimeManager::GetDateTimeFromString(record.value("LastTryTime").toString()));  //ToDo Improved with Galaly
	QString Description = record.value("description").toString();
	QString SaveTo = record.value("SaveTo").toString();
	QString Status;
	if (TempStatus == "Completed")
	{
		Status = tr("Complete");
	}
	else
	{
		float Present = (long double)DownloadedSize / DownloadSize;
		Status = QString::number(Present*100, 'f', 2)+"%";
	}
	

	model->appendRow(TableViewRowCreater::PrepareDataForRowForMainTableView(id, FileName, ConverterSizeToSuitableString::ConvertSizeToSuitableString(DownloadSize), Status, "", "", LastTryTime, Description, SaveTo));
}

bool ProcessDatabaseOutput::ProcessPutLoadedPartDownloadInInPartDownloadObject(const QSqlRecord& record, PartDownload* partDownload, int Download_id)
{
	// Fix NULL value handling for PartDownload

	partDownload->id_download = Download_id;
	partDownload->id_PartDownload = record.value("id").toInt();

	// Fix NULL handling for start_byte
	if (record.value("Start_byte").isNull()) {
		qCritical() << "PartDownload" << partDownload->id_PartDownload << "has NULL start_byte";
		return false;
	}
	partDownload->start_byte = record.value("Start_byte").toLongLong();

	// Fix NULL handling for end_byte
	if (record.value("End_byte").isNull()) {
		qCritical() << "PartDownload" << partDownload->id_PartDownload << "has NULL end_byte";
		return false;
	}
	partDownload->end_byte = record.value("End_byte").toLongLong();

	// Fix NULL handling for PartDownload_SaveTo
	QString saveToFile = record.value("PartDownload_SaveTo").toString();
	if (saveToFile.isNull() || saveToFile.isEmpty() || saveToFile == "NULL") {
		qCritical() << "PartDownload" << partDownload->id_PartDownload << "has NULL or empty PartDownload_SaveTo";
		return false;
	}
	partDownload->PartDownloadFile = new QFile(saveToFile);

	if (!partDownload->PartDownloadFile->open(QIODevice::WriteOnly | QIODevice::Append)) {
		qCritical() << "Failed to open PartDownload file:" << saveToFile;
		delete partDownload->PartDownloadFile;
		partDownload->PartDownloadFile = nullptr;
		return false;
	}

	// Fix NULL handling for LastDownloadedByte calculation
	partDownload->LastDownloadedByte = partDownload->start_byte + partDownload->PartDownloadFile->size() - 1;

	return true;
}

bool ProcessDatabaseOutput::ProcessPutLoadedQueueInformationInQueueObject(const QSqlRecord& record, Queue* queue)
{
	queue->QueueId = record.value("id").toInt();
	queue->QueueName = record.value("Name").toString();
	
	// Fix NULL handling for MaxSpeed
	if (record.value("MaxSpeed").isNull()) {
		queue->MaxSpeed = 0;
	} else {
		queue->MaxSpeed = record.value("MaxSpeed").toInt();
	}
	
	queue->NumberDownloadAtSameTime = record.value("NumberDownloadSameTime").toInt();

	// Fix NULL handling for StartTime - use isNull() instead of comparing to QVariant("NULL")
	if (!record.value("StartTime").isNull()) {
		queue->startDownload.is_active = true;
		queue->startDownload.Time = QTime::fromString(record.value("StartTime").toString());

		// Fix NULL handling for DaysOfWeek, OnceTimeAt, EachDays
		QString daysOfWeek = record.value("DaysOfWeek").toString();
		QString onceTimeAt = record.value("OnceTimeAt").toString();
		QString eachDays = record.value("EachDays").toString();
		
		if (!daysOfWeek.isNull() && !daysOfWeek.isEmpty() && daysOfWeek != "NULL") {
			queue->DownloadDays.append(daysOfWeek.split(","));
		} else if (!onceTimeAt.isNull() && !onceTimeAt.isEmpty() && onceTimeAt != "NULL") {
			queue->DownloadDays.append(onceTimeAt);
		} else if (!eachDays.isNull() && !eachDays.isEmpty() && eachDays != "NULL") {
			queue->DownloadDays.append(eachDays);
		}
	}

	// Fix NULL handling for StopTime
	if (!record.value("StopTime").isNull()) {
		queue->stopDownload.is_active = true;
		queue->stopDownload.Time = QTime::fromString(record.value("StopTime").toString());
	}

	return true;
}

bool ProcessDatabaseOutput::PutDownloadIdInQueueDownloadList(const QSqlRecord& record, Queue* queue)
{
	queue->List_DownloadId.append(record.value("id").toInt());
	return true;
}

bool ProcessDatabaseOutput::PutDownloadInformationOfQueueForScheduleTreeWidget(const QSqlRecord& record,QTreeWidgetItem* item)
{
	//"SELECT QD.Download_id,QD.NumbersInList, "
	//	"D.FileName, DownloadSize, DownloadStatus_id "


	item->setData(4, 0, record.value("NumbersInList").toInt());
	item->setData(5, 0, record.value("Download_id").toInt());

	item->setText(0, record.value("FileName").toString());
	item->setText(1, ConverterSizeToSuitableString::ConvertSizeToSuitableString(record.value("DownloadSize").toLongLong()));

	if (record.value("Status").toString()=="NotStarted")
	{
		item->setText(2, tr("Not Started"));
	}
	else
	{
		item->setText(2, tr("Started"));
	}

	item->setText(3, record.value("").toString());

	return true;
}
