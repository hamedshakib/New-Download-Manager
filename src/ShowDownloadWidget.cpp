#include "HeaderAndUi/ShowDownloadWidget.h"
#include <QDebug>

ShowDownloadWidget::ShowDownloadWidget(DownloadController* DownloadController, QWidget* parent)
    : QWidget(parent)
    , m_DownloadController(DownloadController)
{
    ui.setupUi(this);
    this->setWindowTitle("Download");

    if (m_DownloadController) {
        m_Download = m_DownloadController->Get_Download();
    }
}

ShowDownloadWidget::~ShowDownloadWidget()
{
    // نکته مهم: qDeleteAll(items) حذف شد چون QTreeWidget خودش مالکیت و حذف آیتم‌ها را مدیریت می‌کند[cite: 8]
    qDebug() << "Show Download widget deleted";
}

void ShowDownloadWidget::ProcessSetup()
{
    if (!m_Download || !m_DownloadController) return;

    ui.Url_label->setText(m_Download->get_Url().toString());
    ui.TimeLeft_label->setText("");
    ui.TransferRate_label->setText("");
    ui.transferRateInSpeedLimiter_label->setText("");

    const bool is_Downloading = m_DownloadController->IsDownloading();
    ui.PauseResume_pushButton->setText(is_Downloading ? tr("Pause") : tr("Resume"));
    ui.resumeCapability_label->setText(ProcessEnum::ConvertResumeCapabilityEnumToString(m_Download->ResumeCapability));
    ui.status_label->setText(ProcessEnum::ConvertDownloadStatusEnumToString(m_Download->downloadStatus));
    ui.FileSize_label->setText(ConverterSizeToSuitableString::ConvertSizeToSuitableString(m_Download->DownloadSize));
    ui.Downloded_label->setText(ConverterSizeToSuitableString::ConvertSizeToSuitableString(m_Download->SizeDownloaded));

    // محاسبه ایمن Progress Bar برای جلوگیری از خطای Division by Zero[cite: 8]
    UpdateProgressBar(m_Download->SizeDownloaded, m_Download->DownloadSize);

    // اتصال ایمن سیگنال‌ها (استفاده از this به جای capture با & برای جلوگیری از Crash در چندنخی)[cite: 8]
    connect(m_DownloadController, &DownloadController::UpdateDownloaded, this, &ShowDownloadWidget::UpdateInDownloading);
    connect(m_DownloadController, &DownloadController::DownloadStarted, this, &ShowDownloadWidget::ChangePauseOrResume_Download);
    connect(m_DownloadController, &DownloadController::DownloadPaused, this, &ShowDownloadWidget::ChangePauseOrResume_Download);
    connect(m_DownloadController, &DownloadController::DownloadCompleted, this, [this]() {
        this->close();
        this->deleteLater();
        });

    // آماده‌سازی TreeWidget
    ui.treeWidget->setUpdatesEnabled(false); // جلوگیری از لگ رسم در حین اضافه کردن آیتم‌ها[cite: 8]
    ui.treeWidget->clear();
    items.clear();
    TreeWidgetMap.clear();

    QList<PartDownload*> partdownloads = m_Download->get_PartDownloads();
    const QString InfoString = is_Downloading ? tr("Downloading") : tr("Paused");

    for (int i = 0; i < partdownloads.count(); i++) {
        QTreeWidgetItem* item = new QTreeWidgetItem();
        item->setText(0, QString::number(i));

        qint64 downloadedPartBytes = qMax(0LL, partdownloads[i]->GetLastDownloadedByte() + 1 - partdownloads[i]->start_byte);
        item->setText(1, ConverterSizeToSuitableString::ConvertSizeToSuitableString(downloadedPartBytes));
        item->setText(2, InfoString);

        TreeWidgetMap.insert(item, partdownloads[i]);
        items.append(item);
    }

    ui.treeWidget->addTopLevelItems(items);
    ui.treeWidget->setUpdatesEnabled(true);

    int MaxSpeed = m_DownloadController->Get_MaxSpeed();
    if (MaxSpeed > 0) {
        ChangeShowSpeedFromDownloadController(MaxSpeed);
    }

    connect(m_DownloadController, &DownloadController::SpeedChanged, this, &ShowDownloadWidget::ChangeShowSpeedFromDownloadController);
    connect(ui.checkBox, &QCheckBox::clicked, this, &ShowDownloadWidget::ClickedCheckBox);
    connect(ui.spinBox, &QSpinBox::valueChanged, this, &ShowDownloadWidget::SpinBoxValueChanged);
}

void ShowDownloadWidget::UpdateInDownloading(QString Status, QString speed, QString TimeLeft, QList<qint64> DownloadedBytesEachPartDownloadList)
{
    if (!m_Download) return;

    // به‌روزرسانی برچسب‌ها[cite: 8]
    ui.Downloded_label->setText(ConverterSizeToSuitableString::ConvertSizeToSuitableString(m_Download->SizeDownloaded));
    ui.status_label->setText(Status);
    ui.TransferRate_label->setText(speed);
    ui.transferRateInSpeedLimiter_label->setText(speed);
    ui.TimeLeft_label->setText(TimeLeft);

    UpdateProgressBar(m_Download->SizeDownloaded, m_Download->DownloadSize);

    // به‌روزرسانی ردیف‌ها با غیرفعال کردن موقت Repaint برای جلوگیری از لگ رابط کاربری[cite: 8]
    ui.treeWidget->setUpdatesEnabled(false);
    const int count = qMin(items.count(), DownloadedBytesEachPartDownloadList.count());
    for (int i = 0; i < count; i++) {
        items[i]->setText(1, ConverterSizeToSuitableString::ConvertSizeToSuitableString(DownloadedBytesEachPartDownloadList[i]));
    }
    ui.treeWidget->setUpdatesEnabled(true);
}

void ShowDownloadWidget::UpdateProgressBar(qint64 downloaded, qint64 total)
{
    if (total > 0) {
        ui.progressBar->setMaximum(100);
        int progressPercent = static_cast<int>((downloaded * 100) / total);
        ui.progressBar->setValue(qBound(0, progressPercent, 100));
    }
    else {
        // حالت Indeterminate برای زمانی که حجم فایل نامشخص است[cite: 8]
        ui.progressBar->setMaximum(0);
        ui.progressBar->setValue(0);
    }
}

void ShowDownloadWidget::on_PauseResume_pushButton_clicked()
{
    if (!m_DownloadController) return;

    if (m_DownloadController->IsDownloading()) {
        m_DownloadController->PauseDownload();
    }
    else {
        m_DownloadController->StartDownload();
    }
}

void ShowDownloadWidget::ChangePauseOrResume_Download()
{
    if (!m_DownloadController) return;

    const bool Is_downloading = m_DownloadController->IsDownloading();
    const QString PauseOrResume = Is_downloading ? tr("Pause") : tr("Resume");
    const QString InfoString = Is_downloading ? tr("Downloading") : tr("Paused");

    ui.treeWidget->setUpdatesEnabled(false);
    for (auto* item : items) {
        item->setText(2, InfoString);
    }
    ui.treeWidget->setUpdatesEnabled(true);

    ui.PauseResume_pushButton->setText(PauseOrResume);
}

void ShowDownloadWidget::ChangeShowSpeedFromDownloadController(int Speed)
{
    ui.spinBox->blockSignals(true); // جلوگیری از ایجاد حلقه بی‌نهایت سیگنال‌ها[cite: 8]
    ui.checkBox->blockSignals(true);

    if (Speed > 0) {
        if (!ui.checkBox->isChecked()) {
            ui.checkBox->setChecked(true);
            ui.spinBox->setEnabled(true);
        }
        if (Speed != ui.spinBox->value()) {
            ui.spinBox->setValue(Speed);
        }
    }
    else {
        if (ui.checkBox->isChecked()) {
            ui.checkBox->setChecked(false);
            ui.spinBox->setEnabled(false);
        }
    }

    ui.spinBox->blockSignals(false);
    ui.checkBox->blockSignals(false);
}

void ShowDownloadWidget::ClickedCheckBox(bool is_checked)
{
    if (!m_DownloadController) return;

    ui.spinBox->setEnabled(is_checked);
    int targetSpeed = is_checked ? ui.spinBox->value() : 0;

    if (m_DownloadController->Get_MaxSpeed() != targetSpeed) {
        m_DownloadController->SetMaxSpeed(targetSpeed);
    }
}

void ShowDownloadWidget::SpinBoxValueChanged(int newValue)
{
    if (m_DownloadController && m_DownloadController->Get_MaxSpeed() != newValue) {
        m_DownloadController->SetMaxSpeed(newValue);
    }
}

void ShowDownloadWidget::ChangeDownloadControllerSpeed(int speed)
{
    if (m_DownloadController) {
        m_DownloadController->SetMaxSpeed(speed);
    }
}