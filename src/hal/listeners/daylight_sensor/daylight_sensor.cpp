#include "hal/listeners/daylight_sensor/daylight_sensor.h"

DaylightSensor::DaylightSensor() {
    canbus.addListener(this);
}

void DaylightSensor::handleUpdate(uint32_t id, uint8_t data[8]) {
    const int daylightMessage = DAYLIGHT_MESSAGE;
    if (id == daylightMessage) {
        daylight = data[7] - 0x10;
    }
}