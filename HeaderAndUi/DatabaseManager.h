#pragma once

#include <QObject>
#include "Download.h"
#include "DatabaseQueryPreparer.h"
#include "DatabaseInteract.h"
#include "ProcessDatabaseOutput.h"
#include "qstandarditemmodel.h"
#include <QTreeWidgetItem>
#include "qthread.h"


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

	static bool AddDownloadToQueueOnDatabase(Download* Download,Queue* queue);
	static bool UpdateTimeQueueEvents(Queue* queue);

	static bool AddDownloadTo_Queue_Download(Queue* queue, Download* download);
	static bool RemoveDownloadFrom_Queue_Download(Download* download);
	static bool DecreaseDownloadNumberOfQueueListForNextDownloadInQueueListOn_Queue_Download(Queue* queue,int DownloadNumberInQueueList);
	static bool ExitAllDownloadFrom_Queue_Download(Queue* queue);
	static size_t GetNumberDownloadInListOfQueue(Download* download);

	static bool MoveDownloadIn_Queue_Download(Queue* queue, Download* download, int moveNumber);
	static size_t GetturnInIdOfDownload(Queue* queue, int NumnberDownloadInList);

	static void LoadDownloadInformationOfQueueForScheduleTreeWidget(QList<QTreeWidgetItem*>& TreeWidgetItems,Queue* queue);

	static bool UpdateNumberOfDownloadAtSameTimeOfQueue(Queue* queue);

	static bool RemoveAllCompletedDownload();

public:
	DatabaseManager(QObject *parent=nullptr);
	~DatabaseManager();
};
