#include "graphinfo.h"

GraphInfo::GraphInfo()
{

}

GraphInfo::GraphInfo(QString hostname, QString topic, qint64 startTime, qint64 endTime)
{
    this->hostname = hostname;
    this->topic = topic;
    this->startTime = startTime;
    this->endTime = endTime;
}

GraphInfo::~GraphInfo()
{

}

void GraphInfo::setHostname(QString hostname)
{
    this->hostname = hostname;
}

void GraphInfo::setTopic(QString topic)
{
    this->topic = topic;
}

void GraphInfo::setAlias(QString alias)
{
    this->alias = alias;
}

void GraphInfo::setType(QString type)
{
    this->type = type;
}

void GraphInfo::setUnit(QString unit)
{
    this->unit = unit;
}

void GraphInfo::setRange(qint64 startTime, qint64 endTime)
{
    this->startTime = startTime;
    this->endTime = endTime;
}

QString GraphInfo::getHostname() const
{
    return this->hostname;
}

QString GraphInfo::getTopic() const
{
    return this->topic;
}

QString GraphInfo::getAlias() const
{
    return this->alias;
}

QString GraphInfo::getType() const
{
    return this->type;
}

QString GraphInfo::getUnit() const
{
    return this->unit;
}

std::string GraphInfo::selectQuery()
{
    QString query;

    query = "SELECT * FROM \"" + hostname + "\" WHERE topic='" + topic + "'";
    if (startTime != 0) {
        query += " AND TIME >= " + QString::number(startTime) + "ms";
    }
    if (endTime != 0) {
        query += " AND TIME <= " + QString::number(endTime) + "ms";
    }

    return query.toStdString();
}

std::string GraphInfo::refreshQuery(qint64 refreshInterval)
{
    QString query;
    qint64 lastUpdated = QDateTime::currentMSecsSinceEpoch() - refreshInterval;

    query = "SELECT * FROM \"" + hostname + "\" WHERE topic='" + topic +
            "' AND TIME >= " + QString::number(lastUpdated) + "ms";

    return query.toStdString();
}
