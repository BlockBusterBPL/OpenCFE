#include "hal/daylight_sensor/daylight_sensor.h"

DaylightSensor::DaylightSensor() {
    canbus.addListener([this](uint32_t id, uint8_t d[8]) { this->handleUpdate(id, d); });
}

void DaylightSensor::handleUpdate(uint32_t id, uint8_t data[8]) {
    const int daylightMessage = DAYLIGHT_MESSAGE;
    if (id == daylightMessage) {
        daylight = data[7] - 0x10;
    }
}