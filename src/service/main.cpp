#include <QCoreApplication>
#include "qmqttservice.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QMqttService service;
    service.setConfigFile("../../conf.json");
    service.startService();

    return a.exec();
}
