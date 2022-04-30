#pragma once

#include <QObject>
#include "qtableview.h"

class TableViewController : public QObject
{
	Q_OBJECT

public slots:
	virtual void ClickedOnRow(const QModelIndex& modelindex)=0;
	virtual void doubleClickedOnRow(const QModelIndex& modelindex)=0;
	virtual void OnHeaderRightClicked(const QPoint& pos)=0;
	virtual void ChangeColumnWidth(int numberOfColumn, int NewColumnWidth)=0;
public:
	TableViewController(QTableView* tableView,QObject *parent);
	~TableViewController();
};
