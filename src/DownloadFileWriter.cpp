#include "HeaderAndUi/DownloadFileWriter.h"

DownloadFileWriter::DownloadFileWriter(QObject* parent)
	:QObject(parent)
{

}

bool DownloadFileWriter::WriteDownloadToFile(QByteArray& byteArray, QFile* file, bool CloseFileAfterWrite)
{
	if (!file->isOpen()) {
		if (!file->open(QIODevice::WriteOnly | QIODevice::Append)) {
			qCritical() << "Cannot Open File!";
			return false;
		}
	}

	qint64 bytesWritten = file->write(byteArray);
	if (bytesWritten == -1) {
		qCritical() << "Error writing to file:" << file->errorString();
		return false;
	}

	if (CloseFileAfterWrite) {
		file->close();
	}
	return true;
}

QFile* DownloadFileWriter::BuildFileFromMultipleFiles(QList<QFile*> files, QString AddressOfFile)
{
    if (files.count() == 1) {
        files[0]->setFileName(AddressOfFile);
        return files[0];
    }

    QFile* NewFile = new QFile(AddressOfFile);
    if (NewFile->open(QIODevice::WriteOnly)) {
        constexpr qint64 BUFFER_SIZE = 1024 * 1024;
        for (QFile* file : files) {
            file->close();
            if (!file->open(QFile::ReadOnly)) {
                qCritical() << "Can't Open File for read to merge";
                return file;
            }
            while (!file->atEnd()) {
                QByteArray bytes = file->read(BUFFER_SIZE);
                NewFile->write(bytes);
            }
            file->close();
            file->remove();
        }
        NewFile->flush();
        NewFile->close();
    }
    return NewFile;
}
