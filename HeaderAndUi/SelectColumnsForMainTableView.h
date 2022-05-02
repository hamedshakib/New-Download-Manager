#pragma once

#include <QWidget>
#include "ui_SelectColumnsForMainTableView.h"
#include "qdialogbuttonbox.h"
#include "qpushbutton.h"
#include "SettingInteract.h"

class SelectColumnsForMainTableView : public QWidget
{
	Q_OBJECT

public:
	SelectColumnsForMainTableView(QWidget *parent = nullptr);
	~SelectColumnsForMainTableView();


public:
	void LoadColumnsHide();

private:
	QList<int> NumberOfColumnsThatShouldHide;
	void ProcessColumnsForShow();

private slots:
	void on_buttonBox_clicked(QAbstractButton* button);


protected:
	void closeEvent(QCloseEvent* event);

signals:
	void HideColumns(QList<int> NumberOfColumns);

private:
	Ui::SelectColumnsForMainTableView ui;
};
