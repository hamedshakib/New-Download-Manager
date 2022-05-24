#include "HeaderAndUi/ShowDownloadWidget.h"

ShowDownloadWidget::ShowDownloadWidget(DownloadControl* downloadControl,QWidget *parent)
	:QWidget(parent)
{
	ui.setupUi(this);
	this->setWindowTitle("Download");
	m_DownloadControl = downloadControl;
	m_Download = downloadControl->Get_Download();
}

ShowDownloadWidget::~ShowDownloadWidget()
{
	qDeleteAll(items);
	qDebug() << "Show Download widget deleted";
}

void ShowDownloadWidget::ProcessSetup()
{
	ui.Url_label->setText(m_Download->get_Url().toString());
	ui.TimeLeft_label->setText("");
	ui.TransferRate_label->setText("");
	ui.TimeLeft_label->setText("");
	ui.transferRateInSpeedLimiter_label->setText("");
	bool is_Downloading = m_DownloadControl->IsDownloading();
	QString PauseOrResume= is_Downloading ? tr("Pause") : tr("Resume");
	ui.PauseResume_pushButton->setText(PauseOrResume);
	ui.resumeCapability_label->setText(ProcessEnum::ConvertResumeCapabilityEnumToString(m_Download->ResumeCapability));
	ui.status_label->setText(ProcessEnum::ConvertDownloadStatusEnumToString(m_Download->downloadStatus));
	ui.FileSize_label->setText(ConverterSizeToSuitableString::ConvertSizeToSuitableString(m_Download->DownloadSize));
	ui.Downloded_label->setText(ConverterSizeToSuitableString::ConvertSizeToSuitableString(m_Download->SizeDownloaded));
	ui.progressBar->setValue((int)(m_Download->SizeDownloaded*100 / m_Download->DownloadSize));
//	connect(m_DownloadControl, &DownloadControl::UpdateDownloaded, this, &ShowDownloadWidget::UpdateInDownloading);
	connect(m_DownloadControl, &DownloadControl::Started, this, [&]() {ChangePauseOrResume_Download(); });
	connect(m_DownloadControl, &DownloadControl::Paused, this, [&]() {ChangePauseOrResume_Download(); });
	connect(m_DownloadControl, &DownloadControl::CompeletedDownload, this, [&]() {this->close(); /*if (this)*/ this->deleteLater(); });

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




	int MaxSpeed = m_DownloadControl->Get_MaxSpeed();
	if (MaxSpeed > 0)
	{
		ChangeShowSpeedFromDownloader(MaxSpeed);
	}
	
	connect(m_DownloadControl, &DownloadControl::SpeedChanged, this, &ShowDownloadWidget::ChangeShowSpeedFromDownloader);
	connect(ui.checkBox, &QCheckBox::clicked, this,&ShowDownloadWidget::ClickedCheckBox);
	connect(ui.spinBox, &QSpinBox::valueChanged, this,&ShowDownloadWidget::SpinBoxValueChanged);

}

void ShowDownloadWidget::UpdateInDownloading(QString Status, QString speed, QString TimeLeft)
{
	ui.Downloded_label->setText(ConverterSizeToSuitableString::ConvertSizeToSuitableString(m_Download->SizeDownloaded));
	ui.status_label->setText(Status);
	ui.TransferRate_label->setText(speed);
	ui.transferRateInSpeedLimiter_label->setText(speed);
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
	if (m_DownloadControl->IsDownloading())
	{
		m_DownloadControl->PauseDownload();
	}
	else
	{
		m_DownloadControl->StartDownload();
	}
}

void ShowDownloadWidget::ChangePauseOrResume_Download()
{
	bool Is_downloading = m_DownloadControl->IsDownloading();
	QString PauseOrResume = Is_downloading ? tr("Pause") : tr(("Resume"));

	QString InfoString = Is_downloading ? tr("Downloading") : tr("Paused");
	for(auto item:items)
		item->setText(2, InfoString);
	ui.PauseResume_pushButton->setText(PauseOrResume);
}

void ShowDownloadWidget::ChangeShowSpeedFromDownloader(int Speed)
{
	if (Speed > 0)
	{
		if (!ui.checkBox->isChecked())
		{
			ui.checkBox->setChecked(true);
			ui.spinBox->setEnabled(true);
		}
		if (Speed != ui.spinBox->value())
		{
			ui.spinBox->setValue(Speed);
		}
	}
	else
	{
		//No speed Limit
		if (!ui.checkBox->isChecked())
		{
			ui.checkBox->setChecked(false);
		}
	}
}

void ShowDownloadWidget::ClickedCheckBox(bool is_checked)
{
	if (is_checked)
	{
		ui.spinBox->setEnabled(true);
		if (m_DownloadControl->Get_MaxSpeed() != ui.spinBox->value())
		{
			m_DownloadControl->SetMaxSpeed(ui.spinBox->value());
		}
	}
	else
	{
		ui.spinBox->setEnabled(false);
		if (m_DownloadControl->Get_MaxSpeed() != 0)
		{
			m_DownloadControl->SetMaxSpeed(0);
		}
	}
}

void ShowDownloadWidget::ChangeDownloaderSpeed(int speed)
{
	m_DownloadControl->SetMaxSpeed(speed);
}

void ShowDownloadWidget::SpinBoxValueChanged(int newValue)
{
	if (m_DownloadControl->Get_MaxSpeed() != newValue)
	{
		m_DownloadControl->SetMaxSpeed(newValue);
	}
}