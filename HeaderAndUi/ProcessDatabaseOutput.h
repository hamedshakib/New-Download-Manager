#pragma once

#include <QObject>
#include "Download.h"
#include "qsqlquery.h"
#include "qsqlrecord.h"
#include "qstandarditemmodel.h"
#include "ProcessEnum.h"
#include "ConverterSizeToSuitableString.h"
#include "TableViewRowCreater.h"
#include "DateTimeManager.h"
#include "Queue.h"

class ProcessDatabaseOutput : public QObject
{
	Q_OBJECT


public:
	static bool ProcessPutLoadedDownloadInformationInDownloadObject(const QSqlRecord& record,Download* download,int download_id);
	static QString ProcessLoadedSuffixsForMimeType(const QSqlRecord& record);

	//static void ProcessPutLoadedInformationOfDownloadForMainTableView(const QSqlRecord& record,QStandardItemModel *model);
	static void ProcessPrepareLoadedInformationForMainTableView(const QSqlRecord& record, QStandardItemModel* model);




	static bool ProcessPutLoadedPartDownloadInInPartDownloadObject(const QSqlRecord& record, PartDownload* partDownload, int Download_id);

	static bool ProcessPutLoadedQueueInformationInQueueObject(const QSqlRecord& record, Queue* queue);
	static bool PutDownloadIdInQueueDownloadList(const QSqlRecord& record, Queue* queue);

public:
	ProcessDatabaseOutput(QObject *parent);
	~ProcessDatabaseOutput();
};
