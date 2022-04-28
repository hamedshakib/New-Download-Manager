#pragma once

#include <QObject>

class ScheduleTableViewController : public QObject
{
	Q_OBJECT

public:
	ScheduleTableViewController(QObject *parent);
	~ScheduleTableViewController();
};
