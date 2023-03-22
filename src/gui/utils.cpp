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
