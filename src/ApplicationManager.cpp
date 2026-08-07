#include "HeaderAndUi/ApplicationManager.h"

ApplicationManager::ApplicationManager(QObject *parent,int argc,char* argv[])
	: QObject(parent)
{
	downloadManager=new DownloadManager(this);
	mainWindow=new MainWindow();
	mainWindow->SetDownloadManager(downloadManager);
	queueManager = new QueueManager(downloadManager,this);
	mainWindow->SetQueueManaget(queueManager);
	queueManager->LoadQueuesFormDatabase();
	mainWindow->CreateMainTableViewControllerForMainWindow();

	ProcessArguments(argc, argv);
	if (is_Silent == false)
	{
		mainWindow->show();
	}
	connect(mainWindow, &MainWindow::AddNewDownload, downloadManager, &DownloadManager::CreateNewDownload);
	connect(mainWindow, &MainWindow::NewBatchDownload, downloadManager, &DownloadManager::CreateNewDownloadsFromBatch);


	mainWindow->CreateMainTreeViewController();


	AddMainSystemTrayToTaskbar();
	QApplication::setQuitOnLastWindowClosed(false);
	QApplication::setWindowIcon(QIcon(":Icons/Download_Icon.png"));

	LoadProxySettings();
}

ApplicationManager::~ApplicationManager()
{
	mainWindow->deleteLater();
}

void ApplicationManager::AddMainSystemTrayToTaskbar()
{
	// تعیین this به عنوان Parent جهت جلوگیری از Memory Leak
	m_trayIcon = new QSystemTrayIcon(this);

	QMenu* trayIconMenu = new QMenu();
	QAction* viewWindow = new QAction(tr("View Download Manager"), this);
	QAction* exitAction = new QAction(tr("Exit"), this);

	trayIconMenu->addAction(viewWindow);
	trayIconMenu->addAction(exitAction);

	m_trayIcon->setContextMenu(trayIconMenu);
	m_trayIcon->setIcon(QIcon(":Icons/Download_Icon.png"));
	m_trayIcon->setToolTip(tr("Download Manager"));
	m_trayIcon->show();

	QMainWindow* mainWin = mainWindow;

	// استفاده از Value Capture ([mainWin]) به جای Reference Capture ([&])
	connect(viewWindow, &QAction::triggered, this, [mainWin]() {
		if (mainWin) {
			mainWin->show();
			mainWin->raise();
			mainWin->activateWindow();
		}
		});

	connect(m_trayIcon, &QSystemTrayIcon::activated, this, [mainWin](QSystemTrayIcon::ActivationReason reason) {
		if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick) {
			if (mainWin) {
				mainWin->show();
				mainWin->raise();
				mainWin->activateWindow();
			}
		}
		});

	connect(exitAction, &QAction::triggered, this, []() {
		qApp->exit();
		});
}

void ApplicationManager::LoadProxySettings()
{
	QNetworkProxy::ProxyType Proxytype=ProcessEnum::ConvertEnglishStringProxyTypeToProxyTypeEnum(SettingInteract::GetValue("Proxy/Type").toString());
	QString ProxyHostname=SettingInteract::GetValue("Proxy/hostName").toString();
	quint32 ProxyPort=SettingInteract::GetValue("Proxy/Port").toInt();
	QString ProxyUsername=SettingInteract::GetValue("Proxy/User").toString();
	QString ProxyPassword=SettingInteract::GetValue("Proxy/Password").toString();

	ProxyManager proxyManager;
	proxyManager.SetProxyForApplication(Proxytype, ProxyHostname, ProxyPort, ProxyUsername, ProxyPassword);
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

