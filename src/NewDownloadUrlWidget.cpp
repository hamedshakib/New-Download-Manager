#include "HeaderAndUi/NewDownloadUrlWidget.h"

NewDownloadUrlWidget::NewDownloadUrlWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	connect(ui.buttonBox, &QDialogButtonBox::accepted, this, &NewDownloadUrlWidget::Accepted);
	connect(ui.buttonBox, &QDialogButtonBox::rejected, this, &NewDownloadUrlWidget::Rejected);
}

NewDownloadUrlWidget::~NewDownloadUrlWidget()
{
	if(SignalEmmited==false)
		emit GetInformations(QUrl(""), "", "");
}

void NewDownloadUrlWidget::Accepted()
{
	emit GetInformations(ui.Url_lineEdit->text(), ui.lineEdit->text(), ui.lineEdit_2->text());
	SignalEmmited = true;
}

void NewDownloadUrlWidget::Rejected()
{
	emit GetInformations(QUrl(""), "", "");
	SignalEmmited = true;
}
