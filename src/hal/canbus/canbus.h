#pragma ONCE

#ifndef __CANBUS_H
#define __CANBUS_H

#include <cstdint>
#include <functional>
#include <vector>
#include <ESP32CAN.h>
#include <CAN_config.h>

#include "hal/canbus/can_listener.h"

class CANbus {
    public:
        static CANbus& getInstance() {
            static CANbus instance;
            return instance;
        };
        CANbus();
        void tx(uint32_t id, uint8_t data[8]);
        void rx_periodic();
        void addListener(CAN_listener *listener);

    private:
        std::vector<CAN_listener *> rx_listeners; // allocate up to 128 listeners. This should be enough for a while.
        const int rx_queue_size = 50; // Queue up to 50 message in the RX buffer
        uint32_t last_can_rx = 0;

        CAN_frame_t tx_frame;
        CAN_frame_t rx_frame;
};
#endif