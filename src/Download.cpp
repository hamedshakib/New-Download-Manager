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
	emit DownloadStatusChanged(status);
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

QString Download::get_Username()
{
	return Username;
}

QString Download::get_Password()
{
	return Password;
}

QTime Download::get_TimeLeft()
{
	return TimeLeft;
}

void Download::Set_Url(QUrl url)
{
	Url = url;
}

void Download::Set_MaxSpeed(size_t maxSpeed)
{
	MaxSpeed = maxSpeed;
}

void Download::Set_SavaTo(QUrl saveTo)
{
	SaveTo = saveTo;
}

void Download::Set_Id(size_t id)
{
	IdDownload = id;
}

void Download::Set_LastTryTime(QDateTime lastTryTime)
{
	LastTryTime = lastTryTime;
}
void Download::Set_FullFileName(QString fullFileName)
{
	FullFileName = fullFileName;
}
void Download::Set_Description(QString description)
{
	this->description = description;
}
void Download::Set_SizeDownload(qint64 sizeDownload)
{
	DownloadSize = sizeDownload;
}
void Download::Set_DownloadedSize(qint64 sizeDownloaded)
{
	SizeDownloaded = sizeDownloaded;
}
void Download::Set_TimeLeft(QTime timeLeft)
{
	TimeLeft = timeLeft;
}

void Download::Set_Username(QString username)
{
	Username = username;
}
void Download::Set_Password(QString password)
{
	Password = password;
}

Download::ResumeCapabilityEnum Download::get_ResumeCapability()
{
	return ResumeCapability;
}

void Download::Set_ResumeCapability(Download::ResumeCapabilityEnum resumeCapability)
{
	ResumeCapability = resumeCapability;
}

QString Download::get_FileName()
{
	return FileName;
}

void Download::Set_FileName(QString fileName)
{
	FileName = fileName;
}

QString Download::get_Suffix()
{
	return suffix;
}

void Download::Set_Suffix(QString suffix)
{
	this->suffix = suffix;
}
