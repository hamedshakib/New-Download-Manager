#include "HeaderAndUi/Download.h"

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
