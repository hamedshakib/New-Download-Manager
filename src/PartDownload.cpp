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
	QMutexLocker lock(&stateMutex);
	if (this->LastDownloadedByte >= this->end_byte)
	{
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
	QMutexLocker lock(&stateMutex);
	this->LastDownloadedByte = this->start_byte + this->PartDownloadFile->size() - 1;
}

qint64 PartDownload::GetLastDownloadedByte() const
{
	QMutexLocker lock(&stateMutex);
	return LastDownloadedByte;
}

void PartDownload::SetLastDownloadedByte(qint64 value)
{
	QMutexLocker lock(&stateMutex);
	LastDownloadedByte = value;
}

void PartDownload::AddToLastDownloadedByte(qint64 bytes)
{
	QMutexLocker lock(&stateMutex);
	LastDownloadedByte += bytes;
}
