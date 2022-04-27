#include "HeaderAndUi/CompleteDownloadDialog.h"

CompleteDownloadDialog::CompleteDownloadDialog(QString AddressOfFile, QString SizeString,QString Url,QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	this->AddressOfFile = AddressOfFile;
	this->sizeString = SizeString;
	ui.Address_lineEdit->setText(Url);
	ui.saveTo_lineEdit->setText(AddressOfFile);
	ui.DownloadedSize_label->setText(SizeString);
}

CompleteDownloadDialog::~CompleteDownloadDialog()
{
}

void CompleteDownloadDialog::on_Open_pushButton_clicked()
{
	OpenFileForUser::openFileForShowUser(AddressOfFile);
}

void CompleteDownloadDialog::on_OpenWith_pushButton_clicked()
{
	//"Rundll32 Shell32.dll,OpenAs_RunDLL any-file-name.txt"
	OpenFileForUser::openWithFileForShowUser(AddressOfFile);
}

void CompleteDownloadDialog::on_OpenFolder_pushButton_clicked()
{
	OpenFileForUser::OpenFolderForShowUser(AddressOfFile);
}

void CompleteDownloadDialog::mousePressEvent(QMouseEvent* event)
{
	QLabel* child = static_cast<QLabel*>(childAt(event->pos()));

	if(child!=ui.Drag_label)
		return;

	QDrag* drag = new QDrag(this);
	QMimeData* mimeData = new QMimeData;

	QList<QUrl> urls;
	QUrl url = "file:///" + AddressOfFile;
	urls.append(url);
	mimeData->setUrls(urls);
	drag->setMimeData(mimeData);


	Qt::DropAction dropAction = drag->exec(Qt::MoveAction);
}

void CompleteDownloadDialog::closeEvent(QCloseEvent* event)
{
	this->close();
	this->deleteLater();
}
