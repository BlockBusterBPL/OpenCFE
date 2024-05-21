#include "modules/can_logger/can_logger.h"

CAN_logger::CAN_logger() {
    canbus.addListener(this);
}

void CAN_logger::enable() {
    enabled = true;
}

void CAN_logger::disable() {
    enabled = false;
}

void CAN_logger::setFilter(unsigned int messageID) {
    filter = messageID;
}

void CAN_logger::setFocusByte(unsigned int byteIndex) {
    focusByte = byteIndex;
}

void CAN_logger::setFocused(bool shouldFocus) {
    focused = shouldFocus;
}

void CAN_logger::handleUpdate(uint32_t id, uint8_t data[8]) {
    if (enabled && id == filter) {
        if (focused) {
            printf("RX %08X (Byte %1u) ", id, focusByte);
            for (int i = 0; i < 8; i++) {
                printf("%1d", (data[focusByte] >> i) && 1);
            }
            printf("\n");
        } else {
            printf("RX %08X ", id);
            for (int i = 0; i < 8; i++) {
                printf("%02X ", data[i]);
            }
            printf("\n");
        }
    }
}