#include "HeaderAndUi/ApplicationManager.h"

ApplicationManager::ApplicationManager(QObject *parent)
	: QObject(parent)
{
	downloadManager=new DownloadManager(this);
	mainWindow=new MainWindow();
	mainWindow->SetDownloadManager(downloadManager);
	mainWindow->CreateTableViewControllerForMainWindow();
	mainWindow->show();

	connect(mainWindow, &MainWindow::AddNewDownload, downloadManager, &DownloadManager::CreateNewDownload);
}

ApplicationManager::~ApplicationManager()
{
	mainWindow->deleteLater();
}
