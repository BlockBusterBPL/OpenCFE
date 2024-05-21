#pragma ONCE

#ifndef __LCD_H
#define __LCD_H

#include "hal/canbus/canbus.h"
#include "modules/ModuleBase.h"

#include <Arduino.h>

#define LCD_CONTROL_MESSAGE 0x0160200E
#define LCD_DATA_MESSAGE 0x00800008
#define LCD_MESSAGE_DELAY 40

class LCD : public ModuleBase {
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
        void periodicLoop();

    private:
        LCD();
        CANbus canbus = CANbus::getInstance();
        QueueHandle_t lcd_data_queue;
        const int lcd_data_queue_length = 20;
        unsigned long next_message_time = 0;
        uint8_t next_lcd_message[8];
        void addToQueue(uint8_t data[8]);
};
#endif