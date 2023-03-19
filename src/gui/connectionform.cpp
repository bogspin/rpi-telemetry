#include "connectionform.h"
#include "src_autogen/include/ui_connectionform.h"

ConnectionForm::ConnectionForm(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::ConnectionForm)
{
    ui->setupUi(this);

    this->setStyleSheet("background-color: #333333;");

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &ConnectionForm::okClicked);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &ConnectionForm::cancelClicked);
}

void ConnectionForm::okClicked()
{
    if (!getHostName().isEmpty()) {
        connInfo.insert("hostname", getHostName());
    } else {
        QMessageBox::warning(this, "Empty hostname", "Please fill in the hostname");
        return;
    }

    connInfo.insert("port", getPort());

    if (!getClientID().isEmpty()) {
        connInfo.insert("clientid", getClientID());
    }
    if (!getUserName().isEmpty()) {
        connInfo.insert("clientid", getUserName());
    }
    if (!getPassword().isEmpty()) {
        connInfo.insert("clientid", getPassword());
    }

    emit connection(connInfo);
    this->close();
}

void ConnectionForm::cancelClicked()
{
    this->close();
}

QString ConnectionForm::getClientID() const
{
    return ui->lineEditClientID->text();
}

QString ConnectionForm::getHostName() const
{
    return ui->lineEditHost->text();
}

int ConnectionForm::getPort() const
{
    return ui->spinBoxPort->value();
}

QString ConnectionForm::getUserName() const
{
    return ui->lineEditUser->text();
}

QString ConnectionForm::getPassword() const
{
    return ui->lineEditPassword->text();
}
