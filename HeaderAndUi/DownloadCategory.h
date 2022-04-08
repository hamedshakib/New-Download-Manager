#pragma once

#include <QObject>
#include "qmimetype.h"
#include "qmimedatabase.h"

class DownloadCategory : public QObject
{
	Q_OBJECT

	qint32 idCategory;
	QString Name;
	QStringList listOfMimeType;
public:
	DownloadCategory(QObject *parent);
	~DownloadCategory();
};
