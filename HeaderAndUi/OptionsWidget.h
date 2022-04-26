#pragma once

#include <QWidget>
#include "ui_OptionsWidget.h"
#include "ProxyManager.h"
#include "ProcessEnum.h"
#include "SettingInteract.h"
#include "qfiledialog.h"

class OptionsWidget : public QWidget
{
	Q_OBJECT

private slots:
	void LoadOptions();
	void Accepted();
	void Rejected();

	void on_TempDirectory_toolButton_clicked();
	void on_MainDirectory_toolButton_clicked();

public:
	OptionsWidget(QWidget *parent = nullptr);
	~OptionsWidget();

private:
	Ui::OptionsWidget ui;
};
