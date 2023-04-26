#ifndef CONNECTIONWINDOW_H
#define CONNECTIONWINDOW_H

#include <QWidget>
#include <QMessageBox>
#include <QJsonObject>

QT_BEGIN_NAMESPACE
namespace Ui {
class ConnectionWindow;
}
QT_END_NAMESPACE

class ConnectionWindow : public QWidget
{
    Q_OBJECT
public:
    explicit ConnectionWindow(QWidget *parent = nullptr);
    void setConnection(QJsonObject conn);
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
    Ui::ConnectionWindow *ui;
    QJsonObject connInfo;
};

#endif // CONNECTIONWINDOW_H
