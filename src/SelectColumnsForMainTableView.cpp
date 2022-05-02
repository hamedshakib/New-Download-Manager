#include "HeaderAndUi/SelectColumnsForMainTableView.h"

SelectColumnsForMainTableView::SelectColumnsForMainTableView(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	this->setWindowTitle("Choose columns");
}

SelectColumnsForMainTableView::~SelectColumnsForMainTableView()
{
}

void SelectColumnsForMainTableView::on_buttonBox_clicked(QAbstractButton* button)
{
	if (button == ui.buttonBox->button(QDialogButtonBox::StandardButton::Ok))
	{
		ProcessColumnsForShow();
		QString StringForSaveInIniFile;
		for (int ColumnNumber : NumberOfColumnsThatShouldHide)
		{
			StringForSaveInIniFile += QString::number(ColumnNumber)+",";
		}
		SettingInteract::SetValue("TableView/MainTableView/HiddenColumns", StringForSaveInIniFile);
		emit HideColumns(NumberOfColumnsThatShouldHide);
		this->close();
	}
	else
	{
		this->close();
	}
}

void SelectColumnsForMainTableView::ProcessColumnsForShow()
{
	if (!ui.checkBox->isChecked())
	{
		NumberOfColumnsThatShouldHide.append(1);
	}
	if (!ui.checkBox_2->isChecked())
	{
		NumberOfColumnsThatShouldHide.append(2);
	}
	if (!ui.checkBox_3->isChecked())
	{
		NumberOfColumnsThatShouldHide.append(3);
	}
	if (!ui.checkBox_4->isChecked())
	{
		NumberOfColumnsThatShouldHide.append(4);
	}
	if (!ui.checkBox_5->isChecked())
	{
		NumberOfColumnsThatShouldHide.append(5);
	}
	if (!ui.checkBox_6->isChecked())
	{
		NumberOfColumnsThatShouldHide.append(6);
	}
	if (!ui.checkBox_7->isChecked())
	{
		NumberOfColumnsThatShouldHide.append(7);
	}
	if (!ui.checkBox_8->isChecked())
	{
		NumberOfColumnsThatShouldHide.append(8);
	}

}

void SelectColumnsForMainTableView::closeEvent(QCloseEvent* event)
{
	this->close();
	this->deleteLater();
}

void SelectColumnsForMainTableView::LoadColumnsHide()
{
	QStringList HiddenColumnsStringList = SettingInteract::GetValue("TableView/MainTableView/HiddenColumns").toString().split(",");
	if (HiddenColumnsStringList.contains("1"))
	{
		ui.checkBox->setChecked(false);
	}
	if (HiddenColumnsStringList.contains("2"))
	{
		ui.checkBox_2->setChecked(false);
	}
	if (HiddenColumnsStringList.contains("3"))
	{
		ui.checkBox_3->setChecked(false);
	}
	if (HiddenColumnsStringList.contains("4"))
	{
		ui.checkBox_4->setChecked(false);
	}
	if (HiddenColumnsStringList.contains("5"))
	{
		ui.checkBox_5->setChecked(false);
	}
	if (HiddenColumnsStringList.contains("6"))
	{
		ui.checkBox_6->setChecked(false);
	}
	if (HiddenColumnsStringList.contains("7"))
	{
		ui.checkBox_7->setChecked(false);
	}
	if (HiddenColumnsStringList.contains("8"))
	{
		ui.checkBox_8->setChecked(false);
	}
}
