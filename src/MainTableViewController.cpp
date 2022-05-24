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
	connect(m_downloadManager, &DownloadManager::CreatedDownloadControl, this, &MainTableViewController::ConnectorDownloaderToTableUpdateInDownloading);
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
	QAction *ColomnsSetting=new QAction(tr("Colomns..."), this);
	QMenu menu;
	menu.addAction(ColomnsSetting);
	connect(ColomnsSetting, &QAction::triggered, this, [&](bool clicked) {
		ChooseColumnsHidden();
		});
	menu.exec(QCursor::pos());
}

void MainTableViewController::doubleClickedOnRow(const QModelIndex& modelindex)
{
	int Download_id = FindDownloadIdFromRow(modelindex);
	//TODO Load a Download From Database Or ListOfDownload
	Download* doubleClickedDownload = m_downloadManager->ProcessAchieveDownload(Download_id);
	if (doubleClickedDownload->get_Status() == Download::DownloadStatusEnum::Completed)
	{
		QString UrlOfFile = doubleClickedDownload->get_SavaTo().toString();
		OpenFileForUser::openFileForShowUser(UrlOfFile);
	}
	else
	{
		//Not Completed Download
		DownloadControl* downloadControl = m_downloadManager->ProcessAchieveDownloadControl(doubleClickedDownload);
		ShowDownloadWidget* showDownloadWidget;

		if (MapOfShowDownloadWidgets.find(downloadControl) != MapOfShowDownloadWidgets.end())
		{
			//found
			showDownloadWidget = MapOfShowDownloadWidgets.find(downloadControl).value();
			//MapOfShowDownloadWidgets.insert(downloader, showDownloadWidget);
		}
		else
		{
			//not found
			showDownloadWidget = CreaterShowDownloadWidget(downloadControl);
			MapOfShowDownloadWidgets.insert(downloadControl, showDownloadWidget);
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

void MainTableViewController::GetDownloaderOfDownloadId(int DownloadId)
{

}

QMenu* MainTableViewController::CreaterRightClickMenuForRowRightClicked(int Download_id)
{

	Download* RightClickedRow_Download = m_downloadManager->ProcessAchieveDownload(Download_id);
	Download::DownloadStatusEnum status = RightClickedRow_Download->get_Status();
	QMenu* menu = new QMenu(m_tableView);


	//Open Item For Menu
	if (status == Download::DownloadStatusEnum::Completed)
	{
		QAction* OpenAction = new QAction(tr("Open"), this);
		menu->addAction(OpenAction);
		connect(OpenAction, &QAction::triggered, this, [&, RightClickedRow_Download](bool clicked) {OpenFileActionTriggered(RightClickedRow_Download); });



		QAction* OpenWithAction = new QAction(tr("Open with"), this);
		menu->addAction(OpenWithAction);
		connect(OpenWithAction, &QAction::triggered, this, [&, RightClickedRow_Download](bool clicked) {OpenFileWithActionTriggered(RightClickedRow_Download); });

		QAction* OpenFolderAction = new QAction(tr("Open folder"), this);
		menu->addAction(OpenFolderAction);
		connect(OpenFolderAction, &QAction::triggered, this, [&, RightClickedRow_Download](bool clicked) {OpenFolderActionTriggered(RightClickedRow_Download); });

	}






	//Resume/Pause Item For Menu
	QAction* ResumeOrPause;
	if ((status == Download::DownloadStatusEnum::Pause) || (status == Download::DownloadStatusEnum::NotStarted))
	{
		ResumeOrPause = new QAction(this);
		ResumeOrPause->setText(tr("Resume"));
		ResumeOrPause->setVisible(true);
		menu->addAction(ResumeOrPause);
		connect(ResumeOrPause, &QAction::triggered, this, [&, ResumeOrPause, RightClickedRow_Download](bool clicked) {PauseOrResumeActionTriggered(ResumeOrPause, RightClickedRow_Download); });

	}
	else if (status == Download::DownloadStatusEnum::Downloading)
	{
		QAction* ResumeOrPause = new QAction(this);
		ResumeOrPause->setText(tr("Pause"));
		ResumeOrPause->setVisible(true);
		menu->addAction(ResumeOrPause);
		connect(ResumeOrPause, &QAction::triggered, this, [&, ResumeOrPause, RightClickedRow_Download](bool clicked) {PauseOrResumeActionTriggered(ResumeOrPause, RightClickedRow_Download); });

	}


	//Add To Queue or Remove From Queue
	if (status != Download::DownloadStatusEnum::Completed)
	{
		if (RightClickedRow_Download->get_QueueId() > 0)
		{
			QAction* RemoveFromQueueAction = new QAction(this);
			RemoveFromQueueAction->setText(tr("Delete From Queue"));
			RemoveFromQueueAction->setVisible(true);
			menu->addAction(RemoveFromQueueAction);
			//Todo Edit Connect
			connect(RemoveFromQueueAction, &QAction::triggered, this, [&, RemoveFromQueueAction, RightClickedRow_Download](bool clicked) {RemoveDownloadFromQueue(RightClickedRow_Download); sender()->deleteLater(); });
		}
		else
		{
			QMenu* AddToQueueMenu = new QMenu(tr("Add To Queue"));
			QList<QAction*> ListOfQueueAction;
			for (Queue* queue : queueManager->Get_ListOfQueues())
			{
				QAction* AddToQueue = new QAction(queue->Get_QueueName());
				connect(AddToQueue, &QAction::triggered, this, [&, queue, RightClickedRow_Download, ListOfQueueAction]() {AddDownloadToQueue(queue, RightClickedRow_Download); qDeleteAll(ListOfQueueAction); });
				ListOfQueueAction.append(AddToQueue);
				AddToQueueMenu->addAction(AddToQueue);
			}
			menu->addMenu(AddToQueueMenu);
		}


	}

	menu->addSeparator();



	//Remove Action
	QAction* RemoveDownloadAction = new QAction(tr("Remove"), this);
	RemoveDownloadAction->setVisible(true);
	menu->addAction(RemoveDownloadAction);
	connect(RemoveDownloadAction, &QAction::triggered, this, [&, RightClickedRow_Download](bool clicked) {RemoveActionTriggered(RightClickedRow_Download); });

	menu->addSeparator();


	//Properties Item For Menu
	QAction* PropertiesAction = new QAction(tr("Properties"), this);
	PropertiesAction->setVisible(true);
	menu->addAction(PropertiesAction);

	connect(PropertiesAction, &QAction::triggered, this, [&, RightClickedRow_Download](bool clicked) {PropertiesActionTriggered(RightClickedRow_Download); });


	return menu;
}

void MainTableViewController::ConnectorDownloaderToTableUpdateInDownloading(DownloadControl* downloadControl)
{
	size_t Download_id = downloadControl->Get_Download()->get_Id();
	size_t Row = 0;
	for (size_t i = 0; i < model->rowCount(); i++)
	{
		qDebug() << model->index(i, 0).data().toInt();
		if (model->index(i, 0).data().toInt() == Download_id)
		{
			Row = i;
			break;
		}
	}
	if (Row >= 0)
	{
//		connect(downloadControl, &DownloadControl::UpdateDownloaded, this, [&, Row](QString Status, QString Speed, QString TimeLeft) {UpdateRowInDownloading(Row, Status, Speed, TimeLeft); });
		connect(downloadControl, &DownloadControl::Started, this, [&, Row, downloadControl]() {
			//Update LastStartedTime
			model->setData(model->index(Row, 6), DateTimeManager::ConvertDataTimeToString(downloadControl->Get_Download()->get_LastTryTime()));
			});
	}
}

bool MainTableViewController::UpdateRowInDownloading(size_t row, QString Status, QString Speed, QString TimeLeft)
{
	DownloadControl* downloadControl = static_cast<DownloadControl*>(sender());

	QModelIndex Status_index = model->index(row, 3);
	QModelIndex Speed_index = model->index(row, 4);
	QModelIndex TimeLeft_index = model->index(row, 5);

	if (downloadControl->Get_Download()->get_Status() == Download::DownloadStatusEnum::Completed)
	{
		Status = tr("Complete");
		Speed = "";
		TimeLeft = "";
	}
	model->setData(Status_index, Status);
	model->setData(Speed_index, Speed);
	model->setData(TimeLeft_index, TimeLeft);


	return true;
}

void MainTableViewController::AddNewDownloadToTableView(Download* download)
{
	model->appendRow(TableViewRowCreater::PrepareDataForRowForMainTableView(download));
}

ShowDownloadWidget* MainTableViewController::CreaterShowDownloadWidget(DownloadControl* downloadControl)
{
	ShowDownloadWidget* showDownload = new ShowDownloadWidget(downloadControl);
	showDownload->ProcessSetup();
	return showDownload;
}

void MainTableViewController::PauseOrResumeActionTriggered(QAction* pauseOrResumeAction, Download* download)
{

	if (pauseOrResumeAction->text() == tr("Resume"))
	{
		m_downloadManager->ProcessAchieveDownloadControl(download)->StartDownload();
	}
	else if (pauseOrResumeAction->text() == tr("Pause"))
	{
		m_downloadManager->ProcessAchieveDownloadControl(download)->PauseDownload();
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

void MainTableViewController::ClickedOnRow(const QModelIndex& modelindex)
{
	int Download_id = FindDownloadIdFromRow(modelindex);

	int row = m_tableView->currentIndex().row();
	QModelIndex Statusindex = model->index(row, 3);
	QString Status = model->data(Statusindex).toString();
	if (Status == tr("Complete"))
	{
		SelectedFinishedDownload_id = Download_id;
		emit SelectedDownloadChanged(Download_id, true);
	}
	else
	{
		emit SelectedDownloadChanged(Download_id, false);
	}
}

int MainTableViewController::Get_SeletedFinisedDownloadId()
{
	return SelectedFinishedDownload_id;
}

void MainTableViewController::ChangeColumnWidth(int numberOfColumn, int NewColumnWidth)
{
	QString SettingStringKey = "TableView/MainTableView/WidthColum" + QString::number(numberOfColumn);
	SettingInteract::SetValue(SettingStringKey, NewColumnWidth);
}

void MainTableViewController::ChooseColumnsHidden()
{
	SelectColumnsForMainTableView* selectColumnsForMainTableViewWidget = new SelectColumnsForMainTableView();
	selectColumnsForMainTableViewWidget->LoadColumnsHide();
	selectColumnsForMainTableViewWidget->show();
	HiddenColumns.clear();
	QList<int>& hiddencolumns = HiddenColumns;
	connect(selectColumnsForMainTableViewWidget, &SelectColumnsForMainTableView::HideColumns, this, [=,&hiddencolumns](QList<int> columns) {hiddencolumns.clear(); hiddencolumns.append(columns); HideOrShowColumns(); });

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
