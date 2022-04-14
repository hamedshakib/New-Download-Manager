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

class TableViewController : public QObject
{
	Q_OBJECT


public:
	void ProcessSetupOfTableView();
	void Set_DownloadManager(DownloadManager* downloadManager);
	


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

private:
	QTableView* m_tableView;
	DownloadManager* m_downloadManager;
	DatabaseManager* manager;

	QStandardItemModel* model;
	QStringList listOfColomns;

	QHeaderView* horizontalHeader;


	//QList<ShowDownloadWidget*> ListOfShowDownloadWidgets;

	QMap<Downloader*, ShowDownloadWidget*> MapOfShowDownloadWidgets;
public:
	TableViewController(QTableView *tableView,QObject *parent);
	~TableViewController();
};
