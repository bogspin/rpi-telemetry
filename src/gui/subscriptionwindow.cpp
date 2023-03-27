#include "subscriptionwindow.h"
#include "src_autogen/include/ui_subscriptionwindow.h"

SubscriptionWindow::SubscriptionWindow(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::SubscriptionWindow)
{
    ui->setupUi(this);

    this->setStyleSheet("background-color: #333333;");

    ui->comboBoxQoS->addItem("0 - At most once");
    ui->comboBoxQoS->addItem("1 - At least once");
    ui->comboBoxQoS->addItem("2 - Exactly once");
    ui->comboBoxQoS->setCurrentIndex(0);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &SubscriptionWindow::okClicked);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &SubscriptionWindow::cancelClicked);
}

void SubscriptionWindow::setSubscription(QJsonObject sub)
{
    ui->lineEditTopic->setText(sub.value("topic").toString());
    ui->lineEditAlias->setText(sub.value("alias").toString());
    ui->lineEditType->setText(sub.value("type").toString());
    ui->lineEditUnit->setText(sub.value("unit").toString());
    ui->comboBoxQoS->setCurrentIndex(sub.value("qos").toInt());
}

void SubscriptionWindow::okClicked()
{
    if (!getTopic().isEmpty()) {
        subInfo.insert("topic", getTopic());
    } else {
        QMessageBox::warning(this, "Empty topic", "Please fill in the topic you wish to subscribe to!");
        return;
    }

    subInfo.insert("qos", getQoS());

    if (!getAlias().isEmpty()) {
        subInfo.insert("alias", getAlias());
    }
    if (!getType().isEmpty()) {
        subInfo.insert("type", getType());
    }
    if (!getUnit().isEmpty()) {
        subInfo.insert("unit", getUnit());
    }

    emit subscription(subInfo);
    this->close();
}

void SubscriptionWindow::cancelClicked()
{
    this->close();
}

QString SubscriptionWindow::getTopic() const
{
    return ui->lineEditTopic->text();
}

int SubscriptionWindow::getQoS() const
{
    return ui->comboBoxQoS->currentIndex();
}

QString SubscriptionWindow::getAlias() const
{
    return ui->lineEditAlias->text();
}

QString SubscriptionWindow::getType() const
{
    return ui->lineEditType->text();
}

QString SubscriptionWindow::getUnit() const
{
    return ui->lineEditUnit->text();
}
