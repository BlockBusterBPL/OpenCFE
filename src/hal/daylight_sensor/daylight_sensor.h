#pragma ONCE

#ifndef __DAYLIGHT_SENSOR_H
#define __DAYLIGHT_SENSOR_H

#include "hal/canbus/canbus.h"

#define DAYLIGHT_MESSAGE 0x02802262;

class DaylightSensor {
    public:
        static DaylightSensor& getInstance() {
            static DaylightSensor instance;
            return instance;
        }
        virtual int getDaylight() {
            return daylight;
        }
    private:
        DaylightSensor();
        void handleUpdate(uint32_t id, uint8_t data[8]);
        CANbus canbus = CANbus::getInstance();

        int daylight = 0xF; // default to full brightness
};

#endif