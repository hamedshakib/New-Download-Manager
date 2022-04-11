#include "qapplication.h"
#include "qdebug.h"
#include "qlibrary.h"

#include "HeaderAndUi/ApplicationManager.h"


int main(int argc,char* argv[])
{
	QApplication app(argc, argv);
	QApplication::setApplicationName("Download Manager");
	

	
	QLibrary library1("libssl-1_1-x64");
	library1.load();
	QLibrary library2("libcrypto-1_1-x64");
	library2.load();
	QLibrary library3("vcruntime140");
	library3.load();
	


	SettingInteract::SetValue("Download/DefaultSaveToAddress", "C:/Users/user/Desktop/testDownload");
	SettingInteract::SetValue("Download/DefaultPartForDownload", "8");


	ApplicationManager manager(&app);

	
	return app.exec();
}