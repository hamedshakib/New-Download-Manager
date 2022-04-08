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

	//PartDownload* partDownload = new PartDownload(&app);

	DownloadManager downloadManager;
	downloadManager.CreateNewDownload();







/*
	QNetworkRequest request(QUrl("https://cdn.soft98.ir/2/ShutUp10.jpg"));
	//	request.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);
	//	QString rangeBytes = QString("bytes=%1-%2").arg(partDownload->start_byte).arg(partDownload->end_byte);
	//	qDebug() << "rangeBytes:" << rangeBytes;
	//	request.setRawHeader("Range", rangeBytes.toUtf8());
	QNetworkAccessManager* manager = new QNetworkAccessManager(&app);
	QEventLoop loop;
	QNetworkReply* reply = manager->get(request);
	QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
	loop.exec();
	qDebug() << "Reply:" << reply->readAll();
*/










	/*
	QString url("http://img.p30download.ir/mac/screenshot-thumb/2021/10/1635261459_1632984992_macos-monterey_02.jpeg");
	QNetworkRequest *request=new QNetworkRequest(url);
	QNetworkAccessManager *mm=new QNetworkAccessManager(&app);
	QNetworkReply *m_networkReply;
	m_networkReply = mm->get(*request);


	
	//m_networkReply->waitForBytesWritten(5000);
	//m_networkReply->waitForReadyRead(5000);
	long long sizeOfDownload = m_networkReply->header(QNetworkRequest::ContentLengthHeader).toLongLong();


	if(m_networkReply!=nullptr)
	//qDebug()<<sizeof(Download);
		qDebug() << "";

	
	/*
	QMimeDatabase Mb;
	QUrl url("http://cdn.soft98.ir/CrystalDiskInfo.jpg");
	QUrl url2("http://stackoverflow.com/collectives");
	QUrl url3("https://i.ytimg.com/vi/e8QtsyNXvFg/hq720.jpg?sqp=-oaymwEcCNAFEJQDSFXyq4qpAw4IARUAAIhCGAFwAcABBg==&rs=AOn4CLDz--mO9GKvCw6tOhEBUUwZ1zZy7g");
	
	//QUrl url("C:/Users/user/Desktop/wrong.txt");
	qDebug()<<url3.path();

	QMimeType mimeType = Mb.mimeTypeForUrl(url3);
	qDebug() << mimeType.name();
	qDebug() << mimeType.preferredSuffix();
	*/

	

	
	return app.exec();
}