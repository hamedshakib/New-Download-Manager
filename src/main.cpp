#include "qapplication.h"
#include "qdebug.h"
#include "qlibrary.h"
#include "qmessagebox.h"

#include "HeaderAndUi/ApplicationManager.h"
#include "HeaderAndUi/TranslationManager.h"
#include "HeaderAndUi/RunGuard.h"
#include "qthread.h"


int main(int argc, char* argv[])
{
	try {
		RunGuard guard("Download Manager Application");
		if (!guard.tryToRun())
			return 0;

		QApplication app(argc, argv);

		//qDebug() << argc << argv;
		QDir::setCurrent(qApp->applicationDirPath());
		QThread::currentThread()->setObjectName("Main Thread");
		QApplication::setApplicationName("Download Manager");
		
		TranslationManager translationMaager(&app, &app);
		translationMaager.Translate();

		
		// Load OpenSSL libraries with error checking
		QLibrary library1("libssl-1_1-x64");
		if (!library1.load()) {
			qCritical() << "Failed to load libssl-1_1-x64:" << library1.errorString();
		}
		
		QLibrary library2("libcrypto-1_1-x64");
		if (!library2.load()) {
			qCritical() << "Failed to load libcrypto-1_1-x64:" << library2.errorString();
		}
		
		QLibrary library3("vcruntime140");
		if (!library3.load()) {
			qCritical() << "Failed to load vcruntime140:" << library3.errorString();
		}
		

		ApplicationManager manager(&app, argc, argv);

		
		return app.exec();
	} catch (const std::exception& e) {
		qCritical() << "Critical exception caught:" << e.what();
		QMessageBox::critical(nullptr, "Application Error", 
			QString("An unexpected error occurred:\n%1\n\nPlease restart the application.")
			.arg(e.what()));
		return 1;
	} catch (...) {
		qCritical() << "Unknown exception caught";
		QMessageBox::critical(nullptr, "Application Error", 
			"An unknown error occurred. Please restart the application.");
		return 1;
	}
}
