#pragma once

#include <QList>

struct Mq2FilterResult {
    int raw = 0;
    bool alarm = false;
    bool pending = false;
};

// Lọc các xung ADC ngắn và chỉ đổi trạng thái cảnh báo khi tín hiệu duy trì
// đủ lâu. Hysteresis tránh trạng thái bật/tắt liên tục sát ngưỡng.
class Mq2Filter {
public:
    Mq2FilterResult update(int raw, int threshold);
    void reset();

private:
    QList<int> window_;
    double ema_ = 0.0;
    bool initialized_ = false;
    bool alarm_ = false;
    int highSamples_ = 0;
    int lowSamples_ = 0;
};
