#pragma once

#include <QDialog>
#include "ui_CompleteDownloadDialog.h"
#include "qdesktopservices.h"
#include "qdrag.h"
#include "qmimedata.h"
#include "qevent.h"
#include "OpenFileForUser.h"

//#include <iostream>
//#include <cstring>
//#include <fstream>

class CompleteDownloadDialog : public QDialog
{
	Q_OBJECT

public:
	CompleteDownloadDialog(QString AddressOfFile,QString SizeString,QString Url,QWidget *parent = nullptr);
	~CompleteDownloadDialog();

private slots:
	void on_Open_pushButton_clicked();
	void on_OpenWith_pushButton_clicked();
	void on_OpenFolder_pushButton_clicked();

protected:
	void mousePressEvent(QMouseEvent* event) override;
	void closeEvent(QCloseEvent* event) override;

private:
	QString AddressOfFile;
	QString sizeString;
	Ui::CompleteDownloadDialog ui;
};
