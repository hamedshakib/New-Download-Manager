#pragma once

#include <QMainWindow>
#include "ui_MainWindow.h"
#include "DatabaseManager.h"
#include "qdebug.h"
#include "qsqltablemodel.h"
#include "SettingUpDatabase.h"
#include "qabstractitemmodel.h"
#include "qstandarditemmodel.h"
//#include "TableViewController.h"
#include "MainTableViewController.h"
#include "TreeViewController.h"
#include "DownloadManager.h"
#include "QueueManager.h"
#include "ShowSchedule.h"
#include "OptionsWidget.h"
#include "BatchDownloadCreatorWidget.h"
#include <QResizeEvent>
#include "qmessagebox.h"
#include "qpixmap.h"
#include "qprocess.h"


class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

	void SetDownloadManager(DownloadManager* downloadManager);
	void SetQueueManaget(QueueManager* queueManager);
	void CreateMainTableViewControllerForMainWindow();
	void CreateTreeViewController();
	void LoadTreeView();

private slots:
	void on_actionAdd_new_Download_triggered();
	void on_actionAdd_batch_download_triggered();
	void on_actionAbout_triggered();
	void on_actionExit_triggered();
	void on_actionOptions_triggered();
	void on_actionScheduler_triggered();
	void on_actionDownload_Now_triggered();
	void on_actionStop_Download_triggered();
	void on_actionRemove_triggered();
	void on_actionStop_All_triggered();
	void on_actionEnglish_triggered();
	void on_actionPersian_triggered();

	void LoadDownloadsForMainTable();
	void LoadSizeOfWidnow();

	void ChangedDownloadSelected(int Download_id,bool Is_Completed);
	void ChangedStatusOfSeletedDownload(Download::DownloadStatusEnum NewStatus);
	void ProcessForChangeLanguage();

signals:
	void AddNewDownload();
	void NewBatchDownload(QList<QString> listOfAddress, QString SaveTo, QString Username, QString Password);

protected:
	void resizeEvent(QResizeEvent* event);


private:
	MainTableViewController* mainTableViewController;
	TreeViewController* treeViewController;
	DownloadManager* downloadManagerPointer;
	QueueManager* queueManager;
	Ui::MainWindow ui;

	Download* SelectedDownload=nullptr;
};
