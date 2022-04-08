#include "qapplication.h"
#include "HeaderAndUi/DownloadManager.h"
#include "qdebug.h"
#include "HeaderAndUi/Download.h"



#include "qnetworkreply.h"
#include "qnetworkrequest.h"
#include "qnetworkaccessmanager.h"
#include "qmimedatabase.h"
#include "qmimetype.h"
#include "HeaderAndUi/SettingInteract.h"
#include "HeaderAndUi/PartDownload.h"

#include "qeventloop.h"



int main(int argc,char* argv[])
{
	QApplication app(argc, argv);
	QApplication::setApplicationName("QSettings Example Application");
	

	SettingInteract::SetValue("Download/DefaultSaveToAddress", "C:/Users/user/Desktop/testDownload");
	SettingInteract::SetValue("Download/DefaultPartForDownload", "8");

	DownloadManager downloadManager;
	downloadManager.CreateNewDownload();


	
	return app.exec();
}