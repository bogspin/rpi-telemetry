#ifndef QMQTTSERVICE_H
#define QMQTTSERVICE_H

#include <QObject>
#include <QFile>
#include <QFileSystemWatcher>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QThread>
#include "qmqttdata.h"

class QMqttService : public QObject
{
    Q_OBJECT
public:
    explicit QMqttService(QObject *parent = nullptr);
    ~QMqttService();

    bool setConfigFile(const QString &path);
    void startService();
    QMqttData* addConnection(const QJsonObject &conn);
    void addSubscriptions(QMqttData *conn, const QJsonArray &subs);
    void connectToDB();
    void writeToDB(const QString &hostname, const QMqttMessage &msg);

private:
    QList<QMqttData*> mqttConnections;
    std::unique_ptr<influxdb::InfluxDB> db;
    QFileSystemWatcher configFile;

signals:
    void hasSubs(QMqttData* conn, const QJsonArray &subs);
};

#endif // QMQTTSERVICE_H
