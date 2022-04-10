#include "HeaderAndUi/MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

    TableViewController* tableview = new TableViewController();
    tableview->Set_TableView(ui.tableView);
    tableview->ProcessSetupOfTableView();


}

MainWindow::~MainWindow()
{
}

void MainWindow::on_actionAdd_new_Download_triggered()
{
	emit AddNewDownload();
}

void MainWindow::LoadDownloadsForMainTable()
{

}

