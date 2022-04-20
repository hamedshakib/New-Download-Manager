#pragma once

#include <QObject>
#include "Download.h"
#include "Downloader.h"
#include "qsqlquery.h"
#include "SettingUpDatabase.h"
#include "ProcessEnum.h"
#include "QString"
#include "DateTimeManager.h"
#include "Queue.h"


class DatabaseQueryPreparer : public QObject
{
	Q_OBJECT

public:
	static QSqlQuery* PrepareQueryForLoadDownload(int Download_id);
	static QSqlQuery* PrepareQueryChangeLastbitDownloaded(int PartDownload_id,qint64 LastbitDownloaded);
	static QSqlQuery* PrepareQuerySuffixsFromMimeType(QString MimeType);
	static QSqlQuery* PrepareQueryForCreateNewDownload(Download* download);
	static QSqlQuery* PrepareQueryForCreateNewPartDownload(PartDownload* partDownload);

	static QSqlQuery* PrepareQueryForLoadDownloadForMainTable();

	static QSqlQuery* PrepareQueryForUpdateAllFieldDownload(Download* download);


	static QList<QSqlQuery*> PrepareQueryForFinishDownload(Download* download);
	static QSqlQuery* PrepareQueryForUpdateInStartDownload(Download* download);
	static QList<QSqlQuery*> PrepareQueriesForUpdateInDownloading(Download* download);



	static QSqlQuery* PrepareQueriesForCheckStatusOfDownload(int Download_id);

	static QSqlQuery* PrepareQueryForLoadPartDownloadOfDownload(int Download_id);

	static QSqlQuery* PrepareQueryForRemovePartDownloadsOfDownloadFromDatabase(Download* download);
	static QSqlQuery* PrepareQueryForRemoveDownloadFromDatabase(Download* download);

	static QSqlQuery* PrepareQueryForLoadAllQueuesGeneralInformationFromDatabase();
	static QSqlQuery* PrepareQueryForGetDownloadIdOfQueue(Queue* queue);
	static QSqlQuery* PrepareQueryForRemoveDownloadFromQueueOnDatabase(Download* download);
	static QSqlQuery* PrepareQueryForCreateNewQueue(Queue* queue);
	static QSqlQuery* PrepareQueryForExitAllDownloadFromQueue(Queue* queue);
	static QSqlQuery* PrepareQueryForRemoveQueueFromDatabase(Queue* queue);

public:
	DatabaseQueryPreparer(QObject *parent);
	~DatabaseQueryPreparer();
};
