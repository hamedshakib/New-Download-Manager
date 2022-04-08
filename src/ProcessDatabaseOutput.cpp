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
	
	//Category, DownloadSize, SizeDownloaded, FileType, TimeLeft, LastTryTime, ResumeCapability, MaxSpeed, description, Queue_id
	//is_Done,Url,SaveTo,Category,DownloadSize,SizeDownloaded,FileType,TimeLeft,LastTryTime,ResumeCapability,MaxSpeed,description,Queue_id
	// 
	//query->value("is_Done").toBool();
	
	download->Url = record.value("Url").toString();
	download->Url = record.value("Url").toString();
	download->Url = record.value("Url").toString();
	download->Url = record.value("Url").toString();
	download->Url = record.value("Url").toString();
	download->Url = record.value("Url").toString();
	download->Url = record.value("Url").toString();
	download->Url = record.value("Url").toString();
	download->Url = record.value("Url").toString();

	download->SaveTo = record.value("SaveTo").toString();




	
	return 0;
}

QString ProcessDatabaseOutput::ProcessLoadedSuffixsForMimeType(const QSqlRecord& record)
{
	return record.value("suffix").toString();
}
