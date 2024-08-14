#pragma ONCE

#ifndef __BUS_TOOLS_H
#define __BUS_TOOLS_H

#include "hal/canbus/canbus.h"

class Bus_Tools {
    public:
        void keepalive();

        void whole_bus_silence();

        void ecu_reset(uint8_t ecu_id);
}

#endif