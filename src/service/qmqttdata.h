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

    QMqttClient* getClient();
    void setClient(QString hostname, quint16 port, QString clientID = QString(),
                      QString username = QString(), QString password = QString());
    QMqttSubscription* subscribeToTopic(const QString &topic, quint8 qos);
    void unsubscribeFromTopic(const QString &topic);
    void unsubscribeAll();
    QStringList getTopics();
    void updateClientStatus(QMqttClient::ClientState state);
    void updateSubStatus(QMqttSubscription::SubscriptionState state);

    void onConnect();
    void onDisconnect();
    void sendMessage(QMqttMessage msg);

private:
    QMqttClient *client;
    QList<QMqttSubscription*> topics;

signals:
    void clientConnected(QMqttData*);
    void clientDisconnected(QMqttData*);
    void messageReceived(QString hostname, QMqttMessage msg);
};

#endif // QMQTTDATA_H
