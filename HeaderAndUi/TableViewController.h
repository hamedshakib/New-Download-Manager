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

private:
	QTableView* m_tableView;
	DownloadManager* m_downloadManager;
	DatabaseManager* manager;

	QStandardItemModel* model;
	QStringList listOfColomns;

	QHeaderView* horizontalHeader;

	//void UpdateRow(int Row);
	//void UpdateIndex(QList<QModelIndex&> ListOfIndex);
	//void index
public:
	TableViewController(QTableView *tableView,QObject *parent);
	~TableViewController();
};
