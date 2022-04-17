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


	//ToDo General


}

void OptionsWidget::Accepted()
{
	//Proxy
	ProxyManager proxyManager;
	proxyManager.SetProxyForApplication(ProcessEnum::ConvertProxyTypeStringToProxyTypeEnum(ui.ProxyType_comboBox->currentText()),ui.AddressServer_lineEdit->text(),ui.Port_spinBox->value(),ui.Username_lineEdit->text(),ui.Password_lineEdit->text());
	SettingInteract::SetValue("Proxy/Type", ui.ProxyType_comboBox->currentText());
	SettingInteract::SetValue("Proxy/hostName", ui.AddressServer_lineEdit->text());
	SettingInteract::SetValue("Proxy/Port", ui.Port_spinBox->value());
	SettingInteract::SetValue("Proxy/User", ui.Username_lineEdit->text());
	SettingInteract::SetValue("Proxy/Password", ui.Password_lineEdit->text());









	this->close();
	this->deleteLater();
}

void OptionsWidget::Rejected()
{
	this->close();
	this->deleteLater();
}
