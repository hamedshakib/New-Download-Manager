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
	static Download::ResumeCapabilityEnum ConvertDatabseStringToResumeCapabilityEnum(QString str);
	static QString ConvertResumeCapabilityEnumToString(Download::ResumeCapabilityEnum resumeCapabilityEnum);


	static int ConvertDownloadStatusEnumToDownloadStatusId(Download::DownloadStatusEnum downloadStatusEnum);

	static QString ConvertDownloadStatusEnumToString(Download::DownloadStatusEnum downloadStatusEnum);
public:
	ProcessEnum(QObject *parent);
	~ProcessEnum();
};
