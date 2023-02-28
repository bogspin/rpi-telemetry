#include "qmqttservice.h"

QMqttService::QMqttService(QObject *parent) : QObject(parent)
{

}

QMqttService::~QMqttService()
{

}

bool QMqttService::setConfigFile(const QString &path)
{
    return configFile.addPath(path);
}

void QMqttService::startService()
{
    QString path = configFile.files().constFirst();
    QFile inFile(path);
    inFile.open(QIODevice::ReadOnly|QIODevice::Text);
    QByteArray data = inFile.readAll();
    QJsonParseError parseErr;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data, &parseErr);
    QJsonObject conf;
    QString hostname, clientID, username, password, topic;
    quint16 port;
    quint8 qos;

    if (parseErr.error == QJsonParseError::NoError) {
        conf = jsonDoc.object();
    }
    else {
        return;
    }

    if (conf.contains("connections")) {
        QJsonArray connections = conf.value("connections").toArray();

        foreach (QJsonValue i, connections) {
            QJsonObject conn = i.toObject();

            if (conn.contains("hostname")) {
                hostname = conn.value("hostname").toString();
            }
            if (conn.contains("port")) {
                port = static_cast<quint16>(conn.value("port").toString().toInt());
            }
            if (conn.contains("clientid")) {
                clientID = conn.value("clientid").toString();
            }
            else {
                clientID = QString();
            }
            if (conn.contains("username")) {
                username = conn.value("username").toString();
            }
            else {
                username = QString();
            }
            if (conn.contains("password")) {
                password = conn.value("password").toString();
            }
            else {
                password = QString();
            }

            QMqttData* newConn = new QMqttData();
            newConn->createClient(hostname, port, clientID, username, password);
            mqttConnections.append(newConn);

            // WAIT UNTIL CLIENT CONNECTED -> SEMAPHORE?

            if (conn.contains("subscriptions")) {
                QJsonArray subs = conn.value("subscriptions").toArray();

                foreach (QJsonValue j, subs) {
                    QJsonObject sub = j.toObject();

                    if (sub.contains("topic")) {
                        topic = sub.value("topic").toString();
                    }
                    if (sub.contains("qos")) {
                        qos = static_cast<quint8>(sub.value("qos").toString().toInt());
                    }

                    mqttConnections.back()->subscribeToTopic(topic, qos);
                }
                qDebug() << mqttConnections.back()->getTopics();
            }
        }
    }
}
