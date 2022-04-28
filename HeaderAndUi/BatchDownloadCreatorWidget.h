#pragma once

#include <QWidget>
#include "ui_BatchDownloadCreatorWidget.h"
#include "SettingInteract.h"
#include "qfiledialog.h"

class BatchDownloadCreatorWidget : public QWidget
{
	Q_OBJECT

public:
	BatchDownloadCreatorWidget(QWidget *parent = nullptr);
	~BatchDownloadCreatorWidget();

private slots:
	void on_Download_pushButton_clicked();
	void on_Cancel_pushButton_clicked();
	void on_toolButton_clicked();

	void ChangeWidgetOfBatch(QString Type);
	void DetermineSequentialType();


	void CreateNewBatchDownloadSequentialNumericsAddress(QString BaseUrl,int start,int end);
	void CreateNewBatchDownloadSequentialLettersAddress(QString BaseUrl,QString start,QString end);

signals:
	void NewBatchDownload(QList<QString> listOfAddress,QString SaveTo,QString Username,QString Password);

private:
	Ui::BatchDownloadCreatorWidget ui;
};
