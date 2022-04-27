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

bool OpenFileForUser::openWithFileForShowUser(QString FileUrl)
{
	OPENASINFO oi = { 0 };
	std::string file;
	QString AddressOfFile = FileUrl.replace("/", "\\");
	LPCWSTR strVariable2 = (const wchar_t*)AddressOfFile.utf16();

	oi.pcszFile = strVariable2;
	oi.oaifInFlags = OAIF_EXEC;
	SHOpenWithDialog(NULL, &oi);

	return true;
}

bool OpenFileForUser::OpenFolderForShowUser(QString FileUrl)
{
	QStringList args;
	args << "/select," << QDir::toNativeSeparators(FileUrl);
	return QProcess::startDetached("explorer", args);
}

