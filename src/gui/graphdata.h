#ifndef GRAPHDATA_H
#define GRAPHDATA_H

#include <QVector>
#include <QString>

class GraphData
{
public:
    GraphData();

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

#endif // GRAPHDATA_H
