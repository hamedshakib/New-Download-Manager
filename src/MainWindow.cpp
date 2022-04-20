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
	tableViewController->Set_QueueManager(queueManager);
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

