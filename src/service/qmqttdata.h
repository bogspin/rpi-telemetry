#ifndef QMQTTDATA_H
#define QMQTTDATA_H

#include <QObject>
#include <QList>
#include <QTcpSocket>
#include "QtMqtt/QMqttClient"
#include "InfluxDBFactory.h"
#include "Point.h"

class QMqttData : public QObject
{
    Q_OBJECT
public:
    explicit QMqttData(QObject *parent = nullptr);
    ~QMqttData();

    void setClientInfo(QString hostname, quint16 port, QString clientID = QString(),
                      QString username = QString(), QString password = QString());
    QMqttSubscription* subscribeToTopic(const QString &topic, quint8 qos);
    void unsubscribeFromTopic(const QString &topic);
    void unsubscribeAll();
    QStringList getTopics();
    void printMsg(const QMqttMessage &msg);
    void updateClientStatus(QMqttClient::ClientState state);
    void updateSubStatus(QMqttSubscription::SubscriptionState state);
    void getClientInfo();
    QMqttClient* getClient();

    void connectToDB();
    void writeToDB(const QMqttMessage &msg);
    void testQuery();

    void onConnect();
    void onDisconnect();

private:
    QMqttClient *client;
    QList<QMqttSubscription*> topics;
    std::unique_ptr<influxdb::InfluxDB> db;

signals:
    void clientConnected(QMqttData*);
    void clientDisconnected(QMqttData*);
};

#endif // QMQTTDATA_H
