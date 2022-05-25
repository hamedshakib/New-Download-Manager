#pragma once

#include <QWidget>
#include "ui_ShowDownloadWidget.h"
//#include "Downloader.h"
#include "DownloadControl.h"
#include "qdebug.h"
#include "ProcessEnum.h"
#include "ConverterSizeToSuitableString.h"

class ShowDownloadWidget : public QWidget
{
	Q_OBJECT


private:
	Download* m_Download;
	DownloadControl* m_DownloadControl;
	QMap< QTreeWidgetItem*, PartDownload*> TreeWidgetMap;
	QList<QTreeWidgetItem*> items;

	bool isChangeSpeedNow;

public slots:
	void ProcessSetup();

private slots:
	void UpdateInDownloading(QString Status, QString speed, QString TimeLeft,QList<qint64> DownloadedBytesEachPartDownloadList);
	void on_PauseResume_pushButton_clicked();
	void ChangePauseOrResume_Download();


	void ChangeShowSpeedFromDownloader(int Speed);
	void ChangeDownloaderSpeed(int speed);
	void ClickedCheckBox(bool is_checked);
	void SpinBoxValueChanged(int newValue);



public:
	ShowDownloadWidget(DownloadControl* downloadControl,QWidget *parent=nullptr);
	~ShowDownloadWidget();

private:
	Ui::ShowDownloadWidget ui;
};
