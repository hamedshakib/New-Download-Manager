#include "HeaderAndUi/NewDownloadUrlWidget.h"

NewDownloadUrlWidget::NewDownloadUrlWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

NewDownloadUrlWidget::~NewDownloadUrlWidget()
{
	if(SignalEmmited==false)
		emit GetInformations(QUrl(""), "", "");
}

void NewDownloadUrlWidget::initNewDownloadUrlWidget()
{
	this->setWindowTitle("New Download");
	connect(ui.buttonBox, &QDialogButtonBox::accepted, this, &NewDownloadUrlWidget::Accepted);
	connect(ui.buttonBox, &QDialogButtonBox::rejected, this, &NewDownloadUrlWidget::Rejected);
}

void NewDownloadUrlWidget::Accepted()
{
	emit GetInformations(ui.Url_lineEdit->text(), ui.Username_lineEdit->text(), ui.Password_lineEdit->text());
	SignalEmmited = true;
}

void NewDownloadUrlWidget::Rejected()
{
	emit GetInformations(QUrl(""), "", "");
	SignalEmmited = true;
	this->close();
}
