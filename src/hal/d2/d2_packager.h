#pragma ONCE

#ifndef __D2_PACKAGER_H
#define __D2_PACKAGER_H

#include <unordered_map>
#include <vector>

#include <Arduino.h>

#include "ESP32CAN.h"

#include "hal/canbus/canbus.h"

// Packs and unpacks messages for Volvo's "D2" Diagnostics Protocol
class D2_Packager : private CANbus::Listener {
    public:
        // a container for storing parts of a recieved D2 message
        struct RxStage {
            enum class Status {Empty, Filling, Full} status = Status::Empty;
            enum class MessageType {Invalid, SingleFrame, MultiFrame} type = MessageType::SingleFrame;
            uint8_t frameData[8][8] = {0};
            int nextFrameToCopy = 0;
            int totalMessageLength = 0;
        };

        // a container for staging a D2 message to be sent
        struct TxStage {
            bool extended = false;
            unsigned int frame_count = 0;
            unsigned int last_frame_sig_bytes = 0;
            uint8_t frame_data[8][8] = {0};
        };

        class Listener {
            public:
                void recieve(unsigned int eid, unsigned char payload[]);
        };

        static D2_Packager& getInstance() {
            static D2_Packager instance;
            return instance;
        };
        D2_Packager();
        // Pack and send the included bytes as a D2 message
        void send(unsigned long eid, unsigned char * payload, unsigned int size);

    private:
        CANbus canbus = CANbus::getInstance();

        RxStage empty_rx_stage;

        std::unordered_map<uint32_t, RxStage> rx_message_map = std::unordered_map<uint32_t, RxStage>();
        void handleUpdate(uint32_t id, uint8_t data[8]);

        std::vector<uint32_t> responseIDs = std::vector<uint32_t>();

        std::vector<D2_Packager::Listener*> listeners = std::vector<D2_Packager::Listener*>();
        
        TxStage empty_tx_stage;
        TxStage tx_stage;

        // Create a sequence of CAN frames to send the supplied message
        void internal_pack_frames(TxStage *stage, unsigned char * payload, unsigned int size);

        // Transmit all required frames from the tx stage
        void internal_transmit_frames(unsigned long eid, TxStage *stage);

        void internal_read_response(uint32_t eid, uint8_t data[8]);
};

#endif