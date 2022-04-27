#pragma once

#include <QObject>
#include "qdebug.h"
#include "qdesktopservices.h"
#include "qurl.h"
#include <ShlObj_core.h>
#include "qprocess.h"
#include "qdir.h"

class OpenFileForUser : public QObject
{
	Q_OBJECT

public:
	static bool openFileForShowUser(QString FileUrl);
	static bool openWithFileForShowUser(QString FileUrl);
	static bool OpenFolderForShowUser(QString FileUrl);

public:
	OpenFileForUser(QObject *parent);
	~OpenFileForUser();
};
