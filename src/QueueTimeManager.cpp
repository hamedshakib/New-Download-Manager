#include "HeaderAndUi/QueueTimeManager.h"
#include <QTimer>
#include <QDate>
#include <QTime>
#include <QPointer>
#include <QDebug>

QueueTimeManager::QueueTimeManager(QObject* parent)
    : QObject(parent)
{}

QueueTimeManager::~QueueTimeManager()
{}

bool QueueTimeManager::Is_TodayaDayOfDownload(QStringList DownloadDays)
{
    if (DownloadDays.isEmpty()) {
        return false;
    }

    // **رفع باگ**: دریافت صحیح اولین عنصر لیست[cite: 7]
    const QString firstItem = DownloadDays.first();
    QDate currentDate = QDate::currentDate();

    if (IsNameOfDaysOfWeek(firstItem)) {
        for (const QString& dayOfWeek : DownloadDays) {
            if (ConvertDayStringToNumberOfDayOfWeek(dayOfWeek) == currentDate.dayOfWeek()) {
                return true;
            }
        }
        return false;
    }
    else if (IsNumberOfDays(firstItem)) {
        // پیاده‌سازی منطق روزهای عددی در صورت نیاز[cite: 7]
        return true;
    }
    else {
        // تاریخ مشخص (Specific Date)[cite: 7]
        QDate startDay = QDate::fromString(firstItem, Qt::ISODate);
        return (currentDate == startDay);
    }
}

bool QueueTimeManager::IsNameOfDaysOfWeek(QString day)
{
    static const QStringList days = {
        "Saturday", "Sunday", "Monday", "Tuesday",
        "Wednesday", "Thursday", "Friday"
    };
    return days.contains(day, Qt::CaseInsensitive);
}

bool QueueTimeManager::IsNumberOfDays(QString day)
{
    bool isSuccess = false;
    day.toInt(&isSuccess);
    return isSuccess;
}

int QueueTimeManager::ConvertDayStringToNumberOfDayOfWeek(QString day)
{
    // استفاده از QMap یا مقایسه Case-Insensitive برای جلوگیری از خطا[cite: 7]
    QString d = day.trimmed().toLower();
    if (d == "sunday")    return 1;
    if (d == "monday")    return 2;
    if (d == "tuesday")   return 3;
    if (d == "wednesday") return 4;
    if (d == "thursday")  return 5;
    if (d == "friday")    return 6;
    if (d == "saturday")  return 7;
    return 0;
}

void QueueTimeManager::DayChangedSlot()
{
    emit DayChanged();
    int differentSeconds = QTime::currentTime().secsTo(QTime(23, 59, 59)) + 1;
    QTimer::singleShot(differentSeconds * 1000, this, &QueueTimeManager::DayChangedSlot);
}

void QueueTimeManager::DayTimerSingleShotManage(QList<Queue*> queues)
{
    for (Queue* queue : queues) {
        AddSingleShot(queue);
    }
}

bool QueueTimeManager::CheckQueueForEvent(Queue* queue)
{
    if (!queue) return false;

    const int AllowedSecondsError = 10; // کمی انعطاف بیشتر برای سیستم‌های شلوغ[cite: 7]

    if (queue->startDownload.is_active &&
        std::abs(QTime::currentTime().secsTo(queue->startDownload.Time)) <= AllowedSecondsError)
    {
        qDebug() << "QueueTimeManager: Starting Queue" << queue->Get_QueueName();
        emit StartQueue(queue);
        return true;
    }
    else if (queue->stopDownload.is_active &&
        std::abs(QTime::currentTime().secsTo(queue->stopDownload.Time)) <= AllowedSecondsError)
    {
        qDebug() << "QueueTimeManager: Stopping Queue" << queue->Get_QueueName();
        emit StopQueue(queue);
        return true;
    }
    return false;
}

bool QueueTimeManager::AddSingleShot(Queue* queue)
{
    if (!queue || !Is_TodayaDayOfDownload(queue->DownloadDays)) {
        return false;
    }

    // استفاده از QPointer جهت جلوگیری از Crash در صورت حذف صف قبل از رسیدن تایمر
    QPointer<Queue> safeQueue = queue;

    // ۱. زمان‌بندی شروع دانلود[cite: 5, 7]
    if (queue->startDownload.is_active) {
        int secondsToStart = QTime::currentTime().secsTo(queue->startDownload.Time);
        if (secondsToStart > 0) {
            QTimer::singleShot(secondsToStart * 1000, this, [this, safeQueue]() {
                if (safeQueue) {
                    CheckQueueForEvent(safeQueue);
                }
                });
        }
    }

    // **رفع باگ**: زمان‌بندی توقف اکنون داخل شرط بررسی روز مجاز قرار دارد[cite: 5, 7]
    if (queue->stopDownload.is_active) {
        int secondsToStop = QTime::currentTime().secsTo(queue->stopDownload.Time);
        if (secondsToStop > 0) {
            QTimer::singleShot(secondsToStop * 1000, this, [this, safeQueue]() {
                if (safeQueue) {
                    CheckQueueForEvent(safeQueue);
                }
                });
        }
    }

    return true;
}