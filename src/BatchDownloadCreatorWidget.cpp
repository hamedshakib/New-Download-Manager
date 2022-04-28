#include "HeaderAndUi/BatchDownloadCreatorWidget.h"

BatchDownloadCreatorWidget::BatchDownloadCreatorWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	this->setWindowFlags(Qt::Window);
	ui.SequentialBatch_widget->setEnabled(false);
	ui.SequentialBatch_widget->setHidden(true);
	connect(ui.TypeBatchDownload_comboBox, &QComboBox::currentTextChanged, this, &BatchDownloadCreatorWidget::ChangeWidgetOfBatch);


	connect(ui.Numerics_radioButton, &QRadioButton::clicked, this, [&](bool checked) {DetermineSequentialType(); });
	connect(ui.Letters_radioButton, &QRadioButton::clicked, this, [&](bool checked) {DetermineSequentialType(); });
	ui.Numerics_radioButton->setChecked(true);
	ui.widget_3->setHidden(true);

	QString DefaultSaveToAddress= SettingInteract::GetValue("Download/DefaultSaveToAddress").toString();
	ui.SaveTo_lineEdit->setText(DefaultSaveToAddress);
}

BatchDownloadCreatorWidget::~BatchDownloadCreatorWidget()
{

}

void BatchDownloadCreatorWidget::on_Download_pushButton_clicked()
{
	QString BaseUrl = ui.Address_lineEdit->text();
	if (BaseUrl.isEmpty() || !BaseUrl.contains("*"))
		return;

	if (ui.Letters_radioButton->isChecked())
	{
		if (ui.From_lineEdit->text().isEmpty() || ui.To_lineEdit->text().isEmpty())
			return;

		CreateNewBatchDownloadSequentialLettersAddress(BaseUrl, ui.From_lineEdit->text(), ui.To_lineEdit->text());
	}
	else
	{
		//ui.Numerics_radioButton checked
		CreateNewBatchDownloadSequentialNumericsAddress(BaseUrl,ui.From_spinBox->value(),ui.To_spinBox->value());
	}

	this->close();
	this->deleteLater();
}

void BatchDownloadCreatorWidget::on_Cancel_pushButton_clicked()
{
	this->close();
	this->deleteLater();
}

void BatchDownloadCreatorWidget::on_toolButton_clicked()
{
	QFileDialog fileDialog;
	QString FolderAddress = fileDialog.getExistingDirectory(this, "Select the storage location", ui.SaveTo_lineEdit->text());
	if (!FolderAddress.isEmpty())
	{
		ui.SaveTo_lineEdit->setText(FolderAddress);
	}
}

void BatchDownloadCreatorWidget::ChangeWidgetOfBatch(QString Type)
{
	if (Type == tr("Download from a sequential download names"))
	{
		ui.SequentialBatch_widget->setEnabled(true);
		ui.SequentialBatch_widget->setHidden(false);
	}
	/*else if (Type == tr(""))
	{
		
	}*/
	else
	{
		ui.SequentialBatch_widget->setEnabled(false);
		ui.SequentialBatch_widget->setHidden(true);
		ui.Address_lineEdit->setText("");
	}
}

void BatchDownloadCreatorWidget::DetermineSequentialType()
{
	if (ui.Numerics_radioButton->isChecked())
	{
		ui.widget_2->setHidden(false);
		ui.widget_3->setHidden(true);
	}
	else
	{
		ui.widget_3->setHidden(false);
		ui.widget_2->setHidden(true);
	}
}

void BatchDownloadCreatorWidget::CreateNewBatchDownloadSequentialNumericsAddress(QString BaseUrl,int start,int end)
{
	QList<QString> ListOfDowmloadUrl;
	for (int i = start; i <= end; i++)
	{
		QString TempBaseDownload = BaseUrl;
		QString DownloadAddress = TempBaseDownload.replace("*", QString::number(i));
		ListOfDowmloadUrl.append(DownloadAddress);
	}

	emit NewBatchDownload(ListOfDowmloadUrl, ui.SaveTo_lineEdit->text(), ui.Username_lineEdit->text(), ui.Password_lineEdit->text());
}

void BatchDownloadCreatorWidget::CreateNewBatchDownloadSequentialLettersAddress(QString BaseUrl, QString start, QString end)
{
	QList<QString> ListOfDowmloadUrl;


	for (int NumberOfChar = 0; NumberOfChar < start.length(); NumberOfChar++)
	{
		//for (QChar character = start.at(NumberOfChar).tochar; character <= end.at(NumberOfChar); character.)
		for (char character = start.at(NumberOfChar).toLatin1(); character <= end.at(NumberOfChar).toLatin1(); character++)
		{
			QString TempBaseDownload = BaseUrl;
			QString DownloadAddress = TempBaseDownload.replace("*", QString(character));
			ListOfDowmloadUrl.append(DownloadAddress);
		}
	}

	emit NewBatchDownload(ListOfDowmloadUrl, ui.SaveTo_lineEdit->text(), ui.Username_lineEdit->text(), ui.Password_lineEdit->text());
}
