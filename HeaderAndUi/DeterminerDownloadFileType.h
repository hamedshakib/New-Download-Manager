#pragma once

#include <QObject>
#include "qnetworkreply.h"
#include "DatabaseManager.h"

class DeterminerDownloadFileType : public QObject
{
	Q_OBJECT

public:
	static QString DetermineDownloadFileType(QNetworkReply* reply);

public:
	DeterminerDownloadFileType(QObject *parent);
	~DeterminerDownloadFileType();
};
