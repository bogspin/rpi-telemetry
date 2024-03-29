#include "qmqttservice.h"

QMqttService* QMqttService::serviceInstance = nullptr;

QMqttService::QMqttService(QObject *parent) : QObject(parent)
{
    connectToDB();
}

QMqttService::~QMqttService()
{
    stopService();
}

QMqttService *QMqttService::getInstance()
{
    if (serviceInstance == NULL) {
        serviceInstance = new QMqttService();

        return serviceInstance;
    }
    else {
        return serviceInstance;
    }
}

bool QMqttService::setConfigFile(const QString &path)
{
    return configFile.addPath(path);
}

void QMqttService::startService()
{
    QString path = configFile.files().constFirst();
    QFile inFile(path);

    if (!inFile.exists()) {
        qDebug() << "Error: No config file found at path " << path;
        exit(1);
    }

    if (!inFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error: Could not open configuration file!";
        exit(1);
    }

    QByteArray data = inFile.readAll();
    QJsonParseError parseErr;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data, &parseErr);
    QJsonObject conf;

    if (parseErr.error == QJsonParseError::NoError) {
        conf = jsonDoc.object();
    }
    else {
        qDebug() << "Error: Check for syntax errors in the configuration JSON!";
        exit(1);
    }

    if (conf.contains("connections")) {
        QJsonArray connections = conf.value("connections").toArray();

        foreach (QJsonValue i, connections) {
            QJsonObject conn = i.toObject();
            QMqttData *newConn = addConnection(conn);

            if (conn.contains("subscriptions")) {
                QJsonArray subs = conn.value("subscriptions").toArray();

                connect(newConn, &QMqttData::clientConnected, this, [this, newConn, subs](){
                    addSubscriptions(newConn, subs);
                });
            }
        }
    }
}

void QMqttService::stopService()
{
    for (QMqttData* conn : mqttConnections) {
        delete conn;
    }

    mqttConnections.clear();
}

void QMqttService::restartService()
{
    stopService();
    startService();
}

QMqttData* QMqttService::addConnection(const QJsonObject &conn)
{
    QMqttData* newConn = new QMqttData();

    if (conn.contains("hostname")) {
        newConn->getClient()->setHostname(conn.value("hostname").toString());
    }
    if (conn.contains("port")) {
        newConn->getClient()->setPort(conn.value("port").toInt());
    }
    if (conn.contains("clientid")) {
        newConn->getClient()->setClientId(conn.value("clientid").toString());
    }
    if (conn.contains("username")) {
        newConn->getClient()->setUsername(conn.value("username").toString());
    }
    if (conn.contains("password")) {
        newConn->getClient()->setPassword(conn.value("password").toString());
    }

    newConn->getClient()->connectToHost();
    mqttConnections.append(newConn);

    connect(newConn, &QMqttData::messageReceived, this, &QMqttService::writeToDB);

    return newConn;
}

void QMqttService::addSubscriptions(QMqttData *conn, const QJsonArray &subs)
{
    QString topic;
    quint8 qos = 0;

    foreach (QJsonValue j, subs) {
        QJsonObject sub = j.toObject();

        if (sub.contains("topic")) {
            topic = sub.value("topic").toString();
        }
        if (sub.contains("qos")) {
            qos = static_cast<quint8>(sub.value("qos").toInt());
        }

        conn->subscribeToTopic(topic, qos);
    }
}

void QMqttService::connectToDB()
{
    QString url = "http://localhost:8086?db=rpi_telemetry";

    db = influxdb::InfluxDBFactory::Get(url.toStdString());
    db->createDatabaseIfNotExists();
}

void QMqttService::writeToDB(const QString &hostname, const QMqttMessage &msg)
{
    if (db == nullptr) {
        return;
    }
    db->write(influxdb::Point{hostname.toStdString()}.
              addTag("topic", msg.topic().name().toStdString()).
              addField("value", msg.payload().toFloat()));

    qDebug() << msg.topic().name() + ": " + msg.payload();
}

