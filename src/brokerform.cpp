#include "brokerform.h"
#include "src_autogen/include/ui_brokerform.h"

BrokerForm::BrokerForm(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::BrokerForm)
{
    ui->setupUi(this);

    client = new QMqttClient(parent);

    connect(client, &QMqttClient::stateChanged,
            this, &BrokerForm::emitOnStateChange);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &BrokerForm::okClicked);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &BrokerForm::cancelClicked);
}

void BrokerForm::okClicked()
{
    client->setHostname(getHostName());
    client->setPort(getPort());
    client->setClientId(getClientID());
    client->setUsername(getUserName());
    client->setPassword(getPassword());
    client->connectToHost();
}

void BrokerForm::cancelClicked()
{
    this->close();
}

void BrokerForm::emitOnStateChange()
{
    const QString content = QDateTime::currentDateTime().toString()
                    + QLatin1String(": State Change")
                    + QString::number(client->state())
                    + QLatin1Char('\n');
    qDebug() << content;

    switch (client->state()) {
        case QMqttClient::ClientState::Disconnected: {
            emit disconnected();
            QString info = "Connection refused!";
            QMessageBox::warning(this, "Disconnected", info);
            break;
        }
        case QMqttClient::ClientState::Connecting: {
            break;
        }
        case QMqttClient::ClientState::Connected: {
            emit connected(client);
            QString info = "Connection established to " + getHostName();
            QMessageBox::information(this, "Connected", info);
            this->close();
        }
        default: break;
    }
}

QString BrokerForm::getClientID() const
{
    return ui->lineEditClientID->text();
}

QString BrokerForm::getHostName() const
{
    return ui->lineEditHost->text();
}

int BrokerForm::getPort() const
{
    return ui->spinBoxPort->value();
}

QString BrokerForm::getUserName() const
{
    return ui->lineEditUser->text();
}

QString BrokerForm::getPassword() const
{
    return ui->lineEditPassword->text();
}
