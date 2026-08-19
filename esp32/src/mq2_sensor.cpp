#include "mq2_sensor.h"
#include "config.h"

void Mq2Sensor::begin()
{
    pinMode(Config::MQ2_PIN, INPUT);
    analogReadResolution(12);
    analogSetPinAttenuation(Config::MQ2_PIN, ADC_11db);
    Serial.printf("[SENSOR] MQ2 AO on GPIO%u, %u samples, threshold=%u\n",
                  Config::MQ2_PIN, Config::MQ2_SAMPLES,
                  Config::MQ2_DEFAULT_THRESHOLD);
}

Mq2Reading Mq2Sensor::read(uint16_t threshold)
{
    uint32_t sum = 0;
    for (uint8_t i = 0; i < Config::MQ2_SAMPLES; ++i) {
        sum += static_cast<uint16_t>(analogRead(Config::MQ2_PIN));
        delay(Config::MQ2_SAMPLE_DELAY_MS);
    }

    Mq2Reading result;
    result.raw = static_cast<uint16_t>(sum / Config::MQ2_SAMPLES);
    result.millivolts = static_cast<uint16_t>(analogReadMilliVolts(Config::MQ2_PIN));
    result.detected = result.raw >= threshold;
    result.valid = true;
    result.ready = true;
    return result;
}
