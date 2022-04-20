#pragma once

#include <QWidget>
#include "ui_OptionsWidget.h"
#include "ProxyManager.h"
#include "ProcessEnum.h"
#include "SettingInteract.h"

class OptionsWidget : public QWidget
{
	Q_OBJECT

private slots:
	void LoadOptions();
	void Accepted();
	void Rejected();

public:
	OptionsWidget(QWidget *parent = nullptr);
	~OptionsWidget();

private:
	Ui::OptionsWidget ui;
};
