#ifndef GRAPHINFO_H
#define GRAPHINFO_H

#include <QString>
#include <QDateTime>

class GraphInfo
{
public:
    GraphInfo();
    GraphInfo(QString hostname, QString topic,
              qint64 startTime = 0, qint64 endTime = 0);
    ~GraphInfo();

    void setHostname(QString hostname);
    void setTopic(QString topic);
    void setRange(qint64 startTime, qint64 endTime = 0);
    std::string selectQuery();
    std::string refreshQuery(qint64 refreshInterval);

protected:
    QString hostname, topic;
    qint64 startTime = 0, endTime = 0;
};

#endif // GRAPHINFO_H
