#pragma ONCE

#ifndef SWM_INPUTS_H
#define SWM_INPUTS_H

#include "hal/canbus/canbus.h"

#define SWM_MESSAGE_1 0x00404066
#define SWM_MESSAGE_2 0x0221300A

class SWM_Inputs {
    public:
        enum RTIControls {RTI_NONE, RTI_UP, RTI_DOWN, RTI_LEFT, RTI_RIGHT, RTI_BACK, RTI_ENTER};
        enum MediaControls {MEDIA_NONE, MEDIA_VOL_UP, MEDIA_VOL_DOWN, MEDIA_NEXT, MEDIA_PREV};
        enum CruiseControls {CRUISE_NONE, CRUISE_ACCEL, CRUISE_DECEL, CRUISE_RESUME, CRUISE_CANCEL};
        enum SignalStalkControls {STALK_NONE, STALK_READ, STALK_RESET};

        static SWM_Inputs& getInstance() {
            static SWM_Inputs instance;
            return instance;
        }
        virtual RTIControls getRTIControls() {
            return rti_controls;
        }
        virtual MediaControls getMediaControls() {
            return media_controls;
        }
        virtual CruiseControls getCruiseControls() {
            return cruise_controls;
        }
        virtual SignalStalkControls getSignalStalkControls() {
            return signal_stalk_controls;
        }
        virtual bool getRotaryInitialized() {
            return rotary_switch_initialized;
        }
        virtual uint8_t getRotaryPosition() {
            return rotary_switch_position;
        }

    private:
        SWM_Inputs();
        void handleUpdate(uint32_t id, uint8_t data[8]);
        CANbus canbus = CANbus::getInstance();

        RTIControls rti_controls = RTI_NONE;
        MediaControls media_controls = MEDIA_NONE;
        CruiseControls cruise_controls = CRUISE_NONE;
        SignalStalkControls signal_stalk_controls = STALK_NONE;
        uint8_t rotary_switch_position = 0;
        bool rotary_switch_initialized;
};

#endif