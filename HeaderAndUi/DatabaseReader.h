#pragma once

#include <QObject>
#include "Download.h"
#include "DatabaseQueryPreparer.h"
#include "DatabaseInteract.h"
#include "qsqldatabase.h"
#include "qsqlquery.h"


class DatabaseReader : public QObject
{
	Q_OBJECT

public:
	static bool LoadDownloadFromDatabase(int Download_id, Download* download);
	//static bool LoadCategoryFromDatabase();

public:
	DatabaseReader(QObject *parent);
	~DatabaseReader();
};
