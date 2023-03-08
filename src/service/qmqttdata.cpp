#include "qmqttdata.h"

QMqttData::QMqttData(QObject *parent) : QObject(parent)
{
    client = new QMqttClient(this);

    connect(client, &QMqttClient::stateChanged, this, &QMqttData::updateClientStatus);
    connect(client, &QMqttClient::connected, this, &QMqttData::onConnect);
    connect(client, &QMqttClient::disconnected, this, &QMqttData::onDisconnect);
    connect(client, &QMqttClient::connected, this, &QMqttData::connectToDB);
}

QMqttData::~QMqttData()
{
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

void QMqttData::setClientInfo(QString hostname, quint16 port, QString clientID,
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
                connect(sub, &QMqttSubscription::messageReceived, this, &QMqttData::printMsg);
                connect(sub, &QMqttSubscription::messageReceived, this, &QMqttData::writeToDB);
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
    const QString content = QDateTime::currentDateTime().toString()
                    + QLatin1String(": State Change")
                    + QString::number(state)
                    + QLatin1Char('\n');
    qDebug() << content;
}

void QMqttData::updateSubStatus(QMqttSubscription::SubscriptionState state)
{
    switch (state) {
    case QMqttSubscription::Unsubscribed:
        qDebug() << ("Unsubscribed");
        break;
    case QMqttSubscription::SubscriptionPending:
        qDebug() << ("Pending");
        break;
    case QMqttSubscription::Subscribed:
        qDebug() << ("Subscribed");
        break;
    case QMqttSubscription::Error:
        qDebug() << ("Error");
        break;
    default:
        qDebug() << ("--Unknown--");
        break;
    }
}

void QMqttData::getClientInfo()
{
    qDebug() << client->hostname();
    qDebug() << client->port();
    qDebug() << client->state();
}

void QMqttData::printMsg(const QMqttMessage &msg)
{
    qDebug() << msg.topic().name() << ": " << msg.payload();
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

QMqttClient* QMqttData::getClient()
{
    return this->client;
}

QStringList QMqttData::getTopics()
{
    QStringList topicList;
    for (auto sub : topics) {
        topicList.append(sub->topic().filter());
    }
    return topicList;
}

void QMqttData::connectToDB()
{
    QStringList dbName = client->hostname().split(".");
    QString url = "http://localhost:8086?db=" + dbName[1];

    db = influxdb::InfluxDBFactory::Get(url.toStdString());
    db->createDatabaseIfNotExists();
}

void QMqttData::writeToDB(const QMqttMessage &msg)
{
    if (db == nullptr) {
        return;
    }
    db->write(influxdb::Point{msg.topic().name().toStdString()}.
              addField("value", msg.payload().toFloat()));
}

void QMqttData::testQuery()
{
    for (auto i: db->query("select * from \"channels/182328/subscribe/fields/field3\"")) {
     qDebug()<<i.getName().c_str()<<":";
     qDebug()<<i.getTags().c_str()<<":";
     qDebug()<<i.getFields().c_str()<<":";
     QString timestamp(std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(i.getTimestamp().time_since_epoch()).count()).c_str());
     qDebug()<< QDateTime::fromMSecsSinceEpoch(timestamp.toLongLong());
    }
}
