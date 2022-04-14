#include "HeaderAndUi/ProcessEnum.h"

ProcessEnum::ProcessEnum(QObject *parent)
	: QObject(parent)
{
}

ProcessEnum::~ProcessEnum()
{
}

Download::DownloadStatusEnum ProcessEnum::ConvertStringToDownloadStatusEnum(QString str)
{
	if (str == "NotStarted")
	{
		return Download::DownloadStatusEnum::NotStarted;
	}
	else if (str == "Started")
	{
		return Download::DownloadStatusEnum::Pause;
	}
	else if (str == "Completed")
	{
		return Download::DownloadStatusEnum::Completed;
	}

}

Download::ResumeCapabilityEnum ProcessEnum::ConvertHeaderToResumeCapabilityEnum(QString Header)
{
	if (Header == "bytes")
	{
		return Download::ResumeCapabilityEnum::Yes;
	}
	else if (Header == "none")
	{
		return Download::ResumeCapabilityEnum::No;
	}
	else
	{
		return Download::ResumeCapabilityEnum::UnKnown;
	}
}

int ProcessEnum::ConvertResumeCapabilityEnumToResumeCapabilityId(Download::ResumeCapabilityEnum resumeCapabilityEnum)
{
	if (resumeCapabilityEnum == Download::ResumeCapabilityEnum::Yes)
	{
		return 2;
	}
	else if (resumeCapabilityEnum == Download::ResumeCapabilityEnum::Yes)
	{
		return 3;
	}
	else
	{
		return 1;
	}
}

Download::ResumeCapabilityEnum ProcessEnum::ConvertDatabseStringToResumeCapabilityEnum(QString str)
{
	if (str == "UnKnown")
	{
		return Download::ResumeCapabilityEnum::UnKnown;
	}
	else if (str == "YES")
	{
		return Download::ResumeCapabilityEnum::Yes;
	}
	else if(str=="NO")
	{
		return Download::ResumeCapabilityEnum::No;
	}
}

QString ProcessEnum::ConvertResumeCapabilityEnumToString(Download::ResumeCapabilityEnum resumeCapabilityEnum)
{
	if (resumeCapabilityEnum == Download::ResumeCapabilityEnum::UnKnown)
	{
		return "Unknown";
	}
	else if (resumeCapabilityEnum == Download::ResumeCapabilityEnum::Yes)
	{
		return "Yes";
	}
	else if (resumeCapabilityEnum == Download::ResumeCapabilityEnum::No)
	{
		return "No";
	}
}

int ProcessEnum::ConvertDownloadStatusEnumToDownloadStatusId(Download::DownloadStatusEnum downloadStatusEnum)
{
	if (downloadStatusEnum == Download::DownloadStatusEnum::Completed)
	{
		return 3;
	}
	else if (downloadStatusEnum == (Download::DownloadStatusEnum::Downloading| Download::DownloadStatusEnum::Pause))
	{
		return 2;
	}
	else if (downloadStatusEnum == Download::DownloadStatusEnum::NotStarted)
	{
		return 1;
	}
}

QString ProcessEnum::ConvertDownloadStatusEnumToString(Download::DownloadStatusEnum downloadStatusEnum)
{
	if (downloadStatusEnum == Download::DownloadStatusEnum::Downloading)	
	{
		return "Downloading";
	}
	else if(Download::DownloadStatusEnum::Pause)
	{
		return "Pause";
	}
	else if (downloadStatusEnum == Download::DownloadStatusEnum::NotStarted)
	{
		return "NotStarted";
	}
	if (downloadStatusEnum == Download::DownloadStatusEnum::Completed)
	{
		return "Completed";
	}
}

