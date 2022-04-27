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
	ui.PartDownloads_spinBox->setValue(SettingInteract::GetValue("Download/DefaultPartForDownload").toInt());

	//ToDo General
	QSettings RegistrysettingForStartup("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
	if (RegistrysettingForStartup.value("HDownload Manager").isValid())
	{
		ui.checkBox->setChecked(true);
	}

	ui.ShowCompliteDialog_checkButton->setChecked(SettingInteract::GetValue("Download/ShowCompleteDialog").toBool());

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
	SettingInteract::SetValue("Download/DefaultPartForDownload", ui.PartDownloads_spinBox->value());


	QSettings RegistrysettingForStartup("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
	if (ui.checkBox->isChecked())
	{
		RegistrysettingForStartup.setValue("HDownload Manager", QDir::toNativeSeparators(qApp->applicationFilePath()+" -silent"));
	}
	else
	{
		RegistrysettingForStartup.remove("HDownload Manager");
	}

	if(ui.ShowCompliteDialog_checkButton->isChecked())
	{
		SettingInteract::SetValue("Download/ShowCompleteDialog", true);
	}
	else
	{
		SettingInteract::SetValue("ShowCompleteDialog", false);
	}


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
