#pragma once

#include <QWidget>
#include "ui_ShowDownloadWidget.h" 
#include "DownloadController.h"
#include "qdebug.h"
#include "ProcessEnum.h"
#include "ConverterSizeToSuitableString.h"

class ShowDownloadWidget : public QWidget
{
	Q_OBJECT


private:
	Download* m_Download;
	DownloadController* m_DownloadController;
	QMap< QTreeWidgetItem*, PartDownload*> TreeWidgetMap;
	QList<QTreeWidgetItem*> items;

	bool isChangeSpeedNow;
	void UpdateProgressBar(qint64 downloaded, qint64 total);

public slots:
	void ProcessSetup();

private slots:
	void UpdateInDownloading(QString Status, QString speed, QString TimeLeft,QList<qint64> DownloadedBytesEachPartDownloadList);
	void on_PauseResume_pushButton_clicked();
	void ChangePauseOrResume_Download();

	void ChangeShowSpeedFromDownloadController(int Speed);
	void ChangeDownloadControllerSpeed(int speed);
	void ClickedCheckBox(bool is_checked);
	void SpinBoxValueChanged(int newValue);



public:
	ShowDownloadWidget(DownloadController* DownloadController,QWidget *parent=nullptr);
	~ShowDownloadWidget();

private:
	Ui::ShowDownloadWidget ui;
};
