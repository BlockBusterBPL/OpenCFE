#include "hal/swm_inputs/swm_inputs.h"

#include <Arduino.h>

SWM_Inputs::SWM_Inputs() {
    canbus.addListener(this);
}

void SWM_Inputs::handleUpdate(uint32_t id, uint8_t d[8]) {
    Serial.print("Hmmbs");
    switch (id)
    {
    case SWM_MESSAGE_1: {
        // TODO: test out media controls on 2004 car
        uint8_t raw_media_controls = d[7];
        switch (raw_media_controls)
        {
        case 0x7F:
            media_controls = MEDIA_NONE;
            break;
        case 0x77:
            media_controls = MEDIA_VOL_UP;
            break;
        case 0x7B:
            media_controls = MEDIA_VOL_DOWN;
            break;
        default:
            media_controls = MEDIA_NONE;
            break;
        }

        // TODO: same thing as above for cruise control buttons and stalk buttons

        uint8_t raw_rti_controls = d[6];
        switch (raw_rti_controls)
        {
        case 0x40:
            rti_controls = RTI_NONE;
            break;
        case 0x48:
            rti_controls = RTI_UP;
            break;
        case 0x44:
            rti_controls = RTI_DOWN;
            break;
        case 0x42:
            rti_controls = RTI_LEFT;
            break;
        case 0x41:
            rti_controls = RTI_RIGHT;
            break;
        case 0x50:
            rti_controls = RTI_BACK;
            break;
        case 0x60:
            rti_controls = RTI_ENTER;
            break;
        default:
            rti_controls = RTI_NONE;
            break;
        }

        break;
    }
    case SWM_MESSAGE_2: {
        break;
    }
    default:
        break;
    }
}