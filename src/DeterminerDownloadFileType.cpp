#include "HeaderAndUi/DeterminerDownloadFileType.h"

DeterminerDownloadFileType::DeterminerDownloadFileType(QObject *parent)
	: QObject(parent)
{
}

DeterminerDownloadFileType::~DeterminerDownloadFileType()
{
}

QString DeterminerDownloadFileType::DetermineDownloadFileType(QNetworkReply* reply)
{
	QUrl url = reply->url();
	int lastPostionOfDot=url.path().lastIndexOf(".");

	QString AcceptedSuffix = "";
	QString Type = url.path().mid(lastPostionOfDot + 1);
	QString MimeType=reply->header(QNetworkRequest::ContentTypeHeader).toString();
	DatabaseManager databaseManager;

	//qDebug() << "TempM:" << MimeType;
	QStringList listOfAbleSuffixs=databaseManager.LoadSuffixsForMimeType(MimeType);
	qDebug() << listOfAbleSuffixs;
	for (auto suffix : listOfAbleSuffixs)
	{
		if (suffix == Type)
		{
			AcceptedSuffix = suffix;
			break;
		}
	}

	if (AcceptedSuffix.isEmpty())
	{
		AcceptedSuffix = Type;
	}
	return AcceptedSuffix;
}
