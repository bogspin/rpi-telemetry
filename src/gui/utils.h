#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QPoint>
#include <stdlib.h>
#include <chrono>

#define MAX_PLOTS 4

double valueToDouble(std::string valueAsString);
double timestampToDouble(std::chrono::time_point<std::chrono::system_clock> timestamp);

std::string selectQuery(QString hostname, QString topic);
std::string selectQuery(QString hostname, QString topic, qint64 startTime, qint64 endTime);

QPoint plotPosition(int noOfPlots);

#endif // UTILS_H
