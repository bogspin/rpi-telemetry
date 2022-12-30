#ifndef ADDBROKER_H
#define ADDBROKER_H

#include <QWidget>
#include <QtMqtt>

QT_BEGIN_NAMESPACE
namespace Ui {
class AddBroker;
}
QT_END_NAMESPACE

class AddBroker : public QWidget
{
    Q_OBJECT
public:
    explicit AddBroker(QWidget *parent = nullptr);

private:
    Ui::AddBroker *ui;
};

#endif // ADDBROKER_H
