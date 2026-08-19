#include "mq2_filter.h"

#include <QtGlobal>
#include <algorithm>
#include <cmath>

namespace {
constexpr int WindowSize = 7;
constexpr int AlarmConfirmSamples = 5;
constexpr int ClearConfirmSamples = 5;
constexpr double EmaAlpha = 0.25;
}

Mq2FilterResult Mq2Filter::update(int raw, int threshold)
{
    raw = qBound(0, raw, 4095);
    threshold = qBound(1, threshold, 4095);

    window_.append(raw);
    while (window_.size() > WindowSize)
        window_.removeFirst();

    QList<int> sorted = window_;
    std::sort(sorted.begin(), sorted.end());
    const int median = sorted.at(sorted.size() / 2);

    if (!initialized_) {
        ema_ = median;
        initialized_ = true;
    } else {
        ema_ += EmaAlpha * (median - ema_);
    }

    const int filtered = qBound(0, static_cast<int>(std::lround(ema_)), 4095);
    const int hysteresis = qBound(80, static_cast<int>(threshold * 0.08), 250);

    if (!alarm_) {
        lowSamples_ = 0;
        if (filtered >= threshold) {
            ++highSamples_;
            if (highSamples_ >= AlarmConfirmSamples) {
                alarm_ = true;
                highSamples_ = 0;
            }
        } else {
            highSamples_ = 0;
        }
    } else {
        highSamples_ = 0;
        if (filtered <= threshold - hysteresis) {
            ++lowSamples_;
            if (lowSamples_ >= ClearConfirmSamples) {
                alarm_ = false;
                lowSamples_ = 0;
            }
        } else {
            lowSamples_ = 0;
        }
    }

    return {filtered, alarm_, !alarm_ && highSamples_ > 0};
}

void Mq2Filter::reset()
{
    window_.clear();
    ema_ = 0.0;
    initialized_ = false;
    alarm_ = false;
    highSamples_ = 0;
    lowSamples_ = 0;
}
