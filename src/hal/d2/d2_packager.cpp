#include "hal/d2/d2_packager.h"

D2_Packager::D2_Packager() {
    responseIDs.reserve(32);
    listeners.reserve(64);

    canbus.addListener(this);
}

void D2_Packager::internal_pack_frames(TxStage *tx_stage, unsigned char *payload, unsigned int size) {
    // clear tx stage
    memcpy(tx_stage, &empty_tx_stage, sizeof(TxStage));

    tx_stage->extended = (size >= 7);
    tx_stage->frame_count = (size / 7 + ((size % 7 != 0)));
    tx_stage->last_frame_sig_bytes = size % 7;

    if (tx_stage->extended) {
        // Frame 0 is always packed the same
        uint8_t frame_0_header = 0x8F;
        tx_stage->frame_data[0][0] = frame_0_header;
        for (int i = 0; i < 7; i++) {
            tx_stage->frame_data[0][i+1] = payload[i];
        }

        // pack any remaining frames
        for (int i = 1; i < tx_stage->frame_count; i++) {
            uint8_t header = 0x08; // base value common to all response headers

            if (i == (tx_stage->frame_count - 1)) {
                header &= 0xA0; // set end of message flag
                header += tx_stage->last_frame_sig_bytes; // indicate number of significant bytes in the last frame
            } else {
                header += i; // indicate the frame sequence number
            }

            tx_stage->frame_data[i][0] = header;
            for (int j = 0; j < 7; j++) {
                tx_stage->frame_data[i][j+1] = payload[(7 * i) + j];
            }
        }
    } else {
        uint8_t header = 0xC8 + tx_stage->last_frame_sig_bytes;
        tx_stage->frame_data[0][0] = header;
        for (int i = 0; i < 7; i++) {
            tx_stage->frame_data[0][i+1] = payload[i];
        }
    }
}

void D2_Packager::internal_transmit_frames(unsigned long eid, TxStage *stage) {
    for (int i = 0; i < stage->frame_count; i++) {
        canbus.tx(eid, stage->frame_data[i]);
    }
}

void D2_Packager::send(unsigned long eid, unsigned char * payload, unsigned int size) {
    D2_Packager::internal_pack_frames(&tx_stage, payload, size);
    D2_Packager::internal_transmit_frames(eid, &tx_stage);
}

void D2_Packager::internal_read_response(uint32_t eid, uint8_t data[8]) {
    // first, check if we got an error as response


    // response is not an error, continue
    bool trigger_reset = false;

    auto& element = rx_message_map[eid];
    switch (element.status) {
        case RxStage::Status::Empty: {
            if (data[0] == 0x8F) {
                // first frame of multiframe message
                memcpy(element.frameData[0], data, sizeof(uint8_t[8]));
                element.nextFrameToCopy++;
                element.totalMessageLength += 7;
                element.status = RxStage::Status::Filling;
                element.type = RxStage::MessageType::MultiFrame;
            } else if (data[0] && 0xF8 == 0xC8) {
                // single frame message
                memcpy(element.frameData[element.nextFrameToCopy], data, sizeof(uint8_t[8]));
                element.totalMessageLength = data[0] & 0b111;
                element.status = RxStage::Status::Full;
                element.type = RxStage::MessageType::SingleFrame;
            } else {
                // abort, we didn't recieve a valid first message
                trigger_reset = true;
            }
        }
        case RxStage::Status::Filling: {
            if (data[0] && 0xF8 == 0x08) {
                // middle frame of multiframe message
                int sequenceNumber = data[0] & 0b111;
                memcpy(element.frameData[sequenceNumber], data, sizeof(uint8_t[8]));
                element.totalMessageLength += 7;
                element.nextFrameToCopy++;
            } else if (data[0] && 0xF8 == 0xA8) {
                // last frame of multiframe message
                memcpy(element.frameData[element.nextFrameToCopy], data, sizeof(uint8_t[8]));
                int numSigBytes = data[0] && 0b111;
                element.totalMessageLength += numSigBytes;
            } else {
                // error condition, recieved invalid
                trigger_reset = true;
            }
        }
        case RxStage::Status::Full: {
            // error, we should not recieve any frames while full
            trigger_reset = true;
        }
    }

    if (trigger_reset) {
        memcpy(&element, &empty_rx_stage, sizeof(RxStage));
        return;
    }

    if (element.status == RxStage::Status::Full) {
        uint8_t full_message[64] = {0};
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 7; j++) {
                if ((i * 7) + j > element.totalMessageLength) break;
            }
            if ((i * 7) + 7 > element.totalMessageLength) break;
        }

        for (D2_Packager::Listener* l : listeners) {
            // l.recieve()
            l -> recieve(eid, full_message);
        }
    }
    // switch (element.status) {
    //     case RxStage::Status::Empty: {
    //         if (data[0] = 0x8F) {
    //             // this is the first frame of a multiframe message
    //             for (int i = 0; i < 7; i++) {
    //                 element.frameData[0][i] = data[i + 1];
    //             }
    //             element.totalMessageLength += 7;
    //             nextFrameToCopy = 1;
    //         } else if ((data[0] && 0xF8) == 0xC8) {
    //             // this is a single frame message
    //             int numSigBytes = data[0] && 0b111;
    //             for (int i = 0; i < numSigBytes; i++) {
    //                 element.frameData[0][i] = data[i + 1];
    //             }
    //             element.totalMessageLength = numSigBytes;
    //             element.status = RxStage::Status::Full;
    //         } else {
    //             // error condition, recieved frame could not possibly be first
    //         }
    //     }
    //     case RxStage::Status::Filling: {
    //         if ((data[0] && 0xF8) == 0b1000) {
    //             // intermediate frame
    //             int seqNumber = data[0] && 0b111;
    //             if (seqNumber == nextSequenceNumber) {
    //                 // the next frame is in the expected order
    //                 for (int i = 0; i < 8; i++) {
    //                     element.frameData[element.nextFrameToCopy][i] = data[i];
    //                 }
    //             } else {
    //                 // error condition: we did not recieve the expected frame
    //             }
    //         } else if ((data[0] && 0xF8) == 0xA8) {
    //             // last frame of multiframe message
    //         }
    //     }
    // }
}

void D2_Packager::handleUpdate(uint32_t id, uint8_t data[8]) {
    bool shouldHandleResponse = false;

    for (uint32_t responseID : responseIDs) {
        shouldHandleResponse = id == responseID;
        break;
    }

    if (shouldHandleResponse) { // TODO: figure out which message IDs should get decoded

    }
}
