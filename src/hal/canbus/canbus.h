#pragma ONCE

#ifndef CANBUS_H
#define CANBUS_H

#include <cstdint>
#include <ESP32CAN.h>

typedef void (* can_rx_listener)(uint32_t id, uint8_t d[8]);

class CANbus {
    public:
        static CANbus& getInstance() {
            static CANbus instance;
            return instance;
        };
        CANbus();
        void tx(uint32_t id, uint8_t data[8]);
        void rx_periodic();
        void addListener(can_rx_listener listener);

    private:
        can_rx_listener rx_listeners[128]; // allocate up to 128 listeners. This should be enough for a while.
        uint32_t listener_count = 0; // keep track of how many listeners have been allocated
        CAN_device_t CAN_cfg;
        const int rx_queue_size = 50; // Queue up to 50 message in the RX buffer

        CAN_frame_t tx_frame;
        CAN_frame_t rx_frame;
};
#endif