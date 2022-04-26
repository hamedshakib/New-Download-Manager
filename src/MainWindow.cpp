#include "HeaderAndUi/MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	LoadSizeOfWidnow();

	ui.actionDownload_Now->setEnabled(false);
	ui.actionStop_Download->setEnabled(false);
	ui.actionRemove->setEnabled(false);
}

MainWindow::~MainWindow()
{
}
void MainWindow::CreateTableViewControllerForMainWindow()
{
	tableViewController = new TableViewController(ui.tableView, this);
	tableViewController->Set_DownloadManager(downloadManagerPointer);
	tableViewController->ProcessSetupOfTableView();
	tableViewController->Set_QueueManager(queueManager);
	connect(tableViewController, &TableViewController::SelectedDownloadChanged, this, &MainWindow::ChangedDownloadSelected);
}

void MainWindow::CreateTreeViewController()
{
	treeViewController = new TreeViewController(ui.treeView, this);
	treeViewController->Set_QueueManager(queueManager);
	treeViewController->LoadTreeView();
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

void MainWindow::on_actionScheduler_triggered()
{
	ShowSchedule* showSchedule = new ShowSchedule(queueManager,this);
	showSchedule->show();
}

void MainWindow::SetQueueManaget(QueueManager* queueManager)
{
	this->queueManager = queueManager;
}

void MainWindow::LoadTreeView()
{
	QStandardItemModel* model = new QStandardItemModel(this);
	ui.treeView->setModel(model);
	ui.treeView->setContextMenuPolicy(Qt::CustomContextMenu);
	//QStandardItem *item1=new QStandardItem();
	//model->appendRow(new QStandardItem("Category"));
	//model->appendRow(new QStandardItem("leaf2"));



	/*
	QModelIndex pI;

	model->insertRow(itemCount, QModelIndex());
	pI = model->index(itemCount, 0, QModelIndex());
	model->setData(pI, title, Qt::DisplayRole);
	model->item(itemCount, 0)->setEditable(false);
	*/


	//QModelIndex NewItemModelIndex;
	//model->insertRow(model->rowCount(), QModelIndex());
	//NewItemModelIndex = model->index(model->rowCount(), 0, QModelIndex());
	//model->setData(NewItemModelIndex, "Queues", Qt::DisplayRole);
	//model->item(model->rowCount(), 0)->setEditable(false);




	//model->item(1, 0)->setChild(0,1, new QStandardItem("Main"));

	/*
	QModelIndex cI;
	model->insertRow(1, QModelIndex());
	cI = model->index(1, 0, QModelIndex());
	model->setData(cI, "test", Qt::DisplayRole);
	model->item(1, 0)->setChild(0, 1, model->item(5, 0));
	*/




	QStandardItem* Categoryitem = new QStandardItem("Categories");
	Categoryitem->setEditable(false);
	QStandardItem* Queueitem = new QStandardItem("Queues");
	Queueitem->setEditable(false);


	for (Queue* queue : queueManager->Get_ListOfQueues())
	{
		QStandardItem* child1 = new QStandardItem(queue->Get_QueueName());
		child1->setData(queue->Get_QueueId());
		child1->setEditable(false);
		child1->setDropEnabled(1);
		Queueitem->appendRow(child1);
	}

	connect(ui.treeView,&QTreeView::customContextMenuRequested, this, [&](const QPoint& point) 
		{
			QTreeView* treeView=static_cast<QTreeView*>(sender());
			QModelIndex index = treeView->indexAt(point);
			if (index.isValid() && index.row() % 2 == 0) {
				QMenu* myMenu = new QMenu(this);
				myMenu->addAction("Hi");
				myMenu->exec(treeView->viewport()->mapToGlobal(point));
			}
		}
	);// SLOT(onCustomContextMenu(const QPoint&)));

	/*
	QStandardItem* child1 = new QStandardItem("Main");
	child1->setEditable(false);
	child1->setDropEnabled(1);
	Queueitem->appendRow(child1);

	QStandardItem* child2 = new QStandardItem("Other");
	child2->setEditable(false);
	Queueitem->appendRow(child2);
	*/

	

	model->setItem(0, Categoryitem);
	model->setItem(1, Queueitem);

	model->setHorizontalHeaderItem(0, new QStandardItem("Category"));
}

void MainWindow::ChangedDownloadSelected(int Download_id,bool Is_Completed)
{
	qDebug() << "Download_id:"<< Download_id;
	if (SelectedDownload != nullptr)
	{
		disconnect(SelectedDownload, &Download::DownloadStatusChanged, this, &MainWindow::ChangedStatusOfSeletedDownload);
	}

	ui.actionRemove->setEnabled(true);
	if (Is_Completed == true)
	{
		ui.actionDownload_Now->setEnabled(false);
		ui.actionStop_Download->setEnabled(false);
		SelectedDownload = nullptr;

	}
	else
	{
		//Download Not Completed



		SelectedDownload = downloadManagerPointer->ProcessAchieveDownload(Download_id);
		connect(SelectedDownload, &Download::DownloadStatusChanged, this, &MainWindow::ChangedStatusOfSeletedDownload);

		if (SelectedDownload->get_Status() == Download::NotStarted || SelectedDownload->get_Status() == Download::Pause)
		{
			ui.actionDownload_Now->setEnabled(true);
			ui.actionStop_Download->setEnabled(false);
		}
		else
		{
			//Is Downloading
			ui.actionDownload_Now->setEnabled(false);
			ui.actionStop_Download->setEnabled(true);
		}
	}

}

void MainWindow::ChangedStatusOfSeletedDownload(Download::DownloadStatusEnum NewStatus)
{
	ui.actionRemove->setEnabled(true);
	if (SelectedDownload->get_Status() == Download::NotStarted || SelectedDownload->get_Status() == Download::Pause)
	{
		ui.actionDownload_Now->setEnabled(true);
		ui.actionStop_Download->setEnabled(false);
	}
	else if(SelectedDownload->get_Status() == Download::Downloading)
	{
		ui.actionDownload_Now->setEnabled(false);
		ui.actionStop_Download->setEnabled(true);
	}
	else
	{
		//Finished Download
		ui.actionDownload_Now->setEnabled(false);
		ui.actionStop_Download->setEnabled(false);
	}

}

void MainWindow::on_actionDownload_Now_triggered()
{
	if (SelectedDownload != nullptr)
	{
		Downloader *downloader = downloadManagerPointer->ProcessAchieveDownloader(SelectedDownload);
		downloader->StartDownload();
	}
}

void MainWindow::on_actionStop_Download_triggered()
{
	if (SelectedDownload != nullptr)
	{
		Downloader* downloader = downloadManagerPointer->ProcessAchieveDownloader(SelectedDownload);
		downloader->PauseDownload();
	}
}

void MainWindow::on_actionRemove_triggered()
{
	if (SelectedDownload != nullptr)
	{
		tableViewController->RemoveActionTriggered(SelectedDownload);
	}
	else
	{
		int Download_id=tableViewController->Get_SeletedFinisedDownloadId();
		if (Download_id > 0)
		{
			//is valid
			Download* download = downloadManagerPointer->ProcessAchieveDownload(Download_id);
			tableViewController->RemoveActionTriggered(download);
		}
	}
}

void MainWindow::on_actionStop_All_triggered()
{
	downloadManagerPointer->StopAllDownload();
}

void MainWindow::on_actionEnglish_triggered()
{
	SettingInteract::SetValue("Language/Program", "English");
	ProcessForChangeLanguage();
}

void MainWindow::on_actionPersian_triggered()
{
	SettingInteract::SetValue("Language/Program","Persian");
	ProcessForChangeLanguage();
}

void MainWindow::ProcessForChangeLanguage()
{
	int const EXIT_CODE_REBOOT = -123456789;
	QMessageBox::StandardButton buttonSelected=QMessageBox::question(this, "Change Language", tr("Do you want restart download manager For apply change language?"), QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No);
	if (buttonSelected == QMessageBox::StandardButton::Yes)
	{
		qApp->quit();
		QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
	}
}

