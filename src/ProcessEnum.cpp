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
	//if()
	return Download::DownloadStatusEnum::Commpleted;
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
