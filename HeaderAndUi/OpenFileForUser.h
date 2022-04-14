#pragma once

#include <QObject>
#include "qdebug.h"
#include "qdesktopservices.h"
#include "qurl.h"

class OpenFileForUser : public QObject
{
	Q_OBJECT

public:
	static bool openFileForShowUser(QString FileUrl);

public:
	OpenFileForUser(QObject *parent);
	~OpenFileForUser();
};
