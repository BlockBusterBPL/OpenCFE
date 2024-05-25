#include "hal/d2/d2_packager.h"

D2_Packager::D2_Packager() {
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

void D2_Packager::handleUpdate(uint32_t id, uint8_t data[8]) {
    if (true) { // TODO: figure out which message IDs should get decoded

    }
}