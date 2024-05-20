#pragma ONCE

#ifndef LCD_H
#define LCD_H

#include "hal/canbus/canbus.h"

#include <Arduino.h>

#define LCD_CONTROL_MESSAGE 0x0160200E
#define LCD_DATA_MESSAGE 0x00800008
#define LCD_MESSAGE_DELAY 40

class LCD {
    public:
        static LCD& getInstance() {
            static LCD instance;
            return instance;
        }
        void enable();
        void disable();
        void clear();
        void update(char text[], uint8_t len, uint8_t pos);
        void print(char text[], int len);

    private:
        LCD();
        CANbus canbus = CANbus::getInstance();
};
#endif