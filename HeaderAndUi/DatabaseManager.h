#pragma once

#include <QObject>
#include "Download.h"
#include "DatabaseQueryPreparer.h"
#include "DatabaseInteract.h"
#include "ProcessDatabaseOutput.h"
#include "NewDownloadCreater.h"
#include "NewDownloadUrlWidget.h"

#include "qsqlrecord.h"


class DatabaseManager : public QObject
{
	Q_OBJECT



public:
	bool LoadDownloadComplite(int Download_id, Download* download);
	static QStringList LoadSuffixsForMimeType(QString MimeType);
	static size_t CreateNewDownloadOnDatabase(Download* download);
	static size_t CreateNewPartDownloadOnDatabase(PartDownload* partDownload);
	

public:
	DatabaseManager(QObject *parent=nullptr);
	~DatabaseManager();
};
