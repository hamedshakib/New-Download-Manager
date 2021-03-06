#include "HeaderAndUi/NewDownloadComplitedInformationWidget.h"

NewDownloadComplitedInformationWidget::NewDownloadComplitedInformationWidget(QWidget *parent)
	:QWidget(NULL)
{
	ui.setupUi(this);
}

NewDownloadComplitedInformationWidget::~NewDownloadComplitedInformationWidget()
{

}

void NewDownloadComplitedInformationWidget::initNewDownloadComplitedInformationWidget(QUrl BaseUrl)
{
	this->setWindowTitle("New Download Information");
	ui.Url_lineEdit->setText(BaseUrl.toString());
}

bool NewDownloadComplitedInformationWidget::SetMoreCompliteInformation(QUrl RealUrl, QString TypeFile, QString Size, QString SaveTo, QString description)
{
	this->RealUrl = RealUrl;
	ui.Url_lineEdit->setText(RealUrl.toString());
	ui.Description_lineEdit->setText(description);
	ui.FileSize_label->setText(Size);
	ui.SaveAs_lineEdit->setText(ChooseNameForNewDownloadFile(SaveTo, RealUrl));
	ui.Type_label->setText(TypeFile);
	return true;
}

//bool NewDownloadComplitedInformationWidget::SetQueueManager(QueueManager* queueManager)
//{
//	return 1;
//}

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
	emit VerifiedDownload(ui.Url_lineEdit->text(), QUrl(ui.SaveAs_lineEdit->text()),true);
	this->close();
}

void NewDownloadComplitedInformationWidget::on_DownloadLater_pushButton_clicked()
{
	emit VerifiedDownload(ui.Url_lineEdit->text(), QUrl(ui.SaveAs_lineEdit->text()),false);
	this->close();
}

void NewDownloadComplitedInformationWidget::on_Cancel_pushButton_clicked()
{
	this->close();
}

QString NewDownloadComplitedInformationWidget::ChooseNameForNewDownloadFile(QString SaveTo, QUrl RealUrl)
{
	QString NameForFile= SaveTo + "/" + RealUrl.fileName();
	QFileInfo fileInfo(NameForFile);
	if (fileInfo.exists(NameForFile))
	{
		int TempNumber = 1;
		QString suffix = fileInfo.suffix();
		QString fileName = fileInfo.fileName();
		QString directory = fileInfo.dir().dirName();
		int positionOfSuffix = fileName.lastIndexOf(suffix);
		QString NameWithoutSuffix = fileName.mid(0, positionOfSuffix - 1);

		QString SuggestionName;
		do
		{
			SuggestionName = NameWithoutSuffix + "(" + QString::number(TempNumber) + ")";
			fileInfo.setFile(SaveTo +"/"+SuggestionName + "." + suffix);
			TempNumber++;
		} while (fileInfo.exists());


		NameForFile = SaveTo + "/" +SuggestionName + "." + suffix;
	}
	
	return NameForFile;
}
