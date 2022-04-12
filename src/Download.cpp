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
