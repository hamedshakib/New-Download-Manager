#pragma once

#include <QObject>
#include "Download.h"
#include "qsqlquery.h"
#include "qsqlrecord.h"

class ProcessDatabaseOutput : public QObject
{
	Q_OBJECT


public:
	static bool ProcessPutLoadedDownloadInformationInDownloadObject(const QSqlRecord& record,Download* download,int download_id);
	static QString ProcessLoadedSuffixsForMimeType(const QSqlRecord& record);

	//static QList<Row


public:
	ProcessDatabaseOutput(QObject *parent);
	~ProcessDatabaseOutput();
};
