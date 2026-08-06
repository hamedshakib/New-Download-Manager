#include "HeaderAndUi/MainTableViewController.h"
MainTableViewController::MainTableViewController(QTableView* tableView, QObject* parent)
	:TableViewController(tableView, parent)
{
	m_tableView = tableView;
	listOfColomns << "id" << tr("File Name") << tr("Size") << tr("Status") << tr("Speed") << tr("Time Left") << tr("Last Try Time") << tr("Description") << tr("Save To");
	model = new QStandardItemModel(this);

	m_tableView->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);

	//tableView->setDragDropMode(QAbstractItemView::DragDropMode::DragOnly);
	//tableView->setDragEnabled(true);
	//tableView->setDragEnabled(true);//somewhere in constructor
	QStringList HiddenColumnsStringList= SettingInteract::GetValue("TableView/MainTableView/HiddenColumns").toString().split(",");
	if (!HiddenColumnsStringList[0].isEmpty())
	{
		for (QString numberColumn : HiddenColumnsStringList)
		{
			HiddenColumns.append(numberColumn.toInt());
		}
	}
}

MainTableViewController::~MainTableViewController()
{
}

void MainTableViewController::Set_DownloadManager(DownloadManager* downloadManager)
{
	m_downloadManager = downloadManager;
	connect(m_downloadManager, &DownloadManager::CreatedNewDownload, this, &MainTableViewController::AddNewDownloadToTableView);
	connect(m_downloadManager, &DownloadManager::CreatedDownloadController, this, &MainTableViewController::ConnectorDownloadControllerToTableUpdateInDownloading);
}

void MainTableViewController::ProcessSetupOfTableView()
{
	model->setHorizontalHeaderLabels(listOfColomns);
	m_tableView->setModel(model);





	//Set Header 
	horizontalHeader = m_tableView->horizontalHeader();
	horizontalHeader->setSectionsMovable(true);
	horizontalHeader->setContextMenuPolicy(Qt::CustomContextMenu);     //set contextmenu
	connect(horizontalHeader, &QHeaderView::customContextMenuRequested, this, &MainTableViewController::OnHeaderRightClicked);


	for (int columNumber = 1; columNumber < listOfColomns.count(); columNumber++)
	{
		QString SettingStringKey = "TableView/MainTableView/WidthColum" + QString::number(columNumber);
		int ColumWidth = SettingInteract::GetValue(SettingStringKey).toInt();
		m_tableView->setColumnWidth(columNumber, ColumWidth);
	}

	connect(horizontalHeader, &QHeaderView::sectionResized, this, [&](int numberOfColum, int oldsize, int newSize) {ChangeColumnWidth(numberOfColum, newSize); });



	//Hidden Some Columns
	HideOrShowColumns();




	//Set double click on rows 
	//connect(m_tableView, &QTableView::doubleClicked, this, &MainTableViewController::doubleClickedOnRow);
	connect(m_tableView, &QTableView::doubleClicked, this, [&](const QModelIndex& modelindex) {doubleClickedOnRow(modelindex); ClickedOnRow(modelindex); });




	//Set right click on rows
	m_tableView->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(m_tableView, &QTableView::customContextMenuRequested, this, &MainTableViewController::ProcessCheckAndApply_RightClickOnTable);


	//Set Just Clicked on rows
	connect(m_tableView, &QTableView::clicked, this, &MainTableViewController::ClickedOnRow);


	AdjusteTableViewProperty();


	LoadAllDownloadsFromDatabaseForMainTableView();

}

void MainTableViewController::AdjusteTableViewProperty()
{
	m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_tableView->verticalHeader()->setVisible(false);
	m_tableView->hideColumn(0);
}

void MainTableViewController::OnHeaderRightClicked(const QPoint& pos)
{
	QAction *colomnsSetting=new QAction(tr("Colomns..."), this);
	QMenu menu;
	menu.addAction(colomnsSetting);
	connect(colomnsSetting, &QAction::triggered, this, [&](bool clicked) {
		ChooseColumnsHidden();
		});
	menu.exec(QCursor::pos());
}

void MainTableViewController::doubleClickedOnRow(const QModelIndex& modelindex)
{
	int downloadId = FindDownloadIdFromRow(modelindex);
	//TODO Load a Download From Database Or ListOfDownload
	Download* doubleClickedDownload = m_downloadManager->ProcessAchieveDownload(downloadId);
	if (doubleClickedDownload == nullptr)
	{
		qWarning() << "doubleClickedOnRow: download could not be loaded (id=" << downloadId << ")";
		return;
	}
	if (doubleClickedDownload->get_Status() == Download::DownloadStatusEnum::Completed)
	{
		QString UrlOfFile = doubleClickedDownload->get_SavaTo().toString();
		OpenFileForUser::openFileForShowUser(UrlOfFile);
	}
	else
	{
		//Not Completed Download
		DownloadController* downloadController = m_downloadManager->ProcessAchieveDownloadController(doubleClickedDownload);
		ShowDownloadWidget* showDownloadWidget;

		if (MapOfShowDownloadWidgets.find(downloadController) != MapOfShowDownloadWidgets.end())
		{
			//found
			showDownloadWidget = MapOfShowDownloadWidgets.find(downloadController).value();
		}
		else
		{
			//not found
			showDownloadWidget = CreaterShowDownloadWidget(downloadController);
			MapOfShowDownloadWidgets.insert(downloadController, showDownloadWidget);
		}
		showDownloadWidget->show();

	}
}

int MainTableViewController::FindDownloadIdFromRow(const QModelIndex& modelindex)
{
	int row = m_tableView->currentIndex().row();
	QModelIndex index = model->index(row, 0);
	return model->data(index).toInt();
}

void MainTableViewController::LoadAllDownloadsFromDatabaseForMainTableView()
{
	manager = new DatabaseManager(this);
	manager->LoadAllDownloadsForMainTable(model);
}

void MainTableViewController::ProcessCheckAndApply_RightClickOnTable(const QPoint& point)
{
	int row = m_tableView->indexAt(point).row();
	QModelIndex index = model->index(row, 0);
	if (!index.isValid())
		return;


	int Download_id = model->data(index).toInt();

	qDebug() << "Download id right:" << Download_id;

	QMenu* menu = CreaterRightClickMenuForRowRightClicked(Download_id);
	menu->popup(m_tableView->viewport()->mapToGlobal(point));

	ClickedOnRow(index);
}

QMenu* MainTableViewController::CreaterRightClickMenuForRowRightClicked(int Download_id)
{

	Download* rightClickedRow_Download = m_downloadManager->ProcessAchieveDownload(Download_id);
	Download::DownloadStatusEnum status = rightClickedRow_Download->get_Status();
	QMenu* menu = new QMenu(m_tableView);


	//Open Item For Menu
	if (status == Download::DownloadStatusEnum::Completed)
	{
		QAction* openAction = new QAction(tr("Open"), this);
		menu->addAction(openAction);
		connect(openAction, &QAction::triggered, this, [&, rightClickedRow_Download](bool clicked) {OpenFileActionTriggered(rightClickedRow_Download); });



		QAction* openWithAction = new QAction(tr("Open with"), this);
		menu->addAction(openWithAction);
		connect(openWithAction, &QAction::triggered, this, [&, rightClickedRow_Download](bool clicked) {OpenFileWithActionTriggered(rightClickedRow_Download); });

		QAction* OpenFolderAction = new QAction(tr("Open folder"), this);
		menu->addAction(OpenFolderAction);
		connect(OpenFolderAction, &QAction::triggered, this, [&, rightClickedRow_Download](bool clicked) {OpenFolderActionTriggered(rightClickedRow_Download); });

	}






	//Resume/Pause Item For Menu
	QAction* ResumeOrPause;
	if ((status == Download::DownloadStatusEnum::Pause) || (status == Download::DownloadStatusEnum::NotStarted))
	{
		ResumeOrPause = new QAction(this);
		ResumeOrPause->setText(tr("Resume"));
		ResumeOrPause->setVisible(true);
		menu->addAction(ResumeOrPause);
		connect(ResumeOrPause, &QAction::triggered, this, [&, ResumeOrPause, rightClickedRow_Download](bool clicked) {PauseOrResumeActionTriggered(ResumeOrPause, rightClickedRow_Download); });

	}
	else if (status == Download::DownloadStatusEnum::Downloading)
	{
		QAction* ResumeOrPause = new QAction(this);
		ResumeOrPause->setText(tr("Pause"));
		ResumeOrPause->setVisible(true);
		menu->addAction(ResumeOrPause);
		connect(ResumeOrPause, &QAction::triggered, this, [&, ResumeOrPause, rightClickedRow_Download](bool clicked) {PauseOrResumeActionTriggered(ResumeOrPause, rightClickedRow_Download); });

	}


	//Add To Queue or Remove From Queue
	if (status != Download::DownloadStatusEnum::Completed)
	{
		if (rightClickedRow_Download->get_QueueId() > 0)
		{
			QAction* RemoveFromQueueAction = new QAction(this);
			RemoveFromQueueAction->setText(tr("Delete From Queue"));
			RemoveFromQueueAction->setVisible(true);
			menu->addAction(RemoveFromQueueAction);
			//Todo Edit Connect
			connect(RemoveFromQueueAction, &QAction::triggered, this, [&, RemoveFromQueueAction, rightClickedRow_Download](bool clicked) {RemoveDownloadFromQueue(rightClickedRow_Download); sender()->deleteLater(); });
		}
		else
		{
			QMenu* AddToQueueMenu = new QMenu(tr("Add To Queue"));
			QList<QAction*> ListOfQueueAction;
			for (Queue* queue : queueManager->Get_ListOfQueues())
			{
				QAction* AddToQueue = new QAction(queue->Get_QueueName());
				connect(AddToQueue, &QAction::triggered, this, [&, queue, rightClickedRow_Download, ListOfQueueAction]() {AddDownloadToQueue(queue, rightClickedRow_Download); qDeleteAll(ListOfQueueAction); });
				ListOfQueueAction.append(AddToQueue);
				AddToQueueMenu->addAction(AddToQueue);
			}
			menu->addMenu(AddToQueueMenu);
		}


	}

	menu->addSeparator();



	//Remove Action
	QAction* removeDownloadAction = new QAction(tr("Remove"), this);
	removeDownloadAction->setVisible(true);
	menu->addAction(removeDownloadAction);
	connect(removeDownloadAction, &QAction::triggered, this, [&, rightClickedRow_Download](bool clicked) {RemoveActionTriggered(rightClickedRow_Download); });

	menu->addSeparator();


	//Properties Item For Menu
	QAction* PropertiesAction = new QAction(tr("Properties"), this);
	PropertiesAction->setVisible(true);
	menu->addAction(PropertiesAction);

	connect(PropertiesAction, &QAction::triggered, this, [&, rightClickedRow_Download](bool clicked) {PropertiesActionTriggered(rightClickedRow_Download); });


	return menu;
}

void MainTableViewController::ConnectorDownloadControllerToTableUpdateInDownloading(DownloadController* DownloadController)
{
	size_t download_id = DownloadController->Get_Download()->get_Id();
	size_t row = 0;
	for (size_t i = 0; i < model->rowCount(); i++)
	{
		qDebug() << model->index(i, 0).data().toInt();
		if (model->index(i, 0).data().toInt() == download_id)
		{
			row = i;
			break;
		}
	}
	if (row >= 0)
	{
		connect(DownloadController, &DownloadController::UpdateDownloaded, this, [&, row](QString Status, QString Speed, QString TimeLeft,QList<qint64> list) {UpdateRowInDownloading(row, Status, Speed, TimeLeft); });
		connect(DownloadController, &DownloadController::DownloadCompleted, this, [&, row]() {CompeletedDownload(row); });
		connect(DownloadController, &DownloadController::DownloadStarted, this, [&, row, DownloadController]() {
			//Update LastStartedTime
			model->setData(model->index(row, 6), DateTimeManager::ConvertDataTimeToString(DownloadController->Get_Download()->get_LastTryTime()));
			});

	}
}

bool MainTableViewController::UpdateRowInDownloading(size_t row, QString Status, QString Speed, QString TimeLeft)
{
	DownloadController* downloadController = static_cast<DownloadController*>(sender());

	QModelIndex statusIndex = model->index(row, 3);
	QModelIndex speedIndex = model->index(row, 4);
	QModelIndex timeLeftIndex = model->index(row, 5);

	if (downloadController->Get_Download()->get_Status() == Download::DownloadStatusEnum::Completed)
	{
		Status = tr("Complete");
		Speed = "";
		TimeLeft = "";
	}
	model->setData(statusIndex, Status);
	model->setData(speedIndex, Speed);
	model->setData(timeLeftIndex, TimeLeft);


	return true;
}

void MainTableViewController::AddNewDownloadToTableView(Download* download)
{
	model->appendRow(TableViewRowCreater::PrepareDataForRowForMainTableView(download));
}

ShowDownloadWidget* MainTableViewController::CreaterShowDownloadWidget(DownloadController* DownloadController)
{
	ShowDownloadWidget* showDownload = new ShowDownloadWidget(DownloadController);
	showDownload->ProcessSetup();
	return showDownload;
}

void MainTableViewController::PauseOrResumeActionTriggered(QAction* pauseOrResumeAction, Download* download)
{

	if (pauseOrResumeAction->text() == tr("Resume"))
	{
		m_downloadManager->ProcessAchieveDownloadController(download)->StartDownload();
	}
	else if (pauseOrResumeAction->text() == tr("Pause"))
	{
		m_downloadManager->ProcessAchieveDownloadController(download)->PauseDownload();
	}
}

void MainTableViewController::OpenFileActionTriggered(Download* download)
{
	QString UrlOfFile = download->get_SavaTo().toString();
	OpenFileForUser::openFileForShowUser(UrlOfFile);
}

void MainTableViewController::OpenFileWithActionTriggered(Download* download)
{
	QString UrlOfFile = download->get_SavaTo().toString();
	OpenFileForUser::openWithFileForShowUser(UrlOfFile);
}

void MainTableViewController::OpenFolderActionTriggered(Download* download)
{
	QString UrlOfFile = download->get_SavaTo().toString();
	OpenFileForUser::OpenFolderForShowUser(UrlOfFile);
}

void MainTableViewController::RemoveActionTriggered(Download* download)
{
	m_downloadManager->ProcessRemoveDownload(download);
	int row = m_tableView->currentIndex().row();
	model->removeRow(row);
}

void MainTableViewController::PropertiesActionTriggered(Download* download)
{
	ShowDownloadProperties* showProperties = new ShowDownloadProperties(download);
	showProperties->ShowPropertiesOfDownload();
}

void MainTableViewController::Set_QueueManager(QueueManager* queueManager)
{
	this->queueManager = queueManager;
}

void MainTableViewController::AddDownloadToQueue(Queue* queue, Download* download)
{
	queueManager->AddDownloadToQueue(download, queue);
}

void MainTableViewController::RemoveDownloadFromQueue(Download* download)
{
	//queueManager->RemoveDownloadFromQueue(download);
	queueManager->ProcessRemoveADownloadFromQueue(download);
}

void MainTableViewController::ClickedOnRow(const QModelIndex& modelIndex)
{
	int download_id = FindDownloadIdFromRow(modelIndex);

	int row = m_tableView->currentIndex().row();
	QModelIndex statusIndex = model->index(row, 3);
	QString status = model->data(statusIndex).toString();
	if (status == tr("Complete"))
	{
		SelectedFinishedDownload_id = download_id;
		emit SelectedDownloadChanged(download_id, true);
	}
	else
	{
		emit SelectedDownloadChanged(download_id, false);
	}
}

int MainTableViewController::Get_SeletedFinisedDownloadId()
{
	return SelectedFinishedDownload_id;
}

void MainTableViewController::ChangeColumnWidth(int numberOfColumn, int new_column_width)
{
	QString SettingStringKey = "TableView/MainTableView/WidthColum" + QString::number(numberOfColumn);
	SettingInteract::SetValue(SettingStringKey, new_column_width);
}

void MainTableViewController::ChooseColumnsHidden()
{
	SelectColumnsForMainTableView* selectColumnsForMainTableViewWidget = new SelectColumnsForMainTableView();
	selectColumnsForMainTableViewWidget->LoadColumnsHide();
	selectColumnsForMainTableViewWidget->show();
	HiddenColumns.clear();
	QList<int>& hiddenColumns = HiddenColumns;
	connect(selectColumnsForMainTableViewWidget, &SelectColumnsForMainTableView::HideColumns, this, [=,&hiddenColumns](QList<int> columns) {hiddenColumns.clear(); hiddenColumns.append(columns); HideOrShowColumns(); });

}

void MainTableViewController::HideOrShowColumns()
{
	for (int i = 1; i < listOfColomns.count(); i++)
	{
		if (HiddenColumns.contains(i))
		{
			m_tableView->setColumnHidden(i, true);
		}
		else
		{
			m_tableView->setColumnHidden(i, false);
			if (m_tableView->columnWidth(i) == 0)
			{
				m_tableView->setColumnWidth(i, 100);
			}
		}
	}
}

void MainTableViewController::CompeletedDownload(size_t row)
{
	QModelIndex status_index = model->index(row, 3);
	QModelIndex speed_index = model->index(row, 4);
	QModelIndex time_left_index = model->index(row, 5);

	model->setData(status_index, tr("Complete"));
	model->setData(speed_index, "");
	model->setData(time_left_index, "");
}
