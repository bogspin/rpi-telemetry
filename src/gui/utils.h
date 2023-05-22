#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QPoint>
#include <stdlib.h>
#include <chrono>

#define MAX_PLOTS 4
#define MAX_LEGEND_LENGTH 16

double valueToDouble(std::string valueAsString);
double timestampToDouble(std::chrono::time_point<std::chrono::system_clock> timestamp);

QPoint plotPosition(int noOfPlots);
QString createGraphName(QString alias, QString unit);

#endif // UTILS_H
