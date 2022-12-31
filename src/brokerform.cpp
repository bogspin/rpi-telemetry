#include "brokerform.h"
#include "src_autogen/include/ui_brokerform.h"

BrokerForm::BrokerForm(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::BrokerForm)
{
    ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &BrokerForm::okClicked);
}

void BrokerForm::okClicked()
{
    emit buttonBox(this);
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
