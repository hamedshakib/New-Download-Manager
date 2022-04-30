#include "qapplication.h"
#include "qdebug.h"
#include "qlibrary.h"

#include "HeaderAndUi/ApplicationManager.h"
#include "HeaderAndUi/TranslationManager.h"
#include "HeaderAndUi/RunGuard.h"


int main(int argc,char* argv[])
{
	RunGuard guard("Download Manager Application");
	if (!guard.tryToRun())
		return 0;

	QApplication app(argc, argv);

	//qDebug() << argc << argv;
	QDir::setCurrent(qApp->applicationDirPath());
	QApplication::setApplicationName("Download Manager");
	
	TranslationManager translationMaager(&app, &app);
	translationMaager.Translate();

	
	QLibrary library1("libssl-1_1-x64");
	library1.load();
	QLibrary library2("libcrypto-1_1-x64");
	library2.load();
	QLibrary library3("vcruntime140");
	library3.load();
	
	

	ApplicationManager manager(&app,argc,argv);

	
	return app.exec();
}