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

void GraphInfo::setRange(qint64 startTime, qint64 endTime)
{
    this->startTime = startTime;
    this->endTime = endTime;
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
