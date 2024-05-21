#pragma ONCE

#ifndef __DAYLIGHT_SENSOR_H
#define __DAYLIGHT_SENSOR_H

#include "hal/canbus/canbus.h"
#include "hal/canbus/can_listener.h"

#define DAYLIGHT_MESSAGE 0x02802262;

class DaylightSensor : public CAN_listener {
    public:
        static DaylightSensor& getInstance() {
            static DaylightSensor instance;
            return instance;
        }
        virtual int getDaylight() {
            return daylight;
        }
        void handleUpdate(uint32_t id, uint8_t data[8]);
    private:
        DaylightSensor();
        CANbus canbus = CANbus::getInstance();
        int daylight = 0xF; // default to full brightness
};

#endif