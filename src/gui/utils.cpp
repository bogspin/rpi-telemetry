#include "utils.h"

double valueToDouble(std::string valueAsString)
{
    QString value(valueAsString.c_str());

    value.remove("value=");
    return value.toDouble();
}

double timestampToDouble(std::chrono::time_point<std::chrono::system_clock> timestamp)
{
    auto ts = std::chrono::duration_cast<std::chrono::milliseconds>(timestamp.time_since_epoch());

    return std::chrono::duration<double>(ts).count();
}

std::string createQuery(QString hostname, QString topic)
{
    return QString("SELECT * FROM \"" + hostname + "\" WHERE topic='" + topic + "'").toStdString();

}

std::string createQuery(QString hostname, QString topic, QString time)
{
    return QString().toStdString();
}

QPoint plotPosition(int noOfPlots)
{

    switch (noOfPlots) {
    case 0: {
        return QPoint(0, 0);
    }
    case 1: {
        return QPoint(0, 1);
    }
    case 2: {
        return QPoint(1, 0);
    }
    case 3: {
        return QPoint(1, 1);
    }
    default: return QPoint(0, 0);
    }
}
