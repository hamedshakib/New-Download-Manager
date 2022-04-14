#include "HeaderAndUi/ShowDownloadWidget.h"

ShowDownloadWidget::ShowDownloadWidget(Downloader* downloader,QWidget *parent)
	:QWidget(parent)
{
	ui.setupUi(this);
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
	QString PauseOrResume=m_Downloader->IsDownloading() ? tr("Pause"):("Resume");
	ui.PauseResume_pushButton->setText(PauseOrResume);
	ui.resumeCapability_label->setText(ProcessEnum::ConvertResumeCapabilityEnumToString(m_Download->ResumeCapability));
	ui.status_label->setText(ProcessEnum::ConvertDownloadStatusEnumToString(m_Download->downloadStatus));
	ui.FileSize_label->setText(ConverterSizeToSuitableString::ConvertSizeToSuitableString(m_Download->DownloadSize));
	ui.Downloded_label->setText(ConverterSizeToSuitableString::ConvertSizeToSuitableString(m_Download->SizeDownloaded));
	ui.progressBar->setValue(m_Download->SizeDownloaded / m_Download->DownloadSize);
	connect(m_Downloader, &Downloader::SignalForUpdateDownloading, this, &ShowDownloadWidget::UpdateInDownloading);
	connect(m_Downloader, &Downloader::Started, this, [&]() {ChangePauseOrResume_Download(); });
	connect(m_Downloader, &Downloader::Paused, this, [&]() {ChangePauseOrResume_Download(); });
	connect(m_Downloader, &Downloader::CompeletedDownload, this, [&]() {this->close(); this->deleteLater(); });
}

void ShowDownloadWidget::UpdateInDownloading(QString Status, QString speed, QString TimeLeft)
{
	ui.Downloded_label->setText(ConverterSizeToSuitableString::ConvertSizeToSuitableString(m_Download->SizeDownloaded));
	ui.status_label->setText(Status);
	ui.TransferRate_label->setText(speed);
	ui.TimeLeft_label->setText(TimeLeft);
	ui.progressBar->setValue((int)(m_Download->SizeDownloaded*100 / m_Download->DownloadSize));
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
	QString PauseOrResume = m_Downloader->IsDownloading() ? tr("Pause") : ("Resume");
	ui.PauseResume_pushButton->setText(PauseOrResume);
}
