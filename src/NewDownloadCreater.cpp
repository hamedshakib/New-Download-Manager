#include "HeaderAndUi/NewDownloadCreater.h"

NewDownloadCreater::NewDownloadCreater(QObject *parent)
	: QObject(parent)
{
}

NewDownloadCreater::~NewDownloadCreater()
{
	if(m_networkReply!=nullptr)
		m_networkReply->deleteLater();

	if (newDownloadUrlWidget!=nullptr)
		newDownloadUrlWidget->deleteLater();
}

void NewDownloadCreater::StartProcessOfCreateANewDownload(QObject* parent)
{
	this->parent = parent;
	ProcessNewDownloadUrlWidget();
}

void NewDownloadCreater::StartProcessOfCreateNewDownloadFromBatch(QList<QString> ListOfUrl, QString SaveTo, QString Username, QString Password, QObject* parent)
{
	this->parent = parent;

	for (QString url : ListOfUrl)
	{
		BaseUrl = url;
		//ProcessNewDownloadMoreComplitedInformationWidget();
		this->GetInformationFromUrl(url, Username, Password);
		this->ProcessInitialInformationFromUrl();

		QUrl SaveToForThisDownload=NewDownloadComplitedInformationWidget::ChooseNameForNewDownloadFile(SaveTo, RealDownloadUrl);

		VerifiedDownload(RealDownloadUrl, SaveToForThisDownload, false);
	}
}



Download* NewDownloadCreater::CreateNewDownload(QObject* parent)
{
	Download* download = new Download();
	download->moveToThread(this->thread());
	return download;
}


void  NewDownloadCreater::GetInformationFromUrl(QUrl url, QString UserName, QString Password)
{
		BaseUrl = url;
		RealDownloadUrl = url;
		this->UserName = UserName;
		this->Password = Password;

		int NumberOfTry = 0;
		while (!TryToGetInformationFromUrl(RealDownloadUrl, UserName, Password))
		{
			QString LocationUrl = m_networkReply->header(QNetworkRequest::LocationHeader).toString();
			if(!LocationUrl.isEmpty())
				RealDownloadUrl = LocationUrl;
			else
			{

			}



			NumberOfTry++;
			if (NumberOfAllowedTryForFindRealUrl == NumberOfTry)
				break;
		}
}

bool NewDownloadCreater::TryToGetInformationFromUrl(QUrl url, QString UserName, QString Password)
{
	QNetworkRequest request;

	
	if (!UserName.isEmpty() && !Password.isEmpty())
	{
		url.setUserName(UserName);
		url.setPassword(Password);
	}


	request.setUrl(url);


	//connect(&m_networkAccessManager, &QNetworkAccessManager::authenticationRequired, this,[&](QNetworkReply* rep, QAuthenticator* authenticator) {qDebug() << "HHH"; });


	/*
	if (!UserName.isEmpty() && !Password.isEmpty())
	{
		QString concatenated = UserName + ":" + Password;
		QByteArray data = concatenated.toLocal8Bit().toBase64();
		QString headerData = "Basic " + data;
		request.setRawHeader("Authorization", headerData.toLocal8Bit());
	}
	*/
	if (m_networkAccessManager)
	{
		m_networkAccessManager = new QNetworkAccessManager();
		m_networkAccessManager->moveToThread(this->thread());
	}
	m_networkReply = m_networkAccessManager->head(request);


	connect(m_networkReply, &QNetworkReply::finished, &eventloop, &QEventLoop::quit);
	eventloop.exec();

	//qDebug() << m_networkReply->header(QNetworkRequest::ContentLengthHeader);
	qDebug() << m_networkReply->header(QNetworkRequest::KnownHeaders::ContentTypeHeader);
	long long sizeOfDownload = m_networkReply->header(QNetworkRequest::ContentLengthHeader).toLongLong();
	if (sizeOfDownload == 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void NewDownloadCreater::ProcessInitialInformationFromUrl()
{
	qDebug() << m_networkReply->rawHeaderList();
	for (QByteArray arrey : m_networkReply->rawHeaderList())
	{
		qDebug() << arrey << ":" << m_networkReply->rawHeader(arrey);
	}
	this->description= m_networkReply->header(QNetworkRequest::KnownHeaders::ContentDispositionHeader).toString().remove("attachment;");
	this->DownloadSize= m_networkReply->header(QNetworkRequest::ContentLengthHeader).toLongLong();
	this->suffix = DeterminerDownloadFileType::DetermineDownloadFileType(m_networkReply);
}

void NewDownloadCreater::ProcessCompleteInformation()
{
	download->Set_SizeDownload(DownloadSize);
	download->Set_Suffix(suffix);
	download->Set_Description(description);
	download->Set_DownloadedSize(0);
	download->Set_MaxSpeed(0);
	download->Set_downloadStatus(Download::DownloadStatusEnum::NotStarted);
	download->Set_ResumeCapability(ProcessEnum::ConvertHeaderToResumeCapabilityEnum(m_networkReply->rawHeader("Accept-Ranges").constData()));
	download->Set_LastTryTime(DateTimeManager::GetCurrentDateTime());
	download->Set_Username(UserName);
	download->Set_Password(Password);
	
	m_networkReply->deleteLater();
	m_networkReply = nullptr;
}

void NewDownloadCreater::CancelNewDownload()
{
	download->deleteLater();
	download = nullptr;
}

bool NewDownloadCreater::ProcessNewDownloadUrlWidget()
{
	newDownloadUrlWidget = new NewDownloadUrlWidget();
	newDownloadUrlWidget->moveToThread(this->thread());
	newDownloadUrlWidget->initNewDownloadUrlWidget();
	connect(newDownloadUrlWidget, &NewDownloadUrlWidget::GetInformations, this, [&](QUrl url, QString Username, QString Password)
		{
			if (!url.isEmpty())
			{
				newDownloadUrlWidget->close();
				BaseUrl = url;
				ProcessNewDownloadMoreComplitedInformationWidget();
				this->GetInformationFromUrl(url, Username, Password);
				this->ProcessInitialInformationFromUrl();
				newDownloadComplitedInformationWidget->SetMoreCompliteInformation(RealDownloadUrl, suffix.toUpper(),
														"Size: " + ConverterSizeToSuitableString::ConvertSizeToSuitableString(DownloadSize), DefualtSaveToAddress(), description);
				//ToDo Remove This new
				newDownloadUrlWidget->deleteLater();
				newDownloadUrlWidget = nullptr;
			}
			else
			{
				qCritical() << "Url is Empty!!";
			}
		},Qt::ConnectionType::DirectConnection);
	newDownloadUrlWidget->show();
	return 0;
}

QString NewDownloadCreater::DefualtSaveToAddress()
{
	QString DefualtSaveToString = SettingInteract::GetValue("Download/DefaultSaveToAddress").toString();
	if(DefualtSaveToString=="")
		return QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
	else
		return SettingInteract::GetValue("Download/DefaultSaveToAddress").toString();
}

size_t NewDownloadCreater::DefaultPartForDownload()
{
	return SettingInteract::GetValue("Download/DefaultPartForDownload").toInt();
}

bool NewDownloadCreater::ProcessNewDownloadMoreComplitedInformationWidget()
{
	newDownloadComplitedInformationWidget = new NewDownloadComplitedInformationWidget();
	newDownloadComplitedInformationWidget->moveToThread(this->thread());
	newDownloadComplitedInformationWidget->initNewDownloadComplitedInformationWidget(BaseUrl);
	//connect(newDownloadComplitedInformationWidget, &NewDownloadComplitedInformationWidget::DownloadNow, this, &NewDownloadCreater::VerifiedDownload_DownloadNow);
	//connect(newDownloadComplitedInformationWidget, &NewDownloadComplitedInformationWidget::DownloadLater, this, &NewDownloadCreater::VerifiedDownload_DownloadLater);
	connect(newDownloadComplitedInformationWidget, &NewDownloadComplitedInformationWidget::VerifiedDownload, this, &NewDownloadCreater::VerifiedDownload);
	newDownloadComplitedInformationWidget->show();
	return true;
}

/*
void NewDownloadCreater::VerifiedDownload_DownloadNow(QUrl url, QUrl FileSaveToAddress)
{
	this->download = CreateNewDownload(parent);
	this->download->Url = url;
	this->download->FileName = FileSaveToAddress.fileName();
	this->download->SaveTo = FileSaveToAddress.toString();
	ProcessCompleteInformation();
	size_t download_id=WriteDownloadInDatabase();
	download->IdDownload = download_id;
	ProcessCreatePartDownloadsFromDownload();
	WritePartDownloadsInDatabase();
	emit CreatedNewDownload(download);
	emit DownloadNow(download);
	sender()->deleteLater();
}

void NewDownloadCreater::VerifiedDownload_DownloadLater(QUrl url, QUrl FileSaveToAddress)
{
	this->download = CreateNewDownload(parent);
	this->download->Url = url;
	this->download->FileName = FileSaveToAddress.fileName();
	this->download->SaveTo = FileSaveToAddress.toString();
	ProcessCompleteInformation();
	size_t download_id = WriteDownloadInDatabase();
	download->IdDownload = download_id;
	ProcessCreatePartDownloadsFromDownload();
	WritePartDownloadsInDatabase();
	emit CreatedNewDownload(download);
	emit DownloadNow(download);
	sender()->deleteLater();
}
*/

void NewDownloadCreater::VerifiedDownload(QUrl url, QUrl FileSaveToAddress,bool Is_DownloadNow)
{
	this->download = CreateNewDownload(parent);
	this->download->Set_Url(url);
	QString FileName = FileSaveToAddress.fileName();
	this->download->Set_FileName(FileSaveToAddress.fileName());
	this->download->Set_SavaTo(FileSaveToAddress.toString());
	ProcessCompleteInformation();
	size_t download_id = WriteDownloadInDatabase();
	download->Set_Id(download_id);
	ProcessCreatePartDownloadsFromDownload(FileName);
	WritePartDownloadsInDatabase();
	emit CreatedNewDownload(download);

	if (Is_DownloadNow == true)
	{
		emit DownloadNow(download);
	}
	if(static_cast<NewDownloadComplitedInformationWidget*>(sender())== newDownloadComplitedInformationWidget)
		sender()->deleteLater();
}

size_t NewDownloadCreater::WriteDownloadInDatabase()
{
	return DatabaseManager::CreateNewDownloadOnDatabase(download);
}

void NewDownloadCreater::WritePartDownloadsInDatabase()
{
	auto partDownloads= download->get_PartDownloads();
	for (int i = 0; i < partDownloads.count(); i++)
	{
		partDownloads[i]->Set_PartDownloadId(DatabaseManager::CreateNewPartDownloadOnDatabase(partDownloads[i]));
	}
}

bool NewDownloadCreater::ProcessCreatePartDownloadsFromDownload(QString FileName)
{
	qint64 StartByte = 0;
	size_t downloadPartNumber = 0;

	if (download->get_ResumeCapability() == Download::ResumeCapabilityEnum::Yes)
	{
		int defaultPartForDownload = DefaultPartForDownload();
		qint64 rangeOfEachDlownload=DownloadSize /defaultPartForDownload;

		//For until penult Part
		for (downloadPartNumber = 0; downloadPartNumber < defaultPartForDownload-1; downloadPartNumber++, StartByte = StartByte + rangeOfEachDlownload+1)
		{
			QThread* partDownloadThread = new QThread();
			partDownloadThread->setObjectName("PartDownload Thread");
			partDownloadThread->start();
			PartDownload* partDownload = new PartDownload();
			partDownload->moveToThread(partDownloadThread);


			partDownload->Set_StartByte(StartByte);
			partDownload->Set_EndByte(StartByte+ rangeOfEachDlownload);
			partDownload->Set_LastDownloadedByte(StartByte-1);
			partDownload->Set_DownloadId(download->get_Id());
			partDownload->Set_PartDownloadFile(new QFile(GeneratePartDownloadAddressFromAddressOfDownloadFile(downloadPartNumber, FileName)));
			partDownload->get_PartDownloadFile()->moveToThread(partDownloadThread);
			download->AppendPartDownloadToPartDownloadListOfDownload(partDownload);
		}

	}
	//If the download does not ResumeAble, we will have only one part


	//For End Part
	QThread* partDownloadThread = new QThread();
	partDownloadThread->setObjectName("PartDownload Thread");
	partDownloadThread->start();
	PartDownload* partDownload = new PartDownload();
	partDownload->moveToThread(partDownloadThread);
	partDownload->Set_StartByte(StartByte);
	partDownload->Set_EndByte(DownloadSize - 1);
	partDownload->Set_LastDownloadedByte(StartByte - 1);
	partDownload->Set_DownloadId(download->get_Id());
	partDownload->Set_PartDownloadFile(new QFile(GeneratePartDownloadAddressFromAddressOfDownloadFile(downloadPartNumber, FileName)));
	partDownload->get_PartDownloadFile()->moveToThread(partDownloadThread);
	download->AppendPartDownloadToPartDownloadListOfDownload(partDownload);
	
	return true;
}

QString NewDownloadCreater::GeneratePartDownloadAddressFromAddressOfDownloadFile(int numberOfPart,QString FileName)
{
	QString TempDirAddress=SettingInteract::GetValue("Download/DefaultTempSaveToAddress").toString();
	if (TempDirAddress == "")
	{
		TempDirAddress = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) + "/temp";
		QDir dir(TempDirAddress);
		if (!dir.exists())
			dir.mkpath(".");
	}
	return TempDirAddress+"/" + FileName + "--" + QString::number(numberOfPart);
}