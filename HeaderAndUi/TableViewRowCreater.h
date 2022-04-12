#pragma once

#include <QObject>
#include "qstandarditemmodel.h"
#include "Download.h"
#include "ConverterSizeToSuitableString.h"

template <typename T>
QStandardItem* newItem(const T val) {
	auto item = new QStandardItem;
	item->setData(val, Qt::DisplayRole);
	return item;
}

class TableViewRowCreater : public QObject
{
	Q_OBJECT

public:
	static QList<QStandardItem*> PrepareDataForRow(Download *download);
	static QList<QStandardItem*> PrepareDataForRow(int id, QString FileName, QString Size, QString Status, QString Speed, QString TimeLeft, QString LastTryTime, QString Description, QString SaveTo);

public:
	TableViewRowCreater(QObject *parent);
	~TableViewRowCreater();
};
