#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QtMqtt/QMqttClient>
#include "brokerform.h"

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
    void updateLogStateChange();

//    void brokerDisconnected();

//    void addMessageToDB(const QByteArray &message, const QMqttTopicName &topic);

    void createBrokerForm();
    void addClient(BrokerForm*);

private:
    Ui::MainWindow *ui;
    BrokerForm *brokerFormWindow;
    QMqttClient *m_client;

    QMqttClient *newClient;
    QList<QMqttClient*> brokers;
};

#endif // MAINWINDOW_H
