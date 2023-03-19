#ifndef CONNECTIONWINDOW_H
#define CONNECTIONWINDOW_H

#include <QWidget>
#include <QMessageBox>
#include <QJsonObject>

QT_BEGIN_NAMESPACE
namespace Ui {
class ConnectionForm;
}
QT_END_NAMESPACE

class ConnectionForm : public QWidget
{
    Q_OBJECT
public:
    explicit ConnectionForm(QWidget *parent = nullptr);
    QString getHostName() const;
    int getPort() const;
    QString getClientID() const;
    QString getUserName() const;
    QString getPassword() const;

signals:
    void connection(QJsonObject connInfo);

private slots:
    void okClicked();
    void cancelClicked();

private:
    Ui::ConnectionForm *ui;
    QJsonObject connInfo;
};

#endif // ADDBROKER_H
