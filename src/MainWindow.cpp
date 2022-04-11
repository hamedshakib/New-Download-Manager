#include "HeaderAndUi/MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);


	QApplication::setQuitOnLastWindowClosed(false);
}

MainWindow::~MainWindow()
{
}
void MainWindow::CreateTableViewControllerForMainWindow()
{
	tableViewController = new TableViewController(ui.tableView, this);
	tableViewController->Set_DownloadManager(downloadManagerPointer);
	tableViewController->ProcessSetupOfTableView();
}

void MainWindow::SetDownloadManager(DownloadManager* downloadManager)
{
	downloadManagerPointer = downloadManager;
}


void MainWindow::on_actionAdd_new_Download_triggered()
{
	emit AddNewDownload();
}

void MainWindow::LoadDownloadsForMainTable()
{

}

