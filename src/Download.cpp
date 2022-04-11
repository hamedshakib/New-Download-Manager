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

//bool Download::AddDownloadToDownloadList(Download* download)
//{
//	ListOfDownloads.append(download);
//	return true;
//}
//
//Download* Download::get_Download(int DownloadId)
//{
//	for (Download* download : ListOfDownloads)
//	{
//		return download;
//	}
//
//	//Can't Find 
//	Download* download;
//	return download;
//}
//
//bool Download::RemoveDownloadFromDownloadList(Download* download)
//{
//	if (ListOfDownloads.removeOne(download))
//		return true;
//	else
//		return false;
//}
//
//bool Download::RemoveDownloadFromDownloadList(int download_id)
//{
//	for (Download* download : ListOfDownloads)
//	{
//		if (download->IdDownload == download_id)
//		{
//			ListOfDownloads.removeOne(download);
//			return true;
//		}
//	}
//	return false;
//}
