#pragma once

#include <chrono>

typedef std::chrono::steady_clock chr;

// Une classe pour avoir des moyennes de temps sur différentes parties du code
// dans une boucle.
class TimeMeasuring {
    chr::time_point start, end;
    std::chrono::duration<double> totalDuration;
    std::chrono::duration<double> averageDuration;
    unsigned int count = 0;

  public:
    TimeMeasuring() : totalDuration(0), averageDuration(0) {}

    void setStart() { start = chr::now(); }

    void setEnd() {
        end = chr::now();
        totalDuration += end - start;
        count++;
    }

    std::chrono::duration<double> getAverageDuration() {
        averageDuration = totalDuration / count;
        return averageDuration;
    }
};