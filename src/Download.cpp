#include "HeaderAndUi/Download.h"

//QList<Download*> Download::ListOfDownloads = {};

Download::Download(QObject *parent)
	:QObject(parent)
{
	
}

Download::~Download()
{
}

QUrl Download::get_Url()
{
	return Url;
}

size_t Download::get_MaxSpeed()
{
	return MaxSpeed;
}

QList<PartDownload*> Download::get_PartDownloads()
{
	return DownloadParts;
}

QUrl Download::get_SavaTo()
{
	return SaveTo;
}

size_t Download::get_Id()
{
	return IdDownload;
}

Download::DownloadStatusEnum Download::get_Status()
{
	return downloadStatus;
}

void Download::Set_downloadStatus(Download::DownloadStatusEnum status)
{
	downloadStatus = status;
}

void Download::AppendPartDownloadToPartDownloadListOfDownload(PartDownload* partDownload)
{
	DownloadParts.append(partDownload);
}

int Download::get_QueueId()
{
	return Queue_id;
}

QString Download::get_FullFileName()
{
	return FullFileName;
}

QString Download::get_Description()
{
	return description;
}

qint64 Download::get_SizeDownload()
{
	return DownloadSize;
}

qint64 Download::get_DownloadedSize()
{
	return SizeDownloaded;
}

void Download::Set_QueueId(int Queue_id)
{
	this->Queue_id = Queue_id;
}

QDateTime Download::get_LastTryTime()
{
	return LastTryTime;
}
