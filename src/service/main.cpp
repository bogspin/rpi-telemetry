#include <QCoreApplication>
#include <QDir>
#include "qmqttservice.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QMqttService service;

    switch (argc) {
    case 1: {
        QString configPath;
        QString dirPath = QCoreApplication::applicationDirPath();
        QDir dir(dirPath);

        dir.cdUp();
        dir.cdUp();

        configPath = dir.absolutePath() + "/conf.json";
        service.setConfigFile(configPath);
        break;
    }
    case 2: {
        service.setConfigFile(argv[1]);
    }
    default: {
        qDebug() << "Error: Too many arguments provided!";
        QCoreApplication::exit(1);
    }
    }

    service.startService();

    return a.exec();
}
