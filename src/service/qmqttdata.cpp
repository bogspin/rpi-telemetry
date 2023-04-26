#include "qmqttdata.h"

QMqttData::QMqttData(QObject *parent) : QObject(parent)
{
    client = new QMqttClient(this);

    connect(client, &QMqttClient::stateChanged, this, &QMqttData::updateClientStatus);
    connect(client, &QMqttClient::connected, this, &QMqttData::onConnect);
    connect(client, &QMqttClient::disconnected, this, &QMqttData::onDisconnect);
}

QMqttData::~QMqttData()
{
    unsubscribeAll();
    client->disconnect();

    delete client;
}

void QMqttData::onConnect()
{
    emit clientConnected(this);
}

void QMqttData::onDisconnect()
{
    emit clientDisconnected(this);
}

void QMqttData::sendMessage(QMqttMessage msg)
{
    emit messageReceived(this->client->hostname(), msg);
}

QMqttClient* QMqttData::getClient()
{
    return this->client;
}

void QMqttData::setClient(QString hostname, quint16 port, QString clientID,
                             QString username, QString password)
{
    this->client->setHostname(hostname);
    this->client->setPort(port);
    if (clientID != QString()) {
        this->client->setClientId(clientID);
    }
    if (username != QString()) {
        this->client->setUsername(username);
    }
    if (password != QString()) {
        this->client->setPassword(password);
    }
}

QMqttSubscription* QMqttData::subscribeToTopic(const QString &topic, quint8 qos)
{
    try {
        if (client->state() == QMqttClient::ClientState::Connected) {
            QMqttSubscription* sub = client->subscribe(topic, qos);
            if (sub) {
                connect(sub, &QMqttSubscription::stateChanged, this, &QMqttData::updateSubStatus);
                connect(sub, &QMqttSubscription::messageReceived, this, &QMqttData::sendMessage);
                topics.append(sub);
            }
            return sub;
        } else {
            throw client->state();
        }
    }  catch (QMqttClient::ClientState state) {
        qDebug() << "Can't subscribe!" << state;
    }

    return nullptr;
}

void QMqttData::updateClientStatus(QMqttClient::ClientState state)
{
    QString clientLog = this->client->hostname() + QString(": ");

    switch (state) {
    case QMqttClient::Disconnected: {
        clientLog += QString("Disconnected");
        break;
    }
    case QMqttClient::Connecting: {
        clientLog += QString("Connecting");
        break;
    }
    case QMqttClient::Connected: {
        clientLog += QString("Connected");
        break;
    }
    default: {
        clientLog += QString("--Unknown--");
        break;
    }
    }

    qDebug() << clientLog;
}

void QMqttData::updateSubStatus(QMqttSubscription::SubscriptionState state)
{
    QMqttSubscription *sub = qobject_cast<QMqttSubscription*>(sender());
    QString subLog = sub->topic().filter() + QString(": ");

    switch (state) {
    case QMqttSubscription::Unsubscribed:
        subLog += QString("Unsubscribed");
        break;
    case QMqttSubscription::SubscriptionPending:
        subLog += QString("Pending");
        break;
    case QMqttSubscription::Subscribed:
        subLog += QString("Subscribed");
        break;
    case QMqttSubscription::Error:
        subLog += QString("Error");
        break;
    default:
        subLog += QString("--Unknown--");
        break;
    }

    qDebug() << subLog;
}

void QMqttData::unsubscribeFromTopic(const QString &topic)
{
    QList<QMqttSubscription*>::iterator it;
    for (it = topics.begin(); it != topics.end(); ++it) {
        if (QMqttTopicFilter(topic) == (*it)->topic()) {
            (*it)->unsubscribe();
            topics.erase(it);
            return;
        }
    }
}

void QMqttData::unsubscribeAll()
{
    for (auto sub : topics) {
        sub->unsubscribe();
    }
    topics.clear();
}

QStringList QMqttData::getTopics()
{
    QStringList topicList;
    for (auto sub : topics) {
        topicList.append(sub->topic().filter());
    }
    return topicList;
}
