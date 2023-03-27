#ifndef PLOTDATA_H
#define PLOTDATA_H

#include <QVector>
#include <QString>
#include "qcustomplot.h"

class PlotData : public QCustomPlot
{
public:
    PlotData();
    ~PlotData();
    PlotData& operator=(const PlotData&) = delete;
    PlotData(const PlotData&) = delete;

    void setValue(const QVector<double> &value);
    void setTimestamp(const QVector<double> &timestamp);
    void setHostname(const QString &hostname);
    void setTopic(const QString &topic);
    QVector<double> getValue() const;
    QVector<double> getTimestamp() const;
    QString getHostname() const;
    QString getTopic() const;

    void addPoint(double val, double ts);

private:
    QVector<double> value;
    QVector<double> timestamp;
    QString hostname;
    QString topic;
};

#endif // PLOTDATA_H
