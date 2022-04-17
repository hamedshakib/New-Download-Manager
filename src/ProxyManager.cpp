#include "HeaderAndUi/ProxyManager.h"

ProxyManager::ProxyManager(QObject *parent)
	: QObject(parent)
{
}

ProxyManager::~ProxyManager()
{
}

bool ProxyManager::SetProxyForApplication(QNetworkProxy::ProxyType proxyType, QString host_name, quint16 port, QString User, QString Password)
{
	if (proxyType == QNetworkProxy::NoProxy)
	{
		//unset Proxy;
		m_Proxy.setType(QNetworkProxy::NoProxy);
	}
	else
	{
		m_Proxy.setType(proxyType);
		m_Proxy.setHostName(host_name);
		m_Proxy.setPort(port);
		if (!User.isEmpty())
		{
			m_Proxy.setUser(User);
			m_Proxy.setPassword(Password);
		}
	}

	
	QNetworkProxy::setApplicationProxy(m_Proxy);
	return true;
}
