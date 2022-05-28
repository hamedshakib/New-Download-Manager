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

bool PartDownload::IsPartDownloadFinished()
{
	qDebug() << LastDownloadedByte;
	if (this->LastDownloadedByte >= this->end_byte)
	{
		qDebug() << "**downloadC last:" << this->LastDownloadedByte << " end:" << this->end_byte;
		is_Finished = true;
		return true;
	}
	else
	{
		return false;
	}
}

void PartDownload::UpdatePartDownloadLastDownloadedByte()
{
	this->LastDownloadedByte = this->start_byte + this->PartDownloadFile->size() - 1;
}
