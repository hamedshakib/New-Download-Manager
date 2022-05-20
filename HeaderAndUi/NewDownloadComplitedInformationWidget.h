#pragma once

#include <QWidget>
#include "ui_NewDownloadComplitedInformationWidget.h"
#include "qfiledialog.h"
#include "qcombobox.h"
#include "qthread.h"

class NewDownloadComplitedInformationWidget : public QWidget
{
	Q_OBJECT

public:
	NewDownloadComplitedInformationWidget(QWidget* parent = nullptr);
	~NewDownloadComplitedInformationWidget();

	void initNewDownloadComplitedInformationWidget(QUrl BaseUrl);

	bool SetMoreCompliteInformation(QUrl RealUrl, QString TypeFile, QString Size, QString SaveTo, QString description = "");
	//bool SetQueueManager(QueueManager* queueManager);

signals:
	//void DownloadNow  (QUrl url,QUrl FileSaveToAddress);
	//void DownloadLater(QUrl url,QUrl FileSaveToAddress);
	void VerifiedDownload(QUrl url, QUrl FileSaveToAddress,bool Is_DownloadNow);

private slots:
	void on_SaveAs_toolButton_clicked();
	void on_DownloadNow_pushButton_clicked();
	void on_DownloadLater_pushButton_clicked();
	void on_Cancel_pushButton_clicked();

public slots:
	static QString ChooseNameForNewDownloadFile(QString SaveTo, QUrl RealUrl);

private:
	QUrl RealUrl;
	Ui::NewDownloadComplitedInformationWidget ui;
};
