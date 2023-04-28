#ifndef DATERANGESELECTOR_H
#define DATERANGESELECTOR_H

#include <QWidget>
#include <QDateTime>

QT_BEGIN_NAMESPACE
namespace Ui {
class DateRangeSelector;
}
QT_END_NAMESPACE

class DateRangeSelector : public QWidget
{
    Q_OBJECT
public:
    explicit DateRangeSelector(QWidget *parent = nullptr);

signals:
    void timestampRange(qint64 start, qint64 end, bool allTime);
    void closed();

private slots:
    void okClicked();
    void cancelClicked();

private:
    Ui::DateRangeSelector *ui;
    QDateTime startDate, endDate;
};

#endif // DATERANGESELECTOR_H
