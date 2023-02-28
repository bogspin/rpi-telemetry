#ifndef QMQTTSERVICE_H
#define QMQTTSERVICE_H

#include <QObject>
#include <QFile>
#include <QFileSystemWatcher>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "qmqttdata.h"

class QMqttService : public QObject
{
    Q_OBJECT
public:
    explicit QMqttService(QObject *parent = nullptr);
    ~QMqttService();

    bool setConfigFile(const QString &path);
    void startService();

private:
    QList<QMqttData*> mqttConnections;
    std::unique_ptr<influxdb::InfluxDB> db;
    QFileSystemWatcher configFile;

signals:

};

#endif // QMQTTSERVICE_H
