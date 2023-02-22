#ifndef BROKERFORM_H
#define BROKERFORM_H

#include <QWidget>
#include <QMessageBox>
#include <QtMqtt>

QT_BEGIN_NAMESPACE
namespace Ui {
class BrokerForm;
}
QT_END_NAMESPACE

class BrokerForm : public QWidget
{
    Q_OBJECT
public:
    explicit BrokerForm(QWidget *parent = nullptr);
    QString getHostName() const;
    int getPort() const;
    QString getUserName() const;
    QString getPassword() const;

signals:
    void connected(QMqttClient* client);
    void disconnected();

private slots:
    void okClicked();
    void cancelClicked();
    void emitOnStateChange();

private:
    Ui::BrokerForm *ui;
    QMqttClient *client;
};

#endif // ADDBROKER_H
