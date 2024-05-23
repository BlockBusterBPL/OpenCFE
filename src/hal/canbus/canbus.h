#pragma ONCE

#ifndef __CANBUS_H
#define __CANBUS_H

#include <cstdint>
#include <functional>
#include <ESP32CAN.h>
#include <CAN_config.h>

#include "libraries/linked_list/LinkedList.h"

#include "hal/canbus/can_listener.h"

struct CANPacket {
    uint32_t id;
    uint8_t data[8] = {0};
};
class CANbus {
    public:
        
        static CANbus& getInstance() {
            static CANbus instance;
            return instance;
        };
        CANbus();
        void tx(uint32_t id, uint8_t data[8]);
        void tx_raw(CAN_frame_t *frame);
        void rx_periodic();
        void addListener(CAN_listener *listener);

    private:
        LinkedList<CAN_listener *> rx_listeners;
        const int rx_queue_size = 50; // Queue up to 50 message in the RX buffer
        uint32_t last_can_rx = 0;

        CAN_frame_t tx_frame;
        CAN_frame_t rx_frame;
};
#endif