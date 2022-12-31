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
    this->setStyleSheet("background-color: #5A5A5A;");

    newClient = new QMqttClient(this);
//    m_client->setHostname(ui->lineEditHost->text());
//    m_client->setPort(ui->spinBoxPort->value());

    connect(ui->addBroker, &QPushButton::clicked, this, &MainWindow::createBrokerForm);

    connect(newClient, &QMqttClient::stateChanged, this, &MainWindow::updateLogStateChange);
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

void MainWindow::updateLogStateChange()
{
    const QString content = QDateTime::currentDateTime().toString()
                    + QLatin1String(": State Change")
                    + QString::number(newClient->state())
                    + QLatin1Char('\n');
    qDebug() << content;
}

void MainWindow::createBrokerForm()
{
    brokerFormWindow = new BrokerForm();
    brokerFormWindow->setWindowTitle("Add MQTT Broker");
    brokerFormWindow->show();

    connect(brokerFormWindow, &BrokerForm::buttonBox, this, &MainWindow::addClient);
}

void MainWindow::addClient(BrokerForm *form)
{
    newClient->setHostname(form->getHostName());
    newClient->setPort(form->getPort());
    newClient->connectToHost();

    qDebug() << form->getHostName() << form->getPort();
}
