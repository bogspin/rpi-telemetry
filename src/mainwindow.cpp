#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtCore/QDateTime>
#include <QtMqtt/QMqttClient>
#include <QtWidgets/QMessageBox>

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
    ui->menuFile->setStyleSheet("QMenu::item{"
                                "color: rgb(255, 255, 255);"
                                "}");

    connect(ui->actionQuit, &QAction::triggered, this, [] {
        QApplication::quit();
    });
    connect(ui->actionConnectTS, &QAction::triggered, this, &MainWindow::connectToThingSpeak);
    connect(ui->addBroker, &QPushButton::clicked, this, &MainWindow::createBrokerForm);
    connect(ui->subscribeButton, &QPushButton::clicked, this, &MainWindow::onSubscribe);
    connect(ui->unsubscribeButton, &QPushButton::clicked, this, &MainWindow::onUnsubscribe);
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
    brokerFormWindow->setWindowTitle("Add MQTT broker");
    brokerFormWindow->setWindowState(Qt::WindowActive);
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - brokerFormWindow->width()) / 2;
    int y = (screenGeometry.height() - brokerFormWindow->height()) / 2;
    brokerFormWindow->move(x, y);
    brokerFormWindow->show();

    connect(brokerFormWindow, &BrokerForm::connected, this, &MainWindow::addBroker);
    connect(brokerFormWindow, &BrokerForm::disconnected, this, []{});
}

void MainWindow::addBroker(QMqttClient *newClient)
{
    // connect(newClient, &QMqttClient::stateChanged,
    //         this, &MainWindow::updateLogStateChange(newClient));

    QMqttData* newConn = new QMqttData();
    newConn->assignClient(newClient);
    mqttConnections.append(newConn);

    if(model->insertRow(model->rowCount())) {
        QModelIndex index = model->index(model->rowCount() - 1, 0);
        model->setData(index, newClient->hostname());
    }

    qDebug() << newClient->hostname() << newClient->port();
}

void MainWindow::onSubscribe()
{
    int clientIndex = ui->listView->currentIndex().row();

    if (clientIndex == -1) {
        QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Please select a connection first!"));
        return;
    }

    auto subscription = mqttConnections[clientIndex]->subscribeToTopic(ui->topicEdit->text(),
                                                                 ui->comboQoS->itemData(ui->comboQoS->currentIndex()).toUInt());
    if (!subscription) {
        QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Could not subscribe! Check topic spelling or connection status"));
        return;
    }

    ui->topicLabel->setText(mqttConnections[clientIndex]->getTopics().join("\n"));
    //connect(subscription, &QMqttSubscription::messageReceived, this, &MainWindow::displayMessage);
}

void MainWindow::onUnsubscribe()
{
    int clientIndex = ui->listView->currentIndex().row();

    if (clientIndex == -1) {
        QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Please select a connection first!"));
        return;
    }

    qDebug() << mqttConnections[clientIndex]->getTopics();
}

void MainWindow::displayMessage(const QMqttMessage &msg)
{
    ui->listWidget->addItem(msg.payload());
}

void MainWindow::connectToThingSpeak()
{
    QMqttClient *tsClient = new QMqttClient(this);
    tsClient->setHostname("mqtt3.thingspeak.com");
    tsClient->setPort(1883);
    tsClient->setClientId("OykJDzssDyMJHQUlDRkwMwY");
    tsClient->setUsername("OykJDzssDyMJHQUlDRkwMwY");
    tsClient->setPassword("cMtgWZIIDKNhck8N/cHUg5PC");
    tsClient->connectToHost();
    addBroker(tsClient);
}
