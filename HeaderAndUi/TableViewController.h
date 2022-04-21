#pragma once

#include <QObject>
#include "DownloadManager.h"
#include "qtableview.h"
#include "qstandarditemmodel.h"
#include "qabstractitemmodel.h"
#include "qheaderview.h"
#include "qaction.h"
#include "qmenu.h"
#include "DatabaseManager.h"
#include "OpenFileForUser.h"
#include "ShowDownloadWidget.h"
#include "ShowDownloadProperties.h"
#include "QueueManager.h"


class TableViewController : public QObject
{
	Q_OBJECT


public:
	void ProcessSetupOfTableView();
	void Set_DownloadManager(DownloadManager* downloadManager);
	void Set_QueueManager(QueueManager* queueManager);

public slots:
	void RemoveActionTriggered(Download* download);
	int Get_SeletedFinisedDownloadId();

private slots:
	void OnHeaderRightClicked(const QPoint& pos);
	void doubleClickedOnRow(const QModelIndex& modelindex);
	int FindDownloadIdFromRow(const QModelIndex& modelindex);
	void ProcessCheckAndApply_RightClickOnTable(const QPoint& point);
	void AdjusteTableViewProperty();
	void LoadAllDownloadsFromDatabaseForMainTableView();
	void GetDownloaderOfDownloadId(int DownloadId);

	QMenu* CreaterRightClickMenuForRowRightClicked(int Download_id);

	bool UpdateRowInDownloading(size_t row, QString Status,QString Speed, QString TimeLeft);

	void ConnectorDownloaderToTableUpdateInDownloading(Downloader* downloader);
	void AddNewDownloadToTableView(Download* download);

	ShowDownloadWidget* CreaterShowDownloadWidget(Downloader* downloader);


	void PauseOrResumeActionTriggered(QAction* pauseOrResumeAction,Download* download);
	void OpenFileActionTriggered(Download* download);
	void PropertiesActionTriggered(Download* download);

	void AddDownloadToQueue(Queue* queue, Download* download);
	void RemoveDownloadFromQueue(Download* download);

	void ClickedOnRow(const QModelIndex& modelindex);

signals:
	void SelectedDownloadChanged(int Download_id,bool Is_Completed);

private:
	QTableView* m_tableView;
	QueueManager* queueManager;
	DownloadManager* m_downloadManager;
	DatabaseManager* manager;

	QStandardItemModel* model;
	QStringList listOfColomns;

	QHeaderView* horizontalHeader;
	

	QMap<Downloader*, ShowDownloadWidget*> MapOfShowDownloadWidgets;
	int SelectedFinishedDownload_id = 0;

public:
	TableViewController(QTableView *tableView,QObject *parent);
	~TableViewController();
};
