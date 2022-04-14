#include "HeaderAndUi/PartDownload.h"

PartDownload::PartDownload(QObject *parent)
	: QObject(parent)
{
}

PartDownload::~PartDownload()
{
	if (PartDownloadFile != nullptr)
	{
		PartDownloadFile->deleteLater();
		PartDownloadFile = nullptr;
	}

}
