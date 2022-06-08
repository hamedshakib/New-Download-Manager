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

void PartDownload::Set_StartByte(qint64 start_byte)
{
	this->start_byte = start_byte;
}


void PartDownload::Set_EndByte(qint64 end_byte)
{
	this->end_byte = end_byte;
}

void PartDownload::Set_LastDownloadedByte(qint64 last_downloaded_byte)
{
	this->LastDownloadedByte = last_downloaded_byte;
}

void PartDownload::Set_PartDownloadFile(QFile* part_download_file)
{
	this->PartDownloadFile = part_download_file;
}


void PartDownload::Set_PartDownloadId(size_t id_part_download)
{
	this->PartDownload_id = id_part_download;
}


void PartDownload::Set_DownloadId(size_t id_download)
{
	this->download_id = id_download;
}


qint64 PartDownload::get_StartByte()
{
	return start_byte;
}

qint64 PartDownload::get_EndByte()
{
	return end_byte;
}


qint64 PartDownload::get_LastDownloadedByte()
{
	return LastDownloadedByte;
}

QFile* PartDownload::get_PartDownloadFile()
{
	return PartDownloadFile;
}

size_t PartDownload::get_PartDownloadId()
{
	return PartDownload_id;
}

size_t PartDownload::get_DownloadId()
{
	return download_id;
}
