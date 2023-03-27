#ifndef SUBSCRIPTIONWINDOW_H
#define SUBSCRIPTIONWINDOW_H

#include <QWidget>
#include <QMessageBox>
#include <QJsonObject>

QT_BEGIN_NAMESPACE
namespace Ui {
class SubscriptionWindow;
}
QT_END_NAMESPACE

class SubscriptionWindow : public QWidget
{
    Q_OBJECT
public:
    explicit SubscriptionWindow(QWidget *parent = nullptr);
    void setSubscription(QJsonObject sub);
    QString getTopic() const;
    int getQoS() const;
    QString getAlias() const;
    QString getType() const;
    QString getUnit() const;

signals:
    void subscription(QJsonObject connInfo);

private slots:
    void okClicked();
    void cancelClicked();

private:
    Ui::SubscriptionWindow *ui;
    QJsonObject subInfo;
};

#endif // SUBSCRIPTIONWINDOW_H
