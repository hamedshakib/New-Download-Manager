#include "HeaderAndUi/DownloadFileWriter.h"

DownloadFileWriter::DownloadFileWriter(QObject* parent)
	:QObject(parent)
{
}

bool DownloadFileWriter::WriteDownloadToFile(QByteArray& byteArray, QFile* file, bool CloseFileAfterWrite)
{
	//qDebug() << "Write download Thread :" << QThread::currentThread()->objectName();
	if (!file) {
		qCritical() << "WriteDownloadToFile: file pointer is null";
		return false;
	}
	
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
	if (files.count() == 0) {
		qCritical() << "BuildFileFromMultipleFiles: No files provided";
		return nullptr;
	}
	
	if (files.count() == 1)
	{
		files[0]->setFileName(AddressOfFile);
		return files[0];
	}
	else
	{
		QFile* NewFile = new QFile(AddressOfFile);
		if (!NewFile) {
			qCritical() << "BuildFileFromMultipleFiles: Failed to create new file object";
			return nullptr;
		}
		
		if (NewFile->open(QIODevice::WriteOnly))
		{
			for (QFile* file : files)
			{
				file->close();
				if (!file->open(QFile::ReadOnly))
				{
					qCritical() << "Can't Open File for read for write to one file:" << file->fileName();
					// Clean up the newly created file
					NewFile->remove();
					NewFile->close();
					delete NewFile;
					return nullptr;
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
		else
		{
			qCritical() << "BuildFileFromMultipleFiles: Failed to open new file for writing:" << AddressOfFile;
			// Clean up the newly created file
			NewFile->remove();
			delete NewFile;
			return nullptr;
		}
		return NewFile;
	}
}