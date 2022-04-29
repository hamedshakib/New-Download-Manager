#include "HeaderAndUi/TableViewRowCreater.h"

TableViewRowCreater::TableViewRowCreater(QObject *parent)
	: QObject(parent)
{
}

TableViewRowCreater::~TableViewRowCreater()
{
}

QList<QStandardItem*> TableViewRowCreater::PrepareDataForRowForMainTableView(int id, QString FileName, QString Size, QString Status, QString Speed, QString TimeLeft, QString LastTryTime, QString Description, QString SaveTo)
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

QList<QStandardItem*> TableViewRowCreater::PrepareDataForRowForMainTableView(Download* download)
{
	QList<QStandardItem*> listOfItems;

	
	listOfItems << newItem(download->IdDownload);
	listOfItems << newItem(download->FileName);
	listOfItems << newItem(ConverterSizeToSuitableString::ConvertSizeToSuitableString(download->DownloadSize));

	QString DownloadStatus;
	if (download->downloadStatus==Download::Completed)
	{
		DownloadStatus = tr("Complete");
	}
	else if (download->downloadStatus == Download::NotStarted)
	{
		DownloadStatus = "0%";
	}
	else
	{
		float Present = (long double)download->SizeDownloaded / download->DownloadSize;
		DownloadStatus = QString::number(Present, 'f', 2);
	}






	listOfItems << newItem(DownloadStatus);
	listOfItems << newItem("");
	listOfItems << newItem("");
	listOfItems << newItem(DateTimeManager::ConvertDataTimeToString(download->LastTryTime));
	listOfItems << newItem(download->description);
	listOfItems << newItem(download->SaveTo);
	

	return listOfItems;
}

QList<QStandardItem*> TableViewRowCreater::PrepareDataForRowForScheduleTableView(Download* download)
{
	QList<QStandardItem*> listOfItems;

	listOfItems << newItem(download->IdDownload);
	listOfItems << newItem(download->FileName);
	listOfItems << newItem(ConverterSizeToSuitableString::ConvertSizeToSuitableString(download->DownloadSize));

	QString DownloadStatus;
	if (download->downloadStatus == Download::Completed)
	{
		DownloadStatus = tr("Complete");
	}
	else if (download->downloadStatus == Download::NotStarted)
	{
		DownloadStatus = "0%";
	}
	else
	{
		float Present = (long double)download->SizeDownloaded / download->DownloadSize;
		DownloadStatus = QString::number(Present, 'f', 2);
	}

	listOfItems << newItem(DownloadStatus);
	listOfItems << newItem("");

	return listOfItems;
}

QList<QStandardItem*> TableViewRowCreater::PrepareDataForRowForScheduleTableView(int id, QString FileName, QString Size, QString Status, QString TimeLeft)
{
	QList<QStandardItem*> listOfItems;

	listOfItems << newItem(id);
	listOfItems << newItem(FileName);
	listOfItems << newItem(Size);
	listOfItems << newItem(Status);
	listOfItems << newItem(TimeLeft);

	return listOfItems;
}
