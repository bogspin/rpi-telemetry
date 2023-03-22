#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <QString>
#include <chrono>

double valueToDouble(std::string valueAsString);
double timestampToDouble(std::chrono::time_point<std::chrono::system_clock> timestamp);

#endif // UTILS_H
