#pragma ONCE

#ifndef __CAN_LOGGER_H
#define __CAN_LOGGER_H

#include "hal/canbus/canbus.h"
#include "hal/canbus/can_listener.h"

#include <Arduino.h>

class CAN_logger : public CAN_listener {
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
        void handleUpdate(uint32_t id, uint8_t data[8]);
    private:
        CAN_logger();
        CANbus canbus = CANbus::getInstance();
        bool enabled = false;
        unsigned int filter = 0x0;
        unsigned int focusByte = 0;
        bool focused = false;
};

#endif