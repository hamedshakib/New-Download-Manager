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
	QProcess process;
	if (process.execute(FileUrl) > -1)
	{
		return true;
	}
	else
	{
		qDebug()<<process.errorString();
		return false;
	}
}

