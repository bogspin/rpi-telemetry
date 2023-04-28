#include "daterangeselector.h"
#include "src_autogen/include/ui_daterangeselector.h"
#include <QDebug>

DateRangeSelector::DateRangeSelector(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DateRangeSelector)
{
    ui->setupUi(this);

    ui->endTime->setDateTime(QDateTime::currentDateTime());

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &DateRangeSelector::okClicked);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &DateRangeSelector::cancelClicked);
}

void DateRangeSelector::okClicked()
{
    qint64 startTimestamp, endTimestamp;

    startDate.setDate(ui->startCalendar->selectedDate());
    startDate.setTime(ui->startTime->time());
    endDate.setDate(ui->endCalendar->selectedDate());
    endDate.setTime(ui->endTime->time());
    startTimestamp = startDate.toMSecsSinceEpoch();
    endTimestamp = endDate.toMSecsSinceEpoch();

    emit timestampRange(startTimestamp, endTimestamp, ui->checkBox->isChecked());
    this->close();
}

void DateRangeSelector::cancelClicked()
{
    emit closed();
    this->close();
}
