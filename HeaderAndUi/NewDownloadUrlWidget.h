#pragma once

#include <QWidget>
#include "ui_NewDownloadUrlWidget.h"
#include "qthread.h"


class NewDownloadUrlWidget : public QWidget
{
	Q_OBJECT

public:
	NewDownloadUrlWidget(QWidget *parent = nullptr);
	~NewDownloadUrlWidget();

	void initNewDownloadUrlWidget();

signals:
	void GetInformations(QUrl url,QString Username,QString Password);

private slots:
	void Accepted();
	void Rejected();

private:
	bool SignalEmmited = false;
	Ui::NewDownloadUrlWidget ui;
};
