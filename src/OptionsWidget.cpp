#include "HeaderAndUi/OptionsWidget.h"

OptionsWidget::OptionsWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	this->setWindowFlags(Qt::Window);
	connect(ui.buttonBox, &QDialogButtonBox::accepted, this, &OptionsWidget::Accepted);
	connect(ui.buttonBox, &QDialogButtonBox::rejected, this, &OptionsWidget::Rejected);
	LoadOptions();
}

OptionsWidget::~OptionsWidget()
{
}

void OptionsWidget::LoadOptions()
{
	//Proxy
	QNetworkProxy proxy=QNetworkProxy::applicationProxy();
	ui.ProxyType_comboBox->setCurrentText(ProcessEnum::ConvertProxyTypeEnumToProxyTypeString(proxy.type()));
	ui.AddressServer_lineEdit->setText(proxy.hostName());
	ui.Port_spinBox->setValue(proxy.port());
	ui.Username_lineEdit->setText(proxy.user());
	ui.Password_lineEdit->setText(proxy.password());



	//ToDo SaveTo
	ui.MainDirectory_lineEdit->setText(SettingInteract::GetValue("Download/DefaultSaveToAddress").toString());
	ui.TempDirectory_lineEdit->setText(SettingInteract::GetValue("Download/DefaultTempSaveToAddress").toString());

	//ToDo General


}

void OptionsWidget::Accepted()
{
	//Proxy
	ProxyManager proxyManager;
	proxyManager.SetProxyForApplication(ProcessEnum::ConvertProxyTypeStringToProxyTypeEnum(ui.ProxyType_comboBox->currentText()),ui.AddressServer_lineEdit->text(),ui.Port_spinBox->value(),ui.Username_lineEdit->text(),ui.Password_lineEdit->text());

	QString EnglishStringProxyType=ProcessEnum::ConvertTypeProxyEnumToEnglishStringProxyType(ProcessEnum::ConvertProxyTypeStringToProxyTypeEnum(ui.ProxyType_comboBox->currentText()));
	SettingInteract::SetValue("Proxy/Type", EnglishStringProxyType);
	SettingInteract::SetValue("Proxy/hostName", ui.AddressServer_lineEdit->text());
	SettingInteract::SetValue("Proxy/Port", ui.Port_spinBox->value());
	SettingInteract::SetValue("Proxy/User", ui.Username_lineEdit->text());
	SettingInteract::SetValue("Proxy/Password", ui.Password_lineEdit->text());
	SettingInteract::SetValue("Download/DefaultSaveToAddress", ui.MainDirectory_lineEdit->text());
	SettingInteract::SetValue("Download/DefaultTempSaveToAddress", ui.TempDirectory_lineEdit->text());







	this->close();
	this->deleteLater();
}

void OptionsWidget::Rejected()
{
	this->close();
	this->deleteLater();
}

void OptionsWidget::on_TempDirectory_toolButton_clicked()
{
	QFileDialog fileDialog;
	fileDialog.setFileMode(QFileDialog::FileMode::Directory);
	QString FolderAddress = fileDialog.getExistingDirectory(this, "Select the Temp storage location");
	if (!FolderAddress.isEmpty())
	{
		ui.TempDirectory_lineEdit->setText(FolderAddress);
	}
}

void OptionsWidget::on_MainDirectory_toolButton_clicked()
{
	QFileDialog fileDialog;
	fileDialog.setFileMode(QFileDialog::FileMode::Directory);
	QString FolderAddress = fileDialog.getExistingDirectory(this, "Select the Defualt storage location");
	if (!FolderAddress.isEmpty())
	{
		ui.MainDirectory_lineEdit->setText(FolderAddress);
	}
}
