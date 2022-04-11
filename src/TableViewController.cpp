#include "HeaderAndUi/TableViewController.h"

TableViewController::TableViewController(QTableView* tableView,QObject *parent)
	: QObject(parent)
{
	m_tableView = tableView;
	listOfColomns << "id" << "File Name" << "Size" << "Status" << "Speed" << "Time Left" << "Last Try Time" << "Description" << "Save To";
	model = new QStandardItemModel(this);
}

TableViewController::~TableViewController()
{
}

void TableViewController::Set_DownloadManager(DownloadManager* downloadManager)
{
	m_downloadManager = downloadManager;
}

void TableViewController::ProcessSetupOfTableView()
{
	model->setHorizontalHeaderLabels(listOfColomns);
	m_tableView->setModel(model);


	
	//Set Header 
	horizontalHeader = m_tableView->horizontalHeader();
	horizontalHeader->setSectionsMovable(true);
	horizontalHeader->setContextMenuPolicy(Qt::CustomContextMenu);     //set contextmenu
	connect(horizontalHeader, &QHeaderView::customContextMenuRequested, this, &TableViewController::OnHeaderRightClicked);



	
	//Set double click on rows 
	connect(m_tableView, &QTableView::doubleClicked, this, &TableViewController::doubleClickedOnRow);


	
	//Set right click on rows
	m_tableView->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(m_tableView, &QTableView::customContextMenuRequested, this, &TableViewController::ProcessCheckAndApply_RightClickOnTable);


	
	AdjusteTableViewProperty();

	
	LoadAllDownloadsFromDatabaseForMainTableView();

	
}

void TableViewController::AdjusteTableViewProperty()
{
	m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_tableView->verticalHeader()->setVisible(false);
	m_tableView->hideColumn(0);
}

void TableViewController::OnHeaderRightClicked(const QPoint& pos)
{
	//ToDo Make Colomns Settings
	QAction ColomnsSetting("Colomns...", this);
	QMenu menu;
	menu.addAction(&ColomnsSetting);
	menu.exec(QCursor::pos());
}

void TableViewController::doubleClickedOnRow(const QModelIndex& modelindex)
{
	FindDownloadIdFromRow(modelindex);
	//TODO Load a Download From Database Or ListOfDownload

}

int TableViewController::FindDownloadIdFromRow(const QModelIndex& modelindex)
{
	int row = m_tableView->currentIndex().row();
	QModelIndex index = model->index(row, 0);
	return model->data(index).toInt();
}

void TableViewController::LoadAllDownloadsFromDatabaseForMainTableView()
{
	manager = new DatabaseManager(this);
	manager->LoadAllDownloadsForMainTable(model);
}

void TableViewController::ProcessCheckAndApply_RightClickOnTable(const QPoint& point)
{
	int row = m_tableView->indexAt(point).row();
	QModelIndex index = model->index(row, 0);
	if (!index.isValid())
		return;



	int Download_id = model->data(index).toInt();

	QMenu* menu = CreaterRightClickMenuForRowRightClicked(Download_id);
	menu->popup(m_tableView->viewport()->mapToGlobal(point));
}

void TableViewController::GetDownloaderOfDownloadId(int DownloadId)
{
	
}

QMenu* TableViewController::CreaterRightClickMenuForRowRightClicked(int Download_id)
{

	Download* RightClickedRow_Download=m_downloadManager->ProcessAchieveDownload(Download_id);
	Download::DownloadStatusEnum status = RightClickedRow_Download->get_Status();
	QMenu* menu = new QMenu(m_tableView);


	//Open Item For Menu
	if (status == Download::DownloadStatusEnum::Completed)
	{
		QAction* OpenAction = new QAction("Open", this);
		menu->addAction(OpenAction);
	}
	





	//Resume/Pause Item For Menu
	QAction* ResumeOrPause;
	if (status == (Download::DownloadStatusEnum::Pause | Download::DownloadStatusEnum::NotStarted))
	{
		ResumeOrPause = new QAction(this);
		ResumeOrPause->setText("Resume");
		ResumeOrPause->setVisible(true);
		menu->addAction(ResumeOrPause);
	}
	else if (status == Download::DownloadStatusEnum::Downloading)
	{
		QAction* ResumeOrPause = new QAction(this);
		ResumeOrPause->setText("Pause");
		ResumeOrPause->setVisible(true);
		menu->addAction(ResumeOrPause);
	}




	//Properties Item For Menu
	QAction* PropertiesAction = new QAction("Properties", this);
	menu->addAction(PropertiesAction);




	return menu;
}
