#include "HeaderAndUi/ShowDownloadWidget.h"

ShowDownloadWidget::ShowDownloadWidget(Downloader* downloader,QWidget *parent)
	:QWidget(parent)
{
	ui.setupUi(this);
	this->setWindowTitle("Download");
	m_Downloader = downloader;
	m_Download = downloader->Get_Download();
}

ShowDownloadWidget::~ShowDownloadWidget()
{
}

void ShowDownloadWidget::ProcessSetup()
{
	ui.Url_label->setText(m_Download->get_Url().toString());
	ui.TimeLeft_label->setText("");
	ui.TransferRate_label->setText("");
	ui.TimeLeft_label->setText("");
	ui.transferRateInSpeedLimiter_label->setText("");
	bool is_Downloading = m_Downloader->IsDownloading();
	QString PauseOrResume= is_Downloading ? tr("Pause"):("Resume");
	ui.PauseResume_pushButton->setText(PauseOrResume);
	ui.resumeCapability_label->setText(ProcessEnum::ConvertResumeCapabilityEnumToString(m_Download->ResumeCapability));
	ui.status_label->setText(ProcessEnum::ConvertDownloadStatusEnumToString(m_Download->downloadStatus));
	ui.FileSize_label->setText(ConverterSizeToSuitableString::ConvertSizeToSuitableString(m_Download->DownloadSize));
	ui.Downloded_label->setText(ConverterSizeToSuitableString::ConvertSizeToSuitableString(m_Download->SizeDownloaded));
	ui.progressBar->setValue((int)(m_Download->SizeDownloaded*100 / m_Download->DownloadSize));
	connect(m_Downloader, &Downloader::SignalForUpdateDownloading, this, &ShowDownloadWidget::UpdateInDownloading);
	connect(m_Downloader, &Downloader::Started, this, [&]() {ChangePauseOrResume_Download(); });
	connect(m_Downloader, &Downloader::Paused, this, [&]() {ChangePauseOrResume_Download(); });
	connect(m_Downloader, &Downloader::CompeletedDownload, this, [&]() {this->close(); this->deleteLater(); });

	ui.treeWidget->clear();
	QList<PartDownload*> partdownloads= m_Download->get_PartDownloads();
	QString InfoString = is_Downloading ? tr("Downloading") : tr("Paused");
	for(int i=0; i<partdownloads.count();i++)
	{
		QTreeWidgetItem* item = new QTreeWidgetItem();
		item->setText(0,QString::number(i));
		item->setText(1, ConverterSizeToSuitableString::ConvertSizeToSuitableString(partdownloads[i]->LastDownloadedByte+1 - partdownloads[i]->start_byte));
		item->setText(2, InfoString);
		TreeWidgetMap.insert(item, m_Download->get_PartDownloads()[i]);
		items.append(item);
	}
	ui.treeWidget->addTopLevelItems(items);
	ui.treeWidget->update();
}

void ShowDownloadWidget::UpdateInDownloading(QString Status, QString speed, QString TimeLeft)
{
	ui.Downloded_label->setText(ConverterSizeToSuitableString::ConvertSizeToSuitableString(m_Download->SizeDownloaded));
	ui.status_label->setText(Status);
	ui.TransferRate_label->setText(speed);
	ui.TimeLeft_label->setText(TimeLeft);
	ui.progressBar->setValue((int)(m_Download->SizeDownloaded*100 / m_Download->DownloadSize));


	for(auto item:items)
	{
		PartDownload* partDownload = TreeWidgetMap.value(item);
		if(partDownload)
			item->setText(1, ConverterSizeToSuitableString::ConvertSizeToSuitableString(partDownload->LastDownloadedByte+1 - partDownload->start_byte));
	}
}

void ShowDownloadWidget::on_PauseResume_pushButton_clicked()
{
	if (m_Downloader->IsDownloading())
	{
		m_Downloader->PauseDownload();
	}
	else
	{
		m_Downloader->StartDownload();
	}
}

void ShowDownloadWidget::ChangePauseOrResume_Download()
{
	bool Is_downloading = m_Downloader->IsDownloading();
	QString PauseOrResume = Is_downloading ? tr("Pause") : tr(("Resume"));

	QString InfoString = Is_downloading ? tr("Downloading") : tr("Paused");
	for(auto item:items)
		item->setText(2, InfoString);
	ui.PauseResume_pushButton->setText(PauseOrResume);
}

void ShowDownloadWidget::closeEvent(QCloseEvent* event)
{
	this->close();
	qDeleteAll(items);
	this->deleteLater();
}
