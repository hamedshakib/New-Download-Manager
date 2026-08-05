#include "HeaderAndUi/SettingUpDatabase.h"
#include "qapplication.h"
#include "qdir.h"

/*
SettingUpDatabase::SettingUpDatabase(QObject *parent)
	: QObject(parent)
{
}

SettingUpDatabase::~SettingUpDatabase()
{
}
*/

QMutex& SettingUpDatabase::getMutex()
{
    static QMutex mutex;
    return mutex;
}

QSqlDatabase& SettingUpDatabase::get_Database()
{
    QMutexLocker locker(&getMutex());
    
    static QSqlDatabase db;
    if (!db.isValid())
    {
        SettingUp(db);
    }
    if (!db.isOpen())
    {
        db.open();
    }

    return db;
}

bool SettingUpDatabase::SettingUp(QSqlDatabase& db)
{
    // Use absolute path based on application directory to ensure
    // database file is always found regardless of working directory
    static QString dbPath = QDir(qApp->applicationDirPath()).filePath("DM.db");
    
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbPath);
    if (!db.open())
    {
        qCritical() << "Can not open Database at path:" << dbPath;
        qCritical() << "Error:" << db.lastError().text();
        return false;
    }
    
    // Add database indexes for improved query performance
    // Indexes are critical for:
    // 1. Download lookups by ID
    // 2. PartDownload lookups by Download_id
    // 3. Queue lookups by Queue_id
    // 4. Queue_Download lookups
    // 5. Status and category lookups
    
    // Create indexes with error checking
    QStringList indexQueries = {
        // Index for Download table lookups
        "CREATE INDEX IF NOT EXISTS idx_download_id ON Download(id)",
        "CREATE INDEX IF NOT EXISTS idx_download_queue_id ON Download(Queue_id)",
        "CREATE INDEX IF NOT EXISTS idx_download_status_id ON Download(DownloadStatus_id)",
        "CREATE INDEX IF NOT EXISTS idx_download_category_id ON Download(Category_id)",
        
        // Index for PartDownload lookups by download
        "CREATE INDEX IF NOT EXISTS idx_partdownload_download_id ON PartDownload(Download_id)",
        
        // Index for Queue_Download table
        "CREATE INDEX IF NOT EXISTS idx_queue_download_queue_id ON Queue_Download(Queue_id)",
        "CREATE INDEX IF NOT EXISTS idx_queue_download_download_id ON Queue_Download(Download_id)",
        "CREATE INDEX IF NOT EXISTS idx_queue_download_numbers_in_list ON Queue_Download(NumbersInList)",
        
        // Composite index for frequent combined lookups
        "CREATE INDEX IF NOT EXISTS idx_queue_download_composite ON Queue_Download(Queue_id, NumbersInList)"
    };
    
    QSqlQuery query(db);
    for (const QString& queryString : indexQueries) {
        if (!query.exec(queryString)) {
            qCritical() << "Failed to create index:" << queryString;
            qCritical() << "Error:" << query.lastError().text();
        } else {
            qDebug() << "Created index successfully:" << queryString;
        }
    }
    
    return true;
}
