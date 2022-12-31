#ifndef BROKERFORM_H
#define BROKERFORM_H

#include <QWidget>
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
    void buttonBox(BrokerForm *form);

private slots:
    void okClicked();

private:
    Ui::BrokerForm *ui;
};

#endif // ADDBROKER_H
