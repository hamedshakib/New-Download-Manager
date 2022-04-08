#pragma once

#include <QWidget>
#include "ui_NewDownloadComplitedInformationWidget.h"
#include "qfiledialog.h"

class NewDownloadComplitedInformationWidget : public QWidget
{
	Q_OBJECT

public:
	NewDownloadComplitedInformationWidget(QUrl BaseUrl, QWidget* parent = nullptr);
	~NewDownloadComplitedInformationWidget();

	bool SetMoreCompliteInformation(QUrl RealUrl, QString TypeFile, QString Size, QString SaveTo, QString description = "");

signals:
	void DownloadNow(QUrl url,QString FileSaveToAddress);
	void DownloadLater(QUrl url, QString FileSaveToAddress,int QueueId);

private slots:
	void on_SaveAs_toolButton_clicked();
	void on_DownloadNow_pushButton_clicked();
	void on_DownloadLater_pushButton_clicked();
	void on_Cancel_pushButton_clicked();
private:
	QUrl RealUrl;
	Ui::NewDownloadComplitedInformationWidget ui;
};
