#pragma ONCE

#ifndef __CAN_LISTENER_H
#define __CAN_LISTENER_H

class CAN_listener {
    public:
        virtual void handleUpdate(uint32_t id, uint8_t d[8]);
};

#endif