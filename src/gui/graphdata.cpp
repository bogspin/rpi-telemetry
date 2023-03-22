#include "graphdata.h"

GraphData::GraphData()
{

}

void GraphData::setValue(const QVector<double> &value)
{
    this->value = value;
}

void GraphData::setTimestamp(const QVector<double> &timestamp)
{
    this->timestamp = timestamp;
}

void GraphData::setHostname(const QString &hostname)
{
    this->hostname = hostname;
}

void GraphData::setTopic(const QString &topic)
{
    this->topic = topic;
}

QVector<double> GraphData::getValue() const
{
    return value;
}

QVector<double> GraphData::getTimestamp() const
{
    return timestamp;
}

QString GraphData::getHostname() const
{
    return hostname;
}

QString GraphData::getTopic() const
{
    return topic;
}

void GraphData::addPoint(double val, double ts)
{
    this->value.append(val);
    this->timestamp.append(ts);
}
