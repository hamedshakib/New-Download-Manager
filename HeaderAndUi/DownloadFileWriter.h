#pragma once

#include <QObject>
#include "qfile.h"
#include "qdebug.h"
#include "qthread.h"

class DownloadFileWriter : public QObject
{
	Q_OBJECT

public:
	DownloadFileWriter(QObject* parent=nullptr);


	//static bool WriteDownloadToFile(QByteArray& byteArray, QFile* file,bool CloseFileAfterWrite=false);
	static QFile* BuildFileFromMultipleFiles(QList<QFile*> files,QString AddressOfFile);



public slots:
	bool WriteDownloadToFile(QByteArray& byteArray, QFile* file, bool CloseFileAfterWrite = false);
};
