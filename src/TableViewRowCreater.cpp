#include "HeaderAndUi/TableViewRowCreater.h"

TableViewRowCreater::TableViewRowCreater(QObject *parent)
	: QObject(parent)
{
}

TableViewRowCreater::~TableViewRowCreater()
{
}

QList<QStandardItem*> TableViewRowCreater::PrepareDataForRow(int id, QString FileName, QString Size, QString Status, QString Speed, QString TimeLeft, QString LastTryTime, QString Description, QString SaveTo)
{
	QList<QStandardItem*> listOfItems;

	listOfItems << newItem(id);
	listOfItems << newItem(FileName);
	listOfItems << newItem(Size);
	listOfItems << newItem(Status);
	listOfItems << newItem(Speed);
	listOfItems << newItem(TimeLeft);
	listOfItems << newItem(LastTryTime);
	listOfItems << newItem(Description);
	listOfItems << newItem(SaveTo);

	return listOfItems;
}
