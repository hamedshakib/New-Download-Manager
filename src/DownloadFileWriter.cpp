#include "HeaderAndUi/DownloadFileWriter.h"

bool DownloadFileWriter::WriteDownloadToFile(QByteArray& byteArray, QFile* file, bool CloseFileAfterWrite)
{
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
				while (!file->atEnd()) {
					QByteArray bytes;
					if (file->bytesAvailable() >= 2048)
					{
						bytes = file->read(2048);
					}
					else
					{
						bytes = file->readAll();
					}
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