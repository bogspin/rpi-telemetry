#include "plotdata.h"

PlotData::PlotData()
{

}

PlotData::~PlotData()
{

}

void PlotData::setValue(const QVector<double> &value)
{
    this->value = value;
}

void PlotData::setTimestamp(const QVector<double> &timestamp)
{
    this->timestamp = timestamp;
}

void PlotData::setHostname(const QString &hostname)
{
    this->hostname = hostname;
}

void PlotData::setTopic(const QString &topic)
{
    this->topic = topic;
}

QVector<double> PlotData::getValue() const
{
    return value;
}

QVector<double> PlotData::getTimestamp() const
{
    return timestamp;
}

QString PlotData::getHostname() const
{
    return hostname;
}

QString PlotData::getTopic() const
{
    return topic;
}

void PlotData::addPoint(double val, double ts)
{
    this->value.append(val);
    this->timestamp.append(ts);
}
