#include "hal/lcd/lcd.h"

LCD::LCD() {
    lcd_data_queue = xQueueCreate(lcd_data_queue_length, sizeof( CANPacket ));
}

void LCD::enable() {
    // Turn on display, but media controls are disabled
    uint8_t enableStep1[8] = {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05};
    // canbus.tx(LCD_CONTROL_MESSAGE, enableStep1);
    addToQueue(LCD_CONTROL_MESSAGE, enableStep1);

    // Keep display enabled, but reactivate media controls
    uint8_t enableStep2[8] = {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    // canbus.tx(LCD_CONTROL_MESSAGE, enableStep2);
    addToQueue(LCD_CONTROL_MESSAGE, enableStep2);
}

void LCD::disable() {
    uint8_t disable[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04};
    // canbus.tx(LCD_CONTROL_MESSAGE, disable);
    addToQueue(LCD_CONTROL_MESSAGE, disable);
}

void LCD::clear() {
    uint8_t clear[8] = {0xE1, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    // canbus.tx(LCD_DATA_MESSAGE, clear);
    addToQueue(LCD_DATA_MESSAGE, clear);
}

void LCD::update(char text[], uint8_t len, uint8_t pos) {
    char str[8] = {0};
    memcpy(str, text, len);
    char firstByte = 0xE0 + len;
    uint8_t data[8] = {firstByte, pos, str[0], str[1], str[2], str[3], str[4], str[5]};
    // canbus.tx(LCD_DATA_MESSAGE, data);
    addToQueue(LCD_DATA_MESSAGE, data);
}

void LCD::print(char text[], int len) {
    char str[34] = {0};
    std::fill_n(str, 34, 0x20);
    memcpy(str, text, len);
    int block = 0;
    for(int i = 0; i < 32; i+=6) {
        switch (block) {
            case 0: {
                uint8_t d0[8] = {0xA7, 0x00, str[i], str[i+1], str[i+2], str[i+3], str[i+4], str[i+5]};
                // canbus.tx(LCD_DATA_MESSAGE, d0);
                addToQueue(LCD_DATA_MESSAGE, d0);
                break;
            }
            case 4: {
                uint8_t d4[8] = {0x67, str[i], str[i+1], str[i+2], str[i+3], str[i+4], str[i+5], str[i+6]};
                // canbus.tx(LCD_DATA_MESSAGE, d4);
                addToQueue(LCD_DATA_MESSAGE, d4);
                break;
            }
            default: {
                if(block != 5){
                    char firstByte = 0x20 + block;
                    uint8_t d[8] = {firstByte, str[i], str[i+1], str[i+2], str[i+3], str[i+4], str[i+5], str[i+6]};
                    // canbus.tx(LCD_DATA_MESSAGE, d);
                    addToQueue(LCD_DATA_MESSAGE, d);
                    i++;
                }
                break;
            }
        }
        block++;
    }
}

void LCD::addToQueue(uint32_t id, uint8_t data[8]) {
    // make a copy of the data
    CANPacket packet;
    memcpy(&packet.data, &data, sizeof(packet.data));
    packet.id = id;

    if (xQueueSendToBack(lcd_data_queue, &packet, (TickType_t)0) != pdTRUE) {
        // we have errored
    }
}

void LCD::periodicLoop() {
    // enough time has passed to send the next LCD message
    if (next_message_time < millis()) {
        // we attempt to pull a message off the queue
        if (xQueueReceive(lcd_data_queue, &next_lcd_message, 0) == pdTRUE) {
            // Transmit the message and reset our wait timer
            canbus.tx(next_lcd_message.id, next_lcd_message.data);
            next_message_time = millis() + LCD_MESSAGE_DELAY;
        }
    }
}