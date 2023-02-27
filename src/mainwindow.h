#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScreen>
#include <QTcpSocket>
#include <QtMqtt/QMqttClient>
#include "brokerform.h"
#include "qmqttdata.h"
#include "InfluxDBFactory.h"
#include "Point.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    //void setClientPort(int p);

private slots:
//    void on_buttonConnect_clicked();
//    void on_buttonQuit_clicked();
//    void brokerDisconnected();
//    void addMessageToDB(const QByteArray &message, const QMqttTopicName &topic);

    void createBrokerForm();
    void addBroker(QMqttClient *newClient);
    void updateLogStateChange(QMqttClient *client);
    void onSubscribe();
    void onUnsubscribe();
    void displayMessage(const QMqttMessage &msg);
    void connectToThingSpeak();

private:
    Ui::MainWindow *ui;
    BrokerForm *brokerFormWindow;

    QList<QMqttData*> mqttConnections;
    QStringListModel *model;
    std::unique_ptr<influxdb::InfluxDB> db;
};

#endif // MAINWINDOW_H
