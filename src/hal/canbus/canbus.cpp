#include "hal/canbus/canbus.h"

#include <Arduino.h>

CAN_device_t CAN_cfg;

CANbus::CANbus() {
    rx_listeners = LinkedList<CANbus::Listener *>();

    CAN_cfg.speed = CAN_SPEED_250KBPS; // has to be double on ESPs with 40mhz clocks
    CAN_cfg.tx_pin_id = GPIO_NUM_5;
    CAN_cfg.rx_pin_id = GPIO_NUM_4;
    CAN_cfg.rx_queue = xQueueCreate(rx_queue_size, sizeof(CAN_frame_t));

    ESP32Can.CANInit();
}

void CANbus::tx(uint32_t id, uint8_t data[8]) {
    tx_frame.FIR.B.FF = CAN_frame_ext;
    tx_frame.MsgID = id;
    tx_frame.FIR.B.DLC = 8;
    tx_frame.data.u8[0] = data[0];
    tx_frame.data.u8[1] = data[1];
    tx_frame.data.u8[2] = data[2];
    tx_frame.data.u8[3] = data[3];
    tx_frame.data.u8[4] = data[4];
    tx_frame.data.u8[5] = data[5];
    tx_frame.data.u8[6] = data[6];
    tx_frame.data.u8[7] = data[7];
    
    ESP32Can.CANWriteFrame(&tx_frame);
}

void CANbus::rx_periodic() {
    if (xQueueReceive(CAN_cfg.rx_queue, &rx_frame, 3 * portTICK_PERIOD_MS) == pdTRUE) {
        for (int i = 0; i < rx_listeners.size(); i++) {
            printf("CAN message recieved! %08X\n", rx_frame.MsgID);
            rx_listeners.get(i)->handleUpdate(rx_frame.MsgID, rx_frame.data.u8);
        }
        last_can_rx = millis();
    }
}

void CANbus::addListener(CANbus::Listener *listener) {
    rx_listeners.add(listener);
}