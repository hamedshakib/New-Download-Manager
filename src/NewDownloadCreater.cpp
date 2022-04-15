#include "HeaderAndUi/NewDownloadCreater.h"

NewDownloadCreater::NewDownloadCreater(QObject *parent)
	: QObject(parent)
{
}

NewDownloadCreater::~NewDownloadCreater()
{
	if(m_networkReply!=nullptr)
		m_networkReply->deleteLater();

	qDebug() << "123";
	if (newDownloadUrlWidget!=nullptr)
		newDownloadUrlWidget->deleteLater();
}

void NewDownloadCreater::StartProcessOfCreateANewDownload(QObject* parent)
{
	this->parent = parent;
	ProcessNewDownloadUrlWidget();
}

Download* NewDownloadCreater::CreateNewDownload(QObject* parent)
{
	Download* download = new Download(parent);
	return download;
}


void  NewDownloadCreater::GetInformationFromUrl(QUrl url, QString UserName, QString Password)
{
		BaseUrl = url;
		RealDownloadUrl = url;

		int NumberOfTry = 0;
		while (!TryToGetInformationFromUrl(RealDownloadUrl, UserName, Password))
		{
			qDebug() << "f1";
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

bool NewDownloadCreater::TryToGetInformationFromUrl(QUrl url, QString UserName,QString Password)
{
	if (UserName.isEmpty() || Password.isEmpty())
	{
		QNetworkRequest request(url);
		qDebug() << "url:" << url.toString();

		//connect(&m_networkAccessManager,&QNetworkAccessManager::finished,this, &NewDownloadCreater::ProcessInformationFromUrl);
		m_networkReply = m_networkAccessManager.head(request);

		//connect(m_networkReply, &QNetworkReply::finished, this, &NewDownloadCreater::ProcessInformationFromUrl);

		connect(m_networkReply, &QNetworkReply::finished, &eventloop, &QEventLoop::quit);
		eventloop.exec();

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
	else
	{

	}
}

void NewDownloadCreater::ProcessInitialInformationFromUrl()
{
	this->description= m_networkReply->header(QNetworkRequest::KnownHeaders::ContentDispositionHeader).toString().remove("attachment;");
	this->DownloadSize= m_networkReply->header(QNetworkRequest::ContentLengthHeader).toLongLong();
	this->suffix = DeterminerDownloadFileType::DetermineDownloadFileType(m_networkReply);
}

void NewDownloadCreater::ProcessCompleteInformation()
{
	download->DownloadSize = DownloadSize;
	download->suffix = suffix;
	download->description = description;
	download->SizeDownloaded = 0;
	download->MaxSpeed = 0;
	download->downloadStatus = Download::DownloadStatusEnum::NotStarted;
	download->ResumeCapability = ProcessEnum::ConvertHeaderToResumeCapabilityEnum(m_networkReply->rawHeader("Accept-Ranges").constData());

	
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
		});
	newDownloadUrlWidget->show();
	return 0;
}

QString NewDownloadCreater::DefualtSaveToAddress()
{
	return SettingInteract::GetValue("Download/DefaultSaveToAddress").toString();
}

size_t NewDownloadCreater::DefaultPartForDownload()
{
	return SettingInteract::GetValue("Download/DefaultPartForDownload").toInt();
}

bool NewDownloadCreater::ProcessNewDownloadMoreComplitedInformationWidget()
{
	newDownloadComplitedInformationWidget = new NewDownloadComplitedInformationWidget(BaseUrl);
	connect(newDownloadComplitedInformationWidget, &NewDownloadComplitedInformationWidget::DownloadNow, this, &NewDownloadCreater::VerifiedDownload_DownloadNow);
	connect(newDownloadComplitedInformationWidget, &NewDownloadComplitedInformationWidget::DownloadLater, this, &NewDownloadCreater::VerifiedDownload_DownloadLater);
	newDownloadComplitedInformationWidget->show();
	return true;
}

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

void NewDownloadCreater::VerifiedDownload_DownloadLater(QUrl url, QUrl FileSaveToAddress, int QueueId)
{
	//TODO
	this->download = CreateNewDownload(parent);
}

size_t NewDownloadCreater::WriteDownloadInDatabase()
{
	return DatabaseManager::CreateNewDownloadOnDatabase(download);
}

void NewDownloadCreater::WritePartDownloadsInDatabase()
{
	for (int i = 0; i < download->DownloadParts.count(); i++)
	{
		download->DownloadParts[i]->id_PartDownload = DatabaseManager::CreateNewPartDownloadOnDatabase(download->DownloadParts[i]);
	}
}

bool NewDownloadCreater::ProcessCreatePartDownloadsFromDownload()
{
	qint64 StartByte = 0;
	size_t downloadPartNumber = 0;

	if (download->ResumeCapability == Download::ResumeCapabilityEnum::Yes)
	{
		int defaultPartForDownload = DefaultPartForDownload();
		qint64 rangeOfEachDlownload=DownloadSize /defaultPartForDownload;

		//For until penult Part
		for (downloadPartNumber = 0; downloadPartNumber < defaultPartForDownload-1; downloadPartNumber++, StartByte = StartByte + rangeOfEachDlownload+1)
		{
			PartDownload* partDownload = new PartDownload(parent);
			partDownload->start_byte = StartByte;
			partDownload->end_byte = StartByte+ rangeOfEachDlownload;
			partDownload->LastDownloadedByte = StartByte-1;
			partDownload->id_download = download->IdDownload;
			partDownload->PartDownloadFile = new QFile(GeneratePartDownloadAddressFromAddressOfDownloadFile(downloadPartNumber, download->SaveTo.toString()));
			download->DownloadParts.append(partDownload);
		}

	}
	//If the download does not ResumeAble, we will have only one part


	//For End Part
	PartDownload* partDownload = new PartDownload(parent);
	partDownload->start_byte = StartByte;
	partDownload->end_byte = DownloadSize - 1;
	partDownload->LastDownloadedByte = StartByte - 1;
	partDownload->id_download = download->IdDownload;
	partDownload->PartDownloadFile = new QFile(GeneratePartDownloadAddressFromAddressOfDownloadFile(downloadPartNumber, download->SaveTo.toString()));
	download->DownloadParts.append(partDownload);
	
	return true;
}

QString NewDownloadCreater::GeneratePartDownloadAddressFromAddressOfDownloadFile(int numberOfPart, QString addressOfDownloadFile)
{
	return addressOfDownloadFile + "--" + QString::number(numberOfPart);
}