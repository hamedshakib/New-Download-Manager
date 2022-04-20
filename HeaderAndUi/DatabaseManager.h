#pragma once

#include <QObject>
#include "Download.h"
#include "DatabaseQueryPreparer.h"
#include "DatabaseInteract.h"
#include "ProcessDatabaseOutput.h"
#include "qstandarditemmodel.h"

#include "qsqlrecord.h"


class DatabaseManager : public QObject
{
	Q_OBJECT



public:
	static bool LoadDownloadComplete(int Download_id, Download* download);
	static QStringList LoadSuffixsForMimeType(QString MimeType);
	static size_t CreateNewDownloadOnDatabase(Download* download);
	static size_t CreateNewPartDownloadOnDatabase(PartDownload* partDownload);
	
	bool LoadAllDownloadsForMainTable(QStandardItemModel* model);



	static bool UpdateAllFieldDownloadOnDataBase(Download* download);
	static bool UpdateDownloadInStartOfDownloadOnDatabase(Download* download);
	static bool UpdateInDownloadingOnDataBase(Download* download);
	static bool RemoveDownloadFromQueueOnDatabase(Download* download);


	static bool FinishDownloadOnDatabase(Download* download);


	//static Q LoadStatusAndFileAddressOfDownload(int download_id);
	static QList<PartDownload*> CreatePartDownloadsOfDownload(int Download_id);

	static bool RemoveDownloadCompleteWithPartDownloadsFromDatabase(Download* download);

	static bool LoadAllQueues(QList<Queue*>& listOfQueues,QObject* object);
	static size_t CreateNewQueueOnDatabase(Queue* queue);
	static bool RemoveQueueFromDatabase(Queue* queue);
	static bool ExitDownloadFromQueue(Download* download);
	static bool ExitAllDownloadFromQueue(Queue* queue);

public:
	DatabaseManager(QObject *parent=nullptr);
	~DatabaseManager();
};
