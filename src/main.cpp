#include "qapplication.h"
#include "qdebug.h"
#include "qlibrary.h"

#include "HeaderAndUi/ApplicationManager.h"
#include "HeaderAndUi/TranslationManager.h"


int main(int argc,char* argv[])
{
	QApplication app(argc, argv);
	QApplication::setApplicationName("Download Manager");
	
	TranslationManager translationMaager(&app, &app);
	translationMaager.Translate();

	
	QLibrary library1("libssl-1_1-x64");
	library1.load();
	QLibrary library2("libcrypto-1_1-x64");
	library2.load();
	QLibrary library3("vcruntime140");
	library3.load();
	


	SettingInteract::SetValue("Download/DefaultSaveToAddress", "C:/Users/user/Desktop/testDownload");
	SettingInteract::SetValue("Download/DefaultPartForDownload", "8");

/*	SettingInteract::SetValue("Proxy/Type", "Socks5");
	SettingInteract::SetValue("Proxy/hostName", "127.0.0.1");
	SettingInteract::SetValue("Proxy/Port", ui.Port_spinBox->value());
	SettingInteract::SetValue("Proxy/User", "");
	SettingInteract::SetValue("Proxy/Password", "");
	*/

	ApplicationManager manager(&app);

	
	return app.exec();
}