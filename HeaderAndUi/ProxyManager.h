#pragma once

#include <QObject>
#include "qnetworkproxy.h"

class ProxyManager : public QObject
{
	Q_OBJECT

public:
	bool SetProxyForApplication(QNetworkProxy::ProxyType proxyType, QString host_name,quint16 port, QString User = "", QString Password = "");
private:
	QNetworkProxy m_Proxy;
public:
	ProxyManager(QObject *parent=nullptr);
	~ProxyManager();
};
