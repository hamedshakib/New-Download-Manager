#include "HeaderAndUi/OpenFileForUser.h"

OpenFileForUser::OpenFileForUser(QObject *parent)
	: QObject(parent)
{
}

OpenFileForUser::~OpenFileForUser()
{
}

bool OpenFileForUser::openFileForShowUser(QString FileUrl)
{
	QDesktopServices services;
	if (services.openUrl(QUrl::fromLocalFile(FileUrl)))
	{
		return true;
	}
	else
	{
		return false;
	}
}

