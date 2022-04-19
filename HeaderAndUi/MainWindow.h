#pragma once

#include <QMainWindow>
#include "ui_MainWindow.h"
#include "DatabaseManager.h"
#include "qdebug.h"
#include "qsqltablemodel.h"
#include "SettingUpDatabase.h"
#include "qabstractitemmodel.h"
#include "qstandarditemmodel.h"
#include "TableViewController.h"
#include "DownloadManager.h"
#include "QueueManager.h"
#include "ShowSchedule.h"
#include "OptionsWidget.h"
#include <QResizeEvent>
#include "qmessagebox.h"
#include "qpixmap.h"


class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

	void SetDownloadManager(DownloadManager* downloadManager);
	void SetQueueManaget(QueueManager* queueManager);
	void CreateTableViewControllerForMainWindow();

private slots:
	void on_actionAdd_new_Download_triggered();
	void on_actionAbout_triggered();
	void on_actionExit_triggered();
	void on_actionOptions_triggered();
	void on_actionScheduler_triggered();
	void LoadDownloadsForMainTable();
	void LoadSizeOfWidnow();

signals:
	void AddNewDownload();

protected:
	void resizeEvent(QResizeEvent* event);


private:
	TableViewController* tableViewController;
	DownloadManager* downloadManagerPointer;
	QueueManager* queueManager;
	Ui::MainWindow ui;
};
