#pragma once

#include <QObject>
#include "qtableview.h"
#include "qstandarditemmodel.h"
#include "qabstractitemmodel.h"
#include "qheaderview.h"
#include "qaction.h"
#include "qmenu.h"

template <typename T>
QStandardItem* newItem(const T val) {
	auto item = new QStandardItem;
	item->setData(val, Qt::DisplayRole);
	return item;
}

class TableViewController : public QObject
{
private:
	Q_OBJECT

public:

	bool Set_TableView(QTableView* tableview);
	void ProcessSetupOfTableView();
	

	//void Test();

private slots:
	void doubleClickedOnRow(const QModelIndex& modelindex);
	QList<QStandardItem*> PrepareDataForRow(int id,QString FileName,QString Size,QString Status,QString Speed,QString TimeLeft,QString LastTryTime,QString Description,QString SaveTo);
	void ProcessCheckAndApply_RightClickOnTable(const QPoint& point);

	void AdjusteTableViewProperty();

private slots:
	void OnHeaderRightClicked(const QPoint& pos);
	void LoadAllDownloadsFromDatabase();


	int FindDownloadIdFromRow(const QModelIndex& modelindex);
private:
	QTableView* m_table;
	QStandardItemModel* model;
	QStringList listOfColomns;

	QHeaderView* horizontalHeader;

public:
	TableViewController(QObject *parent=nullptr);
	~TableViewController();
};
