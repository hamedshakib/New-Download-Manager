#pragma once

#include <QWidget>
#include "ui_ShowDownloadProperties.h"
#include "Download.h"
#include "OpenFileForUser.h"
#include "ConverterSizeToSuitableString.h"

#include "qfileinfo.h"

class ShowDownloadProperties : public QWidget
{
	Q_OBJECT

public:
	ShowDownloadProperties(Download* download,QWidget *parent = nullptr);
	~ShowDownloadProperties();

	void ShowPropertiesOfDownload();

private slots:
	void on_Open_pushButton_clicked();
	void on_Ok_pushButton_clicked();


private:
	Download* m_download;


private:
	Ui::ShowDownloadProperties ui;
};
