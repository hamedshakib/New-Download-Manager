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

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();



private slots:
	void on_actionAdd_new_Download_triggered();
	void LoadDownloadsForMainTable();


signals:
	void AddNewDownload();


private:
	Ui::MainWindow ui;
};
