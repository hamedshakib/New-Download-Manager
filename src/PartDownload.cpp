#include "HeaderAndUi/PartDownload.h"

PartDownload::PartDownload(QObject *parent)
	: QObject(parent)
{
}

PartDownload::~PartDownload()
{
	if (PartDownloadFile != nullptr)
	{
		// Close file if it's open before deleting
		if (PartDownloadFile->isOpen()) {
			PartDownloadFile->close();
		}
		PartDownloadFile->deleteLater();
		PartDownloadFile = nullptr;
	}

}

bool PartDownload::IsPartDownloadFinished()
{
	qDebug() << LastDownloadedByte;
	if (this->LastDownloadedByte >= this->end_byte)
	{
		qDebug() << "**downloadC last:" << this->LastDownloadedByte << " end:" << this->end_byte;
		is_Finished = true;
		emit Finished();  // Emit finished signal when part download is complete
		return true;
	}
	else
	{
		return false;
	}
}

void PartDownload::UpdatePartDownloadLastDownloadedByte()
{
	if (PartDownloadFile != nullptr) {
		this->LastDownloadedByte = this->start_byte + PartDownloadFile->size() - 1;
	} else {
		qWarning() << "PartDownload: PartDownloadFile is null, cannot update LastDownloadedByte";
	}
}