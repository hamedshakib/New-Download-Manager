#pragma once

#include <QWidget>
#include "ui_ShowDownloadWidget.h"
#include "Downloader.h"
#include "qdebug.h"
#include "ProcessEnum.h"
#include "ConverterSizeToSuitableString.h"

class ShowDownloadWidget : public QWidget
{
	Q_OBJECT


private:
	Download* m_Download;
	Downloader* m_Downloader;
	QMap< QTreeWidgetItem*, PartDownload*> TreeWidgetMap;
	QList<QTreeWidgetItem*> items;

public slots:
	void ProcessSetup();

private slots:
	void UpdateInDownloading(QString Status, QString speed, QString TimeLeft);
	void on_PauseResume_pushButton_clicked();
	void ChangePauseOrResume_Download();

public:
	ShowDownloadWidget(Downloader* downloader,QWidget *parent=nullptr);
	~ShowDownloadWidget();

private:
	Ui::ShowDownloadWidget ui;
};
