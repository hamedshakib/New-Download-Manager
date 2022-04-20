#pragma once

#include <QObject>
#include "Download.h"
#include "qnetworkproxy.h"

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

	static QNetworkProxy::ProxyType ConvertProxyTypeStringToProxyTypeEnum(QString proxyTypeString);
	static QString ConvertProxyTypeEnumToProxyTypeString(QNetworkProxy::ProxyType proxyType);
public:
	ProcessEnum(QObject *parent);
	~ProcessEnum();
};
