#include "HeaderAndUi/TableViewController.h"

TableViewController::TableViewController(QObject *parent)
	: QObject(parent)
{
	listOfColomns << "id" << "File Name" << "Size" << "Status" << "Speed" << "Time Left" << "Last Try Time" << "Description" << "Save To";
	model = new QStandardItemModel(this);

}

TableViewController::~TableViewController()
{
}

bool TableViewController::Set_TableView(QTableView* tableview)
{
	m_table = tableview;
	return true;
}

void TableViewController::ProcessSetupOfTableView()
{
	model->setHorizontalHeaderLabels(listOfColomns);
	m_table->setModel(model);

	//Set Header 
	horizontalHeader = m_table->horizontalHeader();
	horizontalHeader->setSectionsMovable(true);
	horizontalHeader->setContextMenuPolicy(Qt::CustomContextMenu);     //set contextmenu
	connect(horizontalHeader, &QHeaderView::customContextMenuRequested, this, &TableViewController::OnHeaderRightClicked);




	//Set double click on rows 
	connect(m_table, &QTableView::doubleClicked, this, &TableViewController::doubleClickedOnRow);

	//Set right click on rows
	m_table->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(m_table, &QTableView::customContextMenuRequested, this, &TableViewController::ProcessCheckAndApply_RightClickOnTable);
}

void TableViewController::AdjusteTableViewProperty()
{
	m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_table->verticalHeader()->setVisible(false);
	m_table->hideColumn(0);
}

void TableViewController::OnHeaderRightClicked(const QPoint& pos)
{
	//ToDo Make Colomns Settings
	QAction ColomnsSetting("Colomns...", this);
	QMenu menu;
	menu.addAction(&ColomnsSetting);
	menu.exec(QCursor::pos());
}

/*
void TableViewController::Test()
{
	model=new QStandardItemModel(this);
	//model.setColumnCount(3);
	model->setHorizontalHeaderLabels(listOfColomns);


	model->appendRow(PrepareDataForRow(1, "anyDesk-P30.zip","3.67 MB","51.61%","208.425 KB/sec","47 sec","8:20 4/9/2022","an application","C://User/hamed/Desktop"));
	model->appendRow(PrepareDataForRow(1, "avengers.mkv", "2.51 GB", "28.35%", "304.425 KB/sec", "34 sec", "5:10 4/9/2022", "a movie", "C://User/hamed/Desktop"));
	
	//model->appendRow(newRow2());

	
	m_table->setModel(model);
	m_table->horizontalHeader()->move(2, 1);

	//m_bankListView->sortItems(0, Qt::AscendingOrder);


	//m_table->setSelectionMode(QAbstractItemView::SingleSelection);
	m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_table->verticalHeader()->setVisible(false);
	m_table->hideColumn(0);

	connect(m_table, &QTableView::doubleClicked, this, &TableViewController::doubleClickedOnRow);
	QHeaderView* horizontalHeader;
	horizontalHeader = m_table->horizontalHeader();
	//horizontalHeader->sectionMoved(0, 1, 3);
	horizontalHeader->setSectionsMovable(true);
	horizontalHeader->setContextMenuPolicy(Qt::CustomContextMenu);     //set contextmenu

	connect(horizontalHeader, &QHeaderView::customContextMenuRequested, this,[&](const QPoint &pos ) {
	{
		//ToDo Make Colomns Settings
		QAction ColomnsSetting("Colomns...",this);
		QMenu menu;
		menu.addAction(&ColomnsSetting);
		menu.exec(QCursor::pos());
	}
		});
	connect(horizontalHeader, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(tablev_customContextMenu(const QPoint&)));
	//m_table->setcolor





	//connect(m_table, &QTableView::event, this, [&](QEvent* event) {qDebug() << "Clicke"; });

	m_table->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(m_table, &QTableView::customContextMenuRequested, this, &TableViewController::ProcessCheckAndApply_RightClickOnTable);
}
*/

int TableViewController::FindDownloadIdFromRow(const QModelIndex& modelindex)
{
	int row = m_table->currentIndex().row();
	QModelIndex index = model->index(row, 0);
	return model->data(index).toInt();
}

void TableViewController::doubleClickedOnRow(const QModelIndex& modelindex)
{
	FindDownloadIdFromRow(modelindex);
	//TODO Load a Download From Database Or ListOfDownload

}

QList<QStandardItem*> TableViewController::PrepareDataForRow(int id, QString FileName, QString Size, QString Status, QString Speed, QString TimeLeft, QString LastTryTime, QString Description, QString SaveTo)
{
	QList<QStandardItem*> listOfItems;

	listOfItems << newItem(id);
	listOfItems << newItem(FileName);
	listOfItems << newItem(Size);
	listOfItems << newItem(Status);
	listOfItems << newItem(Speed);
	listOfItems << newItem(TimeLeft);
	listOfItems << newItem(LastTryTime);
	listOfItems << newItem(Description);
	listOfItems << newItem(SaveTo);

	return listOfItems;
}

void TableViewController::ProcessCheckAndApply_RightClickOnTable(const QPoint& point)
{
	int row = m_table->indexAt(point).row();
	QModelIndex index = model->index(row, 0);
	if (!index.isValid())
		return;







	int Download_id = model->data(index).toInt();

	qDebug() << index;
	qDebug() << "Name File:" << model->data(model->index(row, 1));

	QString ResumeOrPause="Resume";

	QMenu* menu = new QMenu();
	menu->addAction(new QAction("Open", this));
	menu->addAction(new QAction(ResumeOrPause, this));
	menu->addAction(new QAction("Pro", this));
	menu->popup(m_table->viewport()->mapToGlobal(point));
}

void TableViewController::LoadAllDownloadsFromDatabase()
{
	
}
