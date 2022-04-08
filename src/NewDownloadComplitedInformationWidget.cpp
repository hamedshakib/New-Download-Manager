#include "HeaderAndUi/NewDownloadComplitedInformationWidget.h"

NewDownloadComplitedInformationWidget::NewDownloadComplitedInformationWidget(QUrl BaseUrl,QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	ui.Url_lineEdit->setText(BaseUrl.toString());
}

NewDownloadComplitedInformationWidget::~NewDownloadComplitedInformationWidget()
{

}

bool NewDownloadComplitedInformationWidget::SetMoreCompliteInformation(QUrl RealUrl, QString TypeFile, QString Size, QString SaveTo, QString description)
{
	this->RealUrl = RealUrl;
	ui.Url_lineEdit->setText(RealUrl.toString());
	ui.Description_lineEdit->setText(description);
	ui.FileSize_label->setText(Size);
	ui.SaveAs_lineEdit->setText(SaveTo+"/" + RealUrl.fileName());
	ui.Type_label->setText(TypeFile);
	return true;
}
void NewDownloadComplitedInformationWidget::on_SaveAs_toolButton_clicked()
{
	QFileDialog fileDialog;
	QString FileAddress=fileDialog.getSaveFileName(this,"Select the storage location",RealUrl.fileName(), "All files (*.*)");
	if (!FileAddress.isEmpty())
	{
		ui.SaveAs_lineEdit->setText(FileAddress);
	}
}


void NewDownloadComplitedInformationWidget::on_DownloadNow_pushButton_clicked()
{
	emit DownloadNow(ui.Url_lineEdit->text(), ui.SaveAs_lineEdit->text());
	this->close();
}

void NewDownloadComplitedInformationWidget::on_DownloadLater_pushButton_clicked()
{

}

void NewDownloadComplitedInformationWidget::on_Cancel_pushButton_clicked()
{
	this->close();
}
