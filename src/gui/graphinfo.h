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
    void setAlias(QString alias);
    void setType(QString type);
    void setUnit(QString unit);
    void setRange(qint64 startTime, qint64 endTime = 0);
    QString getHostname() const;
    QString getTopic() const;
    QString getAlias() const;
    QString getType() const;
    QString getUnit() const;
    std::string selectQuery();
    std::string refreshQuery(qint64 refreshInterval);

protected:
    QString hostname, topic, alias, type, unit;
    qint64 startTime = 0, endTime = 0;
};

#endif // GRAPHINFO_H
