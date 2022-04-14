#include "HeaderAndUi/ConverterSizeToSuitableString.h"

QString ConverterSizeToSuitableString::ConvertSizeToSuitableString(qint64 size)
{
	long double preparedSize;
	QString Str3;

	if (size < 1024)
	{
		preparedSize= size;
		Str3= "B";
	}
	else if (size >= 1024 && size < 1024 * 1024)
	{
		preparedSize = (long double)size/1024;
		Str3 = "KB";
	}
	else if (size >= 1024 * 1024 && size < 1024 * 1024 * 1024)
	{
		preparedSize = (long double)size / (1024*1024);
		Str3 = "MB";
	}
	else if (size >= 1024 * 1024* 1024 && size < 1024 * 1024 * 1024 * 1024)
	{
		preparedSize = (long double)size / (1024 * 1024 * 1024);
		Str3 = "GB";
	}
	else if(size >= 1024 * 1024 * 1024 *1024 && size < 1024 * 1024 * 1024 * 1024 * 1024)
	{
		preparedSize = (long double)size / (1024 * 1024 * 1024 * 1024);
		Str3 = "TB";
	}

	return QString::number(preparedSize, 'f', 2) + " " + Str3;
}

