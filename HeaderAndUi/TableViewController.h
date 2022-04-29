#pragma once

#include <QObject>
#include "qtableview.h"

class TableViewController : public QObject
{
	Q_OBJECT

public:
	TableViewController(QTableView* tableView,QObject *parent);
	~TableViewController();
};
