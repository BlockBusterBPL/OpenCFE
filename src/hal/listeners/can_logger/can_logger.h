#pragma ONCE

#ifndef __CAN_LOGGER_H
#define __CAN_LOGGER_H

#include "hal/canbus/canbus.h"

#include <Arduino.h>

class CAN_logger : private CANbus::Listener {
    public:
        static CAN_logger& getInstance() {
            static CAN_logger instance;
            return instance;
        }
        void enable();
        void disable();
        void setFilter(unsigned int messageID);
        void setFocusByte(unsigned int byteIndex);
        void setFocused(bool shouldFocus);
    private:
        CAN_logger();
        CANbus canbus = CANbus::getInstance();
        void handleUpdate(uint32_t id, uint8_t data[8]);

        bool enabled = false;
        unsigned int filter = 0x0;
        unsigned int focusByte = 0;
        bool focused = false;
};

#endif