#include "HeaderAndUi/ConverterSizeToSuitableString.h"

QString ConverterSizeToSuitableString::ConvertSizeToSuitableString(qint64 size)
{
    if (size < 0) {
        return "0 B";
    }

    static const char* const units[] = { "B", "KB", "MB", "GB", "TB", "PB" };
    int unitIndex = 0;
    double preparedSize = static_cast<double>(size);

    while (preparedSize >= 1024.0 && unitIndex < 5) {
        preparedSize /= 1024.0;
        unitIndex++;
    }

    if (unitIndex == 0) {
        return QString::number(static_cast<int>(preparedSize)) + " " + units[unitIndex];
    }

    return QString::number(preparedSize, 'f', 2) + " " + units[unitIndex];
}

