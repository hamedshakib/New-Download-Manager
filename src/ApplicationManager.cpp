#include "HeaderAndUi/ApplicationManager.h"
#include "qdebug.h"

ApplicationManager::ApplicationManager(QObject *parent,int argc,char* argv[])
	: QObject(parent)
{
	// Use try-catch for better exception safety
	try {
		downloadManager = new DownloadManager(this);
		if (!downloadManager) {
			qCritical() << "ApplicationManager: Failed to create DownloadManager";
			return;
		}
		
		mainWindow = new MainWindow();
		if (!mainWindow) {
			qCritical() << "ApplicationManager: Failed to create MainWindow";
			// Properly clean up downloadManager
			downloadManager->deleteLater();
			downloadManager = nullptr;
			return;
		}
		mainWindow->SetDownloadManager(downloadManager);
		
		queueManager = new QueueManager(downloadManager, this);
		if (!queueManager) {
			qCritical() << "ApplicationManager: Failed to create QueueManager";
			// Properly clean up both created objects
			mainWindow->deleteLater();
			mainWindow = nullptr;
			downloadManager->deleteLater();
			downloadManager = nullptr;
			return;
		}
		mainWindow->SetQueueManaget(queueManager);
		queueManager->LoadQueuesFormDatabase();
		
		mainWindow->CreateMainTableViewControllerForMainWindow();

		ProcessArguments(argc, argv);
		if (!is_Silent) {
			mainWindow->show();
		}
		
		connect(mainWindow, &MainWindow::AddNewDownload, downloadManager, &DownloadManager::CreateNewDownload);
		connect(mainWindow, &MainWindow::NewBatchDownload, downloadManager, &DownloadManager::CreateNewDownloadsFromBatch);

		mainWindow->CreateMainTreeViewController();
		AddMainSystemTrayToTaskbar();
		
		QApplication::setQuitOnLastWindowClosed(false);
		QApplication::setWindowIcon(QIcon(":Icons/Download_Icon.png"));

		// Create proxyManager as member variable
		proxyManager = new ProxyManager();
		if (!proxyManager) {
			qCritical() << "ApplicationManager: Failed to create ProxyManager";
			// ProxyManager is not critical, continue without it
			proxyManager = nullptr;
		}
		LoadProxySettings();
	} catch (const std::exception& e) {
		qCritical() << "ApplicationManager exception:" << e.what();
		// Clean up on exception
		if (proxyManager) {
			proxyManager->deleteLater();
			proxyManager = nullptr;
		}
		if (queueManager) {
			queueManager->deleteLater();
			queueManager = nullptr;
		}
		if (mainWindow) {
			mainWindow->deleteLater();
			mainWindow = nullptr;
		}
		if (downloadManager) {
			downloadManager->deleteLater();
			downloadManager = nullptr;
		}
	}
}

ApplicationManager::~ApplicationManager()
{
	// Properly delete all objects
	if (m_trayIcon != nullptr) {
		// The menu will be deleted when the trayIcon is deleted
		// But we need to explicitly delete actions and menu if not properly connected
		m_trayIcon->deleteLater();
		m_trayIcon = nullptr;
	}
	
	if (mainWindow != nullptr) {
		mainWindow->deleteLater();
		mainWindow = nullptr;
	}
	
	if (downloadManager != nullptr) {
		downloadManager->deleteLater();
		downloadManager = nullptr;
	}
	
	if (queueManager != nullptr) {
		queueManager->deleteLater();
		queueManager = nullptr;
	}
	
	if (proxyManager != nullptr) {
		proxyManager->deleteLater();
		proxyManager = nullptr;
	}
}

void ApplicationManager::AddMainSystemTrayToTaskbar()
{
	m_trayIcon = new QSystemTrayIcon();

	QMenu* trayIconMenu = new QMenu();

	QAction* viewWindow = new QAction(tr("View Download Manager"));
	QAction* exitAction = new QAction(tr("Exit"));
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

void ApplicationManager::LoadProxySettings()
{
	QNetworkProxy::ProxyType Proxytype=ProcessEnum::ConvertEnglishStringProxyTypeToProxyTypeEnum(SettingInteract::GetValue("Proxy/Type").toString());
	QString ProxyHostname=SettingInteract::GetValue("Proxy/hostName").toString();
	quint32 ProxyPort=SettingInteract::GetValue("Proxy/Port").toInt();
	QString ProxyUsername=SettingInteract::GetValue("Proxy/User").toString();
	QString ProxyPassword=SettingInteract::GetValue("Proxy/Password").toString();

	// Use member proxyManager instead of local variable
	if (proxyManager) {
		proxyManager->SetProxyForApplication(Proxytype, ProxyHostname, ProxyPort, ProxyUsername, ProxyPassword);
	}
}

void ApplicationManager::ProcessArguments(int argc, char* argv[])
{
	for (int i = 0; i < argc; i++)
	{
		if (QString(argv[i]) == "-silent")
		{
			is_Silent = true;
		}



	}
}

