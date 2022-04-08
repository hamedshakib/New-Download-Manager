#pragma once

#include <QObject>
#include "qnetworkreply.h"
#include "qnetworkrequest.h"
#include "qnetworkaccessmanager.h"
#include "Download.h"
#include "DeterminerDownloadFileType.h"
#include "DatabaseManager.h"
#include "NewDownloadUrlWidget.h"
#include "NewDownloadComplitedInformationWidget.h"
#include "SettingInteract.h"
#include "ConverterSizeToSuitableString.h"
#include "DatabaseManager.h"
#include "ProcessEnum.h"

#include "qeventloop.h"
#include "qnetworkreply.h"
#include "qnetworkrequest.h"
#include "qnetworkaccessmanager.h"

class NewDownloadCreater : public QObject
{
	Q_OBJECT



public:
	void StartProcessOfCreateANewDownload(QObject* parent);
	
private:
	Download* download;
	QUrl BaseUrl,RealDownloadUrl;
	QString UserName, Password;
	QNetworkAccessManager m_networkAccessManager;
	QNetworkReply* m_networkReply=nullptr;
	NewDownloadUrlWidget* newDownloadUrlWidget=nullptr;
	NewDownloadComplitedInformationWidget* newDownloadComplitedInformationWidget=nullptr;
	QEventLoop eventloop;
	QObject* parent;


	
	qint64 DownloadSize;
	QString suffix;
	QString description;


private slots:
	void GetInformationFromUrl(QUrl url, QString UserName = "", QString Password = "");
	bool TryToGetInformationFromUrl(QUrl url, QString UserName = "", QString Password = "");

	bool ProcessNewDownloadUrlWidget();
	bool ProcessNewDownloadMoreComplitedInformationWidget();
	bool ProcessCreatePartDownloadsFromDownload();

	QString GeneratePartDownloadAddressFromAddressOfDownloadFile(int numberOfPart,QString addressOfDownloadFile);


	QString DefualtSaveToAddress();
	size_t DefaultPartForDownload();

	Download* CreateNewDownload(QObject* parent);
	void ProcessInitialInformationFromUrl();
	void CancelNewDownload();
	size_t WriteDownloadInDatabase();
	void WritePartDownloadsInDatabase();

	void ProcessCompleteInformation();
	void VerifiedDownload_DownloadNow(QUrl url, QString FileSaveToAddress);
	void VerifiedDownload_DownloadLater(QUrl url, QString FileSaveToAddress, int QueueId);

signals:
	void CreatedNewDownload(Download* download);
	void DownloadNow(Download* download);

public:
	NewDownloadCreater(QObject *parent);
	~NewDownloadCreater();
};
