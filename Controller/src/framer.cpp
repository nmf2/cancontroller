#include "../include/interface.h"
#include "stdbool.h"

bool *crc_calc ();

void framer (bool *id, bool *payload, Frame *frm){
    int i = 1, // frm->data's index.
        j = 0, // *id or *payload index.
        bit_end_data,
        bit_start_crc,
        bit_end_crc,
        
        crc_d;
    frm->data[0] = 0; //SOF
    
    // Write IDA
    for (i = BIT_START_ID_A, j = 0; i <= BIT_END_ID_A; i++, j++){
        frm->data[i] = id[j];
    }

    if (frm->extended){
        frm->data[BIT_SRR_B] = 1; // Must be recessive
        frm->data[BIT_IDE] = 1; // Must be recessive
        // Reserved bits
        frm->data[BIT_R1_B] = 0;  // Must be 0
        frm->data[BIT_R0_B] = 0;  // Must be 0
        
        // Write IDB
        for (i = BIT_START_ID_B; i <= BIT_END_ID_B; i++, j++){
            frm->data[i] = id[j];
        }
        
        if (frm->type == DATA_FRAME){
            frm->data[BIT_RTR_B] = 0; // Not remote

            bit_end_data = BIT_START_DATA_B + 8*frm->payload_size; 

            for (i = BIT_START_DATA_B, j = 0; i <= bit_end_data; i++, j++){
                frm->data[i] = payload[j];
            }

            bit_start_crc = i;
            bit_end_crc = i + 15;
        }
        else { //Remote Frame 
            frm->data[BIT_RTR_B] = 1; // Must be 1
            

            bit_start_crc = BIT_REMOTE_START_CRC_B;
            bit_end_crc = BIT_REMOTE_END_CRC_B;
        }

        bool dlc[4] = { 0 };
        int_to_bits(frm->payload_size, dlc);
        for (i = BIT_START_DLC_B, j = 0; i <= BIT_END_DLC_B; i++, j++){
            frm->data[i] = dlc[j];
        }

        /*
            A FOR TREATING THE CRC
        */

        crc_d = bit_end_crc + 1;
        frm->data[crc_d] = 1; // write CRCd
        frm->data[crc_d + 1] = 1; // ACK
        frm->data[crc_d + 2] = 1; // ACKd
        i = crc_d + 3; // EOF
        j = i + 7;

        for(; i <= j; i++){ // Write EOF flag
            frm->data[i] = 1;
        }
    }
    else { // Not extended
        frm->data[BIT_IDE] = 1; // Must be recessive, not extended
        frm->data[BIT_R0_A] = 0; // Must be dominant

        if (frm->type == DATA_FRAME){
            frm->data[BIT_RTR_A] = 0; // Must be dominant

            bit_end_data = BIT_START_DATA_A + 8*frm->payload_size; 

            for (i = BIT_START_DATA_A, j = 0; i <= bit_end_data; i++, j++){
                frm->data[i] = payload[j];
            }

            bit_start_crc = i;
            bit_end_crc = i + 15;
        }
        else { //Remote Frame 
            frm->data[BIT_RTR_A] = 1; // Must be 1

            bit_start_crc = BIT_REMOTE_START_CRC_A;
            bit_end_crc = BIT_REMOTE_END_CRC_A;
        }

        bool dlc[4] = { 0 };
        int_to_bits(frm->payload_size, dlc);
        for (i = BIT_START_DLC_A, j = 0; i <= BIT_END_DLC_A; i++, j++){
            frm->data[i] = dlc[j];
        }

        /*
            A FOR TREATING THE CRC
        */

        crc_d = bit_end_crc + 1;
        frm->data[crc_d] = 1; // write CRCd
        frm->data[crc_d + 1] = 1; // ACK
        frm->data[crc_d + 2] = 1; // ACKd
        i = crc_d + 3; // Start of EOF
        j = i + 7; // End of EOF

        for(; i <= j; i++){ // Write EOF flag
            frm->data[i] = 1;
        }
    }
    frm->frame_size = i; // By the end, the i'th bit is the one after the last 
                         // EOF bit.
}