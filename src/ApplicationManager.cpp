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



	AddMainSystemTrayToTaskbar();
	QApplication::setQuitOnLastWindowClosed(false);
	QApplication::setWindowIcon(QIcon(":Icons/Download_Icon.png"));
}

ApplicationManager::~ApplicationManager()
{
	mainWindow->deleteLater();
}

void ApplicationManager::AddMainSystemTrayToTaskbar()
{
	m_trayIcon = new QSystemTrayIcon();

	QMenu* trayIconMenu = new QMenu();

	QAction* viewWindow = new QAction("View Download Manager");
	QAction* exitAction = new QAction("Exit");
	trayIconMenu->addAction(viewWindow);
	trayIconMenu->addAction(exitAction);

	m_trayIcon->setContextMenu(trayIconMenu);
	m_trayIcon->setIcon(QIcon(":Icons/Download_Icon.png"));
	m_trayIcon->show();
	m_trayIcon->setToolTip(tr("Download Manager"));


	QMainWindow* mainWin = mainWindow;
	connect(viewWindow, &QAction::triggered, this, [&, mainWin](bool clicked) {mainWin->show();});
	connect(m_trayIcon, &QSystemTrayIcon::activated, this, [mainWin](QSystemTrayIcon::ActivationReason activationReason) {
		if (activationReason == QSystemTrayIcon::Trigger || activationReason == QSystemTrayIcon::DoubleClick)
		{
			mainWin->show();
		}
		});
	connect(exitAction, &QAction::triggered, this, [&, mainWin](bool clicked) {qApp->exit();});
}
