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
    QMqttService(const QMqttService &service) = delete;
    ~QMqttService();
    static QMqttService* getInstance();

    bool setConfigFile(const QString &path);
    void startService();
    void stopService();
    void restartService();
    QMqttData* addConnection(const QJsonObject &conn);
    void addSubscriptions(QMqttData *conn, const QJsonArray &subs);
    void connectToDB();
    void writeToDB(const QString &hostname, const QMqttMessage &msg);

private:
    explicit QMqttService(QObject *parent = nullptr);
    static QMqttService* serviceInstance;

    QList<QMqttData*> mqttConnections;
    std::unique_ptr<influxdb::InfluxDB> db;
    QFileSystemWatcher configFile;
};

#endif // QMQTTSERVICE_H
