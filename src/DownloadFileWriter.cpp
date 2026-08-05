#include "HeaderAndUi/DownloadFileWriter.h"

DownloadFileWriter::DownloadFileWriter(QObject* parent)
	:QObject(parent)
{

}

bool DownloadFileWriter::WriteDownloadToFile(QByteArray& byteArray, QFile* file, bool CloseFileAfterWrite)
{
	//qDebug() << "Write download Thread :" << QThread::currentThread()->objectName();
	if (!file->isOpen())
	{
		if (!file->open(QIODevice::WriteOnly | QIODevice::Append))
		{
			qCritical() << "Can not Open File!";
			return false;
		}
	}

	file->write(byteArray);
	file->flush();

	if (CloseFileAfterWrite)
	{
		file->close();
	}
	return true;

}

QFile* DownloadFileWriter::BuildFileFromMultipleFiles(QList<QFile*> files, QString AddressOfFile)
{
	if (files.count() == 1)
	{

		files[0]->setFileName(AddressOfFile);
		return files[0];
	}
	else
	{
		QFile* NewFile = new QFile(AddressOfFile);
		if (NewFile->open(QIODevice::WriteOnly))
		{
			for (QFile* file : files)
			{
				file->close();
				if (!file->open(QFile::ReadOnly))
				{
					qCritical() << "Can't Open File for read for write to one file";
					return file;
				}
				// Use 64KB buffer for better performance
				const int BUFFER_SIZE = 65536;
				while (!file->atEnd()) {
					QByteArray bytes = file->read(BUFFER_SIZE);
					if (bytes.isEmpty())
						break;
					NewFile->write(bytes);
				}
				file->remove();
			}
			NewFile->flush();
			NewFile->close();
		}
		return NewFile;
	}
}
