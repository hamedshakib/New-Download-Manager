#pragma once

#include <QObject>
#include "qfile.h"
#include "qdebug.h"

class DownloadFileWriter : public QObject
{
	Q_OBJECT

public:
	static bool WriteDownloadToFile(QByteArray& byteArray, QFile* file,bool CloseFileAfterWrite=false);
	static QFile* BuildFileFromMultipleFiles(QList<QFile*> files,QString AddressOfFile);
};
