#pragma ONCE

#ifndef __DAYLIGHT_SENSOR_H
#define __DAYLIGHT_SENSOR_H

#include "hal/canbus/canbus.h"

#define DAYLIGHT_MESSAGE 0x02802262;

class DaylightSensor : private CANbus::Listener {
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
        CANbus canbus = CANbus::getInstance();
        void handleUpdate(uint32_t id, uint8_t data[8]);

        int daylight = 0xF; // default to full brightness
};

#endif