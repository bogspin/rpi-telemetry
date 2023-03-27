#include "connectionwindow.h"
#include "src_autogen/include/ui_connectionwindow.h"

ConnectionWindow::ConnectionWindow(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::ConnectionWindow)
{
    ui->setupUi(this);

    this->setStyleSheet("background-color: #333333;");

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &ConnectionWindow::okClicked);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &ConnectionWindow::cancelClicked);
}

void ConnectionWindow::setConnection(QJsonObject conn)
{
    ui->lineEditHost->setText(conn.value("hostname").toString());
    ui->lineEditClientID->setText(conn.value("clientid").toString());
    ui->lineEditUser->setText(conn.value("username").toString());
    ui->lineEditPassword->setText(conn.value("password").toString());
    ui->spinBoxPort->setValue(conn.value("port").toInt());
}

void ConnectionWindow::okClicked()
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
        connInfo.insert("username", getUserName());
    }
    if (!getPassword().isEmpty()) {
        connInfo.insert("password", getPassword());
    }

    emit connection(connInfo);
    this->close();
}

void ConnectionWindow::cancelClicked()
{
    this->close();
}

QString ConnectionWindow::getClientID() const
{
    return ui->lineEditClientID->text();
}

QString ConnectionWindow::getHostName() const
{
    return ui->lineEditHost->text();
}

int ConnectionWindow::getPort() const
{
    return ui->spinBoxPort->value();
}

QString ConnectionWindow::getUserName() const
{
    return ui->lineEditUser->text();
}

QString ConnectionWindow::getPassword() const
{
    return ui->lineEditPassword->text();
}
