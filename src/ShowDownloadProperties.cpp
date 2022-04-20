#include "HeaderAndUi/ShowDownloadProperties.h"

ShowDownloadProperties::ShowDownloadProperties(Download* download,QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	m_download = download;
}

ShowDownloadProperties::~ShowDownloadProperties()
{
}

void ShowDownloadProperties::on_Open_pushButton_clicked()
{
	qDebug() << "Open Clicked";

	OpenFileForUser::openFileForShowUser(m_download->get_SavaTo().toString());
	this->close();
	this->deleteLater();
}

void ShowDownloadProperties::on_Ok_pushButton_clicked()
{
	qDebug() << "Ok Clicked";
	this->close();
	this->deleteLater();
}

void ShowDownloadProperties::ShowPropertiesOfDownload()
{
	ui.SaveTo_lineEdit->setText(m_download->get_SavaTo().toString());
	ui.SaveTo_lineEdit->setModified(false);

	ui.Url_lineEdit->setText(m_download->get_Url().toString());
	ui.Url_lineEdit->setModified(false);

	ui.Description_label->setText(m_download->get_Description());


	QString FullFileName = m_download->get_FullFileName();
	if (FullFileName.isEmpty())
	{
		QFileInfo fileInfo(m_download->get_SavaTo().toString());
		FullFileName=fileInfo.fileName();

	}
	ui.NameOfDownload_label->setText(FullFileName);

	qint64 sizeOfDownload = m_download->get_SizeDownload();
	QString SizeDownloadString = ConverterSizeToSuitableString::ConvertSizeToSuitableString(sizeOfDownload)+QString(" (%1 Bytes)").arg(sizeOfDownload);
	ui.Size_label->setText(SizeDownloadString);






	Download::DownloadStatusEnum status = m_download->get_Status();
	if (status == Download::DownloadStatusEnum::NotStarted)
	{
		ui.Status_label->setText("Not Started");
	}
	else if (status == Download::DownloadStatusEnum::Completed)
	{
		ui.Status_label->setText("Complete");
		ui.Open_pushButton->setEnabled(true);
	}
	else
	{
		qint64 DownloadedSize = m_download->get_DownloadedSize();
		float Present = (long double)DownloadedSize / sizeOfDownload;
		QString Status = QString::number(Present * 100, 'f', 2) + "%";
		ui.Status_label->setText(Status + QString(" (%1 Bytes)").arg(ConverterSizeToSuitableString::ConvertSizeToSuitableString(DownloadedSize)));
	}

	this->show();
}
