#include "HeaderAndUi/MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	LoadSizeOfWidnow();
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

void MainWindow::LoadSizeOfWidnow()
{
	QString StringOfWidnowSize = SettingInteract::GetValue("SizeOfThing/MainWindow").toString();
	if (!StringOfWidnowSize.isEmpty())
	{
		int positionOfDash = StringOfWidnowSize.indexOf("-");
		this->resize(StringOfWidnowSize.mid(0, positionOfDash).toInt(), StringOfWidnowSize.mid(positionOfDash + 1, 30).toInt());
	}
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
	//Size Of Widnow Is Changed
	SettingInteract::SetValue("SizeOfThing/MainWindow",QString::number(this->size().width()) + "-" + QString::number(this->size().height()));
}

void MainWindow::on_actionAbout_triggered()
{
	QMessageBox messageBox(this);
	messageBox.setText("Download Manager developed by Hamed shakib");
	messageBox.setWindowTitle("About Appication");
	messageBox.setStandardButtons(QMessageBox::Ok);
	messageBox.setDefaultButton(QMessageBox::Ok);
	QPixmap pixmap_qtLogo(":Icons/qt-logo.png");

	pixmap_qtLogo = pixmap_qtLogo.scaled(75, 25,Qt::AspectRatioMode::KeepAspectRatioByExpanding,Qt::TransformationMode::SmoothTransformation);
	messageBox.setIconPixmap(pixmap_qtLogo);
	messageBox.exec();

}

void MainWindow::on_actionExit_triggered()
{
	qApp->exit();
}

void MainWindow::on_actionOptions_triggered()
{
	OptionsWidget* optionsWidget = new OptionsWidget(this);
	optionsWidget->show();
	//connect(optionsWidget, &QWidget::closeEvent, this, [&optionsWidget](QCloseEvent *event) {optionsWidget->deleteLater(); });
}

