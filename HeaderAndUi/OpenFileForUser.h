#pragma once

#include <QObject>
#include "qprocess.h"
#include "qdebug.h"

class OpenFileForUser : public QObject
{
	Q_OBJECT

public:
	static bool openFileForShowUser(QString FileUrl);

public:
	OpenFileForUser(QObject *parent);
	~OpenFileForUser();
};
