#pragma once

#include <QWidget>
#include "ui_QtWidgetsClass.h"
#include ""

class QtWidgetsClass : public QWidget
{
	Q_OBJECT

public:
	QtWidgetsClass(QWidget *parent = nullptr);
	~QtWidgetsClass();

private:
	Ui::QtWidgetsClass ui;
};
