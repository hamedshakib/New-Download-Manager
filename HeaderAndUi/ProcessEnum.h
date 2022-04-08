#pragma once

#include <QObject>
#include "Download.h"

class ProcessEnum : public QObject
{
	Q_OBJECT
public:

	static Download::DownloadStatusEnum ConvertStringToDownloadStatusEnum(QString str);
	static Download::ResumeCapabilityEnum ConvertHeaderToResumeCapabilityEnum(QString Header);




	static int ConvertResumeCapabilityEnumToResumeCapabilityId(Download::ResumeCapabilityEnum resumeCapabilityEnum);

public:
	ProcessEnum(QObject *parent);
	~ProcessEnum();
};
