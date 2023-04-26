#include "daterangeselector.h"
#include "src_autogen/include/ui_daterangeselector.h"

DateRangeSelector::DateRangeSelector(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DateRangeSelector)
{
    ui->setupUi(this);
}

void DateRangeSelector::okClicked()
{
    if (ui->checkBox->isChecked()) {
        emit allTime();
        this->close();
    }

    qint64 startTimestamp, endTimestamp;

    startDate.setDate(ui->startCalendar->selectedDate());
    startDate.setTime(ui->startTime->time());
    endDate.setDate(ui->endCalendar->selectedDate());
    endDate.setTime(ui->endTime->time());
    startTimestamp = startDate.toMSecsSinceEpoch();
    endTimestamp = endDate.toMSecsSinceEpoch();

    emit timestampRange(startTimestamp, endTimestamp);
    this->close();
}

void DateRangeSelector::cancelClicked()
{
    emit closed();
    this->close();
}
