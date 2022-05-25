#pragma once

#include "qobject.h"
#include "TableViewController.h"

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
#include "SelectColumnsForMainTableView.h"
#include "qinputdialog.h"


class MainTableViewController : public TableViewController
{
	Q_OBJECT

public:
	void ProcessSetupOfTableView();
	void Set_DownloadManager(DownloadManager* downloadManager);
	void Set_QueueManager(QueueManager* queueManager);

public slots:
	void RemoveActionTriggered(Download* download);
	int Get_SeletedFinisedDownloadId();
	void LoadAllDownloadsFromDatabaseForMainTableView();

private slots:
	void ClickedOnRow(const QModelIndex& modelindex);
	void doubleClickedOnRow(const QModelIndex& modelindex);
	void OnHeaderRightClicked(const QPoint& pos);
	void ChangeColumnWidth(int numberOfColumn, int NewColumnWidth);
	int FindDownloadIdFromRow(const QModelIndex& modelindex);
	void ProcessCheckAndApply_RightClickOnTable(const QPoint& point);
	void AdjusteTableViewProperty();
	void GetDownloaderOfDownloadId(int DownloadId);

	QMenu* CreaterRightClickMenuForRowRightClicked(int Download_id);

	bool UpdateRowInDownloading(size_t row, QString Status, QString Speed, QString TimeLeft);

	void ConnectorDownloaderToTableUpdateInDownloading(DownloadControl* DownloadControl);
	void AddNewDownloadToTableView(Download* download);

	ShowDownloadWidget* CreaterShowDownloadWidget(DownloadControl* DownloadControl);


	void PauseOrResumeActionTriggered(QAction* pauseOrResumeAction, Download* download);
	void OpenFileActionTriggered(Download* download);
	void OpenFileWithActionTriggered(Download* download);
	void OpenFolderActionTriggered(Download* download);

	void PropertiesActionTriggered(Download* download);

	void AddDownloadToQueue(Queue* queue, Download* download);
	void RemoveDownloadFromQueue(Download* download);

	void HideOrShowColumns();

	void CompeletedDownload(size_t row);

public slots:
	void ChooseColumnsHidden();

signals:
	void SelectedDownloadChanged(int Download_id, bool Is_Completed);

private:
	QTableView* m_tableView;
	QueueManager* queueManager;
	DownloadManager* m_downloadManager;
	DatabaseManager* manager;

	QStandardItemModel* model;
	QStringList listOfColomns;

	QHeaderView* horizontalHeader;


	QMap<DownloadControl*, ShowDownloadWidget*> MapOfShowDownloadWidgets;
	int SelectedFinishedDownload_id = 0;

	QList<int> HiddenColumns;

public:
	MainTableViewController(QTableView* tableView,QObject *parent);
	~MainTableViewController();
};
