#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtCore/QDateTime>
#include <QtMqtt/QMqttClient>
#include <QtWidgets/QMessageBox>

#include "InfluxDBFactory.h"
#include "Point.h"
#include "QDebug"
#include <string.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QMainWindow::showFullScreen();
    this->setStyleSheet("background-color: #333333;");

    model = new QStringListModel(this);
    ui->listView->setModel(model);

    ui->comboQoS->addItem("0");
    ui->comboQoS->addItem("1");
    ui->comboQoS->addItem("2");
    ui->comboQoS->setEditText("QoS");

    connect(ui->addBroker, &QPushButton::clicked, this, &MainWindow::createBrokerForm);
    connect(ui->listView, &QAbstractItemView::clicked, this, &MainWindow::clientClicked);
    connect(ui->subscribeButton, &QPushButton::clicked, this, &MainWindow::subscribeToTopic);
//    connect(m_client, &QMqttClient::disconnected, this, &MainWindow::brokerDisconnected);

//    connect(m_client, &QMqttClient::messageReceived, this, &MainWindow::addMessageToDB);

//    connect(m_client, &QMqttClient::pingResponseReceived, this, [this]() {
//        const QString content = QDateTime::currentDateTime().toString()
//                    + QLatin1String(" PingResponse")
//                    + QLatin1Char('\n');
//        ui->editLog->insertPlainText(content);
//    });

//    connect(ui->lineEditHost, &QLineEdit::textChanged, m_client, &QMqttClient::setHostname);
//    connect(ui->spinBoxPort, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::setClientPort);
//    connect(ui->lineEditUser, &QLineEdit::textChanged, m_client, &QMqttClient::setUsername);
//    connect(ui->lineEditPassword, &QLineEdit::textChanged, m_client, &QMqttClient::setPassword);
//    updateLogStateChange();
}

MainWindow::~MainWindow()
{
    delete ui;
    qApp->quit();
}

void MainWindow::updateLogStateChange(QMqttClient *client)
{
    const QString content = QDateTime::currentDateTime().toString()
                    + QLatin1String(": State Change")
                    + QString::number(client->state())
                    + QLatin1Char('\n');
    qDebug() << content;
}

void MainWindow::createBrokerForm()
{
    brokerFormWindow = new BrokerForm(this);
    brokerFormWindow->setWindowTitle("Add MQTT Client");
    brokerFormWindow->setWindowState(Qt::WindowActive);
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - brokerFormWindow->width()) / 2;
    int y = (screenGeometry.height() - brokerFormWindow->height()) / 2;
    brokerFormWindow->move(x, y);
    brokerFormWindow->show();

    connect(brokerFormWindow, &BrokerForm::connected, this, &MainWindow::addClient);
    connect(brokerFormWindow, &BrokerForm::disconnected, this, []{});
}

void MainWindow::addClient(QMqttClient *newClient)
{
    // connect(newClient, &QMqttClient::stateChanged,
    //         this, &MainWindow::updateLogStateChange(newClient));

    mqttClients.append(newClient);
    QList<QMqttSubscription*> emptySubList;
    mqttSubs.append(emptySubList);

    if(model->insertRow(model->rowCount())) {
        QModelIndex index = model->index(model->rowCount() - 1, 0);
        model->setData(index, newClient->hostname());
    }

    qDebug() << newClient->hostname() << newClient->port();
}

void MainWindow::clientClicked(const QModelIndex &index)
{
    clientIndex = index.row();
}

void MainWindow::subscribeToTopic()
{
    auto subscription = mqttClients[clientIndex]->subscribe(ui->topicEdit->text(),
                                                               ui->comboQoS->itemData(ui->comboQoS->currentIndex()).toUInt());
    if (!subscription) {
        QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Could not subscribe. Is there a valid connection?"));
        return;
    }
    mqttSubs[clientIndex].append(subscription);
    connect(subscription, &QMqttSubscription::messageReceived, this, &MainWindow::displayMessage);
}

void MainWindow::displayMessage(const QMqttMessage &msg)
{
    ui->listWidget->addItem(msg.payload());
}
